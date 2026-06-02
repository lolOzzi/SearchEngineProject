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

#include "components/stores/ModularStoreEliasFanoNoBuffer.cpp"
#include "components/stores/ModularStoreEliasFanoPackedBuffer.cpp"
//#include "components/stores/BurstTrieEliasFanoStaticStore.cpp"
//#include "components/stores/BurstTrieStore.cpp"
#include "components/stores/CompressibleBurstTrieStore.cpp"

#include "components/rankers/MostMatchesRanker.cpp"
#include "components/rankers/TFIDFRanker.cpp"
#include "components/sorters/MergeSort.cpp"
#include "components/stores/FuzzyTreeWrapper.cpp"
#include "TestDynamicArray.cpp"

//#include "components/stores/GenericFKSWithDocId.cpp"
//#include "components/stores/BurstTrieStore.cpp"
#include "components/stores/DictionaryWithDocId.cpp"
//#include "components/stores/GenericDATWithDocId.cpp"

#include "extras/basic/HashFamily.h"

#include "components/stores/ModularStorePacked.cpp"
#include "components/stores/DictionaryWithTrieAndDocId.cpp"
#include <cstdio>
/*
*/

int main(int argc, char* argv[]) {
    printf("m a i n \n");
    

    BasicPreprocessorWordCleaner preprocessor;
    SimpleFingerprint hasher;
    StringHashFamily hashing;
    BasicSearcher searcher;

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
    CompressibleBurstTrieStoreNS::CompressibleBurstTrieStore mse = CompressibleBurstTrieStoreNS::CompressibleBurstTrieStore(); //800MB-file: 450-mib
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
    return 0;
}
