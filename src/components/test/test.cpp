#include <fstream>
#include <iostream>
#include <algorithm>

#include "../../core/interfaces.h"
#include <map>
#include <sstream>
#include <regex>
#include <cstdlib>
#include <chrono>

std::map<std::string, std::vector<std::string>> load_test(std::string data_filename) {

    std::map<std::string, std::vector<std::string>> results = std::map<std::string, std::vector<std::string>>{};
    std::string partial_test_path = data_filename.substr(5);
    std::string filename = "src/components/test/TEST_";
    filename += partial_test_path;
    std::ifstream file;

    file.open (filename);
    if (!file.is_open()) {
        printf("Running Java Program To Produce Test File\n");
        system("javac src/extras/java/Test.java");
        system(("java src/extras/java/Test.java " + data_filename).c_str());
        printf("Finished Running Java Program\n");
        file.open (filename);
    }

    if (!file.good()) {
        printf("Error reading file\n");
        return results;
    }

    std::string word;
    std::string line;
    std::string end = "---END.OF.WORD---";

    std::vector<std::string> curr_vector;

    short take_next = 1;

    while (std::getline(file, line))
    {
        if (line == end) {
            take_next = 1;
            continue;
        }
        if (take_next) {
            results.insert({word, curr_vector});
            word = line;
            curr_vector = std::vector<std::string>{};
            take_next = 0;
            continue;
        }

        curr_vector.push_back(line);

    }
    results.insert({word, curr_vector});
    file.close();

    return results;
}

void CompareResult(std::string query, std::vector<Doc> index_res, std::vector<std::string> benchmark_res) {

    std::vector<std::string> index_res_as_strings = std::vector<std::string>{};
    for (const Doc& doc : index_res) {
        index_res_as_strings.push_back(doc.title);
    }
    std::sort(index_res_as_strings.begin(), index_res_as_strings.end());
    std::sort(benchmark_res.begin(), benchmark_res.end());


    for (int i = 0; i < benchmark_res.size(); i++) {
        if (benchmark_res[i] == " [Pivot] ++ % Use the anonymous fun (here named 'Smaller') to test the 'Pivo") continue;
        if (i >= index_res_as_strings.size()) {
            if (!std::binary_search(index_res_as_strings.begin(), index_res_as_strings.end(), benchmark_res[i])) {
                if (std::find(index_res_as_strings.begin(), index_res_as_strings.end(), benchmark_res[i]) == index_res_as_strings.end()) {
                    printf("On Query %s\nWas supposed to have ", query.c_str());
                    std::cout << benchmark_res[i] << std::endl;
                }
            }
            continue;
        }

        if (benchmark_res[i] != index_res_as_strings[i]) {
            if (!std::binary_search(index_res_as_strings.begin(), index_res_as_strings.end(), benchmark_res[i])) {
                if (std::find(index_res_as_strings.begin(), index_res_as_strings.end(), benchmark_res[i]) == index_res_as_strings.end()) {
                    printf("On Query %s\nWas supposed to have ", query.c_str());
                    std::cout << benchmark_res[i] << std::endl;
                }
            }
        }
    }
}

std::map<std::string, std::vector<std::string>> results = std::map<std::string, std::vector<std::string>>{};
void test_correctness(Index* index, std::string filename) {
    if (results.empty()) results = load_test(filename);
    std::vector<std::string> some = std::vector<std::string>{};
    int count = 0;
    std::regex re(R"([0-9A-Z\"\(\)\$'\,#\;:\-\.\/!\*\?\<┬\+&\%=\>\@╬\[\]\\\^\_\`\├ù\Õ\Ó\┘\▒\Ç\©\Î\ê])");
    for (std::map<std::string, std::vector<std::string>>::iterator it = results.begin(); it != results.end(); ++it) {
        if (it->first.empty()) continue;
        if (std::regex_search(it->first, re)) continue;
        if (it->first.length() < 2) continue;
        count++;
        if (count > 100000) break;
        some.push_back(it->first);
    }
    std::cout << count << std::endl;
    SearchQuery tmp;
    int i = 0;
    for (const std::string& s : some) {
        tmp.q = s;
        auto res = index->search(tmp);
        CompareResult(s, res, results.at(s));
        /*
        i++;
        if (i % 1000 == 0)
            printf("At test nr %d its word %s\n", i, s.c_str());
        */
    }
}

