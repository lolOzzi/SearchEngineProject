#include "../../core/interfaces.h"

#include <fstream>

class BasicPreprocessorHandlesPunctuation : public IPreprocessor {
public:
    ~BasicPreprocessorHandlesPunctuation() override = default;
    void* preprocess(std::string filename, IStore* store) override;
};

void* BasicPreprocessorHandlesPunctuation::preprocess(std::string filename, IStore *store) {
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
            char last_char_title = document.title.back();
            if (last_char_title == ',' || last_char_title == '.' || last_char_title == '?') {
                document.title.erase(document.title.size() - 1);
            }
            document.start_loc = before_first_word;
            file.seekg(after_first_word);
        }
        char last_char = word.back();
        if (last_char == ',' || last_char == '.' || last_char == '?') {
            word.erase(word.size() - 1);
        }

        store->add(word, document);
        if ((take_next = word == END)) {
            before_first_word = file.tellg();
        }
    }
    file.close();
    return nullptr;
}


