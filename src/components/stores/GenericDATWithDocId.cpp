#include <memory>
#include "../../core/interfaces.h"
#include "../../extras/basic/DoubleArrayDSTreeWithInfo.h"
#include "../../extras/basic/DynamicArray.h"

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

GenericDATWithDocId::GenericDATWithDocId(int n) :
    tree(n),
    doc_arr(128)
{
}

int GenericDATWithDocId::get_num_docs() {
    return words_added;
}

void GenericDATWithDocId::add_document(Doc document) {
    tree.add(document.title);
    DictDoc dict_doc = DictDoc(document.title, document.start_loc);
    doc_arr.add(dict_doc);
    last_docid_added = doc_arr.n - 1;
    documents_added++;
}

void GenericDATWithDocId::add(std::string word, Doc document) {
    if (word == "") return;

    int edge_index = tree.search(word);

    if (edge_index == -1) {
        words_added++;
        edge_index = tree.add(word);
        tree.documents_in[edge_index].add(last_docid_added);
        return;
    }

    if (tree.documents_in[edge_index].n != 0) {
        if (tree.documents_in[edge_index][tree.documents_in[edge_index].n-1] == last_docid_added) {
            return;
        }
    }
    tree.documents_in[edge_index].add(last_docid_added);
}

std::vector<Doc> GenericDATWithDocId::get(std::string word) {
    int edge_index = tree.search(word);
    if (edge_index == -1) return std::vector<Doc>{};

    std::vector<Doc> temp = std::vector<Doc>{};
    DynamicArray<int> docs_in = tree.documents_in[edge_index];

    for (int i = 0; i < docs_in.n; i++) {
        Doc new_doc;
        new_doc.title = doc_arr[docs_in[i]].word;
        new_doc.start_loc = doc_arr[docs_in[i]].start_loc;
        temp.push_back(new_doc);
    }

    return temp;
}