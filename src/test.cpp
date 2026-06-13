#include "indexes/Index5.h"
#include "components/test/test_helpers.h"
#include "components/preprocessors/BasicPreprocessorWordCleaner.h"

#include "components/hashers/SimpleFingerprint.h"
#include "components/searchers/BasicSearcher.h"
//#include "components/stores/GenericFKSWithDocId.cpp"
#include "components/stores/BasicHashTable.h"
//#include "components/searchers/BooleanSearcher.h"
#include "components/searchers/RegexSearch.h"

//#include "components/stores/FuzzyDynamicFKSRadixTree.hpp"

//#include "components/stores/TreeWrapper.hpp"

//#include "components/stores/ModularStoreEliasFanoNoBuffer.hpp"
//#include "components/stores/ModularStoreEliasFanoPackedBuffer.hpp"
//#include "components/stores/BurstTrieEliasFanoStaticStore.hpp"

/**
 stores
 */
//#include "components/stores/BurstTrieRegexStore.h"
//#include "components/stores/BurstTrieDeltaDynamicStoreRegex.hpp"
#include "components/stores/BurstTrieEliasFanoDynamicStoreRegex.hpp"


//#include "components/stores/CompressibleBurstTrieStore.cpp"

//#include "components/rankers/MostMatchesRanker.h"
//#include "components/rankers/TFIDFRanker.h"
//#include "components/sorters/MergeSort.h"
//#include "components/stores/FuzzyTreeWrapper.hpp"
//#include "TestDynamicArray.cpp"

//#include "components/stores/GenericFKSWithDocId.cpp"
//#include "components/stores/BurstTrieStore.h"
//#include "components/stores/DictionaryWithDocId.h"
//#include "components/stores/GenericDATWithDocId.h"

//#include "extras/basic/HashFamily.h"

//#include "components/stores/ModularStorePacked.hpp"
//#include "components/stores/DictionaryWithTrieAndDocId.h"
//#include <cstdio>
/*
*/

int main(int argc, char* argv[]) {
    printf("m a i n \n");


    BasicPreprocessorWordCleaner preprocessor;
    SimpleFingerprint hasher;
    //BasicSearcher searcher;
   // BooleanSearcher searcher;
    RegexSearch searcher;


    BurstTrieEliasFanoDynamicStoreNS::BurstTrieEliasFanoDynamicStoreRegex store = BurstTrieEliasFanoDynamicStoreNS::BurstTrieEliasFanoDynamicStoreRegex();

    Index index = Index(&store, &preprocessor, &hasher, &searcher, nullptr, nullptr);
    // 57408850400
    // 348283157567 ns, fuzzy
    printf("Started preprocessing \n");
   // auto t0 = chrono::steady_clock::now();
    std::string filename = "data/WestburyLab.wikicorp.201004_100MB.txt";
    index.preprocess(filename);
  //  auto t1 = chrono::steady_clock::now();
   // auto elapsed = duration_cast<chrono::nanoseconds>(t1 - t0).count();
  //  std::cout << " elapsed=" << elapsed << " ns\n";
    printf("Finished preprocessing \n");

    printf("Start searching \n");
    SearchQuery q;
    q.q = "efs";

    std::vector<Doc> res = index.search(q);
    printf("Finished searching \n");
    for (std::vector<Doc>::iterator it = res.begin(); it != res.end(); ++it) {
        std::cout << it->title << " "  << endl;
    }

    //test(&index);
    /**
    std::string filename = "data/WestburyLab.wikicorp.201004_800MB.txt";


    //BurstTrieEliasFanoStaticStoreNS::BurstTrieEliasFanoStaticStore mse =  BurstTrieEliasFanoStaticStoreNS::BurstTrieEliasFanoStaticStore(); // 800MB-file: 526 MiB
    //ModularStoreEliasFanoNS::ModularStoreEliasFano mse = ModularStoreEliasFanoNS::ModularStoreEliasFano(&hashing, 10'000); // 100MB-File: 276 MB,  800MB-file: 1.64 GB
    //ModularStoreEliasFanoPackedBufferNS::ModularStoreEliasFanoPackedBuffer mse = ModularStoreEliasFanoPackedBufferNS::ModularStoreEliasFanoPackedBuffer(&hashing, 10'000); // 100MB-File: 288 MB
    //ModularStoreEliasFanoNoBufferNS::ModularStoreEliasFanoNoBuffer mse = ModularStoreEliasFanoNoBufferNS::ModularStoreEliasFanoNoBuffer(&hashing, 10'000); // 100MB-file: 206 MB, 800MB-file: 1.33 gb
    //ModularStorePackedNS::ModularStorePacked mse = ModularStorePackedNS::ModularStorePacked(&hashing, 10'000); // 100MB-file: 236 MB, 800MB-file: 1.52 GB
    //BurstTrieStore mse = BurstTrieStore(); // 800MB-file: 475-MB
    //ModularStorePackedNS::ModularStorePacked ms = ModularStorePackedNS::ModularStorePacked(&hashing, 10'000); // 579 MB
    //DictionaryWithTrieAndDocId dict_store = DictionaryWithTrieAndDocId(4, &hasher);
    //DictionaryWithDocId dict_store = DictionaryWithDocId(4);
    //DictionaryWithDocId dict_store_0 = DictionaryWithDocId(4);
    CompressibleBurstTrieStoreNS::CompressibleBurstTrieStore mse = CompressibleBurstTrieStoreNS::CompressibleBurstTrieStore(); //800MB-file: 455-mib
    IStore* s = &mse;
    IPreprocessor* p = &preprocessor;
    IHash* h = &hasher;
    ISearcher* se = &searcher;

    Index index_1 = Index(s, p, h, se, nullptr, nullptr);
    //Index dict_index = Index(&dict_store, &preprocessor, &hasher, &searcher, nullptr, nullptr);
    //

    index_1.preprocess(filename);
    //test_time_of_preprocess(&trie_index, &dict_index, filename);
    //test_time_of_search(&trie_index, &dict_index, filename, 100);
    printf("done \n");
    //printf("trie correctness \n");
    //test_correctness(&index_1, filename);
    //printf("dict correctness \n");
    //test_correctness(&dict_index, filename);

    //dict_store.print_spaced_used();
    */
    return 0;
}