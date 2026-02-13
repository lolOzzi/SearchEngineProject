#include "../../core/interfaces.h"


class BasicSearcher : public ISearcher {
public:
    ~BasicSearcher() override = default;
    std::vector<Doc> search(SearchQuery q, IStore* store) override;
};

std::vector<Doc> BasicSearcher::search(SearchQuery q, IStore *store) {
    if (!store) return {};
    std::vector<Doc> docs = store->get(q.q);
    return docs;
}
