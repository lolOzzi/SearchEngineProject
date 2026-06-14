#pragma once

#include <algorithm>
#include <cstring>
#include <string>

class Record {
private:
    char* word;
    int length;

    void deep_copy(const char* str, int len) {
        length = len;
        word = new char[length + 1];
        for (int i = 0; i < length; i++) {
            word[i] = str[i];
        }
        word[length] = '\0';
    }

public:
    Record() : word(new char[1]{'\0'}), length(0) {}

    Record(const char* str) {
        if (str == nullptr) {
            word = new char[1]{'\0'};
            length = 0;
        } else {
            int len = 0;
            while (str[len] != '\0') len++;
            deep_copy(str, len);
        }
    }

    Record(const std::string& str) {
        deep_copy(str.c_str(), str.size());
    }

    Record(const Record& other) {
        deep_copy(other.word, other.length);
    }

    Record& operator=(const Record& other) {
        if (this != &other) {
            delete[] word; // Free existing memory
            deep_copy(other.word, other.length);
        }
        return *this;
    }

    ~Record() {
        delete[] word;
    }

    const char* get_string() const { return word; }
    int size() const { return length; }

    Record substring(const int start_index) const {
        if (start_index >= length) return Record("");
        return Record(&word[start_index]);
    }

    char operator[](int index) const { return word[index]; }

    bool operator<(const Record& other) const {
        int min_len = std::min(length, other.length);
        for (int i = 0; i < min_len; i++) {
            if (word[i] != other.word[i]) {
                return word[i] < other.word[i];
            }
        }
        return length < other.length;
    }

    bool operator==(const Record& other) const {
        if (length != other.length) return false;
        for (int i = 0; i < length; i++) {
            if (word[i] != other.word[i]) return false;
        }
        return true;
    }
};