#include <memory>
#include <vector>
#include <string>
#include <cstdint>
#include "../../core/interfaces.h"
#include "./RadixTree++.cpp"


class TreeWrapper : public IStore {
private:
    RadixTree tree;

    std::vector<Node*> doc_nodes;

    uint32_t next_doc_id = 0;
    uint32_t last_doc_id = UINT32_MAX;
    uint32_t debug_counter = 0;

public:
    TreeWrapper() : tree() {}
    ~TreeWrapper() override = default;

    void add_document(Doc document) override {
        Node* docNode = tree.add(document.title);
        if (debug_counter == 10000 ) {
            std::cout << next_doc_id << "\n";
            debug_counter = 0;
        }

        if (docNode->doc_id == UINT32_MAX) {
            uint32_t id = next_doc_id++;
            debug_counter++;
            doc_nodes.push_back(docNode);
            docNode->doc_id = id;
            last_doc_id = id;
        } else {
            last_doc_id = docNode->doc_id;
        }
    }

    void add(std::string word, Doc) override {
        if (word.empty()) return;
        if (last_doc_id == UINT32_MAX) return;

        Node* wordNode = tree.add(word);
        auto &plist = wordNode->postings;

        if (plist.empty() || plist.back() != last_doc_id) {
            plist.push_back(last_doc_id);
        }

    }

    std::vector<Doc> get(std::string word) override {
        std::vector<Doc> res;
        if (word.empty()) return res;
        Node* wordNode = tree.find(word);
        if (!wordNode) return res;
        const std::vector<uint32_t>& plist = wordNode->postings;
        res.reserve(plist.size());
        for (uint32_t docid : plist) {
            if (docid < doc_nodes.size()) {
                res.emplace_back(get_string_from_node(doc_nodes[docid]));
            }
        }
        return res;
    }

    int get_num_docs() override {
        return static_cast<int>(doc_nodes.size());
    }
};