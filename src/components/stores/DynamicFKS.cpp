#include <memory>
#include "../../core/interfaces.h"


class DictWord {
public:
    std::string word;
    std::vector<DocId> documents_in;

    DictWord(){
        this->word = "";
        documents_in = std::vector<DocId>{};
    }
    explicit DictWord(const std::string &word) {
        this->word = word;
        documents_in = std::vector<DocId>{};
    }
};

class DocHolder {
private:
    Doc* arr;
    int n;
public:
    DocHolder() {
        arr = new Doc[100'000];
        n = 0;
    }
    ~DocHolder() {
        delete[] arr;
    }
    void AddDoc(Doc doc) {
        assert(n < 100'000);
        arr[n] = doc;
        n++;
    }
    DocId GetDocId(Doc* doc) {
        for (int i = n-1; i >= 0; i--) {
            if (arr[i].title == doc->title) {
                return {i};
            }
        }
        assert(false);
    }
    Doc* GetDoc(DocId id) {
        return &arr[id.id];
    }

    std::vector<Doc> GetDocVector(std::vector<DocId>* ids) {

        std::vector<Doc> return_vector = std::vector<Doc>{};
        for(DocId id : *ids) {
            return_vector.push_back(arr[id.id]);
        }
        return return_vector;
    }
};

class CollisionFree {
private:
    int n;
    int size;
    int arr_size;
    DictWord* arr;
    SimpleFingerprint hasher;
public:
    ~CollisionFree() = default;
    CollisionFree();
    explicit CollisionFree(int size);
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
    DocHolder doc_holder;
    DictWord* get_word(std::string& word);
public:
    ~DynamicFKS() override = default;
    DynamicFKS(int n, IHash* hash_function);
    void add(std::string word, Doc document) override;
    void add_document(Doc document) override;
    std::vector<Doc> get(std::string word) override;
};

DynamicFKS::DynamicFKS(int n, IHash* hash_function) {
    numBuckets = n;
    this->hash_function = hash_function;
    buckets = new CollisionFree*[numBuckets];

    for (int i = 0; i < numBuckets; i++) {
        buckets[i] = new CollisionFree(4);
    }
}

DictWord *DynamicFKS::get_word(std::string& word) {
    std::uint64_t index = hash_function->hash(word, numBuckets);
    DictWord* got_word = buckets[index]->get(word);
    return got_word;
}

void DynamicFKS::add_document(Doc document) {
    doc_holder.AddDoc(document);
}


void DynamicFKS::add(std::string word, Doc document) {

    if (word == "") return;
    std::uint64_t index = hash_function->hash(word, numBuckets);
    DictWord* word_in_document = buckets[index]->get(word);
    DocId id = doc_holder.GetDocId(&document);

    if (!word_in_document || word_in_document->word == "") {
        buckets[index]->add(word);
        word_in_document = buckets[index]->get(word);
        word_in_document->documents_in.push_back(id);
        return;
    }

    if (!word_in_document->documents_in.empty()) {
        if (word_in_document->documents_in[word_in_document->documents_in.size() - 1].id == id.id) {
            return;
        }
    }

    word_in_document->documents_in.push_back(id);
}

std::vector<Doc> DynamicFKS::get(std::string word) {
    auto index = hash_function->hash(word, numBuckets);
    DictWord* res = buckets[index]->get(word);
    if (res == nullptr) return std::vector<Doc>{};
    return doc_holder.GetDocVector(&res->documents_in);
}

CollisionFree::CollisionFree() {
    this->n = 0;
    this->size = 4;
    arr_size = size*size;
    this->arr = new DictWord[arr_size];
    for (int i = 0; i < arr_size; i++) {
        arr[i] = DictWord();
    }
    hasher = SimpleFingerprint();
    new_hash();
}

CollisionFree::CollisionFree(int size) {
    this->n = 0;
    this->size = size;
    arr_size = size*size;
    this->arr = new DictWord[arr_size];
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
            if (arr[i].word == "") continue;
            int new_index = hasher.hash(arr[i].word, arr_size);
            assert(new_index >= 0 && new_index < arr_size);

            if (new_arr[new_index].word != "") {
                continue_after = true;
                break;
            }
            new_arr[new_index] = arr[i];
        }
        if (continue_after) {
            continue;
        }
        int new_index = hasher.hash(new_word->word, arr_size);
        if (new_arr[new_index].word != "") continue;

        assert(new_index >= 0 && new_index < arr_size);
        new_arr[new_index] = *new_word;
        did_new_success = true;
        delete[] arr;
        arr = new_arr;
    }
}

DictWord* CollisionFree::add(std::string& word) {
    n++;
    DictWord new_word = DictWord(word);
    if (n > size) {
        int old_size = arr_size;
        size = size*2;
        arr_size = size*size;
        DoNew(&new_word, old_size);
        return get(word);
    }

    auto index = hasher.hash(word, arr_size);
    if (arr[index].word != "") {
        DoNew(&new_word, arr_size);
        return get(word);
    }
    assert(index >= 0 && index < arr_size);
    arr[index] = new_word;
    return &arr[index];
}

DictWord* CollisionFree::get(std::string& word) {
    auto index = hasher.hash(word, arr_size);
    if (arr[index].word == word)
        return &arr[index];
    return nullptr;
}