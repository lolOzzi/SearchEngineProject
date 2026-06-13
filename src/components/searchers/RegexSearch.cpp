#include "RegexSearch.h"

#include <algorithm>
#include "../stores/BurstTrieRegexStore.cpp"
#include "../stores/BurstTrieDeltaDynamicStoreRegex.cpp"
#include "../stores/BurstTrieEliasFanoDynamicStoreRegex.cpp"

std::string preprocess(std::string reg) {
    std::string res = "";

    for (int i = 0; i < reg.length(); i++) {
        char c1 = reg[i];
        res += c1;

        if (i + 1 < reg.length()) {
            char c2 = reg[i + 1];

            bool left = (c1 != '(' && c1 != '|');
            bool right = (c2 != ')' && c2 != '|' && c2 != '*' && c2 != '+' && c2 != '?');

            if (left && right) {
                res += '.';
            }
        }
    }
    return res;
}

std::string postfix(std::string reg) {
    std::string pre = preprocess(reg);
    std::string res = "";
    std::stack<char> stack;



    for (int i = 0; i < pre.length(); i++) {
        char c = pre[i];
        switch (c) {
            case '(':
                stack.push(c);
            break;
            case ')':
                while (!stack.empty() && stack.top() != '(') {
                    char top = stack.top();
                    res += top;
                    stack.pop();
                }
            stack.pop();
            break;

            case '|':
                while (!stack.empty() && (stack.top() == '.' || stack.top() == '|')) {
                    char top = stack.top();
                    res += top;
                    stack.pop();
                }
            stack.push(c);
            break;

            case '.':
                while (!stack.empty() && stack.top() == '.') {
                    char top = stack.top();
                    res += top;
                    stack.pop();
                }
            stack.push(c);
            break;


            default:
                res += c;
            break;
        }


    }
    while (!stack.empty()) {
        char c = stack.top();
        res+=c;
        stack.pop();
    }


    return res;
}

void patcher(std::vector<State**> &list, State* state) {
    for (auto& l : list) {
        *l = state;
    }
}

std::vector<State**> append(std::vector<State**> list1, std::vector<State**> list2) {
    list1.insert(list1.end(), list2.begin(), list2.end());
    return list1;
}


State* createNFA(std::string pf, std::vector<State*>& nodes) {
    std::stack<NFAFragment> stack;

    for (int i = 0; i < pf.length(); i++) {
        char c = pf[i];
        Token t;
        switch (c) {
            case '+': {
                NFAFragment f = stack.top();
                stack.pop();
                t.type = TokenType::SPLIT;
                t.val = ' ';
                State* s = new State();
                *s = {t, f.start, nullptr};
                patcher(f.end, s);
                nodes.emplace_back(s);

                NFAFragment frag {f.start};
                frag.end.push_back(&(s->out2));
                stack.push(frag);
                break;

            }
            case '*': {
                NFAFragment f = stack.top();
                stack.pop();
                t.type = TokenType::SPLIT;
                t.val = ' ';
                State* s = new State();
                *s = {t, f.start, nullptr};
                patcher(f.end, s);
                nodes.emplace_back(s);

                NFAFragment frag {s};
                frag.end.push_back(&(s->out2));
                stack.push(frag);
                break;

            }

            case '?': {
                NFAFragment f = stack.top();
                stack.pop();
                t.type = TokenType::SPLIT;
                t.val = ' ';
                State* s = new State();
                *s = {t, f.start, nullptr};
                nodes.emplace_back(s);

                NFAFragment frag {s, f.end};
                frag.end.push_back(&(s->out2));
                stack.push(frag);
                break;
            }

            case '|': {
                NFAFragment f2 = stack.top();
                stack.pop();
                NFAFragment f1 = stack.top();
                stack.pop();
                t.type = TokenType::SPLIT;
                t.val = ' ';
                State* s = new State();
                *s = {t, f1.start, f2.start};
                nodes.emplace_back(s);
                NFAFragment frag {s, append(f1.end,f2.end)};
                stack.push(frag);
                break;
            }

            case '.': {
                NFAFragment f2 = stack.top();
                stack.pop();
                NFAFragment f1 = stack.top();
                stack.pop();
                patcher(f1.end, f2.start);
                NFAFragment frag {f1.start, f2.end};
                stack.push(frag);
                break;
            }
            default: {
                t.type = TokenType::LITERAL;
                t.val = c;
                State* s = new State();
                *s = {t, nullptr, nullptr};
                nodes.emplace_back(s);
                NFAFragment frag {s};
                frag.end.push_back(&(s->out1));
                stack.push(frag);
                break;
            }
        }

    }
    NFAFragment f = stack.top();
    stack.pop();
    Token t{TokenType::MATCH, ' '};
    State* matchState = new State();
    *matchState = {t, nullptr, nullptr};
    nodes.emplace_back(matchState);
    patcher(f.end, matchState);
    return f.start;
}

