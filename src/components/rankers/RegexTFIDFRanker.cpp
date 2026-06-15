#include "RegexTFIDFRanker.h"

static double regex_tf_idf(int matches_in_doc, int words_in_doc, int num_docs, int num_docs_with_match) {
    if (words_in_doc == 0 || num_docs_with_match == 0) return 0.0;
    double tf = matches_in_doc / (double) words_in_doc;
    double idf = log(num_docs / (double) num_docs_with_match);
    return tf * idf;
}

std::vector<ScoredDoc> RegexTFIDFRanker::rank(const std::vector<Doc>& candidates,
                                              SearchQuery& query, IStore* store,
                                              std::string& filename, ISorter* sorter) {
    int doc_count = store->get_num_docs();
    std::vector<ScoredDoc> scoredDocs(candidates.size());

    std::ifstream file;
    file.open(filename);
    if (!file.good()) {
        printf("Error reading file");
        return scoredDocs;
    }
    if (!file.is_open()) {
        printf("No file is open");
        return scoredDocs;
    }

    std::vector<State*> nodes;
    std::string pf = postfix(query.q);
    State* NFA = createNFA(pf, nodes);

    for (int i = 0; i < candidates.size(); ++i) {
        file.clear();
        file.seekg(candidates[i].start_loc, std::ios::beg);

        int match_counter = 0;
        int word_counter = 0;
        std::string line;
        bool reached_end = false;

        while (!reached_end && std::getline(file, line)) {
            std::vector<std::string> words;
            getWordsFromLine(line, patterns, words);
            for (int wi = 0; wi < words.size(); ++wi) {
                std::string word = words[wi];
                if (word == END) {
                    reached_end = true;
                    break;
                }
                char last_char = word.back();
                if (last_char == ',' || last_char == '.' || last_char == '?') {
                    word.erase(word.size() - 1);
                }

                std::vector<std::string> clean_words;
                cleanString(word, clean_words);
                for (const std::string& clean_word : clean_words) {
                    std::string token = removeUnwantedAndTrim(&clean_word);
                    if (token.empty()) continue;
                    word_counter++;
                    if (match(NFA, token, nodes)) {
                        match_counter++;
                    }
                }
            }
        }

        scoredDocs[i].title = candidates[i].title;
        scoredDocs[i].start_loc = candidates[i].start_loc;
        scoredDocs[i].score = regex_tf_idf(match_counter, word_counter, doc_count, candidates.size());
    }
    file.close();

    for (State* node : nodes) {
        delete node;
    }
    nodes.clear();

    sorter->sort(scoredDocs);
    return scoredDocs;
}
