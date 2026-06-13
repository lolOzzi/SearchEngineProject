#pragma once
#include "../../core/interfaces.h"

class MostMatchesRanker : public IRanker {
public:
    ~MostMatchesRanker() = default;
    std::vector<ScoredDoc> rank(const std::vector<Doc>& candidates, SearchQuery& q, IStore* store, std::string& filename, ISorter* sorter) override;
};
