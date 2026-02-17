#include <memory>
#include "../../core/interfaces.h"
#include "./RadixTree.cpp"


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
};

class CollisionFree {
private:
    int n;
    int size;
    int arr_size;
    DictWord* arr;
    SimpleFingerprint hasher;
    RadixTree* tree;
public:
    ~CollisionFree() = default;
    explicit CollisionFree(int size, RadixTree* tree);
    void new_hash();
    DictWord* add(std::string& word);
    DictWord* get(std::string &word);
    void DoNew(DictWord *new_word, int old_arr_size);
};

class DynamicFKS : public IStore {
private:
    int numBuckets;
    CollisionFree** buckets;
    IHash* hash_function;
    RadixTree doc_holder;
    DictWord* get_word(std::string& word);
    Node* last_document_node_added;
    std::string last_document_title_added;

public:
    ~DynamicFKS() override = default;
    DynamicFKS(int n, IHash* hash_function);
    void add(std::string word, Doc document) override;
    void add_document(Doc document) override;
    std::vector<Doc> get(std::string word) override;
    int words_added = 0;
    int documents_added = 0;
};

DynamicFKS::DynamicFKS(int n, IHash* hash_function) {
    numBuckets = n;
    this->hash_function = hash_function;
    buckets = new CollisionFree*[numBuckets];
    doc_holder = RadixTree();
    for (int i = 0; i < numBuckets; i++) {
        buckets[i] = new CollisionFree(4, &doc_holder);
    }

}

DictWord *DynamicFKS::get_word(std::string& word) {
    std::uint64_t index = hash_function->hash(word, numBuckets);
    DictWord* got_word = buckets[index]->get(word);
    return got_word;
}

void DynamicFKS::add_document(Doc document) {
    last_document_node_added = doc_holder.add(document.title);
    last_document_title_added = document.title;
    documents_added++;
}


void DynamicFKS::add(std::string word, Doc document) {

    if (word == "") return;
    std::uint64_t index = hash_function->hash(word, numBuckets);
    DictWord* word_in_document = buckets[index]->get(word);

    if (!word_in_document || word_in_document->word == nullptr) {
        words_added++;
        buckets[index]->add(word);
        word_in_document = buckets[index]->get(word);
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

std::vector<Doc> DynamicFKS::get(std::string word) {
    auto index = hash_function->hash(word, numBuckets);
    DictWord* res = buckets[index]->get(word);
    if (res == nullptr) return std::vector<Doc>{};

    std::vector<Doc> temp = std::vector<Doc>{};

    for (Node* node : res->documents_in) {
        Doc new_doc = Doc(get_string_from_node(node));
        temp.push_back(new_doc);
    }

    return temp;
}

CollisionFree::CollisionFree(int size, RadixTree* tree) {
    this->n = 0;
    this->size = size;
    arr_size = size*size;
    this->arr = new DictWord[arr_size];
    this->tree = tree;
    for (int i = 0; i < arr_size; i++) {
        arr[i] = DictWord();
    }
    hasher = SimpleFingerprint();
    new_hash();
}

void CollisionFree::new_hash() {
    hasher.init();
}

void CollisionFree::DoNew(DictWord* new_word, int old_arr_size) {
    bool did_new_success = false;
    DictWord* new_arr = new DictWord[arr_size];

    while (!did_new_success) {
        for (int i = 0; i < arr_size; i++) {
            new_arr[i] = DictWord();
        }
        new_hash();
        bool continue_after = false;
        for (int i = 0; i < old_arr_size; i++) {
            if (arr[i].word == nullptr) continue;
            int new_index = hasher.hash(get_string_from_node(arr[i].word), arr_size);
            assert(new_index >= 0 && new_index < arr_size);

            if (new_arr[new_index].word != nullptr) {
                continue_after = true;
                break;
            }
            new_arr[new_index] = arr[i];
        }
        if (continue_after) {
            continue;
        }
        int new_index = hasher.hash(get_string_from_node(new_word->word), arr_size);
        if (new_arr[new_index].word != nullptr) continue;

        assert(new_index >= 0 && new_index < arr_size);
        new_arr[new_index] = *new_word;
        did_new_success = true;
        delete[] arr;
        arr = new_arr;
    }
}

DictWord* CollisionFree::add(std::string& word) {
    n++;
    Node* node = tree->add(word);
    DictWord new_word = DictWord(node);
    if (n > size) {
        int old_size = arr_size;
        size = size*2;
        arr_size = size*size;
        DoNew(&new_word, old_size);
        return get(word);
    }

    auto index = hasher.hash(word, arr_size);
    if (arr[index].word != nullptr) {
        DoNew(&new_word, arr_size);
        return get(word);
    }
    assert(index >= 0 && index < arr_size);
    arr[index] = new_word;
    return &arr[index];
}

DictWord* CollisionFree::get(std::string& word) {
    auto index = hasher.hash(word, arr_size);
    if (arr[index].word != nullptr && get_string_from_node(arr[index].word) == word)
        return &arr[index];
    return nullptr;
}