#include <cstdio>
#include <iostream>
#include <vector>

#include "indexes/Index5.h"
#include "indexes/Index6.hpp"
#include "indexes/Index7.hpp"
#include "indexes/Index8.hpp"
#include "indexes/Index9.hpp"
#include "indexes/Index10.hpp"
#include "indexes/Index11.hpp"
/*
*/


int main(int argc, char* argv[]) {
    printf("m a i n \n");
    std::string filename = "data/WestburyLab.wikicorp.201004_100MB.txt";

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
