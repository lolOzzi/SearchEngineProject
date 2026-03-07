#include <string>
#include <vector>
#include <memory>
#include <cassert>
#include "Label.h"
#include "GenericFKSDictionary.h"
#include "HashFamily.h"
template <typename T, typename U>
using FKSDict = GenericFKSDictionary<T, U>;
namespace Trie {

static int has_prefix(std::string_view prefix, std::string_view word) {
    int max_its = static_cast<int>(std::min(prefix.size(), word.size()));
    for (int i = 0; i < max_its; i++) {
        if (prefix[i] != word[i]) return i;
    }
    return max_its;
}

extern CharHashFamily hasher;

class Node {
public:
    Label label;
    FKSDict<char, std::unique_ptr<Node>> targets;
    Node* parent;

    Node(std::string text = ""): targets(100, &hasher) {
        label = Label();
        label.set(text.c_str());
        parent = nullptr;
    }

    bool operator<(const Node& other) const {
        return std::strcmp(label.get(), other.label.get()) < 0;
    }

    static Node* create_parent(std::unique_ptr<Node>* child_to_be,
                               const std::string& parent_string,
                               const std::string &child_string) {
        Node* child_node = child_to_be->get();

        auto parent_node = std::make_unique<Node>(parent_string);
        child_node->label.set(child_string.c_str());

        Node* grandparent = child_node->parent;
        parent_node->parent = grandparent;
        child_node->parent = parent_node.get();

        std::swap(parent_node, *child_to_be);

        (*child_to_be)->targets.add(child_string[0], std::move(parent_node));

        return child_node;
    }

    Node* AddTarget(const std::string& word) {
        auto maybe_child_ptr = targets.get(word[0]);
        if (!maybe_child_ptr) {
            auto new_target = std::make_unique<Node>(word);
            new_target->parent = this;
            Node* result = new_target.get();
            targets.add(word[0], std::move(new_target));
            return result;
        }

        std::unique_ptr<Node>& child_uptr = *maybe_child_ptr;
        Node& node = *child_uptr;
        int num_shared_chars = has_prefix(node.label.get(), word);

        if (num_shared_chars == 0) {
            auto new_target = std::make_unique<Node>(word);
            new_target->parent = this;
            Node* result = new_target.get();
            targets.add(word[0], std::move(new_target));
            return result;
        }

        std::string lab = node.label.get();

        if (num_shared_chars == static_cast<int>(lab.size())) {
            if (word == lab) {
                return child_uptr.get();
            }
            std::string remainder = word.substr(num_shared_chars);
            return child_uptr->AddTarget(remainder);
        }

        if (num_shared_chars == static_cast<int>(word.size())) {
            std::string child_string = lab.substr(num_shared_chars);
            Node* child_node = create_parent(&child_uptr, word, child_string);
            return child_node->parent;
        }

        std::string split_string = lab.substr(0, num_shared_chars);
        std::string old_child_remainder = lab.substr(num_shared_chars);
        std::string new_child_remainder = word.substr(num_shared_chars);

        Node* child_node = create_parent(&child_uptr, split_string, old_child_remainder);

        auto other_child_node = std::make_unique<Node>(new_child_remainder);
        other_child_node->parent = child_node->parent;
        Node* result_node = other_child_node.get();
        child_node->parent->targets.add(new_child_remainder[0], std::move(other_child_node));
        return result_node;
    }
};

static std::string get_string_from_node(Node* node) {
    std::vector<std::string_view> parts;
    for (Node* n = node; n; n = n->parent) {
        parts.push_back(n->label.get());
    }
    std::string result;
    for (auto it = parts.rbegin(); it != parts.rend(); ++it) {
        result += *it;
    }
    return result;
}

class CompressedHashTrie {
public:
    Node root;

    CompressedHashTrie() : root(Node()) {}

    Node* add(std::string word) {
        return root.AddTarget(word);
    }
};

}