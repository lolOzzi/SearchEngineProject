#pragma once
#include <memory>
#include "../../core/interfaces.h"
#include "../../extras/basic/DoubleArrayDSTreeWithInfo.h"
#include "../../extras/basic/DynamicArray.h"

namespace GenericDATWithDocIdNS {

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

class GenericDATWithDocId : public IStore {
private:
    DoubleArrayDSTreeWithInfo tree;
    DynamicArray<DictDoc> doc_arr;

    int last_document_edge_added;
    std::string last_document_title_added;
    int last_docid_added;
public:
    ~GenericDATWithDocId() override = default;
    GenericDATWithDocId(int n);
    void add(std::string word, Doc document) override;
    void add_document(Doc document) override;
    int get_num_docs() override;
    std::vector<Doc> get(std::string word) override;
    std::vector<Doc> prefix_search(std::string word);
    int words_added = 0;
    int documents_added = 0;
};

}
