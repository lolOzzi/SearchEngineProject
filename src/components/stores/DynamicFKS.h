#pragma once
#include <memory>
#include "../../core/interfaces.h"
#include <cassert>

#include "SimpleFingerprint.h"

namespace DynamicFKSNS {

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
        arr = new Doc[5'000'000];
        n = 0;
    }
    ~DocHolder() {
        delete[] arr;
    }
    void AddDoc(Doc doc) {
        assert(n < 5'000'000);
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
    int get_num_docs() {
        return n+1;
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
    int get_num_docs() override;
    std::vector<Doc> get(std::string word) override;
};

}
