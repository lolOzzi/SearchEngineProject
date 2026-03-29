#pragma once

#include "ChaniedHashDictionary.h"
#include "CuckooHasingDictionary.h"


class AlphaMap {
private:
    IHashFamily<char>* hash_family;
    ChainedHashDictionary<char, int> dict;
public:
    int n;
    AlphaMap(const int start_size) :
        hash_family(new CharHashFamily()),
        dict(ChainedHashDictionary<char, int>(start_size, hash_family)),
        n(0)
    {}

    int get(const char c) {
        const int* val = dict.get(c);
        if (val == nullptr) return add(c);
        return *val;
    }
    int add(const char c) {
        n++;
        return *dict.add(c, n);
    }


};