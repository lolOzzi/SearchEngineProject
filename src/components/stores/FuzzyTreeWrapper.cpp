#include <string>
#include <vector>
#include "../../core/interfaces.h"
#include <cmath>
#include "./RadixTreeWithPostings.cpp"
// MAJORLY WORK IN PROGRESS!!!



class FuzzyTreeWrapper : public IStore {
private:
    RadixTreeWithPostings tree;

    std::vector<Node*> doc_nodes;
    IHash* hash_function;

    uint32_t next_doc_id = 0;
    uint32_t last_doc_id = UINT32_MAX;
    uint32_t debug_counter = 0;

public:
    FuzzyTreeWrapper(IHash* hasher) : tree() {hash_function = hasher;}
    ~FuzzyTreeWrapper() override = default;

    void add_document(Doc document) override {
        Node* docNode = tree.add(document.title);
        if (debug_counter == 10000 ) {
            std::cout << next_doc_id << "\n";
            debug_counter = 0;
        }

        if (docNode->doc_id == UINT32_MAX) {
            uint32_t id = next_doc_id++;
            debug_counter++;
            doc_nodes.push_back(docNode);
            docNode->doc_id = id;
            last_doc_id = id;
        } else {
            last_doc_id = docNode->doc_id;
        }
    }

    void add(std::string word, Doc) override {
        if (word.empty()) return;
        if (last_doc_id == UINT32_MAX) return;

        Node* wordNode = tree.add(word);
        auto &plist = wordNode->postings;
        if (plist.empty() || plist.back() != last_doc_id) {
            plist.push_back(last_doc_id);
        }
        auto delVars = create_variants(word, 1);
        for (int i = 0; i < delVars.size(); ++i) {
            auto delNode = tree.add(delVars[i]);
            if (!delNode->original.empty() && delNode->original.back() == wordNode) continue;
            delNode->original.push_back(wordNode);
        }
    }
    
    void insert_postings_into_set(const std::vector<uint32_t>& plist, SimpleSet<Doc>& res_set) const {
        for (uint32_t docid : plist) {
            if (docid < doc_nodes.size()) {
                res_set.insert({ get_string_from_node(doc_nodes[docid]) });
            }
        }
    }

    std::vector<Doc> get(const std::string word) override {
        std::vector<Doc> res;
        SimpleSet<Doc> res_set{ hash_function, 500 };

        if (word.empty()) return res;

        // handle exact-word node
        Node* wordNode = tree.find(word);
        if (wordNode) {
            insert_postings_into_set(wordNode->postings, res_set);

            for (Node* delOrig : wordNode->original) {
                insert_postings_into_set(delOrig->postings, res_set);
            }
        }

        // handle deletion variants
        const auto delVars = create_variants(word, 1);
        for (const auto& var : delVars) {
            Node* wordNodeDel = tree.find(var);
            if (!wordNodeDel) continue;

            // original behavior only inserts postings of the 'original' nodes
            for (Node* delOrig : wordNodeDel->original) {
                insert_postings_into_set(delOrig->postings, res_set);
            }
        }

        res_set.copy_elements_to_vector(res);
        return res;
    }
    int funny() {return 3;};

    int get_num_docs() override {
        return static_cast<int>(doc_nodes.size());
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