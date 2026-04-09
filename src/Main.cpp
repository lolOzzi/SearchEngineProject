#include "core/Index5.h"
#include "components/test/test.cpp"
#include "components/preprocessors/BasicPreprocessorWordCleaner.cpp"

#include "components/hashers/SimpleFingerprint.h"
#include "components/searchers/BasicSearcher.cpp"

//#include "components/stores/GenericFKSWithDocId.cpp"
#include "components/stores/BurstTrieStore.cpp"
//#include "components/stores/GenericDATWithDocId.cpp"

/*
*/

int main(int argc, char* argv[]) {
    printf("main \n");

    BasicPreprocessorWordCleaner preprocessor;
    SimpleFingerprint hasher;
    BasicSearcher searcher;

    std::string filename = "data/WestburyLab.wikicorp.201004_100MB.txt";

    BurstTrieStore store = BurstTrieStore();

    Index index = Index(&store, &preprocessor, &hasher, &searcher, nullptr, nullptr);
    index.preprocess(filename);
    printf("amt words added %d \n", store.words_added);

    printf("Finished Preprocessing \n");

    SearchQuery q;
    q.q = "albedo";
    std::vector<Doc> res = index.search(q);
    printf("Finished searching \n");
    for (std::vector<Doc>::iterator it = res.begin(); it != res.end(); ++it) {
        std::cout << it->title << " "  << endl;
    }

    test_correctness(&index, filename);

    return 0;
}
