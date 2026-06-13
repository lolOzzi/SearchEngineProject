#include "BasicSearcher.h"

std::vector<Doc> BasicSearcher::search(SearchQuery q, IStore *store) {
    if (!store) return {};
    std::vector<Doc> docs = store->get(q.q);
    return docs;
}
