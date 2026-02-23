#include "core/Index5.h"
#include "components/test/test.cpp"
#include "components/preprocessors/BasicPreprocessor.cpp"
#include "components/preprocessors/BasicPreprocessorHandlesPunctuation.cpp"
#include "components/preprocessors/BasicPreprocessorHandlesWhitespaceChars.cpp"

#include "components/hashers/SimpleFingerprint.cpp"
#include "components/searchers/BasicSearcher.cpp"
#include "components/searchers/BooleanSearcher.cpp"
#include "components/stores/DynamicFKSRadixTree.cpp"
#include "components/rankers/MostMatchesRanker.cpp"
#include "components/rankers/TFIDFRanker.cpp"
#include "components/sorters/MergeSort.cpp"

/*
*/

int main(int argc, char* argv[]) {
    printf("main \n");

    BasicPreprocessorHandlesWhitespaceChars preprocessor;
    SimpleFingerprint hasher;
    //BooleanSearcher searcher;
    BasicSearcher searcher;
    DynamicFKS store = DynamicFKS(300'000, &hasher);
    TFIDFRANKER TFIDFRanker;

    MergeSort sorter;
    Index index = Index(&store, &preprocessor, &hasher, &searcher, &TFIDFRanker, &sorter);

    printf("Started preprocessing \n");
    std::string filename = "data/WestburyLab.wikicorp.201004_10MB.txt";
    index.preprocess(filename);
    printf("Finished preprocessing \n");

    printf("Start searching \n");
    SearchQuery q;
    q.q = "albedo";
    std::vector<Doc> res = index.search(q);
    printf("Finished searching \n");
    for (std::vector<Doc>::iterator it = res.begin(); it != res.end(); ++it) {
        std::cout << it->title << " "  << endl;
    }


    return 0;
}
