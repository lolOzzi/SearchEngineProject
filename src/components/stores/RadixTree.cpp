static int has_prefix(std::string prefix, std::string word) {
    for (int i = 0; i < prefix.size(); i++) {
        if (prefix[i] != word[i]) return i;
    }
    return prefix.size();
}

class Node {
public:
    std::string label;
    std::vector<Node> targets;
    Node* parent;
    Node() {
        label = "";
        targets = std::vector<Node>{};
        parent = nullptr;
    }
    Node(std::string label) {
        this->label = label;
        targets = std::vector<Node>{};
        parent = nullptr;
    }

    static Node* find_target(std::string label, std::vector<Node>& domain) {
        for (Node& node : domain) {
            if (node.label == label) {
                return &node;
            }
        }
        return nullptr;
    }


    Node* AddTarget(std::string word) {
        bool no_prefix = true;
        for (Node& node : targets) {
            int num_shared_chars = has_prefix(node.label, word);
            if (num_shared_chars == 0) continue;

            no_prefix = false;
            if (num_shared_chars == node.label.size()) {
                std::string sub = word.substr(num_shared_chars);
                return node.AddTarget(sub);
            }
            std::string split_string = node.label.substr(0, num_shared_chars);

            std::string new_string = node.label.substr(num_shared_chars);
            Node new_child_with_this_info = Node(new_string);
            new_child_with_this_info.parent = &node;
            new_child_with_this_info.targets = node.targets;
            node.targets.clear();
            node.targets.push_back(new_child_with_this_info);

            std::string other_new_string = word.substr(num_shared_chars);
            Node new_child_node = Node(other_new_string);
            new_child_node.parent = &node;
            node.targets.push_back(new_child_node);

            node.label = split_string;

            return find_target(new_child_node.label, node.targets);
        }

        if (no_prefix) {
            Node new_target = Node(word);
            new_target.parent = this;
            targets.push_back(new_target);
            return find_target(new_target.label, targets);
        }
        assert(false);
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