void moveToNextLiteralHelper(std::vector<State*> &states, int time, State* currentState) {
    if (currentState == nullptr || currentState->time == time) {
        return;
    }
    currentState->time = time;
    if (currentState->t.type == TokenType::SPLIT) {
        moveToNextLiteralHelper(states, time, currentState->out1);
        moveToNextLiteralHelper(states, time, currentState->out2);
        return;
    }
    states.emplace_back(currentState);


}

std::vector<State*> moveToNextLiteral(std::vector<State*> &states, int time) {
    std::vector<State*> res;

    for (auto& s : states) {
        moveToNextLiteralHelper(res, time, s->out1);
    }
    return res;

}

std::vector<State*> checkLiteral(std::vector<State*> &states, char literal) {
    std::vector<State*> res;

    for (auto& s : states) {
        if (s->t.val == literal) {
            res.emplace_back(s);
        }
    }
    return res;
}


bool match(State* start, std::string q, std::vector<State*>& nodes) {
    for (State* s : nodes) {
        s->time = -2;
    }
    std::vector<State*> states;


    moveToNextLiteralHelper(states, -1, start);
    for (auto& s : states) {
        if (s->t.type == TokenType::MATCH) return true;
    }


    for (int time = 0; time < q.length(); time++) {
        moveToNextLiteralHelper(states, time + q.length(), start);
        char c = q[time];
        states = checkLiteral(states, c);
        states = moveToNextLiteral(states, time);
        for (auto& s : states) {
            if (s->t.type == TokenType::MATCH) {
                return true;
            }
        }

    }
    return false;
}

std::unordered_set<std::string> setUnion(std::unordered_set<std::string> set1, std::unordered_set<std::string> set2) {
    if (set1.size() >= set2.size()) {
        for (auto& s : set2) {
            set1.insert(s);
        }
        return set1;
    }
    for (auto& s : set1) {
        set2.insert(s);
    }
    return set2;
}

std::unordered_set<std::string> setCross(std::unordered_set<std::string> set1, std::unordered_set<std::string>& set2) {
    std::unordered_set<std::string> res;
    for (auto& s1 : set1) {
        for (auto& s2 : set2) {
            res.insert(s1+s2);
        }
    }
    return res;
}


int minLength(std::unordered_set<std::string> strings) {
    if (strings.empty()) {
        return 0;
    }
    int min = -1;
    for (auto& s : strings) {
        if (s.length() < min || min == -1) {
            min = s.length();
        }
    }
    return min;
}
void addAndSimplifyTree(std::shared_ptr<TrigramNode> parent, std::shared_ptr<TrigramNode> child) {

    if (parent->type == TrigramType::AND && child->type == TrigramType::ANY) return;
    if (parent->type == TrigramType::OR && child->type == TrigramType::NONE) return;

    if (child->type == parent->type) {
        for (auto& grandChild : child->children) {
            parent->children.emplace_back(grandChild);
        }
    } else {
        parent->children.emplace_back(child);
    }
}

std::shared_ptr<TrigramNode> trigramsFromString(std::string s) {
    std::vector<std::shared_ptr<TrigramNode>> trigrams;

    for (int i = 0; i < s.length() - 2; i++) {
        std::string tri = s.substr(i, 3);
        auto term = std::make_shared<TrigramNode>();
        term->type = TrigramType::TRI;
        term->trigram = tri;
        trigrams.emplace_back(term);
    }
    if (trigrams.size() == 1) {
        return trigrams[0];
    }
    auto res = std::make_shared<TrigramNode>();
    res->type = TrigramType::AND;
    res->children = std::move(trigrams);

    return res;
}

