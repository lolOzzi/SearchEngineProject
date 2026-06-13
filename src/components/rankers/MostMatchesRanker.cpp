#include "MostMatchesRanker.h"
#include <cstdio>
#include <fstream>
#include <istream>

std::vector<ScoredDoc> MostMatchesRanker::rank(const std::vector<Doc>& candidates,
                                        SearchQuery& query, IStore* store,
                                        std::string& filename, ISorter* sorter) {
    std::vector<ScoredDoc> scoredDocs(candidates.size());
    std::ifstream file;
    printf("Warning: Only single query support");
    file.open (filename);
    if (!file.good()) {
        printf("Error reading file");
        return scoredDocs;
    }
    if (!file.is_open()) {
        printf("No file is open");
        return scoredDocs;
    }
    for (int i = 0; i < candidates.size(); ++i) {
        file.seekg(candidates[i].start_loc, std::ios::beg);
        std::string word;
        std::string line;
        int counter = 0;
        while (std::getline(file, line)) {
            std::vector<std::string> words;
            getWordsFromLine(line, patterns, words);
            for (int wi = 0; wi < words.size(); ++wi) {
                word = words[wi];
                char last_char = word.back();
                if (last_char == ',' || last_char == '.' || last_char == '?') {
                    word.erase(word.size() - 1);
                }
                if (word == query.q) counter++;
            }
            if (word == END) break;
        }
        scoredDocs[i].title = candidates[i].title;
        scoredDocs[i].start_loc = candidates[i].start_loc;
        scoredDocs[i].score = counter;
    }
    file.close();
    sorter->sort(scoredDocs);
    return scoredDocs;
}
