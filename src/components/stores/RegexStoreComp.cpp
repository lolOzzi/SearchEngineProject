#include <unordered_map>
#include <string>



struct RegexStoreComp {
    int indexId = 0;
    DynamicArray<std::string> indexString{65536};

    std::unordered_map<std::string, std::vector<int>> trigram;


    void trigramAdd(std::string word) {
        indexString.add(word);
        for (int i = 0; i < word.length() - 2; i++) {
            std::string tri = word.substr(i, 3);
            trigram[tri].emplace_back(indexId);
        }
        indexId++;
    }

    std::vector<std::string> getWordsFromTrigram(std::string tri) {

        if (!trigram.contains(tri)) {
            std::vector<std::string> emp;
            return emp;
        }
        std::vector<int> indexes = trigram[tri];
        std::vector<std::string> res;
        for (int i : indexes) {
            res.emplace_back(indexString[i]);
        }
        return res;
    }

};

