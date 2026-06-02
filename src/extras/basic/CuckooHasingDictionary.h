#pragma once
#include "./HashFamily.h"
#include <cmath>
#include "Dictionary.h"
#include "DynamicArray.h"

// Se: https://studwww.itu.dk/~pagh/papers/cuckoo-jour.pdf

constexpr double EPSILON = 0.1;
const double EPSILON_LOG_CONSTANT = std::log(1+EPSILON);

template <typename T, typename U>
class CuckooHashingDictionary : public Dictionary<T, U> {
private:
    int size;
    int n;
    IHashFamily<T>* hash_family1;
    Item<T, U>* T1;
    IHashFamily<T>* hash_family2;
    Item<T, U>* T2;

    int calculate_max_loop() const;
    double calculate_load_factor() const;
    void rehash(int new_size, int old_size);
    void rehash_add(T key, U val); // Add specific to rehash, since some assumptions can be made.
public:
    CuckooHashingDictionary(int start_size, IHashFamily<T>* hash_family);
    U* add(T key, U val) override;
    U* get(T key) override;
    void remove(T key) override;
    int get_num_items() const { return n; };
    int get_size() const { return size; };
    int get_item_size() { return sizeof(Item<T, U>); }
    Item<T,U>** get_all_items();
};
template<typename T, typename U>
CuckooHashingDictionary<T, U>::CuckooHashingDictionary(int start_size, IHashFamily<T>* hash_family) : size(start_size), n(0) {
    T1 = new Item<T, U>[size];
    T2 = new Item<T, U>[size];
    hash_family1 = hash_family->clone();
    hash_family1->get_new_hash();
    hash_family2 = hash_family->clone();
    hash_family2->get_new_hash();
}

template<typename T, typename U>
int CuckooHashingDictionary<T, U>::calculate_max_loop() const {
    return ceil(3*(std::log(n+1)/EPSILON_LOG_CONSTANT));
}

template<typename T, typename U>
double CuckooHashingDictionary<T, U>::calculate_load_factor() const {
    return static_cast<double>(n+1)/(2*size);
}


template<typename T, typename U>
U *CuckooHashingDictionary<T, U>::add(T key, U val) {
    // Lookup
    uint64_t index1 = hash_family1->hash(key, size);
    if (T1[index1].used && T1[index1].key == key) { T1[index1].val = std::move(val); return &T1[index1].val; }
    uint64_t index2 = hash_family2->hash(key, size);
    if (T2[index2].used && T2[index2].key == key) { T2[index2].val = std::move(val); return &T2[index2].val; }

    // Insert
    auto load = calculate_load_factor();
    if (load > 0.5) { rehash(size*2, size); }
    n++;
    Item new_item{key, std::move(val), true};

    int max_loop = calculate_max_loop();
    for (int i = 0; i < max_loop; i++) {
        index1 = hash_family1->hash(new_item.key, size);
        std::swap(new_item, T1[index1]);
        if (new_item.used == false) return get(key);

        index2 = hash_family2->hash(new_item.key, size);
        std::swap(new_item, T2[index2]);
        if (new_item.used == false) return get(key);
    }
    // Failed, so reset and try again
    rehash(size, size);
    add(std::move(new_item.key), std::move(new_item.val));
    return get(key);
}

template<typename T, typename U>
U *CuckooHashingDictionary<T, U>::get(T key) {
    uint64_t index1 = hash_family1->hash(key, size);
    if (T1[index1].used && T1[index1].key == key) return &T1[index1].val;
    uint64_t index2 = hash_family2->hash(key, size);
    if (T2[index2].used && T2[index2].key == key) return &T2[index2].val;
    return nullptr;
}

template<typename T, typename U>
void CuckooHashingDictionary<T, U>::remove(T key) {
    uint64_t index1 = hash_family1->hash(key, size);
    if (T1[index1].used && T1[index1].key == key) { T1[index1] = Item<T, U>(); n--; return; }
    uint64_t index2 = hash_family2->hash(key, size);
    if (T2[index2].used && T2[index2].key == key) { T2[index2] = Item<T, U>(); n--; return; }
}


template<typename T, typename U>
void CuckooHashingDictionary<T, U>::rehash(int new_size, int old_size) {
    DynamicArray<Item<T, U>> items = DynamicArray<Item<T, U>>(old_size);
    for (int i = 0; i < old_size; i++) {
        if (T1[i].used) items.add(std::move(T1[i]));
        if (T2[i].used) items.add(std::move(T2[i]));
    }
    delete[] T1;
    delete[] T2;
    size = new_size;
    T1 = new Item<T, U>[size];
    T2 = new Item<T, U>[size];
    hash_family1->get_new_hash();
    hash_family2->get_new_hash();
    n = 0;
    for (int i = 0; i < items.n; i++) {
        rehash_add(std::move(items[i].key), std::move(items[i].val));
    }
}

template<typename T, typename U>
void CuckooHashingDictionary<T, U>::rehash_add(T key, U val) {
    n++;
    Item new_item{key, std::move(val), true};

    int max_loop = calculate_max_loop();
    for (int i = 0; i < max_loop; i++) {
        uint64_t index1 = hash_family1->hash(new_item.key, size);
        std::swap(new_item, T1[index1]);
        if (new_item.used == false) return;

        uint64_t index2 = hash_family2->hash(new_item.key, size);
        std::swap(new_item, T2[index2]);
        if (new_item.used == false) return;
    }

    rehash(size, size);
    add(std::move(new_item.key), std::move(new_item.val));
}

template<typename T, typename U>
Item<T,U>** CuckooHashingDictionary<T, U>::get_all_items() {
    Item<T,U>** items = new Item<T, U>*[n];
    int items_pos = 0;
    for (int i = 0; i < size; i++) {
        if (T1[i].used) {
            items[items_pos] = &T1[i];
            items_pos++;
        }
        if (T2[i].used) {
            items[items_pos] = &T2[i];
            items_pos++;
        }
    }

    return items;
}
