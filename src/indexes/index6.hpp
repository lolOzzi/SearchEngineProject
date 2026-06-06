#include "../core/interfaces.h"
#include "../components/hashers/SimpleFingerprint.h"
#include "../components/preprocessors/BasicPreprocessorWordCleaner.cpp"
#include "../components/stores/DictionaryWithDocId.cpp"
#include "../components/searchers/BooleanSearcher.cpp"
#include "../components/rankers/TFIDFRanker.cpp"
#include "../components/sorters/MergeSort.cpp"

class Index6 {
private:
    BooleanSearcher searcher;
    TFIDFRANKER ranker;
    SimpleFingerprint hasher;
    BasicPreprocessorWordCleaner preprocessor;
    DictionaryWithDocId dict_store = DictionaryWithDocId(4);
    MergeSort sort;
    Index index;
    std::string filename;

    std::vector<Doc> lastSearchResults;
    SearchQuery lastSearchQuery;

public:
    Index6() : index(&dict_store, &preprocessor, &hasher, &searcher, &ranker, &sort) {}

    void preprocess(std::string fn) {
        filename = fn;
        index.preprocess(filename);
    }

    std::vector<Doc> search(SearchQuery q) {
        auto res = index.search(q);
        lastSearchQuery = q;
        lastSearchResults = res;
        return res;
    }

    std::vector<ScoredDoc> rank(SearchQuery q) {
        if (lastSearchQuery.q != q.q)
            lastSearchResults = index.search(q);

        return index.rank(lastSearchResults, q, filename);
    }
};
