#include "../../core/interfaces.h"
#include <fstream>
#include <istream>
#include <iostream>
#include <cmath>
class TFIDFRANKER : public IRanker {
public:
    ~TFIDFRANKER() = default;
    std::vector<ScoredDoc> rank(const std::vector<Doc>& candidates, std::string& query, IStore* store, std::string& filename, ISorter* sorter) override;
};

static int tf_idf(std::string& query, int query_count_in_doc, int words_in_doc, int num_docs, int num_docs_containing_query) {
  double tf = query_count_in_doc /(double) words_in_doc;
  double idf = log(num_docs /(double) num_docs_containing_query);
  std::cout << tf*idf;
  return tf*idf*100;
}

std::vector<ScoredDoc> TFIDFRANKER::rank(const std::vector<Doc>& candidates,
                                        std::string& query, IStore* store,
                                        std::string& filename, ISorter* sorter) {
  	int doc_count = getDocumentCountFromFile(filename);
    std::vector<ScoredDoc> scoredDocs(candidates.size());
    std::ifstream file;
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
        int query_counter = 0;
        int word_counter = 0;
        while (std::getline(file, line)) {
            std::vector<std::string> words;
            getWordsFromLine(line, words);
            for (int wi = 0; wi < words.size(); ++wi) {
                word = words[wi];
                char last_char = word.back();
                if (last_char == ',' || last_char == '.' || last_char == '?') {
                    word.erase(word.size() - 1);
                }
                if (word == query) query_counter++;
                word_counter++;
            }
            if (word == END) break;
        }

        scoredDocs[i].title = candidates[i].title;
        scoredDocs[i].start_loc = candidates[i].start_loc;
        scoredDocs[i].score = tf_idf(query, query_counter, word_counter, doc_count, candidates.size());
    }
    file.close();
    sorter->sort(scoredDocs);
    return scoredDocs;
}

