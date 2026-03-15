#include "core/Index5.h"
#include "components/test/test.cpp"
#include "components/preprocessors/BasicPreprocessorWordCleaner.cpp"

#include "components/hashers/SimpleFingerprint.h"
#include "components/searchers/BasicSearcher.cpp"
#include "components/stores/GenericFKSWithDocId.cpp"
#include "components/stores/BasicHashTable.cpp"
//#include "components/stores/DynamicFKSRadixTree.cpp"
#include "components/searchers/BooleanSearcher.cpp"
#include "components/stores/FuzzyDynamicFKSRadixTree.cpp"
//#include "components/stores/TreeWrapper.cpp"


#include "components/rankers/MostMatchesRanker.cpp"
#include "components/rankers/TFIDFRanker.cpp"
#include "components/sorters/MergeSort.cpp"
#include "components/stores/FuzzyTreeWrapper.cpp"

/*
*/

int main(int argc, char* argv[]) {
    printf("main \n");

    BasicPreprocessorWordCleaner preprocessor;
    SimpleFingerprint hasher;
    BasicSearcher searcher;

    GenericFKSWithDocId store = GenericFKSWithDocId(300'000, &hasher);
    Index index = Index(&store, &preprocessor, &hasher, &searcher, nullptr, nullptr);

    BasicHashTable store2 = BasicHashTable(300'000, &hasher);
    Index index2 = Index(&store2, &preprocessor, &hasher, &searcher, nullptr, nullptr);

    std::string filename = "data/WestburyLab.wikicorp.201004_50MB.txt";

    test_time_of_preprocess(&index, &index2, filename);
    int search_iterations = 50; //
    test_time_of_search(&index, &index2, filename, search_iterations);

    test_correctness(&index, filename);

    return 0;
}
