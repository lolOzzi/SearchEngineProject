#include <string>
#include "../../core/interfaces.h"
#include <cmath>
#include "../../extras/basic/RadixTreeWithPostings.h"
// MAJORLY WORK IN PROGRESS!!!
using RadixTreeWithPostings = RadixTreeWithPostingsNS::RadixTreeWithPostings;
using RTPNode = RadixTreeWithPostingsNS::Node;



class FuzzyTreeWrapper : public IStore {
private:
    RadixTreeWithPostings tree;

    std::vector<RTPNode*> doc_RTPNodes;
    IHash* hash_function;

    uint32_t next_doc_id = 0;
    uint32_t last_doc_id = UINT32_MAX;
    uint32_t debug_counter = 0;

public:
    FuzzyTreeWrapper(IHash* hasher) : tree() {hash_function = hasher;}
    ~FuzzyTreeWrapper() override = default;

    void add_document(Doc document) override {
        RTPNode* docRTPNode = tree.add(document.title);
        if (debug_counter == 10000 ) {
            std::cout << next_doc_id << "\n";
            debug_counter = 0;
        }

        if (docRTPNode->doc_id == UINT32_MAX) {
            uint32_t id = next_doc_id++;
            debug_counter++;
            doc_RTPNodes.push_back(docRTPNode);
            docRTPNode->doc_id = id;
            last_doc_id = id;
        } else {
            last_doc_id = docRTPNode->doc_id;
        }
    }

    void add(std::string word, Doc) override {
        if (word.empty()) return;
        if (last_doc_id == UINT32_MAX) return;

        RTPNode* wordRTPNode = tree.add(word);
        auto &plist = wordRTPNode->postings;
        if (plist.empty() || plist.back() != last_doc_id) {
            plist.push_back(last_doc_id);
        }
        auto delVars = create_variants(word, 1);
        for (int i = 0; i < delVars.size(); ++i) {
            auto delRTPNode = tree.add(delVars[i]);
            if (!delRTPNode->original.empty() && delRTPNode->original.back() == wordRTPNode) continue;
            delRTPNode->original.push_back(wordRTPNode);
        }
    }
    
    void insert_postings_into_set(const std::vector<uint32_t>& plist, SimpleSet<Doc>& res_set) const {
        for (uint32_t docid : plist) {
            if (docid < doc_RTPNodes.size()) {
                res_set.insert({ get_string_from_node(doc_RTPNodes[docid]) });
            }
        }
    }

    std::vector<Doc> get(const std::string word) override {
        std::vector<Doc> res;
        SimpleSet<Doc> res_set{ hash_function, 500 };

        if (word.empty()) return res;

        // handle exact-word RTPNode
        RTPNode* wordRTPNode = tree.find(word);
        if (wordRTPNode) {
            insert_postings_into_set(wordRTPNode->postings, res_set);

            for (RTPNode* delOrig : wordRTPNode->original) {
                insert_postings_into_set(delOrig->postings, res_set);
            }
        }

        // handle deletion variants
        const auto delVars = create_variants(word, 1);
        for (const auto& var : delVars) {
            RTPNode* wordRTPNodeDel = tree.find(var);
            if (!wordRTPNodeDel) continue;

            for (RTPNode* delOrig : wordRTPNodeDel->original) {
                insert_postings_into_set(delOrig->postings, res_set);
            }
        }

        res_set.copy_elements_to_vector(res);
        return res;
    }
    int funny() {return 3;};

    int get_num_docs() override {
        return static_cast<int>(doc_RTPNodes.size());
    }


    int Levenshtein_distance(string s, string t) {
        int n = t.length();
        int m = s.length();
        vector<int> v0 (n+1);
        vector<int> v1 (n+1);

        for (int i = 0; i < n; ++i) {
            v0[i] = i;
        }
        for (int i = 0; i < m-1; ++i) {
            v1[0] = i +1;

            for (int j = 0; j < n-1; ++j) {
                int deletion_cost = v0[j+1] +1;
                int insertion_cost = v1[j] + 1;
                int substitution_cost = 0;
                if (s[i] == t[j]) {
                    substitution_cost = v0[j];
                }
                else {
                    substitution_cost = v0[j]+1;
                }
                v1[j+1] = min(min(deletion_cost, insertion_cost),substitution_cost);
            }
            swap(v0, v1);
        }
        return v0[n];
    }

    vector<string> create_variants(string str, int max_distance) {
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
};