std::shared_ptr<TrigramNode> trigrams(std::unordered_set<std::string> strings) {
    if (minLength(strings) < 3) {
        return nullptr;
    }
    auto sub = std::make_shared<TrigramNode>();
    sub->type = TrigramType::OR;

    for (auto& s : strings) {
        auto child = trigramsFromString(s);
        addAndSimplifyTree(sub,child);
    }
    if (sub->children.size() == 1) {
        return sub->children[0];
    }

    return sub;
}

std::shared_ptr<TrigramNode> trigramAdd(std::shared_ptr<TrigramNode> match, std::vector<std::unordered_set<std::string>> strings) {
    std::shared_ptr<TrigramNode> res;
    if (match->type == TrigramType::AND) {
        res = match;
    } else if (match->type == TrigramType::ANY) {
        res = std::make_shared<TrigramNode>();
        res->type = TrigramType::AND;
    }
    else {
        res = std::make_shared<TrigramNode>();
        res->type = TrigramType::AND;
        res->children.emplace_back(match);
    }

    for (auto& se : strings) {
        std::shared_ptr<TrigramNode> tris = trigrams(se);
        if (tris) {
            addAndSimplifyTree(res,tris);
        }
    }
    if (res->children.size() == 1) {
        return res->children[0];
    }
    if (res->children.empty()) {
        auto tmp = std::make_shared<TrigramNode>();
        tmp->type = TrigramType::ANY;
        return tmp;
    }

    return  res;
}

std::unordered_set<std::string> chop(std::unordered_set<std::string> set, bool chopFirst) {
    int MAX_PS_SETS = 20;
    int l = 2;

    do {
        std::unordered_set<std::string> tmpset;

        if (chopFirst) {
            for (auto& s : set) {
                tmpset.insert(s.substr(0,l));
            }
        } else {
            for (auto& s : set) {
                if (s.length() <= l) {
                    tmpset.insert(s);
                } else {
                    tmpset.insert(s.substr(s.length()-l,l));
                }
            }
        }
        l--;
        set = std::move(tmpset);



    } while(set.size() > MAX_PS_SETS);

    return set;


}

std::unordered_set<std::string> removeRedundant(std::unordered_set<std::string> set, bool prefix) {
    std::vector<std::string> v(set.begin(), set.end());
    if (!prefix) {
        for (auto& s : v) {
            std::ranges::reverse(s);
        }
    }
    std::ranges::sort(v);



    std::string last;
    bool first = true;
    std::unordered_set<std::string> res;


    for (auto& s : v) {
        if (first) {
            last = s;
            if (!prefix) {
                std::ranges::reverse(s);
            }
            res.insert(s);
            first = false;
        } else {
            if (!s.starts_with(last)) {
                last = s;
                if (!prefix) {
                    std::ranges::reverse(s);
                }
                res.insert(s);

            }
        }

    }


    return res;

}

TrigramInfo simplify(TrigramInfo t, bool final) {
    int MAX_EXACT = 10;

    if (t.exact.size() > MAX_EXACT || final) {
        t.match = trigramAdd(t.match,{t.exact});
        for (auto& s : t.exact) {
            if (s.length() < 3) {
                t.prefix.insert(s);
                t.suffix.insert(s);
            } else {
                t.prefix.insert(s.substr(0,2));
                t.suffix.insert(s.substr(s.length()-2,2));
            }
        }
        t.known = false;
        t.exact.clear();
    }

    if (!t.known) {
        t.match = trigramAdd(t.match, {t.prefix, t.suffix});
        t.prefix = chop(t.prefix, true);
        t.prefix = removeRedundant(t.prefix, true);
        t.suffix = chop(t.suffix, false);
        t.suffix = removeRedundant(t.suffix, false);
    }

    return t;
}



