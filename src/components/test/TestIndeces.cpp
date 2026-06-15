#include <chrono>
#include <cstring>
#include <locale>

#include "Index10.hpp"
#include "Index11.hpp"
#include "Index12.hpp"
#include "Index5.h"
#include "index6.hpp"
#include "index7.hpp"
#include "Index8.hpp"
#include "Index9.hpp"
#include "core/interfaces.h"
#include "test.cpp"

long long ParseLine(char* line) {
    int len = std::strlen(line);
    const char* p = line;

    while (*p < '0' || *p > '9') p++;
    line[len - 3] = '\0';
    long long val = atoll(p);

    return val;
}

long long GetRamUsage() {
    FILE* file = fopen("/proc/self/status", "r");
    long long result = -1;
    char line[128];

    if (file == NULL) {
        return -1;
    }
    while (fgets(line, sizeof(line), file) != NULL) {
        // Change from VmSize to VmRSS
        if (strncmp(line, "VmRSS:", 6) == 0) {
            // Safely parse the integer ignoring the " kB" at the end
            if (sscanf(line, "VmRSS: %lld", &result) == 1) {
                break;
            }
        }
    }

    fclose(file);
    return result; // Returns KB
}

long long GetIndexTestPreprocessingTime(Index* index, std::string fileName) {
    auto tPreprocessStart = std::chrono::steady_clock::now();
    index->preprocess(fileName);
    auto tPreprocessEnd = std::chrono::steady_clock::now();
    auto tPreprocessing = duration_cast<std::chrono::nanoseconds>(tPreprocessEnd - tPreprocessStart).count();
    return tPreprocessing;
}

long long GetIndexTestQueryTime(Index* index, std::string fileName) {
    auto queryTime = test_time_of_search(index, fileName, 1);
    return queryTime;
}

void WriteToFileIndex5(std::string fileName, std::string outputFileName) {
    Index5 index5 = Index5();
    // Linux only malloc_trim(0);
    auto initalRamUsage = GetRamUsage();
    auto tPreprocessStart = std::chrono::steady_clock::now();
    index5.preprocess(fileName);
    // Linux only malloc_trim(0);
    auto tPreprocessEnd = std::chrono::steady_clock::now();
    auto preprocessingTime = duration_cast<std::chrono::nanoseconds>(tPreprocessEnd - tPreprocessStart).count();
    auto queryTime = 0;//test_time_of_search_index5(&index5, fileName, 1);
    auto currentRamUsage = GetRamUsage();
    auto ramUsageOfIndex = currentRamUsage - initalRamUsage;
    std::cout << std::endl << ramUsageOfIndex << std::endl;

    std::ofstream outFile("results/" + outputFileName);

    if (outFile.is_open()) {
        outFile << ramUsageOfIndex << "\n";
        outFile << preprocessingTime  << "\n";
        outFile << queryTime << "\n";
        outFile.close();

        std::cout << "Successfully wrote three lines to the file." << std::endl;
    } else {
        std::cout << "Error: Unable to open file." << std::endl;
    }
}

void WriteToFile(Index* index, std::string fileName, std::string outputFileName, bool withQueryTime = true) {
    // Linux only malloc_trim(0);
    auto initalRamUsage = GetRamUsage();
    auto preprocessingTime = GetIndexTestPreprocessingTime(index, fileName);
    std::cout << std::endl << "after prepro" << std::endl;
    long long queryTime = 0;
    if (withQueryTime)
    {
        queryTime = GetIndexTestQueryTime(index, fileName);
    }
    // Linux only malloc_trim(0);
    auto currentRamUsage = GetRamUsage();
    auto ramUsageOfIndex = currentRamUsage - initalRamUsage;
    std::cout << ramUsageOfIndex << std::endl;

    std::ofstream outFile("results/" + outputFileName);

    if (outFile.is_open()) {
        outFile << ramUsageOfIndex << "\n";
        outFile << preprocessingTime  << "\n";
        outFile << queryTime << "\n";
        outFile.close();

        std::cout << "Successfully wrote three lines to the file." << std::endl;
    } else {
        std::cout << "Error: Unable to open file." << std::endl;
    }
}

std::string CleanFileName(std::string fileName) {
    std::string target = "wikicorp.";
    size_t pos = fileName.find(target);
    std::string result = fileName.substr(pos + target.length());
    return result;
}

void TestIndex5(std::string fileName, int iteration) {
    std::string clean = CleanFileName(fileName);
    WriteToFileIndex5(fileName, "Index5_" + std::to_string(iteration) + "_" + clean);
}
void TestIndex6(std::string fileName, int iteration) {
    std::string clean = CleanFileName(fileName);
    Index6 index = Index6();
    WriteToFile(&index.index, fileName, "Index6_" + std::to_string(iteration) + "_" + clean);
}
void TestIndex7(std::string fileName, int iteration, bool withQueryTime = false) {
    std::string clean = CleanFileName(fileName);
    Index7 index = Index7();
    WriteToFile(&index.index, fileName, "Index7_" + std::to_string(iteration) + "_" + clean, withQueryTime);
}
void TestIndex8(std::string fileName, int iteration) {
    std::string clean = CleanFileName(fileName);
    Index8 index = Index8();
    WriteToFile(&index.index, fileName, "Index8_" + std::to_string(iteration) + "_" + clean, true);
}
void TestIndex9(std::string fileName, int iteration) {
    std::string clean = CleanFileName(fileName);
    Index9 index = Index9();
    WriteToFile(&index.index, fileName, "Index9_" + std::to_string(iteration) + "_" + clean, true);
}
void TestIndex10(std::string fileName, int iteration) {
    std::string clean = CleanFileName(fileName);
    Index10 index = Index10();
    WriteToFile(&index.index, fileName, "Index10_" + std::to_string(iteration) + "_" + clean, true);
}
void TestIndex11(std::string fileName, int iteration) {
    std::string clean = CleanFileName(fileName);
    Index11 index = Index11();
    WriteToFile(&index.index, fileName, "Index11_" + std::to_string(iteration) + "_" + clean, true);
}

void TestIndex12(std::string fileName, int iteration) {
    std::string clean = CleanFileName(fileName);
    Index12 index = Index12();
    WriteToFile(&index.index, fileName, "Index11_" + std::to_string(iteration) + "_" + clean, true);
}