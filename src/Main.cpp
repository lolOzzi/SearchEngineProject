#include "core/Index5.h"
#include "components/test/test.cpp"
#include "components/preprocessors/BasicPreprocessor.cpp"
#include "components/preprocessors/BasicPreprocessorHandlesPunctuation.cpp"
#include "components/preprocessors/BasicPreprocessorHandlesWhitespaceChars.cpp"

#include "components/hashers/BasicHasher.cpp"
#include "components/searchers/BasicSearcher.cpp"
#include "components/searchers/BooleanSearcher.cpp"
#include "components/stores/BasicHashTable.cpp"
#include "components/rankers/MostMatchesRanker.cpp"

/*
*/

int main(int argc, char* argv[]) {
    printf("main \n");

    BasicPreprocessorHandlesWhitespaceChars preprocessor;
    BasicHasher hasher;
    //BooleanSearcher searcher;
    BasicSearcher searcher;
    BasicHashTable store = BasicHashTable(300'000, &hasher);
    MostMatchesRanker MMRanker;
    Index index = Index(&store, &preprocessor, &hasher, &searcher, &MMRanker);

    printf("Started preprocessing \n");
    std::string filename = "data/WestburyLab.wikicorp.201004_100MB.txt";
    index.preprocess(filename);
    printf("Finished preprocessing \n");

    /* testing for non-boolean search
    printf("Started Testing \n");
    test(&index);
    printf("Finished Testing \n");
    */
    printf("Start searching \n");
    SearchQuery q;
    q.q = "totally";
    /*
    std::vector<std::string> words;
    words.emplace_back("hiking");
    words.emplace_back("jumping");
    words.emplace_back("fishing");
    words.emplace_back("swimming");
    q.queries = words;
    std::vector<boolOperator> operations;
    operations.push_back(boolOperator::_or);
    operations.push_back(boolOperator::_or);
    operations.push_back(boolOperator::_or);

    q.mode = operations;
    */
    std::vector<Doc> res = index.search(q);
    printf("Finished searching \n");
    for (std::vector<Doc>::iterator it = res.begin(); it != res.end(); ++it) {
        std::cout << it->title << " "  << endl;
    }


    /*
    printf("Started Ranking \n");
    std::vector<ScoredDoc> res_scored = index.rank(res, qy, filename);
    printf("Finished Ranking \n");
    for (std::vector<ScoredDoc>::iterator it = res_scored.begin(); it != res_scored.end(); it++) {
        std::cout << it->title << " " << it->score << endl;
    }
    */


    return 0;
}