std::shared_ptr<TrigramNode> trigramTree(std::string reg) {
    std::string pre = postfix(reg);
    std::stack<TrigramInfo> stack;

    for (int i = 0; i < pre.length(); i++) {
        char c = pre[i];
        switch (c) {
            case '+':{
                TrigramInfo e = stack.top();
                stack.pop();

                e.match = trigramAdd(e.match, {e.exact});

                if (e.known) {
                    e.prefix = e.exact;
                    e.suffix = e.exact;
                }

                e.known = false;
                e.exact.clear();

                stack.push(e);
                break;
            }


            case '*': {
                TrigramInfo e = stack.top();
                stack.pop();

                e.emptyable = true;
                e.known = false;
                e.exact.clear();
                e.prefix = {""};
                e.suffix = {""};
                e.match = std::make_shared<TrigramNode>();
                e.match->type = TrigramType::ANY;

                stack.push(e);
                break;
                }

            case '?': {
                TrigramInfo e = stack.top();
                stack.pop();

                e.emptyable = true;
                e.known = true;
                e.exact = setUnion(e.exact, {""});
                e.prefix = {""};
                e.suffix = {""};
                e.match = std::make_shared<TrigramNode>();
                e.match->type = TrigramType::ANY;

                stack.push(e);
                break;
            }

            case '|': {
                TrigramInfo t;
                TrigramInfo e2 = stack.top();
                stack.pop();
                TrigramInfo e1 = stack.top();
                stack.pop();

                t.emptyable = e1.emptyable || e2.emptyable;
                t.known = false;

                if (e1.known && e2.known) {
                    t.known = true;
                    t.exact = setUnion(e1.exact,e2.exact);
                } else if (e1.known){
                    t.prefix = setUnion(e1.exact, e2.prefix);
                    t.suffix = setUnion(e1.exact, e2.suffix);
                    e1.match = trigramAdd(e1.match,{e1.exact});
                    e1.known = false;
                    e1.exact.clear();
                } else if (e2.known){
                    t.prefix = setUnion(e2.exact, e1.prefix);
                    t.suffix = setUnion(e2.exact, e1.suffix);
                    e2.match = trigramAdd(e2.match,{e2.exact});
                    e2.known = false;
                    e2.exact.clear();
                } else {
                    t.prefix = setUnion(e1.prefix, e2.prefix);
                    t.suffix = setUnion(e1.suffix, e2.suffix);
                }

                auto n = std::make_shared<TrigramNode>();
                n->type = TrigramType::OR;
                addAndSimplifyTree(n, e1.match);
                addAndSimplifyTree(n, e2.match);

                if (n->children.size() == 1) {
                    t.match = n->children[0];
                }else if (n->children.empty()) {
                    auto n1 = std::make_shared<TrigramNode>();
                    n1->type = TrigramType::NONE;
                    t.match = n1;
                } else if (std::ranges::any_of(n->children, [](auto& c) { return c->type == TrigramType::ANY; }) ) {
                    auto n2 = std::make_shared<TrigramNode>();
                    n2->type = TrigramType::ANY;
                    t.match = n2;
                } else {
                    t.match = n;
                }
                TrigramInfo sim = simplify(t, false);
                stack.push(sim);

                break;
            }

            case '.': {
                TrigramInfo t;
                TrigramInfo e2 = stack.top();
                stack.pop();
                TrigramInfo e1 = stack.top();
                stack.pop();

                t.emptyable = e1.emptyable && e2.emptyable;

                if (e1.known && e2.known) {
                    t.known = true;
                    t.exact = setCross(e1.exact,e2.exact);
                } else {
                    t.known = false;
                    if (e1.known) {
                        t.prefix = setCross(e1.exact, e2.prefix);
                    } else if (e1.emptyable) {
                        t.prefix = setUnion(e1.prefix, e2.prefix);
                    } else {
                        t.prefix = e1.prefix;
                    }

                    if (e2.known) {
                        t.suffix = setCross(e1.suffix, e2.exact);
                    } else if (e2.emptyable) {
                        t.suffix = setUnion(e2.suffix, e1.suffix);
                    } else {
                        t.suffix = e2.suffix;
                    }
                }
                if (!e1.known && !e2.known && e1.suffix.size() <= 20 & e2.prefix.size() <= 20) {
                    std::unordered_set<std::string> middle = setCross(e1.suffix, e2.prefix);
                    if (minLength(middle) >= 3) {
                        t.match = trigramAdd(t.match, {middle});
                    }
                }

                auto n = std::make_shared<TrigramNode>();
                n->type = TrigramType::AND;
                addAndSimplifyTree(n, e1.match);
                addAndSimplifyTree(n, e2.match);

                if (n->children.size() == 1) {
                    t.match = n->children[0];
                } else if (n->children.empty()) {
                    auto n1 = std::make_shared<TrigramNode>();
                    n1->type = TrigramType::ANY;
                    t.match = n1;
                } else if (std::ranges::any_of(n->children, [](auto& c) { return c->type == TrigramType::NONE; }) ) {
                    auto n2 = std::make_shared<TrigramNode>();
                    n2->type = TrigramType::NONE;
                    t.match = n2;
                } else {
                    t.match = n;
                }

                TrigramInfo sim = simplify(t, false);
                stack.push(sim);

                break;
            }


            default: {
                auto n = std::make_shared<TrigramNode>();
                n->type = TrigramType::ANY;
                TrigramInfo t {false, true, {std::string(1,c)},
                    {std::string(1,c)}, {std::string(1,c)}, n };
                stack.push(t);
                break;
            }
        }
    }
    return simplify(stack.top(),true).match;

}


