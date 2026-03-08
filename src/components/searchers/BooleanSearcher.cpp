#include "../../core/interfaces.h"
#include <unordered_set>
#include <utility>

struct Expr;
struct Token;


class BooleanSearcher : public ISearcher {
private:
    std::vector<Doc> Intersection(std::vector<Doc> main, std::vector<Doc> v);
    std::vector<Doc> Union(std::vector<Doc> main, const std::vector<Doc>& v);
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



std::unordered_set<std::string> BooleanSearcher::createDocSet(std::vector<Doc>* v) {
    std::unordered_set<std::string> set;
    for (Doc& doc : *v) {
        set.insert(doc.title);
    }
    return set;
}


std::vector<Doc> BooleanSearcher::Intersection(std::vector<Doc> main, std::vector<Doc> v) {
    std::unordered_set<std::string> mainSet = createDocSet(&main);
    std::unordered_set<std::string> vSet = createDocSet(&v);
    std::unordered_set<std::string> resultSet;
    for (auto& docName : mainSet) {
        if (vSet.contains(docName)) {
            resultSet.insert(docName);
        }
    }
    mainSet = resultSet;
    std::vector<Doc> resultsDoc;
    for (Doc& doc : main) {
        if (resultSet.find(doc.title) != resultSet.end()) {
            resultsDoc.push_back(doc);
        }
    }

    return resultsDoc;


}

std::vector<Doc> BooleanSearcher::Union(std::vector<Doc> main, const std::vector<Doc>& v) {
    std::unordered_set<std::string> mainSet = createDocSet(&main);
    for (const Doc& doc : v) {
        if (!mainSet.contains(doc.title)) {
            main.push_back(doc);
            mainSet.insert(doc.title);
        }
    }
    return main;


}


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
    std::variant<string, BooleanOR, BooleanAND, Parenthesis> type;
};


std::vector<Token> BooleanSearcher::lexer (std::string q) {
    std::vector<Token> res;

    for (int i = 0; i < q.length(); i++) {
        Token t;
        switch (q[i]) {
            case ' ':
                continue;
            break;
            case '(':
                t.type = TokenType::LPAR;
                t.val = "";
                res.push_back(t);
            break;
            case ')':
                t.type = TokenType::RPAR;
                t.val = "";
                res.push_back(t);
            break;
            case '&':
                t.type = TokenType::AND;
                t.val = "";
                res.push_back(t);
            break;
            case '|':
                t.type = TokenType::OR;
                t.val = "";
                res.push_back(t);
            break;
            default:
                std::string var = "";
                while (i < q.length() && q[i] != ' ' &&
                    q[i] != '(' && q[i] != ')' &&
                    q[i] != '|' && q[i] != '&') {
                    var += q[i++];
                }
                t.type = TokenType::VAR;
                t.val = var;
                res.push_back(t);
                i--;
            break;
        }
    }
    Token end;
    end.type = TokenType::END;
    res.push_back(end);
    return res;

}




Expr BooleanSearcher::parse(std::vector<Token> tokens) {
    struct parser {
        std::vector<Token> tokens;

        Token next() {
            return tokens.front();
        }

        void consume() {
            tokens.erase(tokens.begin());
        }

        Expr highPrecendence() {
            if (next().type == TokenType::LPAR) {
                consume();
                Expr par = lowPrecendence();
                consume();
                return Expr{Parenthesis{std::make_unique<Expr>(std::move(par))}};
            }
            if (next().type == TokenType::VAR) {
                std::string val = next().val;
                consume();
                return Expr{val};

            }
            throw std::invalid_argument("Invalid Boolean search syntax");




        }

        Expr medPrecedence() {
            Expr left = highPrecendence();
            while (next().type == TokenType::AND) {
                consume();
                Expr right = highPrecendence();
                left = Expr{BooleanAND{std::make_unique<Expr>(std::move(left)),
                                             std::make_unique<Expr>(std::move(right))}};

            }
            return left;
        }

        Expr lowPrecendence() {
            Expr left = medPrecedence();
            while (next().type == TokenType::OR) {
                consume();
                Expr right = medPrecedence();
                left = Expr{BooleanOR{std::make_unique<Expr>(std::move(left)),
                                             std::make_unique<Expr>(std::move(right))}};

            }
            return left;
        }


    };
    parser p;
    p.tokens = std::move(tokens);
    Expr res = p.lowPrecendence();



    return res;
}


template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

std::vector<Doc> BooleanSearcher::eval(Expr& exp, IStore* store) {
    return std::visit(overloaded {
            [store](std::string c) {
               return  store->get(c);},
            [store, this](BooleanOR& c) {
                return ((Union((eval(*(c.left), store)), eval(*c.right, store))));
            },
            [store, this](BooleanAND& c) {
                return ((Intersection((eval(*(c.left), store)), eval(*c.right, store))));

            },
            [store, this](Parenthesis& c) {
                return eval(*(c.exp), store);
            },
    }, exp.type);

}



std::vector<Doc> BooleanSearcher::search(SearchQuery q, IStore* store) {
    if (!store) return {};
    std::vector<Token> tokens = lexer(q.q);
    Expr exp = parse(tokens);
    std::vector<Doc> res = eval(exp, store);




    return res;
}
