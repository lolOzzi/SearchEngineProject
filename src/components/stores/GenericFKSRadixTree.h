#pragma once
#include <memory>
#include "../../core/interfaces.h"
#include "../../extras/basic/RadixTree.h"
#include "../../extras/basic/GenericFKSDictionary.h"

namespace GenericFKSRadixTreeNS {

class DictWord {
public:
    Node* word;
    std::vector<Node*> documents_in;

    DictWord(){
        this->word = nullptr;
        documents_in = std::vector<Node*>{};
    }
    explicit DictWord(Node* node) {
        this->word = node;
        documents_in = std::vector<Node*>{};
    }

    bool operator==(const DictWord& other) const {
        return word == other.word;
    }
};


class GenericFKSRadixTree : public IStore {
private:
    RadixTree tree;
    DictWord* get_word(std::string& word);
    Node* last_document_node_added;
    std::string last_document_title_added;
    StringHashFamily hash_family = StringHashFamily();
    GenericFKSDictionary<std::string, DictWord> dict;
public:
    ~GenericFKSRadixTree() override = default;
    GenericFKSRadixTree(int n, IHash* hash_function);
    void add(std::string word, Doc document) override;
    void add_document(Doc document) override;
    int get_num_docs() override;
    std::vector<Doc> get(std::string word) override;
    int words_added = 0;
    int documents_added = 0;
};

}
