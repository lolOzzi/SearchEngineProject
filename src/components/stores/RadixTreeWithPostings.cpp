#include <memory>
#include <vector>
#include <string>
#include <cstdint>
#include <cassert>
#include <algorithm>

#include "../../core/interfaces.h"

class Node {
public:
    std::string label;
    std::vector<std::unique_ptr<Node>> targets;
    std::vector<Node*> original;
    Node* parent;

    std::vector<uint32_t> postings;

    // UINT32_MAX = NA.
    uint32_t doc_id;

    Node(std::string label_ = "")
        : label(std::move(label_)), parent(nullptr), postings(), doc_id(UINT32_MAX) {}

    static Node* create_parent(std::unique_ptr<Node>* child_to_be,
                            const std::string& parent_string,
                            const std::string& child_string)
    {
        Node* child_node = child_to_be->get();
        std::unique_ptr<Node> parent_node = std::make_unique<Node>(parent_string);

        child_node->label = child_string;
        parent_node->parent = child_node->parent;
        child_node->parent = parent_node.get();
        parent_node->targets.push_back(std::move(*child_to_be));

        assert(parent_node->parent);

        *child_to_be = std::move(parent_node);

        return child_node;
    }

    Node* AddTarget(const std::string& word) {
        bool no_prefix = true;

        for (auto& node_ptr : targets) {
            Node& node = *node_ptr;
            int num_shared_chars = 0;
            int max_its = std::min(node.label.size(), word.size());
            for (int i = 0; i < max_its; ++i) {
                if (node.label[i] != word[i]) break;
                ++num_shared_chars;
            }

            if (num_shared_chars == 0) continue;
            no_prefix = false;

            if (num_shared_chars == node.label.size()) {
                if (word.size() == node.label.size()) {
                    return node_ptr.get();
                }
                std::string sub = word.substr(num_shared_chars);
                return node.AddTarget(sub);
            }

            if (num_shared_chars == word.size()) {

                std::string new_string = node.label.substr(num_shared_chars);
                Node* new_child_node = create_parent(&node_ptr, word, new_string);

                return new_child_node->parent;
            }

            std::string split_string = node.label.substr(0, num_shared_chars);
            std::string new_string = node.label.substr(num_shared_chars);

            Node* new_child_node = create_parent(&node_ptr, split_string, new_string);

            std::string other_new_string = word.substr(num_shared_chars);
            auto other_child_node = std::make_unique<Node>(other_new_string);
            other_child_node->parent = new_child_node->parent;
            Node* word_node_res = other_child_node.get();
            new_child_node->parent->targets.push_back(std::move(other_child_node));

            return word_node_res;
        }

        if (no_prefix) {
            auto new_target = std::make_unique<Node>(word);
            new_target->parent = this;
            Node* result = new_target.get();
            targets.push_back(std::move(new_target));
            return result;
        }

        assert(false);
        return nullptr;
    }
    Node* FindTarget(const std::string& word) const {
        for (const auto& node_ptr : targets) {
            const Node& node = *node_ptr;
            size_t num_shared_chars = 0;
            size_t max_its = std::min(node.label.size(), word.size());
            for (size_t i = 0; i < max_its; ++i) {
                if (node.label[i] != word[i]) break;
                ++num_shared_chars;
            }
            if (num_shared_chars == 0) continue;
            if (num_shared_chars == node.label.size()) {
                if (word.size() == node.label.size()) return node_ptr.get();
                return node.FindTarget(word.substr(num_shared_chars));
            }
            return nullptr;
        }
        return nullptr;
    }
};

static std::string get_string_from_node(const Node* node) {
    std::vector<std::string_view> parts;
    for (const Node* n = node; n; n = n->parent)
        parts.push_back(n->label);
    std::string result;
    for (auto it = parts.rbegin(); it != parts.rend(); ++it)
        result += *it;
    return result;
}

class RadixTreeWithPostings {
public:
    Node root;

    RadixTreeWithPostings() : root("") {
    }

    Node* add(const std::string& word) {
        if (word.empty()) return &root;
        return root.AddTarget(word);
    }
    Node* find(const std::string& word) const {
        if (word.empty()) return nullptr;
        return root.FindTarget(word);
    }
};