#pragma once
#include "../../core/interfaces.h"

#include <string>
#include <vector>

namespace DocBlockNS {

class BlockTable {
  private:
      std::vector<std::vector<int>> abe;
    public:

};

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

class DocBlock : public IStore {
private:
    int numBuckets;
    Word** buckets;
    IHash* hash_function;
    Word* get_word(std::string word);
    int num_docs;
public:
    ~DocBlock() override;
    DocBlock(int n, IHash* hash_function);
    void add(std::string word, Doc document) override;
    void add_document(Doc document) override;
    int get_num_docs() override;
    std::vector<Doc> get(std::string word) override;
};

}
