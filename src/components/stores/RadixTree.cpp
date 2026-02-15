static int has_prefix(std::string prefix, std::string word) {
    for (int i = 0; i < prefix.size(); i++) {
        if (prefix[i] != word[i]) return i;
    }
    return prefix.size();
}

class Node {
public:
    std::string label;
    // Store unique pointers so node addresses in memory never change
    std::vector<std::unique_ptr<Node>> targets;
    Node* parent;

    Node(std::string label = "") : label(label), parent(nullptr) {}

    // Helper method (assuming you have something like this)
    static int has_prefix(const std::string& a, const std::string& b) {
        int i = 0;
        while (i < a.size() && i < b.size() && a[i] == b[i]) {
            i++;
        }
        return i;
    }

    Node* AddTarget(std::string word) {
        bool no_prefix = true;

        for (auto& node_ptr : targets) {
            Node& node = *node_ptr;
            int num_shared_chars = has_prefix(node.label, word);

            if (num_shared_chars == 0) continue;

            no_prefix = false;

            if (num_shared_chars == node.label.size()) {
                std::string sub = word.substr(num_shared_chars);
                return node.AddTarget(sub);
            }

            std::string split_string = node.label.substr(0, num_shared_chars);
            std::string new_string = node.label.substr(num_shared_chars);

            auto new_child_with_this_info = std::make_unique<Node>(new_string);
            new_child_with_this_info->parent = &node;
            new_child_with_this_info->targets = std::move(node.targets);
            for (auto& grandchild : new_child_with_this_info->targets) {
                grandchild->parent = new_child_with_this_info.get();
            }

            node.targets.clear();
            node.targets.push_back(std::move(new_child_with_this_info));

            std::string other_new_string = word.substr(num_shared_chars);
            auto new_child_node = std::make_unique<Node>(other_new_string);
            new_child_node->parent = &node;
            Node* result = new_child_node.get();
            node.targets.push_back(std::move(new_child_node));

            node.label = split_string;
            return result;
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



static string get_string_from_node(Node* node) {
    std::string full_string = node->label;
    Node* loop_node = node->parent;
    while (loop_node) {
        full_string.insert(0, loop_node->label);
        loop_node = loop_node->parent;
    }
    return full_string;
}

class RadixTree {
public:
    Node root;

    RadixTree() {
        root = Node();
    }

    Node* add(std::string word) {
        return root.AddTarget(word);
    }
};
