#pragma once

#include "../../core/interfaces.h"

namespace BasicHashTableNS {

class Word {
public:
    Word(std::string word) {
        this->word = word;
        documents_in = std::vector<Doc>{};
        next = nullptr;
    }
    std::string word;
    std::vector<Doc> documents_in;
    Word* next;
};

class BasicHashTable : public IStore {
private:
    int numBuckets;
    Word** buckets;
    IHash* hash_function;
    Word* get_word(std::string word);
    int num_docs;
public:
    ~BasicHashTable() override;
    BasicHashTable(int n, IHash* hash_function);
    void add(std::string word, Doc document) override;
    void add_document(Doc document) override;
    int get_num_docs() override;
    std::vector<Doc> get(std::string word) override;
};

}
