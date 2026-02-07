#include "../../core/interfaces.h"

#include <fstream>

class BasicPreprocessorHandlesWhitespaceChars : public IPreprocessor {
public:
    ~BasicPreprocessorHandlesWhitespaceChars() override = default;
    void* preprocess(std::string filename, IStore* store) override;
};



void* BasicPreprocessorHandlesWhitespaceChars::preprocess(std::string filename, IStore *store) {
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
    std::string line;
    short take_next = 1;
    long long before_first_word = file.tellg();
    //std::vector<std::string> patterns = {"\xE3\x80\x80", "\xE2\x80\x89"};

    int dbcounter = 0;
    while (std::getline(file, line)) {
        std::vector<std::string> words;
        getWordsFromLine(line, words);

        for (int i = 0;i < words.size(); ++i) {
            word = words[i];
            if (take_next) {
                std::string full_line = word;
                for (int wc = 1; wc < words.size(); ++wc) full_line += " " + words[wc];
                document.title = full_line;
                char last_char_title = document.title.back();
                if (last_char_title == ',' || last_char_title == '.' || last_char_title == '?') {
                    document.title.erase(document.title.size() - 1);
                }
                document.start_loc = before_first_word;
            }
            char last_char = word.back();
            if (last_char == ',' || last_char == '.' || last_char == '?') {
                word.erase(word.size() - 1);
            }
            store->add(word, document);
            take_next = word == END;
        }
        before_first_word = file.tellg();

    }
    std::cout << dbcounter;
    file.close();
    return nullptr;
}


