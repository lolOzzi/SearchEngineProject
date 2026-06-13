#pragma once
#include "../../core/interfaces.h"

class BasicSearcher : public ISearcher {
public:
    ~BasicSearcher() override = default;
    std::vector<Doc> search(SearchQuery q, IStore* store) override;
};
