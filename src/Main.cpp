#include <cstdio>
#include <iostream>
#include <vector>

#include "./indexes/Index12.hpp"
#include "components/test/TestIndeces.cpp"
#include "core/tui.hpp"


int main(int argc, char* argv[]) {
    printf("m a i n \n");
    std::string filename = "data/WestburyLab.wikicorp.201004_1MB.txt";
    Index12 index = Index12();
    tui(index.index);
    return 0;
    int iteration = 1;

    if (argc == 3) {
        filename = argv[1];
        iteration = std::stoi(argv[2]);
    }

    TestIndex5(filename, iteration);
    TestIndex6(filename, iteration);
    TestIndex7(filename, iteration);
    TestIndex8(filename, iteration);
    TestIndex9(filename, iteration);
    TestIndex10(filename, iteration);
    TestIndex11(filename, iteration);

    return 0;
}
