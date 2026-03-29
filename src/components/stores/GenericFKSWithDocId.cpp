#include <memory>
#include "../../core/interfaces.h"
#include "../../extras/basic/ChaniedHashDictionary.h"
#include "../../extras/basic/RadixTree.h"
#include "../../extras/basic/DoubleArrayDSTree.h"
#include "../../extras/basic/GenericFKSDictionary.h"
#include "../../extras/basic/DynamicArray.h"
#include "../../extras/basic/CuckooHasingDictionary.h"

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



class GenericFKSWithDocId : public IStore {
private:
    RadixTree tree;
    StringHashFamily hash_family = StringHashFamily();
    ChainedHashDictionary<std::string, DictWord> dict;

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
    std::vector<Doc> prefix_search(std::string word);
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
    node->label.set_to_document();
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


static void prefix_search_from_node(Node* node, std::vector<Node*>* document_nodes) {
    for (auto& node_ptr : node->targets) {
        Node* loop_node = node_ptr.get();
        prefix_search_from_node(loop_node, document_nodes);

        if (loop_node->label.is_document()) {
            document_nodes->push_back(loop_node);
        }
    }
}

std::vector<Doc> GenericFKSWithDocId::prefix_search(std::string word) {
    Node* best = find_best_node(&tree.root, word);
    std::vector<Doc> temp = std::vector<Doc>{};
    std::vector<Node*> docs = std::vector<Node*>{};
    prefix_search_from_node(best, &docs);

    for (int i = 0; i < doc_arr.n; i++) {
        for (auto& node_ptr : docs) {
            if (doc_arr[i].word == node_ptr) {
                Doc new_doc;
                new_doc.title = get_string_from_node(doc_arr[i].word);
                new_doc.start_loc = doc_arr[i].start_loc;
                temp.push_back(new_doc);
            }
        }
    }
    return temp;
}
