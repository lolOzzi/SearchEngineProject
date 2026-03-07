#include <memory>
#include "../../core/interfaces.h"
#include "../../extras/basic/RadixTree.h"
#include "../../extras/basic/ChainSet.h"

namespace fuzzy {
    class FuzzyDictWord {
    public:
        Node* word;
        std::vector<Node*> documents_in;
        std::vector<FuzzyDictWord*> originals;


        FuzzyDictWord(){
            this->word = nullptr;
            documents_in = std::vector<Node*>{};
            originals = std::vector<FuzzyDictWord*>{};
        }
        explicit FuzzyDictWord(Node* node) {
            this->word = node;
            documents_in = std::vector<Node*>{};
            originals = std::vector<FuzzyDictWord*>{};
        }
    };


    class StringCollisionFree {
    private:
        int n;
        int size;
        int arr_size;
        SimpleFingerprint hasher;
        RadixTree* tree;
        FuzzyDictWord** arr;
    public:
        ~StringCollisionFree() = default;
        explicit StringCollisionFree(int size, RadixTree* tree);

        void new_SimpleFingerprinthash();
        FuzzyDictWord* add(std::string& word);
        FuzzyDictWord* get(std::string &word);
        void DoNew(FuzzyDictWord *new_word, int old_arr_size);
    };

    class FuzzyDynamicFKS : public IStore {
    private:
        int numBuckets;
        StringCollisionFree** buckets;
        IHash* hash_function;
        RadixTree doc_holder;
        FuzzyDictWord* get_word(std::string& word);
        Node* last_document_node_added;
        std::string last_document_title_added;
        FuzzyDictWord* add_word(std::string word);
        FuzzyDictWord* add_word_with_last_doc(std::string word);
        std::vector<Doc> get_single(std::string word);
        vector<string> create_variants(string str, int max_distance);


    public:
        ~FuzzyDynamicFKS() override = default;
        FuzzyDynamicFKS(int n, IHash* hash_function);
        void add(std::string word, Doc document) override;
        void add_document(Doc document) override;
        std::vector<Doc> get(std::string word) override;
        int get_num_docs() override;
        int words_added = 0;
        int documents_added = 0;
    };

    FuzzyDynamicFKS::FuzzyDynamicFKS(int n, IHash* hash_function) {
        numBuckets = n;
        this->hash_function = hash_function;
        buckets = new StringCollisionFree*[numBuckets];
        doc_holder = RadixTree();
        for (int i = 0; i < numBuckets; i++) {
            buckets[i] = new StringCollisionFree(4, &doc_holder);
        }

    }

    FuzzyDictWord *FuzzyDynamicFKS::get_word(std::string& word) {
        std::uint64_t index = hash_function->hash(word, numBuckets);
        FuzzyDictWord* got_word = buckets[index]->get(word);
        return got_word;
    }

    void FuzzyDynamicFKS::add_document(Doc document) {
        last_document_node_added = doc_holder.add(document.title);
        last_document_title_added = document.title;
        documents_added++;
    }
    void FuzzyDynamicFKS::add(std::string word, Doc document) {
        if (word.empty()) return;

        FuzzyDictWord* fdw = add_word_with_last_doc(word);
        auto delVars = create_variants(word, 1);
        for (int i = 0; i < delVars.size(); ++i) {
            auto delFDW = add_word(delVars[i]);
            fdw = get_word(word);
            if (!delFDW || !delFDW->originals.empty() && delFDW->originals.back() == fdw) continue;
            delFDW->originals.push_back(fdw);
        }
    }

    FuzzyDictWord* FuzzyDynamicFKS::add_word_with_last_doc(std::string word) {

        if (word == "") return nullptr;
        std::uint64_t index = hash_function->hash(word, numBuckets);
        FuzzyDictWord* word_in_document = buckets[index]->get(word);

        if (!word_in_document || word_in_document->word == nullptr) {
            words_added++;
            buckets[index]->add(word);
            word_in_document = buckets[index]->get(word);
            assert(word_in_document->word != nullptr);
            word_in_document->documents_in.push_back(last_document_node_added);
            return word_in_document;
        }

        if (!word_in_document->documents_in.empty()) {
            if (word_in_document->documents_in[word_in_document->documents_in.size() - 1] == last_document_node_added) {
                return word_in_document;
            }
        }
        word_in_document->documents_in.push_back(last_document_node_added);
        return word_in_document;
    }
    FuzzyDictWord* FuzzyDynamicFKS::add_word(std::string word) {

        if (word == "") return nullptr;
        std::uint64_t index = hash_function->hash(word, numBuckets);
        FuzzyDictWord* word_in_document = buckets[index]->get(word);

        if (!word_in_document || word_in_document->word == nullptr) {
            words_added++;
            buckets[index]->add(word);
            word_in_document = buckets[index]->get(word);;
        }
        return word_in_document;
    }


