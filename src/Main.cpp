#include <cstdio>
#include <iostream>
#include <vector>

#include "extras/basic/recsplit/RecSplit.h"
#include "indexes/Index9.hpp"
//#include "indexes/Index8.hpp"
#include "components/test/test.cpp"
/*
*/


int main(int argc, char* argv[]) {
    printf("m a i n \n");
    std::string filename = "data/WestburyLab.wikicorp.201004.txt";

    Index9 index;
    index.preprocess(filename);

    printf("done \n");
    std::string n = "";
    while (true) {
        std::cin >> n;
        SearchQuery tmp;
        tmp.q = n;
        std::vector<Doc> res = index.search(tmp);
        for (std::vector<Doc>::iterator it = res.begin(); it != res.end(); ++it) {
            std::cout << it->title << " "  << std::endl;
        }
    }

    return 0;
}
