#pragma once
#include "./Dictionary.h"
#include <cmath>
#include "DynamicArray.h"

template <typename T, typename U>
class ChainedHashDictionary : Dictionary<T, U> {
private:
    int size;
    int n;
    ChainedItem<T, U>** buckets;
    IHashFamily<T>* hash_family;
    double calculate_load_factor() const;
    void rehash(int old_size, int new_size);
    void rehash_add(T key, U val);
public:
    ChainedHashDictionary(int start_size, IHashFamily<T>* hash_family);
    U* add(T key, U val) override;
    U* get(T key) override;
    void remove(T key) override;
    int get_num_items() const { return n; };
    int get_size() const { return size; };
    int get_item_size() { return sizeof(Item<T, U>); }

};

template<typename T, typename U>
ChainedHashDictionary<T, U>::ChainedHashDictionary(int start_size, IHashFamily<T>* hash_family) : size(start_size), n(0) {
    buckets = new ChainedItem<T, U>*[size]();
    this->hash_family = hash_family->clone();
    this->hash_family->get_new_hash();
}

template<typename T, typename U>
double ChainedHashDictionary<T, U>::calculate_load_factor() const {
    return static_cast<double>(n + 1) / size;
}

template<typename T, typename U>
U* ChainedHashDictionary<T, U>::add(T key, U val) {
    if (calculate_load_factor() > 1) rehash(size, size * 2);
    uint64_t index = hash_family->hash(key, size);
    ChainedItem<T, U>* loop_item = buckets[index];
    while (loop_item) {
        if (loop_item->key == key) {
            loop_item->val = std::move(val);
            return &loop_item->val;
        }
        loop_item = loop_item->next;
    }

    ChainedItem<T, U>* new_item = new ChainedItem<T, U>{std::move(key), std::move(val), true, buckets[index]};
    buckets[index] = new_item;
    n++;
    return &buckets[index]->val;
}

template<typename T, typename U>
U* ChainedHashDictionary<T, U>::get(T key) {
    uint64_t index = hash_family->hash(key, size);
    ChainedItem<T, U>* loop_item = buckets[index];
    while (loop_item) {
        if (loop_item->key == key) return &loop_item->val;
        loop_item = loop_item->next;
    }
    return nullptr;
}

template<typename T, typename U>
void ChainedHashDictionary<T, U>::remove(T key) {
    uint64_t index = hash_family->hash(key, size);

    if (buckets[index] && buckets[index]->key == key) {
        ChainedItem<T, U>* to_delete = buckets[index];
        buckets[index] = buckets[index]->next;
        delete to_delete;
        n--;
        return;
    }

    for (ChainedItem<T, U>* item = buckets[index]; item && item->next; item = item->next) {
        if (item->next->key == key) {
            ChainedItem<T, U>* to_delete = item->next;
            item->next = item->next->next;
            delete to_delete;
            n--;
            return;
        }
    }
}

template<typename T, typename U>
void ChainedHashDictionary<T, U>::rehash(int old_size, int new_size) {
    DynamicArray<ChainedItem<T, U>> items(n + 1);
    for (int i = 0; i < old_size; i++) {
        ChainedItem<T, U>* loop_item = buckets[i];
        while (loop_item) {
            items.add(*loop_item);
            loop_item = loop_item->next;
        }
    }

    for (int i = 0; i < old_size; i++) {
        ChainedItem<T, U>* curr = buckets[i];
        while (curr) {
            ChainedItem<T, U>* next = curr->next;
            delete curr;
            curr = next;
        }
    }
    delete[] buckets;

    size = new_size;
    buckets = new ChainedItem<T, U>*[size]();
    hash_family->get_new_hash();
    n = 0;

    for (int i = 0; i < items.n; i++) {
        rehash_add(items[i].key, items[i].val);
    }
}

template<typename T, typename U>
void ChainedHashDictionary<T, U>::rehash_add(T key, U val) {
    uint64_t index = hash_family->hash(key, size);
    ChainedItem<T, U>* new_item = new ChainedItem<T, U>{std::move(key), std::move(val), true, buckets[index]};
    buckets[index] = new_item;
    n++;
}