    std::vector<Doc> FuzzyDynamicFKS::get_single(std::string word) {
        auto index = hash_function->hash(word, numBuckets);
        FuzzyDictWord* res = buckets[index]->get(word);
        if (res == nullptr) return std::vector<Doc>{};

        std::vector<Doc> temp = std::vector<Doc>{};

        for (Node* node : res->documents_in) {
            Doc new_doc;
            new_doc.title = get_string_from_node(node);
            temp.push_back(new_doc);
        }

        return temp;
    }

    std::vector<Doc> FuzzyDynamicFKS::get(std::string word) {
        vector<Doc> res;
        SimpleSet<Doc> res_set{ hash_function, 500 };
        auto fdw = get_word(word);
        if (fdw) {
            res_set.insert_all(get_single(word));
            for (FuzzyDictWord* delOrig : fdw->originals) {
                res_set.insert_all(get_single(get_string_from_node(delOrig->word)));
            }
        }


        // handle deletion variants
        const auto delVars = create_variants(word, 1);
        for (const std::string &var : delVars) {
            std::string tmp = var;
            FuzzyDictWord* wordDel = get_word(tmp);
            if (!wordDel) continue;
            for (FuzzyDictWord* delOrig : wordDel->originals) {
                res_set.insert_all(get_single(get_string_from_node(delOrig->word)));
            }
        }
        res_set.copy_elements_to_vector(res);
        return res;
    }


    StringCollisionFree::StringCollisionFree(int size, RadixTree* tree) {
        this->n = 0;
        this->size = size;
        arr_size = size*size;
        this->arr = new FuzzyDictWord*[arr_size];
        this->tree = tree;
        for (int i = 0; i < arr_size; i++) {
            arr[i] = nullptr;
        }
        hasher = SimpleFingerprint();
        new_SimpleFingerprinthash();
    }

    void StringCollisionFree::new_SimpleFingerprinthash() {
        hasher.init();
    }

    void StringCollisionFree::DoNew(FuzzyDictWord* new_word, int old_arr_size) {
        bool did_new_success = false;
        FuzzyDictWord** new_arr = new FuzzyDictWord*[arr_size];

        while (!did_new_success) {
            for (int i = 0; i < arr_size; i++) {
                new_arr[i] = nullptr;
            }
            new_SimpleFingerprinthash();
            bool continue_after = false;
            for (int i = 0; i < old_arr_size; i++) {
                if (arr[i] == nullptr) continue;
                int new_index = hasher.hash(get_string_from_node(arr[i]->word), arr_size);
                assert(new_index >= 0 && new_index < arr_size);

                if (new_arr[new_index] != nullptr) {
                    continue_after = true;
                    break;
                }
                new_arr[new_index] = arr[i];
            }
            if (continue_after) {
                continue;
            }
            int new_index = hasher.hash(get_string_from_node(new_word->word), arr_size);
            if (new_arr[new_index] != nullptr) continue;

            assert(new_index >= 0 && new_index < arr_size);
            new_arr[new_index] = new_word;
            did_new_success = true;
            delete[] arr;
            arr = new_arr;
        }
    }

    FuzzyDictWord* StringCollisionFree::add(std::string& word) {
        n++;
        Node* node = tree->add(word);
        FuzzyDictWord* new_word = new FuzzyDictWord(node);
        if (n > size) {
            int old_size = arr_size;
            size = size*2;
            arr_size = size*size;
            DoNew(new_word, old_size);
            return get(word);
        }

        auto index = hasher.hash(word, arr_size);
        if (arr[index] != nullptr) {
            DoNew(new_word, arr_size);
            return get(word);
        }
        assert(index >= 0 && index < arr_size);
        arr[index] = new_word;
        return arr[index];
    }

    FuzzyDictWord* StringCollisionFree::get(std::string& word) {
        auto index = hasher.hash(word, arr_size);

        if (arr[index] != nullptr && get_string_from_node(arr[index]->word) == word)
            return arr[index];
        return nullptr;
    }
    int FuzzyDynamicFKS::get_num_docs() {
        return documents_added;
    }
    vector<string> FuzzyDynamicFKS::create_variants(string str, int max_distance) {
        vector<string> variants{};
        if (max_distance >= 1) {
            for (int i = 0; i < str.length(); ++i) {
                string new_str = str;
                variants.push_back(new_str.erase(i, 1));
            }
        }
        if (max_distance >= 2) {
            for (int i = 0; i < str.length(); ++i) {
                for (int j = i+1; j < str.length(); ++j) {
                    string new_str = str;
                    new_str.erase(i, 1);
                    new_str.erase(j-1, 1);
                    variants.push_back(new_str);
                }
            }
        }
        if (max_distance >= 3) {
            for (int i = 0; i < str.length(); ++i) {
                for (int j = i+1; j < str.length(); ++j) {
                    for (int k = j+1; k < str.length(); ++k) {
                        string new_str = str;
                        new_str.erase(i, 1);
                        new_str.erase(j-1, 1);
                        new_str.erase(k-2, 1);
                        variants.push_back(new_str);
                    }
                }
            }
        }
        return variants;
    }
}