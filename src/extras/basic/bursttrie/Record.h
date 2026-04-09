#include <algorithm>

#include "../Label.h"

class Record {
public:
    Record() : word("") {}
    Record(const char* str);
    Record(const std::string& str);
    const char* word;
    const char* get_string() const { return word; };
    int size() const;

    Record substring(const int start_index) const;

    char operator[](int index) { return word[index]; }
    const char& operator[](int index) const { return word[index]; }
    bool operator<(const Record& other) const;
    bool operator==(const Record& other) const;
};

inline Record::Record(const char *str) {
    word = str;
}
inline Record::Record(const std::string& str) {
    int len = str.size() + 1;
    char* new_word = new char[len];
    const char* old_word = str.c_str();

    for (int i = 0; i < len; i++)
        new_word[i] = old_word[i];
    word = new_word;
}
inline int Record::size() const {
    int size = 0;
    while (word[size] != '\0') size++;
    return size;
}

inline Record Record::substring(const int start_index) const {
    return {&word[start_index]};
}

inline bool Record::operator<(const Record &other) const {
    int it_num = std::min(size(), other.size()) + 1;
    for (int i = 0; i < it_num; i++) {
        if (word[i] != other.word[i])
            return word[i] < other.word[i];
    }
    assert(false);
    return -1;
}

inline bool Record::operator==(const Record &other) const {
    int size_1 = size();
    if (size_1 != other.size())
        return false;

    for (int i = 0; i < size_1; i++) {
        if (word[i] != other.word[i])
            return false;
    }
    return true;
}


