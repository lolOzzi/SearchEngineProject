#include "core/Index5.h"
#include "components/test/test.cpp"
#include "components/preprocessors/BasicPreprocessorWordCleaner.cpp"

#include "components/hashers/SimpleFingerprint.h"
#include "components/searchers/BasicSearcher.cpp"
//#include "components/stores/GenericFKSWithDocId.cpp"
#include "components/stores/BasicHashTable.cpp"
//#include "components/stores/DynamicFKSRadixTree.cpp"
#include "components/searchers/BooleanSearcher.cpp"
#include "components/stores/FuzzyDynamicFKSRadixTree.cpp"
#include "components/stores/ModularStoreEliasFano.cpp"
//#include "components/stores/TreeWrapper.cpp"


#include "components/rankers/MostMatchesRanker.cpp"
#include "components/rankers/TFIDFRanker.cpp"
#include "components/sorters/MergeSort.cpp"
#include "components/stores/FuzzyTreeWrapper.cpp"
#include "TestDynamicArray.cpp"
#include "extras/basic/HashFamily.h"

//#include "components/stores/GenericFKSWithDocId.cpp"
#include "components/stores/BurstTrieStore.cpp"
#include "components/stores/DictionaryWithDocId.cpp"
//#include "components/stores/GenericDATWithDocId.cpp"

#include "extras/test/SortedDynamicArrayTest.cpp"

#include "extras/test/CompressedTrieTest.cpp"
#include "extras/test/CompressedHashTrieTest.cpp"
#include "components/stores/ModularStorePackedNHashed.cpp"
/*
*/

int main(int argc, char* argv[]) {
    printf("main \n");
    
    run_sorted_dynamic_array_tests();
    run_trie_tests();
    run_hashtrie_tests();
    //return 0; 

    BasicPreprocessorWordCleaner preprocessor;
    StringHashFamily hasher;
    BasicSearcher searcher;

    std::string filename = "data/WestburyLab.wikicorp.201004_100MB.txt";

    BurstTrieStore trie_store = BurstTrieStore();
    DictionaryWithDocId dict_store = DictionaryWithDocId(4);

    Index trie_index = Index(&trie_store, &preprocessor, &hasher, &searcher, nullptr, nullptr);
    Index dict_index = Index(&dict_store, &preprocessor, &hasher, &searcher, nullptr, nullptr);

    test_time_of_preprocess(&trie_index, &dict_index, filename);
    test_time_of_search(&trie_index, &dict_index, filename, 100);

    printf("trie correctness \n");
    test_correctness(&trie_index, filename);
    printf("dict correctness \n");
    test_correctness(&dict_index, filename);

    dict_store.print_spaced_used();
    return 0;
}