std::unordered_set<std::string> setIntersection(std::unordered_set<std::string> set1, std::unordered_set<std::string> set2) {
    std::unordered_set<std::string> res;
    if (set1.size() >= set2.size()) {
        for (auto& s : set2) {
            if (set1.contains(s)) {
                res.insert(s);
            }
        }
        return set1;
    }
    for (auto& s : set1) {
        if (set2.contains(s)) {
            res.insert(s);
        }
    }
    return res;
}


std::vector<std::string> hasRegex(IStore* store, std::string tri) {
    if (auto* burstTrieStore = dynamic_cast<BurstTrieRegexStore*>(store)) {
        return burstTrieStore->getWordsFromTrigram(tri);
    }
    if (auto* burstTrieDeltaStore = dynamic_cast<BurstTrieDeltaDynamicStoreRegexNS::BurstTrieDeltaDynamicStoreRegex*>(store)) {
        return burstTrieDeltaStore->getWordsFromTrigram(tri);
    }
    if (auto* burstTrieEliasStore = dynamic_cast<BurstTrieEliasFanoDynamicStoreRegexNS::BurstTrieEliasFanoDynamicStoreRegex*>(store)) {
        return burstTrieEliasStore->getWordsFromTrigram(tri);
    }
    throw std::invalid_argument("Store does not support regex\n");
}
std::unordered_set<std::string> extractTrigrams(std::shared_ptr<TrigramNode> node, IStore* store ) {
    switch (node->type) {
        case TrigramType::AND: {
            std::unordered_set<std::string> res;
            bool first = true;
            for (const auto& c : node->children) {
                if (first) {
                    first = false;
                    res = extractTrigrams(c, store);
                    continue;
                }
                res = setIntersection(res,extractTrigrams(c, store));
            }
            return res;
            break;
        }

        case TrigramType::OR: {
            std::unordered_set<std::string> res;
            for (const auto& c : node->children) {
                res = setUnion(res,extractTrigrams(c, store));
            }
            return res;
            break;
        }

        case TrigramType::TRI: {
            std::vector<std::string> vec = hasRegex(store, node->trigram);
            std::unordered_set<std::string> res(
            std::make_move_iterator(vec.begin()),
            std::make_move_iterator(vec.end())
            );
            return res;
            break;
        }

        default:
            throw std::invalid_argument("Not enough litterals for any trigrams to be produced");
    }

}

std::unordered_set<std::string> filterCandidates(State* NFA, std::unordered_set<std::string> candidateList, std::vector<State*>& nodes) {
    std::unordered_set<std::string> res;

    for (auto& s : candidateList) {
        if (match(NFA, s, nodes)) {
            res.insert(s);
        }

    }
    return res;
}

std::vector<Doc> findDocs(IStore* store, std::unordered_set<std::string> strings) {
    std::unordered_set<std::string> documentTitles;
    std::vector<Doc> res;
    for (auto& s : strings) {
        std::vector<Doc> docs = store->get(s);
        for (auto& doc : docs) {
            if (!documentTitles.contains(doc.title)) {
                res.push_back(doc);
                documentTitles.insert(doc.title);
            }
        }

    }

    return res;

}

std::vector<Doc> RegexSearch::search(SearchQuery q, IStore *store) {
    if (!store) return {};
    std::vector<State*> nodes;
    std::string regex = q.q;

    std::string pf = postfix(regex);
    State* NFA = createNFA(pf, nodes);
    std::shared_ptr<TrigramNode> ast = trigramTree(regex);
    std::unordered_set<std::string> candidateList = extractTrigrams(ast, store);
    candidateList = filterCandidates(NFA, candidateList, nodes);
    std::vector<Doc> res = findDocs(store, candidateList);




    for (State* node : nodes) {
        delete node;
    }
    nodes.clear();

    return res;
}
