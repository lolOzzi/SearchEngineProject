#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include "../extras/helpers/FileHelpers.cpp"
#include "../extras/basic/ChainSet.cpp"
const static std::string END = "---END.OF.DOCUMENT---";

class Doc {
public:
    std::string title;
    long long start_loc;
};

class ScoredDoc : public Doc {
public:
    int score{};
};
class DocId {
public:
    int id;
};

enum class boolOperator {
    _and,
    _or,
    };

class SearchQuery {
public:
    std::string q;
    std::vector<std::string> queries;
    std::vector<boolOperator> mode;
};

class IStore {
public:
    virtual ~IStore() = default;
    virtual void add(std::string word, Doc document) = 0;
    virtual void add_document(Doc document) = 0;
    virtual std::vector<Doc> get(std::string word) = 0;
    virtual int get_num_docs() = 0;
};

class IPreprocessor {
public:
    virtual ~IPreprocessor() = default;
    virtual void* preprocess(std::string filename, IStore* store) = 0;
};

class IHash {
public:
    virtual ~IHash() = default;
    virtual std::uint64_t hash(std::string key, int max_val) = 0;
};

class ISearcher {
public:
    virtual ~ISearcher() = default;
    virtual std::vector<Doc> search(SearchQuery q, IStore* store) = 0;
};
class ISorter {
public:
    virtual ~ISorter() = default;
    virtual void sort(std::vector<ScoredDoc> &A) = 0;
};
class IRanker {
public:
    virtual ~IRanker() = default;
    virtual std::vector<ScoredDoc> rank(const std::vector<Doc>& candidates,
                                        std::string& query, IStore* store,
                                        std::string& filename, ISorter* sorter) = 0;
};


class Index {
private:
    IStore* store;
    IPreprocessor* preprocessor;
    IHash* hash;
    ISearcher* searcher;
    IRanker* ranker;
    ISorter* sorter;

public:
    Index(IStore* store, IPreprocessor* preprocessor, IHash* hash, ISearcher* searcher, IRanker* ranker, ISorter* sorter) {
        this->store = store;
        this->preprocessor = preprocessor;
        this->hash = hash;
        this->searcher = searcher;
        this->ranker = ranker;
        this->sorter = sorter;
    }

    void preprocess(std::string filename) {
        preprocessor->preprocess(filename, this->store);
    }

    std::vector<Doc> search(SearchQuery q) {
        return searcher->search(q, this->store);
    }
    std::vector<ScoredDoc> rank(std::vector<Doc>& docs, std::string& q, std::string& fn) {
        return ranker->rank(docs, q, this->store, fn, this->sorter);
    };
};