#pragma once
#include "./HashFamily.h"

template <typename T, typename U>
struct Item{
    T key;
    U val;
    bool used = false;
    bool operator==(const Item& other) const {
        return used == other.used && key == other.key;
    }
};

template <typename T, typename U>
class Dictionary {
private:
public:
    virtual ~Dictionary() = default;
    virtual U* add(T key, U val) = 0;
    virtual U* get(T key) = 0;
    virtual void remove(T key) = 0;
};