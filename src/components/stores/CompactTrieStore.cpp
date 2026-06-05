#include <memory>
#include "../../core/interfaces.h"
#include "../../extras/basic/DynamicArray.h"
#include "../../extras/basic/RadixTreeWithPostings.h"

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

class CompactTrieStore : public IStore {
private:
    RadixTreeWithPostingsNS::RadixTreeWithPostings tree;
    DynamicArray<DictDoc> doc_arr;
    int last_document_edge_added;
    std::string last_document_title_added;
    int last_docid_added;
public:
    ~CompactTrieStore() override = default;
    CompactTrieStore();
    void add(std::string word, Doc document) override;
    void add_document(Doc document) override;
    int get_num_docs() override;
    std::vector<Doc> get(std::string word) override;
    int words_added = 0;
    int documents_added = 0;
};

CompactTrieStore::CompactTrieStore() :
    tree(),
    doc_arr(128)
{
}

int CompactTrieStore::get_num_docs() {
    return words_added;
}

void CompactTrieStore::add_document(Doc document) {
    DictDoc dict_doc = DictDoc(document.title, document.start_loc);
    doc_arr.add(dict_doc);
    last_docid_added = doc_arr.n - 1;
    documents_added++;
}

void CompactTrieStore::add(std::string word, Doc document) {
    if (word == "") return;
    RadixTreeWithPostingsNS::Node* treeNode = tree.find(word);

    if (treeNode == nullptr) {
        words_added++;
        treeNode = tree.add(word);
        treeNode->postings.push_back(last_docid_added);
        return;
    }

    if (treeNode->postings.size() != 0) {
        if (treeNode->postings[treeNode->postings.size() - 1] == last_docid_added) {
            return;
        }
    }
    treeNode->postings.push_back(last_docid_added);
}

std::vector<Doc> CompactTrieStore::get(std::string word) {
    RadixTreeWithPostingsNS::Node* treeNode = tree.find(word);

    if (treeNode == nullptr)
        return std::vector<Doc>{};

    std::vector<Doc> temp = std::vector<Doc>{};
    for (int i = 0; i < treeNode->postings.size(); i++) {
        Doc new_doc;
        new_doc.title = (doc_arr[treeNode->postings[i]].word);
        new_doc.start_loc = doc_arr[treeNode->postings[i]].start_loc;
        temp.push_back(new_doc);
    }
    return temp;
}