#include <string>
#include "./Label.cpp"

static int has_prefix(std::string prefix, std::string word) {
    int max_its = std::min(prefix.size(), word.size());
    for (int i = 0; i < max_its; i++) {
        if (prefix[i] != word[i]) return i;
    }
    return max_its;
}

class Node {
public:
    Label label;
    std::vector<std::unique_ptr<Node>> targets;
    Node* parent;

    Node(std::string text = ""){
        label = Label();
        label.set(text.c_str());
        parent = nullptr;
    }

    static Node* create_parent(std::unique_ptr<Node>* child_to_be, std::string parent_string, const std::string &child_string) {
        Node* child_node = child_to_be->get();

        std::unique_ptr<Node> parent_node = std::make_unique<Node>(parent_string);
        child_node->label.set(child_string.c_str());

        parent_node->parent = child_node->parent;
        child_node->parent = parent_node.get();
        parent_node->targets.push_back(std::move(*child_to_be));

        for (auto it = parent_node->parent->targets.begin(); it != parent_node->parent->targets.end(); ) {
            if (it->get() == nullptr) {
                it = parent_node->parent->targets.erase(it);
                break;
            } else {
                ++it;
            }
        }

        parent_node->parent->targets.push_back(std::move(parent_node));

        return child_node;
    }

    Node* AddTarget(const std::string& word) {
        bool no_prefix = true;

        for (auto& node_ptr : targets) {
            Node& node = *node_ptr;
            int num_shared_chars = has_prefix(node.label.get(), word);

            if (num_shared_chars == 0) continue;

            no_prefix = false;

            std::string lab = node.label.get();
            if (num_shared_chars == lab.size()) {
                if (word == lab) {
                    return node_ptr.get();
                }
                std::string sub = word.substr(num_shared_chars);
                return node.AddTarget(sub);
            }

            if (num_shared_chars == word.size()) {
                std::string new_string = node.label.get();
                new_string = new_string.substr(num_shared_chars);
                Node* new_child_node = create_parent(&node_ptr, word, new_string);

                return new_child_node->parent;
            }

            std::string split_string = node.label.get();
            split_string = split_string.substr(0, num_shared_chars);
            std::string new_string = node.label.get();
            new_string = new_string.substr(num_shared_chars);

            Node* new_child_node = create_parent(&node_ptr, split_string, new_string);
            std::string other_new_string = word.substr(num_shared_chars);

            std::unique_ptr<Node> other_child_node = std::make_unique<Node>(other_new_string);
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
};



static std::string get_string_from_node(Node* node) {
    /*
    std::string full_string = node->label.get();
    Node* loop_node = node->parent;
    while (loop_node) {
        full_string.insert(0, loop_node->label.get());
        loop_node = loop_node->parent;
    }

    return full_string;
    */

    char* val = node->label.get();
    size_t val_len = strlen(val);
    char* dest = new char[val_len + 100]();
    std::memcpy(dest, val, val_len);
    size_t dest_len = std::strlen(dest);

    Node* loop_node = node->parent;
    while (loop_node) {
        char* prefix = loop_node->label.get();
        size_t prefix_len = std::strlen(prefix);
        std::memmove(dest + prefix_len, dest, dest_len + 1);
        std::memcpy(dest, prefix, prefix_len);
        dest_len += prefix_len;
        loop_node = loop_node->parent;
    }
    std::string converted = dest;
    delete[] dest;
    return converted;

}

class RadixTree {
public:
    Node root;

    RadixTree() {
        root = Node();
    }

    Node* add(std::string word) {
        if (word == "Alf") {
            int i = 1;
        }
        return root.AddTarget(word);
    }
};
