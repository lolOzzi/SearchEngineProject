#pragma once

#include <memory>
#include "../../core/interfaces.h"
#include "../../extras/basic/ChaniedHashDictionary.h"
#include "../../extras/basic/RadixTree.h"
#include "../../extras/basic/GenericFKSDictionary.h"
#include "../../extras/basic/DynamicArray.h"
#include "../../extras/basic/CuckooHasingDictionary.h"
#include "../../extras/basic/bursttrie/Record.h"

class DictionaryWithDocId : public IStore {
private:
    class DictWord {
    public:
        Record word;
        DynamicArray<int> documents_in;
        DictWord() : word(""), documents_in(2) {}
        explicit DictWord(const std::string &w) : word(w), documents_in(2) {}

        bool operator==(const DictWord& other) const {
            return word == other.word;
        }
    };

    class DictDoc {
    public:
        Record word;
        long long start_loc;

        DictDoc() : word(""), start_loc(-1) { }
        explicit DictDoc(const std::string &w, const long long loc) : word(w), start_loc(loc) { }

        bool operator==(const DictDoc& other) const {
            return word == other.word;
        }
    };

    StringHashFamily hash_family = StringHashFamily();
    CuckooHashingDictionary<std::string, DictWord> dict;
    DynamicArray<DictDoc> doc_arr;
    std::string last_document_title_added;
    int last_docid_added;
public:
    ~DictionaryWithDocId() override = default;
    DictionaryWithDocId(int n);
    void add(std::string word, Doc document) override;
    void add_document(Doc document) override;
    int get_num_docs() override;
    std::vector<Doc> get(std::string word) override;
    int words_added = 0;
    int documents_added = 0;
    void print_spaced_used();
};
