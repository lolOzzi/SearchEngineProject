#pragma once

#include "../../core/interfaces.h"
#include <fstream>
#include <istream>
#include <iostream>
#include <cmath>
class TFIDFRANKER : public IRanker {
public:
    ~TFIDFRANKER() = default;
    std::vector<ScoredDoc> rank(const std::vector<Doc>& candidates, SearchQuery& query, IStore* store, std::string& filename, ISorter* sorter) override;
};