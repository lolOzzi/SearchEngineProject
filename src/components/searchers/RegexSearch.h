#pragma once

#include "../../core/interfaces.h"
#include "../../extras/basic/ChainSet.h"
#include <unordered_set>
#include <ranges>
#include <stack>


enum class TokenType {
    SPLIT,
    MATCH,
    LITERAL
};

struct Token {
    TokenType type;
    char val;
};

struct State {
    Token t;
    State* out1;
    State* out2;
    int time = -2;
};

struct NFAFragment{
    State* start;
    std::vector<State**> end;
};

enum class TrigramType {
    ANY,
    NONE,
    TRI,
    AND,
    OR
};

struct TrigramNode {
    TrigramType type;
    std::string trigram;
    std::vector<std::shared_ptr<TrigramNode>> children;

};

struct TrigramInfo {
    bool emptyable;
    bool known;
    std::unordered_set<std::string> exact;
    std::unordered_set<std::string> prefix;
    std::unordered_set<std::string> suffix;
    std::shared_ptr<TrigramNode> match;
};

class RegexSearch : public ISearcher {
public:
    ~RegexSearch() override = default;
    std::vector<Doc> search(SearchQuery q, IStore* store) override;
};

void regexExpandToTerms(std::string regex, IStore* store, SimpleSet<std::string>& out);

// NFA building / matching, implemented in RegexSearch.cpp. Exposed so other
// components (e.g. RegexTFIDFRanker) can reuse the exact same engine instead of
// recreating it.
std::string postfix(std::string reg);
State* createNFA(std::string pf, std::vector<State*>& nodes);
bool match(State* start, std::string q, std::vector<State*>& nodes);
