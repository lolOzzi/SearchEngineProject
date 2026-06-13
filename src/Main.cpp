#include <cstdio>
#include <iostream>
#include <vector>

#include "components/test/TestIndeces.cpp"
/*
*/


int main(int argc, char* argv[]) {
    printf("m a i n \n");
    std::string filename = "data/WestburyLab.wikicorp.201004_1MB.txt";
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
    /*
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
    */
    return 0;
}
