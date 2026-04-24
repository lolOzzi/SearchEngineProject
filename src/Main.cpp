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

    ModularStoreEliasFanoNS::ModularStoreEliasFano store = ModularStoreEliasFanoNS::ModularStoreEliasFano(&hasher, 300'000); // 256: 4.26 gb 
    //ModularStorePackedNHashedNS::ModularStorePackedNHashed store = ModularStorePackedNHashedNS::ModularStorePackedNHashed(&hasher, 300'000); // 4.19gb
    Index index = Index(&store, &preprocessor, nullptr, &searcher, nullptr, nullptr);
    // 57408850400
    // 348283157567 ns, fuzzy
    // 95951786518 ns, elias fano  
    printf("Started preprocessing \n");
    auto t0 = chrono::steady_clock::now();
    std::string filename = "data/WestburyLab.wikicorp.201004_100MB.txt";

    index.preprocess(filename);
    auto t1 = chrono::steady_clock::now();
    auto elapsed = duration_cast<chrono::nanoseconds>(t1 - t0).count();
    std::cout << " elapsed=" << elapsed << " ns\n";
    printf("Finished preprocessing \n");

    auto res = index.search({"albedo"});
    for (int i = 0; i < res.size(); ++i) {
        std::cout << res[i].title << "\n";
    }

    //BasicHashTable store2 = BasicHashTable(300'000, &hasher);
    //Index index2 = Index(&store2, &preprocessor, &hasher, &searcher, nullptr, nullptr);

    //std::string filename = "data/WestburyLab.wikicorp.201004_50MB.txt";

    //test_time_of_preprocess(&index, &index2, filename);
    //int search_iterations = 50; //
    //test_time_of_search(&index, &index2, filename, search_iterations);

    //test_correctness(&index, filename);
    

    return 0;
}