void test_time_of_search(Index* index1, Index* index2, std::string filename, int iterations) {
    if (results.empty()) results = load_test(filename);
    std::vector<std::string> some = std::vector<std::string>{};
    int count = 0;
    std::regex re(R"([0-9A-Z\"\(\)\$'\,#\;:\-\.\/!\*\?\<┬\+&\%=\>\@╬\[\]\\\^\_\`\├ù\Õ\Ó\┘\▒\Ç\©\Î\ê])");
    for (std::map<std::string, std::vector<std::string>>::iterator it = results.begin(); it != results.end(); ++it) {
        if (it->first.empty()) continue;
        if (std::regex_search(it->first, re)) continue;
        if (it->first.length() < 2) continue;
        count++;
        if (count > 100000) break;
        some.push_back(it->first);
    }
    SearchQuery tmp;
    std::cout << "Will do " << count*iterations << " queries." << std::endl;
    printf("Searching Index 1\n");
    auto t0_i1 = std::chrono::steady_clock::now();
    for (int i = 0; i < iterations; i++) {
        for (const std::string& s : some) {
            tmp.q = s;
            auto res = index1->search(tmp);
        }
    }
    auto t1_i1 = std::chrono::steady_clock::now();
    auto i1_elapsed = duration_cast<std::chrono::nanoseconds>(t1_i1 - t0_i1).count();

    printf("Searching Index 2\n");
    auto t0_i2 = std::chrono::steady_clock::now();
    for (int i = 0; i < iterations; i++) {
        for (const std::string& s : some) {
            tmp.q = s;
            auto res = index2->search(tmp);
        }
    }
    auto t1_i2 = std::chrono::steady_clock::now();
    auto i2_elapsed = duration_cast<std::chrono::nanoseconds>(t1_i2 - t0_i2).count();

    std::cout << "Index1 used " << i1_elapsed << " nanoseconds." << std::endl;
    std::cout << "Index2 used " << i2_elapsed << " nanoseconds." << std::endl;
    auto diff = i1_elapsed - i2_elapsed;
    std::cout << "Difference between i1 and i2 is " << diff << " nanoseconds." << std::endl;
    double percent = ((double)diff / i2_elapsed)*100;
    std::cout << "Difference in percent between i1 and i2 is " << percent << "%." << std::endl;
}

void test_time_of_preprocess(Index* index1, Index* index2, std::string filename) {
    printf("Preprocessing Index 1\n");
    auto t0_i1 = std::chrono::steady_clock::now();
    index1->preprocess(filename);
    auto t1_i1 = std::chrono::steady_clock::now();
    auto i1_elapsed = duration_cast<std::chrono::nanoseconds>(t1_i1 - t0_i1).count();

    printf("Preprocessing Index 2\n");
    auto t0_i2 = std::chrono::steady_clock::now();
    index2->preprocess(filename);
    auto t1_i2 = std::chrono::steady_clock::now();
    auto i2_elapsed = duration_cast<std::chrono::nanoseconds>(t1_i2 - t0_i2).count();

    std::cout << "Index1 used " << i1_elapsed << " nanoseconds." << std::endl;
    std::cout << "Index2 used " << i2_elapsed << " nanoseconds." << std::endl;
    auto diff = i1_elapsed - i2_elapsed;
    std::cout << "Difference between i1 and i2 is " << diff << " nanoseconds." << std::endl;
    double percent = ((double)diff / i2_elapsed)*100;
    std::cout << "Difference in percent between i1 and i2 is " << percent << "%." << std::endl;
}