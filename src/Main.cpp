#include <chrono>

#include "core/Index5.h"
#include "components/test/test.cpp"
#include "components/preprocessors/BasicPreprocessorWordCleaner.cpp"

#include "components/hashers/SimpleFingerprint.cpp"
#include "components/searchers/BasicSearcher.cpp"
#include "components/stores/GenericFKSWithDocId.cpp"
//#include "components/stores/DynamicFKSRadixTree.cpp"
/*
*/

int main(int argc, char* argv[]) {
    printf("main \n");

    BasicPreprocessorWordCleaner preprocessor;
    SimpleFingerprint hasher;
    BasicSearcher searcher;
    GenericFKSWithDocId store = GenericFKSWithDocId(3'000'000, &hasher);

    Index index = Index(&store, &preprocessor, &hasher, &searcher, nullptr, nullptr);
    // 57408850400
    // 348283157567 ns, fuzzy
    printf("Started preprocessing \n");
    auto t0 = chrono::steady_clock::now();
    std::string filename = "data/WestburyLab.wikicorp.201004_100MB.txt";
    index.preprocess(filename);
    auto t1 = chrono::steady_clock::now();
    auto elapsed = duration_cast<chrono::nanoseconds>(t1 - t0).count();
    std::cout << " elapsed=" << elapsed << " ns\n";
    printf("Finished preprocessing \n");

    printf("Start searching \n");
    SearchQuery q;
    q.q = "albedo";
    std::vector<Doc> res = index.search(q);
    printf("Finished searching \n");
    for (std::vector<Doc>::iterator it = res.begin(); it != res.end(); ++it) {
        std::cout << it->title << " "  << endl;
    }

    test(&index);

    return 0;
}
