#pragma once
#include "../../core/interfaces.h"
#include <unordered_set>

namespace LinearProbingTableNS {

class BooleanSearcher : public ISearcher {
private:
    void Intersection(std::vector<Doc>* main, std::vector<Doc>* v, std::unordered_set<std::string>* mainSet);
    void Union(std::vector<Doc>* main, std::vector<Doc>* v, std::unordered_set<std::string>* mainSet);
    std::unordered_set<std::string> createDocSet(std::vector<Doc>* v);

public:
    ~BooleanSearcher() override = default;
    std::vector<Doc> search(SearchQuery q, IStore* store) override;
};

}
