#include "indexes/index8.h"
#include <cstdio>
#include <iostream>
#include <thread>
/*
*/

int main(int argc, char* argv[]) {
    printf("m a i n \n");
    std::string filename = "data/WestburyLab.wikicorp.201004_10MB.txt";

    Index8 index;
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
