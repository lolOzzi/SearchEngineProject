#include "core/Index5.h"
#include "components/test/test.cpp"
#include "components/preprocessors/BasicPreprocessorWordCleaner.cpp"

#include "components/hashers/SimpleFingerprint.h"
#include "components/searchers/BasicSearcher.cpp"

//#include "components/stores/GenericFKSWithDocId.cpp"
#include "components/stores/BurstTrieStore.cpp"
#include "components/stores/DictionaryWithDocId.cpp"
//#include "components/stores/GenericDATWithDocId.cpp"

/*
*/

int main(int argc, char* argv[]) {
    printf("main \n");

    BasicPreprocessorWordCleaner preprocessor;
    SimpleFingerprint hasher;
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
