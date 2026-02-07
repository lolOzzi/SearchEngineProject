#include "../../core/interfaces.h"
#include <unordered_set>


class BooleanSearcher : public ISearcher {
private:
    void Intersection(std::vector<Doc>* main, std::vector<Doc>* v, std::unordered_set<std::string>* mainSet);
    void Union(std::vector<Doc>* main, std::vector<Doc>* v, std::unordered_set<std::string>* mainSet);
    std::unordered_set<std::string> createDocSet(std::vector<Doc>* v);

public:
    ~BooleanSearcher() override = default;
    std::vector<Doc> search(SearchQuery q, IStore* store) override;
};

std::unordered_set<std::string> BooleanSearcher::createDocSet(std::vector<Doc>* v) {
    std::unordered_set<std::string> set;
    for (Doc& doc : *v) {
        set.insert(doc.title);
    }
    return set;
}


void BooleanSearcher::Intersection(std::vector<Doc>* main, std::vector<Doc>* v, std::unordered_set<std::string>* mainSet) {
    std::unordered_set<std::string> vSet = createDocSet(v);
    std::unordered_set<std::string> resultSet;
    for (auto& docName : *mainSet) {
        if (vSet.contains(docName)) {
            resultSet.insert(docName);
        }
    }
    *mainSet = resultSet;
    std::vector<Doc> resultsDoc;
    for (Doc& doc : *main) {
        if (resultSet.find(doc.title) != resultSet.end()) {
            resultsDoc.push_back(doc);
        }
    *main = resultsDoc;
    }


}
void BooleanSearcher::Union(std::vector<Doc>* main, std::vector<Doc>* v, std::unordered_set<std::string>* mainSet) {
    for (Doc& doc : *v) {
        if (!mainSet->contains(doc.title)) {
            main->push_back(doc);
            mainSet->insert(doc.title);
        }
    }


}



std::vector<Doc> BooleanSearcher::search(SearchQuery q, IStore* store) {
    if (!store) return {};
    if (!(q.queries.size()-1 <= q.mode.size())) {
        throw std::invalid_argument("Size of mode is too small for query size. Queries size = " +
           std::to_string(q.queries.size()) + ", Mode size = " + std::to_string(q.mode.size()));
    }


    printf("%lu",q.queries.size());
    std::vector<std::vector<Doc>> allDocs;
    for (auto& qu : q.queries) {
        if (std::vector<Doc>* tmp = store->get(qu)) {
            allDocs.push_back(*tmp);
        }
    }

    std::vector<Doc> res;
    std::unordered_set<std::string> resSet;
    int counter = -1;
    for (auto& doc : allDocs) {
        if (counter == -1) {
            res = doc;
            resSet = createDocSet(&res);
            counter++;
            continue;
        }
        q.mode[counter] == boolOperator::_and ? Intersection(&res, &doc, &resSet) : Union(&res,&doc, &resSet);
        counter++;
    }

    return res;
}
