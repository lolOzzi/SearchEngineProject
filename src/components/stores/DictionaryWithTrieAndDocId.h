#pragma once
#include <memory>
#include "../../core/interfaces.h"
#include "../../extras/basic/ChaniedHashDictionary.h"
#include "../../extras/basic/RadixTree.h"
#include "../../extras/basic/DoubleArrayDSTree.h"
#include "../../extras/basic/GenericFKSDictionary.h"
#include "../../extras/basic/DynamicArray.h"
#include "../../extras/basic/CuckooHasingDictionary.h"

class DictionaryWithTrieAndDocId : public IStore {
private:
    class DictWord {
    public:
        Node* word;
        DynamicArray<int> documents_in;

        DictWord() : word(nullptr), documents_in(2) {}
        explicit DictWord(Node* node) : word(node), documents_in(2) {}

        ~DictWord() { word = nullptr; }
        DictWord(const DictWord& other) : word(other.word), documents_in(other.documents_in) {};
        DictWord& operator=(const DictWord& other) { word = other.word; documents_in = other.documents_in; return *this;};

        DictWord(DictWord&& other) noexcept : word(other.word), documents_in(std::move(other.documents_in)) {other.word = nullptr; other.documents_in = DynamicArray<int>(); };
        DictWord& operator=(DictWord&& other) noexcept { if (this != &other) {word = other.word; documents_in = other.documents_in; other.word = nullptr; other.documents_in = DynamicArray<int>();} return *this; };

        bool operator==(const DictWord& other) const {
            return word == other.word;
        }
    };

    class DictDoc {
    public:
        Node* word;
        long long start_loc;

        DictDoc() : word(nullptr), start_loc(-1) { }
        explicit DictDoc(Node* node, long long loc) : word(node), start_loc(loc) { }

        bool operator==(const DictDoc& other) const {
            return word == other.word;
        }
    };

    RadixTree tree;
    StringHashFamily hash_family = StringHashFamily();
    CuckooHashingDictionary<std::string, DictWord> dict;

    DynamicArray<DictDoc> doc_arr;

    Node* last_document_node_added;
    std::string last_document_title_added;
    int last_docid_added;
public:
    ~DictionaryWithTrieAndDocId() override = default;
    DictionaryWithTrieAndDocId(int n, IHash* hash_function);
    void add(std::string word, Doc document) override;
    void add_document(Doc document) override;
    int get_num_docs() override;
    std::vector<Doc> get(std::string word) override;
    std::vector<Doc> prefix_search(std::string word);
    int words_added = 0;
    int documents_added = 0;
    void print_spaced_used();
};
