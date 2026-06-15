#pragma once
#include "../../core/interfaces.h"

class CombinedTFIDFRanker : public IRanker {
public:
    ~CombinedTFIDFRanker() = default;
    std::vector<ScoredDoc> rank(const std::vector<Doc>& candidates, SearchQuery& query, IStore* store, std::string& filename, ISorter* sorter) override;
};
