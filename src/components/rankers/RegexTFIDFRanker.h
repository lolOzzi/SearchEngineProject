#pragma once

#include "../../core/interfaces.h"
#include "../../extras/basic/ChaniedHashDictionary.h"
#include "../searchers/RegexSearch.h"
#include "../hashers/SimpleFingerprint.h"
#include <fstream>
#include <iostream>
#include <cmath>
class RegexTFIDFRanker : public IRanker {
public:
    ~RegexTFIDFRanker() = default;
    std::vector<ScoredDoc> rank(const std::vector<Doc>& candidates, SearchQuery& query, IStore* store, std::string& filename, ISorter* sorter) override;
};
