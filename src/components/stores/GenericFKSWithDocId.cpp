#include <memory>
#include "../../core/interfaces.h"
#include "./RadixTree.cpp"
#include "../../extras/helpers/GenericFKSDictionary.cpp"
#include "../../extras/helpers/DynamicArray.cpp"

class DictWord {
public:
    Node* word;
    DynamicArray<int> documents_in;

    DictWord() : word(nullptr), documents_in(2) { }
    explicit DictWord(Node* node) : word(node), documents_in(2) { }

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



class GenericFKSWithDocId : public IStore {
private:
    RadixTree tree;
    StringHashFamily hash_family = StringHashFamily();
    GenericFKSDictionary<std::string, DictWord> dict;

    DynamicArray<DictDoc> doc_arr;

    Node* last_document_node_added;
    std::string last_document_title_added;
    int last_docid_added;
public:
    ~GenericFKSWithDocId() override = default;
    GenericFKSWithDocId(int n, IHash* hash_function);
    void add(std::string word, Doc document) override;
    void add_document(Doc document) override;
    int get_num_docs() override;
    std::vector<Doc> get(std::string word) override;
    int words_added = 0;
    int documents_added = 0;
};

GenericFKSWithDocId::GenericFKSWithDocId(int n, IHash* hash_function) :
    tree(),
    dict(n, &hash_family),
    doc_arr(128)
{
}
int GenericFKSWithDocId::get_num_docs() {
    return words_added;
}

void GenericFKSWithDocId::add_document(Doc document) {
    Node* node = tree.add(document.title);
    DictDoc dict_doc = DictDoc(node, document.start_loc);
    doc_arr.add(dict_doc);
    last_docid_added = doc_arr.n - 1;
    documents_added++;
}

void GenericFKSWithDocId::add(std::string word, Doc document) {
    if (word == "") return;

    DictWord* word_in_document = dict.get(word);

    if (!word_in_document || word_in_document->word == nullptr) {
        words_added++;

        Node* node = tree.add(word);
        DictWord new_word = DictWord(node);

        word_in_document = dict.add(word, new_word);
        assert(word_in_document->word != nullptr);
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

std::vector<Doc> GenericFKSWithDocId::get(std::string word) {
    DictWord* res = dict.get(word);
    if (res == nullptr) return std::vector<Doc>{};

    std::vector<Doc> temp = std::vector<Doc>{};

    for (int i = 0; i < res->documents_in.n; i++) {
        Doc new_doc;
        new_doc.title = get_string_from_node(doc_arr[res->documents_in[i]].word);
        new_doc.start_loc = doc_arr[res->documents_in[i]].start_loc;
        temp.push_back(new_doc);
    }

    return temp;
}
