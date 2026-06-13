#pragma once

#include "../core/interfaces.h"
#include "../components/hashers/SimpleFingerprint.h"
#include "../components/preprocessors/BasicPreprocessorWordCleaner.h"
#include "../components/stores/BurstTrieDeltaDynamicStoreRegex.hpp"
#include "../components/searchers/RegexSearch.h"
#include "../components/rankers/TFIDFRanker.h"
#include "../components/sorters/RadixSort.h"

class Index8 {
private:
    RegexSearch searcher;
    TFIDFRANKER ranker;
    SimpleFingerprint hasher;
    BasicPreprocessorWordCleaner preprocessor;
    BurstTrieDeltaDynamicStoreRegexNS::BurstTrieDeltaDynamicStoreRegex store;
    RadixSort sort;

    std::string filename;

    std::vector<Doc> lastSearchResults;
    SearchQuery lastSearchQuery;

public:
    Index index;
    Index8() : index(&store, &preprocessor, &hasher, &searcher, &ranker, &sort) {}

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
