#include <memory>
#include "../../core/interfaces.h"
#include "../../extras/basic/DoubleArrayDSTreeWithInfo.h"
#include "../../extras/basic/DynamicArray.h"
#include "../../extras/basic/bursttrie/BurstTrie.h"

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

class BurstTrieStore : public IStore {
private:
    BurstTrie<TreeWord> tree;
    DynamicArray<DictDoc> doc_arr;
    int last_document_edge_added;
    std::string last_document_title_added;
    int last_docid_added;
public:
    ~BurstTrieStore() override = default;
    BurstTrieStore();
    void add(std::string word, Doc document) override;
    void add_document(Doc document) override;
    int get_num_docs() override;
    std::vector<Doc> get(std::string word) override;
    int words_added = 0;
    int documents_added = 0;
};

BurstTrieStore::BurstTrieStore() :
    tree(),
    doc_arr(128)
{
}

int BurstTrieStore::get_num_docs() {
    return words_added;
}

void BurstTrieStore::add_document(Doc document) {
    TreeWord rec = TreeWord();
    tree.add(document.title, rec);
    DictDoc dict_doc = DictDoc(document.title, document.start_loc);
    doc_arr.add(dict_doc);
    last_docid_added = doc_arr.n - 1;
    documents_added++;
}

void BurstTrieStore::add(std::string word, Doc document) {
    if (word == "") return;
    TreeWord* word_in_document = tree.get(word);

    if (word_in_document == nullptr) {
        words_added++;
        word_in_document = tree.add(word, TreeWord());
        assert(word_in_document != nullptr);
        word_in_document->documents_in.add(last_docid_added);
        return;
    }

    if (word_in_document->documents_in.n != 0) {
        if (word_in_document->documents_in[word_in_document->documents_in.n - 1] == last_docid_added) {
            return;
        }
    }
    word_in_document->documents_in.add(last_docid_added);
}

std::vector<Doc> BurstTrieStore::get(std::string word) {
    TreeWord* res = tree.get(word);
    if (res == nullptr) return std::vector<Doc>{};
    std::vector<Doc> temp = std::vector<Doc>{};
    for (int i = 0; i < res->documents_in.n; i++) {
        Doc new_doc;
        new_doc.title = (doc_arr[res->documents_in[i]].word);
        new_doc.start_loc = doc_arr[res->documents_in[i]].start_loc;
        temp.push_back(new_doc);
    }
    return temp;
}