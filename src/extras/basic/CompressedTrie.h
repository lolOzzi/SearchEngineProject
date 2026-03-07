#include <string>
#include <vector>
#include <memory>
#include <cassert>
#include "Label.h"
#include "SortedDynamicArray.h"
template<typename T, typename Compare = std::less<T>>
using SDA = SortedDynamicArray<T, Compare>;

namespace Trie {

static int has_prefix(std::string_view prefix, std::string_view word) {
    int max_its = std::min(prefix.size(), word.size());
    for (int i = 0; i < max_its; i++) {
        if (prefix[i] != word[i]) return i;
    }
    return max_its;
}

class Node {
public:
    struct PtrLess {
        bool operator()(const std::unique_ptr<Node>& a,
                        const std::unique_ptr<Node>& b) const {
            return *a < *b;
        }
    };

    Label label;
    SDA<std::unique_ptr<Node>, PtrLess> targets;
    Node* parent;

    Node(std::string text = ""): targets(8, PtrLess()){
        label = Label();
        label.set(text.c_str());
        parent = nullptr;
    }

    bool operator<(const Node& other) const {
        return std::strcmp(label.get(), other.label.get()) < 0;
    }

    static Node* create_parent(std::unique_ptr<Node>* child_to_be,
                               std::string parent_string,
                               const std::string &child_string) {
        Node* child_node = child_to_be->get();

        auto parent_node = std::make_unique<Node>(std::move(parent_string));
        child_node->label.set(child_string.c_str());

        Node* grandparent = child_node->parent;
        parent_node->parent = grandparent;
        child_node->parent = parent_node.get();

        std::swap(parent_node, *child_to_be);
        (*child_to_be)->targets.push_back(std::move(parent_node));

        return child_node;
    }

    Node* AddTarget(const std::string& word) {
        if (targets.size() == 0) {
            auto new_target = std::make_unique<Node>(word);
            new_target->parent = this;
            Node* result = new_target.get();
            targets.push_back(std::move(new_target));
            return result;
        }

        auto tmp = std::make_unique<Node>(word);
        int idx = targets.find_insert_index(tmp);

        const int candidates[2] = { idx, idx - 1 };
        for (int c = 0; c < 2; ++c) {
            int i = candidates[c];
            if (i < 0 || i >= targets.size()) continue;

            std::unique_ptr<Node>& child_uptr = targets[i];
            Node& node = *child_uptr;
            int num_shared_chars = has_prefix(node.label.get(), word);

            if (num_shared_chars == 0) continue;

            std::string lab = node.label.get();
            if (num_shared_chars == static_cast<int>(lab.size())) {
                if (word == lab) {
                    return child_uptr.get();
                }
                std::string sub = word.substr(num_shared_chars);
                return child_uptr->AddTarget(sub);
            }

            if (num_shared_chars == static_cast<int>(word.size())) {
                std::string new_string = node.label.get();
                new_string = new_string.substr(num_shared_chars);
                Node* new_child_node = create_parent(&child_uptr, word, new_string);
                return new_child_node->parent;
            }

            std::string split_string = node.label.get();
            split_string = split_string.substr(0, num_shared_chars);
            std::string new_string = node.label.get();
            new_string = new_string.substr(num_shared_chars);

            Node* new_child_node = create_parent(&child_uptr, split_string, new_string);
            std::string other_new_string = word.substr(num_shared_chars);

            auto other_child_node = std::make_unique<Node>(other_new_string);
            other_child_node->parent = new_child_node->parent;
            Node* word_node_res = other_child_node.get();
            new_child_node->parent->targets.push_back(std::move(other_child_node));

            return word_node_res;
        }

        auto new_target = std::make_unique<Node>(word);
        new_target->parent = this;
        Node* result = new_target.get();
        targets.push_back(std::move(new_target));
        return result;
    }
};

static std::string get_string_from_node(Node* node) {
    std::vector<std::string_view> parts;
    for ( Node* n = node; n; n = n->parent) {
        parts.push_back(n->label.get());
    }
    std::string result;
    for (auto it = parts.rbegin(); it != parts.rend(); ++it) {
        result += *it;
    }
    return result;
}

class CompressedTrie {
public:
    Node root;

    CompressedTrie() : root(Node()) {}

    Node* add(std::string word) {
        return root.AddTarget(word);
    }
};

}
