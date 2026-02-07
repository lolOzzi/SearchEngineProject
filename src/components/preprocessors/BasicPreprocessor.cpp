#include "../../core/interfaces.h"

#include <fstream>

class BasicPreprocessor : public IPreprocessor {
public:
    ~BasicPreprocessor() override = default;
    void* preprocess(std::string filename, IStore* store) override;
};

void* BasicPreprocessor::preprocess(std::string filename, IStore *store) {
    std::ifstream file;
    file.open (filename);
    if (!file.good()) {
        printf("Error reading file");
        return nullptr;
    }
    if (!file.is_open()) {
        printf("No file is open");
        return nullptr;
    }

    Doc document{};
    std::string word;
    short take_next = 1;
    long long before_first_word = file.tellg();

    while (file >> word)
    {
        if (take_next) {
            long long after_first_word = file.tellg();
            std::string rest;
            std::getline(file, rest);
            document.title = word.append(rest);
            document.start_loc = before_first_word;
            file.seekg(after_first_word);
        }

        store->add(word, document);
        take_next = word == END;
        if (take_next) {
            before_first_word = file.tellg();
        }
    }
    file.close();
    return nullptr;
}
