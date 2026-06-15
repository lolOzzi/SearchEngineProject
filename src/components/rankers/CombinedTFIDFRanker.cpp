#include "CombinedTFIDFRanker.h"

#include "RegexTFIDFRanker.h"
#include "TFIDFRanker.h"

std::vector<ScoredDoc> CombinedTFIDFRanker::rank(const std::vector<Doc>& candidates,
                                                 SearchQuery& query, IStore* store,
                                                 std::string& filename, ISorter* sorter) {
    if (query.q.starts_with("r ")) {
        RegexTFIDFRanker ranker;
        return ranker.rank(candidates, query, store, filename, sorter);
    }

    TFIDFRANKER ranker;
    return ranker.rank(candidates, query, store, filename, sorter);
}
