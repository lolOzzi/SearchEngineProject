#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <limits>
#include "interfaces.h"
#include <algorithm>
#include <cctype>

inline void printRegexHelp() {
    std::cout << "\nIf you want search as regex start query with r \n";
    std::cout << "regex tokens:\n";
    std::cout << "  abc   literal characters, matched exactly\n";
    std::cout << "  |     alternation: match the left OR right side (cat|dog)\n";
    std::cout << "  *     zero or more of the previous token (abcd*)\n";
    std::cout << "  +     one or more of the previous token (abc+)\n";
    std::cout << "  ?     zero or one of the previous token (abc?)\n";
    std::cout << "  ( )   group tokens together ((abc)+)\n";
    std::cout << "  Note: all queries must use at least 3 characters before a regex token, 4 for *\n\n";
}

inline std::string getDocContent(const std::vector<std::string>& files, const Doc& doc) {
    std::string fallback;
    for (const std::string& fn : files) {
        std::ifstream file(fn);
        if (!file.is_open()) continue;
        file.seekg(doc.start_loc);
        std::string content;
        std::string line;
        bool first = true;
        bool match = false;
        while (std::getline(file, line)) {
            std::string::size_type end = line.find(END);
            if (end != std::string::npos) {
                content += line.substr(0, end);
                break;
            }
            if (first) {
                std::string t = line;
                while (!t.empty() && (t.back() == ',' || t.back() == '.' || t.back() == '?' || t.back() == '\r' || t.back() == ' ')) {
                    t.erase(t.size() - 1);
                }
                if (t == doc.title) match = true;
                first = false;
            }
            content += line + "\n";
        }
        if (match) return content;
        if (fallback.empty()) fallback = content;
    }
    return fallback.empty() ? "(could not read document contents)" : fallback;
}

inline void tui(Index& index) {
    std::vector<std::string> files;
    while (true) {
        std::cout << "\n--- search index ---\n";
        std::cout << "1) add file\n";
        std::cout << "2) search\n";
        std::cout << "3) search (ranked)\n";
        std::cout << "4) quit\n";
        std::cout << "> ";
        int choice = 0;
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

       if (choice == 1) {
        std::cout << "filename: ";
        std::string filename;
        std::getline(std::cin, filename);

        auto not_space = [](unsigned char c) { return !std::isspace(c); };
        filename.erase(filename.begin(), std::find_if(filename.begin(), filename.end(), not_space));
        filename.erase(std::find_if(filename.rbegin(), filename.rend(), not_space).base(),filename.end());

        std::ifstream check(filename);
        if (!check.is_open()) {
            std::cout << "error: could not open file '" << filename << "'\n";
            continue;
        }
        check.close();

        index.preprocess(filename);
        files.push_back(filename);
        std::cout << "added " << filename << "\n";
        } else if (choice == 2) {
            printRegexHelp();
            std::cout << "query: ";
            std::string q;
            std::getline(std::cin, q);
            SearchQuery query;
            query.q = q;
            std::vector<Doc> res;
            if (!q.starts_with("r ")) {
                res = index.searchInStore(query);
            }
            else {
                query.q = query.q.substr(2);
                res = index.search(query);
            }
            if (res.empty()) {
                std::cout << "no results\n";
                continue;
            }
            int shown = (int) res.size();
            if (shown > 20) shown = 20;
            if ((int) res.size() > shown) {
                std::cout << "... and " << (res.size() - shown) << " more\n";
            }
            for (int i = shown - 1; i >= 0; --i) {
                std::cout << (i + 1) << ") " << res[i].title << "\n";
            }
            std::cout << "select a result to view (0 to go back): ";
            int sel = 0;
            if (!(std::cin >> sel)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (sel >= 1 && sel <= shown) {
                std::cout << "\n" << getDocContent(files, res[sel - 1]) << "\n";
            }

        }  else if (choice == 3) {
            printRegexHelp();
            std::cout << "query: ";
            std::string q;
            std::getline(std::cin, q);
            SearchQuery query;
            query.q = q;
            std::vector<Doc> res;
            if (!q.starts_with("r ")) {
                res = index.searchInStore(query);
            }
            else {
                query.q = query.q.substr(2);
                res = index.search(query);
            }
            if (res.empty()) {
                std::cout << "no results\n";
                continue;
            }
            std::vector<ScoredDoc> ranked = index.rank(res, query, files[0]);
            int shown = (int) ranked.size();
            if (shown > 20) shown = 20;
            if ((int) ranked.size() > shown) {
                std::cout << "... and " << (ranked.size() - shown) << " more\n";
            }
            for (int i = shown - 1; i >= 0; --i) {
                std::cout << (i + 1) << ") " << ranked[i].title << "\n";
            }
            std::cout << "select a result to view (0 to go back): ";
            int sel = 0;
            if (!(std::cin >> sel)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                continue;
            }
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            if (sel >= 1 && sel <= shown) {
                std::cout << "\n" << getDocContent(files, ranked[sel - 1]) << "\n";
            }
        } else if (choice == 4) {
            return;
        }
    }
}
