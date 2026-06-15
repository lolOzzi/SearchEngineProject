#pragma once
#include "extras/basic/DynamicArray.h"
#include <unordered_map>
#include <unordered_set>
#include <string>
#include "../../extras/basic/DeltaEncodedArray.h"



struct RegexStoreComp {
    unsigned int indexId = 0;
    DynamicArray<std::string> indexString{65536};

    std::unordered_map<std::string, DeltaEncodedArray> trigram;


    void trigramAdd(std::string word) {
        indexString.add(word);
        std::unordered_set<std::string> dupes;
        for (int i = 0; i < word.length() - 2; i++) {
            std::string tri = word.substr(i, 3);
            if (!dupes.contains(tri)) {
                trigram[tri].add(indexId);
                dupes.insert(tri);
            }
        }
        indexId++;
    }

    std::vector<std::string> getWordsFromTrigram(std::string tri) {

        if (!trigram.contains(tri)) {
            std::vector<std::string> emp;
            return emp;
        }
        std::vector<unsigned int> indexes;
        trigram[tri].copy_elements_to_vector(indexes);
        std::vector<std::string> res;
        for (int i : indexes) {
            res.emplace_back(indexString[i]);
        }
        return res;
    }

};