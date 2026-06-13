#pragma once
#include <memory>
#include <unordered_set>

#include "../../core/interfaces.h"
//#include "../../extras/basic/DoubleArrayDSTreeWithInfo.h"
#include "../../extras/basic/DynamicArray.h"
#include "RegexStoreComp.hpp"
#include "../../extras/basic/bursttrie/BurstTrie.h"

namespace BurstTrieRegexStoreNS {

class TreeWord {
public:
    DynamicArray<int> documents_in;
    TreeWord() : documents_in(2) {}
};

class DictDoc {
public:
    std::string word;
    long long start_loc;

    DictDoc() : word(""), start_loc(-1) { }
    explicit DictDoc(const std::string &word_, long long loc) : word(word_), start_loc(loc) { }

    bool operator==(const DictDoc& other) const {
        return word == other.word;
    }
};

class BurstTrieRegexStore : public IStore {
private:
    BurstTrie<TreeWord> tree;
    DynamicArray<DictDoc> doc_arr;
    int last_document_edge_added;
    std::string last_document_title_added;
    int last_docid_added;
    RegexStoreComp reg;
public:
    ~BurstTrieRegexStore() override = default;
    BurstTrieRegexStore();
    void add(std::string word, Doc document) override;
    void add_document(Doc document) override;
    int get_num_docs() override;
    std::vector<Doc> get(std::string word) override;
    int words_added = 0;
    int indexId = 0;
    int documents_added = 0;
    std::vector<std::string> getWordsFromTrigram(std::string tri);

};

}
