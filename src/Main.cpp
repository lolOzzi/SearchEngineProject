#include "core/Index5.h"
#include "components/test/test.cpp"
#include "components/preprocessors/BasicPreprocessor.cpp"
#include "components/preprocessors/BasicPreprocessorHandlesPunctuation.cpp"
#include "components/preprocessors/BasicPreprocessorHandlesWhitespaceChars.cpp"

#include "components/hashers/SimpleFingerprint.cpp"
#include "components/searchers/BasicSearcher.cpp"
#include "components/searchers/BooleanSearcher.cpp"
//#include "components/stores/DynamicFKSRadixTree.cpp"
//#include "components/stores/TreeWrapper.cpp"
#include <chrono>

#include "components/rankers/MostMatchesRanker.cpp"
#include "components/rankers/TFIDFRanker.cpp"
#include "components/sorters/MergeSort.cpp"
#include "components/stores/FuzzyTreeWrapper.cpp"

/*
*/

int main(int argc, char* argv[]) {
    printf("main \n");

    BasicPreprocessorHandlesWhitespaceChars preprocessor;
    SimpleFingerprint hasher;
    //BooleanSearcher searcher;
    BasicSearcher searcher;
    FuzzyTreeWrapper store = FuzzyTreeWrapper(&hasher);
    TFIDFRANKER TFIDFRanker;

    MergeSort sorter;
    Index index = Index(&store, &preprocessor, &hasher, &searcher, &TFIDFRanker, &sorter);

    printf("Started preprocessing \n");
    auto t0 = chrono::steady_clock::now();
    std::string filename = "data/WestburyLab.wikicorp.201004_100MB.txt"; // 348283157567 ns, fuzzy
    //std::string filename = "data/WestburyLab.wikicorp.201004.txt";
    index.preprocess(filename);
    auto t1 = chrono::steady_clock::now();
    auto elapsed = duration_cast<chrono::nanoseconds>(t1 - t0).count();
    std::cout << " elapsed=" << elapsed << " ns\n";
    printf("Finished preprocessing \n");

    //test(&index);
    printf("Start searching \n");
    SearchQuery q;
    q.q = "albado";
    std::vector<Doc> res = index.search(q);
    printf("Finished searching \n");
    for (std::vector<Doc>::iterator it = res.begin(); it != res.end(); ++it) {
        std::cout << it->title << " "  << endl;
    }


    return 0;
}
