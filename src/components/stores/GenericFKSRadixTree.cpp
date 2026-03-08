#include <memory>
#include "../../core/interfaces.h"
#include "../../extras/basic/RadixTree.h"
#include "../../extras/basic/GenericFKSDictionary.h"

class DictWord {
public:
    Node* word;
    std::vector<Node*> documents_in;

    DictWord(){
        this->word = nullptr;
        documents_in = std::vector<Node*>{};
    }
    explicit DictWord(Node* node) {
        this->word = node;
        documents_in = std::vector<Node*>{};
    }

    bool operator==(const DictWord& other) const {
        return word == other.word;
    }
};


class GenericFKSRadixTree : public IStore {
private:
    RadixTree tree;
    DictWord* get_word(std::string& word);
    Node* last_document_node_added;
    std::string last_document_title_added;
    StringHashFamily hash_family = StringHashFamily();
    GenericFKSDictionary<std::string, DictWord> dict;
public:
    ~GenericFKSRadixTree() override = default;
    GenericFKSRadixTree(int n, IHash* hash_function);
    void add(std::string word, Doc document) override;
    void add_document(Doc document) override;
    int get_num_docs() override;
    std::vector<Doc> get(std::string word) override;
    int words_added = 0;
    int documents_added = 0;
};

GenericFKSRadixTree::GenericFKSRadixTree(int n, IHash* hash_function) :
    tree(),
    dict(n, &hash_family)
{
}
int GenericFKSRadixTree::get_num_docs() {
    return -1;
}


DictWord *GenericFKSRadixTree::get_word(std::string& word) {
    DictWord* got_word = dict.get(word);
    return got_word;
}

void GenericFKSRadixTree::add_document(Doc document) {
    last_document_node_added = tree.add(document.title);
    last_document_title_added = document.title;
    documents_added++;
}

void GenericFKSRadixTree::add(std::string word, Doc document) {
    if (word == "") return;

    DictWord* word_in_document = dict.get(word);

    if (!word_in_document || word_in_document->word == nullptr) {
        words_added++;

        Node* node = tree.add(word);
        DictWord new_word = DictWord(node);

        word_in_document = dict.add(word, new_word);
        assert(word_in_document->word != nullptr);
        word_in_document->documents_in.push_back(last_document_node_added);
        return;
    }

    if (!word_in_document->documents_in.empty()) {
        if (word_in_document->documents_in[word_in_document->documents_in.size() - 1] == last_document_node_added) {
            return;
        }
    }

    word_in_document->documents_in.push_back(last_document_node_added);
}

std::vector<Doc> GenericFKSRadixTree::get(std::string word) {
    DictWord* res = dict.get(word);
    if (res == nullptr) return std::vector<Doc>{};

    std::vector<Doc> temp = std::vector<Doc>{};

    for (Node* node : res->documents_in) {
        Doc new_doc;
        new_doc.title = get_string_from_node(node);
        temp.push_back(new_doc);
    }

    return temp;
}
