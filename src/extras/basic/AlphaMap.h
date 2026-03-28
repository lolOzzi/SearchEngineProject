#pragma once

#include "ChaniedHashDictionary.h"
#include "CuckooHasingDictionary.h"


class AlphaMap {
private:
    int n;
    IHashFamily<char>* hash_family;
    ChainedHashDictionary<char, int> dict;
public:
    AlphaMap(const int start_size) :
        n(0),
        hash_family(new CharHashFamily()),
        dict(ChainedHashDictionary<char, int>(start_size, hash_family))
    {}

    int get(const char c) {
        int* val = dict.get(c);
        if (val == nullptr) return add(c);
        return *val;
    }
    int add(const char c) {
        n++;
        return *dict.add(c, n);
    }


};