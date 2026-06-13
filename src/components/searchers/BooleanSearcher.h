#pragma once

#include "../../core/interfaces.h"
#include <unordered_set>
#include <utility>
#include <variant>

namespace BooleanSearcherNS {
    struct Expr;
    struct Token;

    class BooleanSearcher : public ISearcher {
    private:
        std::vector<Doc> Intersection(std::vector<Doc> main, std::vector<Doc> v);
        std::vector<Doc> Union(std::vector<Doc> main, std::vector<Doc> v);
        std::unordered_set<std::string> createDocSet(std::vector<Doc>* v);
        std::vector<Token> lexer (std::string query);
        Expr parse(std::vector<Token> tokens);
        std::vector<Doc> eval(Expr &exp, IStore *store);

    public:
        ~BooleanSearcher() override = default;
        std::vector<Doc> search(SearchQuery q, IStore* store) override;
    };

    enum class TokenType {
        VAR,
        AND,
        OR,
        LPAR,
        RPAR,
        END,
    };

    struct Token {
        TokenType type;
        std::string val;
    };

    struct BooleanOR  {
        std::unique_ptr<Expr> left;
        std::unique_ptr<Expr> right;
    };

    struct BooleanAND  {
        std::unique_ptr<Expr> left;
        std::unique_ptr<Expr> right;
    };
    struct Parenthesis {
        std::unique_ptr<Expr> exp;
    };

    struct Expr {
        std::variant<std::string, BooleanOR, BooleanAND, Parenthesis> type;
    };
}