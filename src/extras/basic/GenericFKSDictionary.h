#pragma once
#include "./HashFamily.h"
#include <utility>
#include <cassert>
#include "Dictionary.h"

template <typename T, typename U>
class GenericCollisionFree {
private:
    int n;
    int size;
    int arr_size;
    Item<T, U> * arr;
    IHashFamily<T>* hash_family;
    void do_new(int old_arr_size, Item<T, U>* new_item);
public:
    ~GenericCollisionFree() = default;
    GenericCollisionFree(int size, IHashFamily<T>* hash_family);
    U* add(T key, U val);
    U* get(T key);
    bool remove(T &key);
    int get_num_elements();
};

template<typename T, typename U>
GenericCollisionFree<T, U>::GenericCollisionFree(int size, IHashFamily<T>* hash_family) {
    this->n = 0;
    this->size = size;
    arr_size = size*size;
    this->arr = new Item<T, U>[arr_size];
    for (int i = 0; i < arr_size; i++) {
        arr[i] = Item<T, U>();
    }
    this->hash_family = hash_family->clone();
    this->hash_family->get_new_hash();
}

template<typename T, typename U>
U *GenericCollisionFree<T, U>::get(T key) {
    uint64_t index = hash_family->hash(key, arr_size);
    if (!arr[index].used) return nullptr;
    if (arr[index].key != key) return nullptr;
    return &arr[index].val;
}

template<typename T, typename U>
U* GenericCollisionFree<T,U>::add(T key, U val) {

    Item new_item{key, std::move(val), true};
    if (n >= size) {
        int old_size = arr_size;
        size = size*2;
        arr_size = size*size;
        do_new(old_size, &new_item);
        ++n;
        return get(key);
    }

    auto index = hash_family->hash(key, arr_size);
    if (arr[index].used) {
        if (arr[index].key == key) {
            arr[index].val = std::move(new_item.val);
            return &arr[index].val;
        } else {
            do_new(arr_size, &new_item);
            ++n;
            return get(key);
        }
    }

    arr[index] = std::move(new_item);
    ++n;
    return &arr[index].val;
}

template<typename T, typename U>
bool GenericCollisionFree<T, U>::remove(T &key) {
    uint64_t index = hash_family->hash(key, arr_size);
    assert(index < static_cast<uint64_t>(arr_size));
    if (!arr[index].used) return false;
    if (!(arr[index].key == key)) return false;

    arr[index].used = false;
    --n;
    return true;
}

template<typename T, typename U>
void GenericCollisionFree<T, U>::do_new(int old_arr_size, Item<T, U>* new_item) {
    Item<T, U>* new_arr = new Item<T, U>[arr_size];

    while (true) {
        hash_family->get_new_hash();

        // Phase 1: probe only — no moves, just check for collisions
        bool collision = false;
        bool* slots = new bool[arr_size]();

        for (int i = 0; i < old_arr_size; i++) {
            if (!arr[i].used) continue;
            uint64_t idx = hash_family->hash(arr[i].key, arr_size);
            if (slots[idx]) { collision = true; break; }
            slots[idx] = true;
        }

        if (!collision) {
            uint64_t idx = hash_family->hash(new_item->key, arr_size);
            if (slots[idx]) collision = true;
        }

        delete[] slots;
        if (collision) continue;

        // Phase 2: layout confirmed — now safe to move everything once
        for (int i = 0; i < old_arr_size; i++) {
            if (!arr[i].used) continue;
            uint64_t idx = hash_family->hash(arr[i].key, arr_size);
            new_arr[idx] = std::move(arr[i]);
        }
        uint64_t idx = hash_family->hash(new_item->key, arr_size);
        new_arr[idx] = std::move(*new_item);

        delete[] arr;
        arr = new_arr;
        return;
    }

    for (int i = 0; i < old_arr_size; i++) {
        if (!arr[i].used) continue;
        uint64_t new_index = hash_family->hash(arr[i].key, arr_size);
        assert(new_index < static_cast<uint64_t>(arr_size));
        new_arr[new_index] = std::move(arr[i]);
    }
    delete[] arr;
    arr = new_arr;

}
template<typename T, typename U>
int GenericCollisionFree<T, U>::get_num_elements() {
  return n;
};


template <typename T, typename U>
class GenericFKSDictionary : Dictionary<T, U> {
private:
    int num_buckets;
    IHashFamily<T>* hash_family;
    GenericCollisionFree<T, U>** buckets;
public:
    GenericFKSDictionary(int num_buckets, IHashFamily<T>* hash_family);
    U* add(T key, U val) override;
    U* get(T key) override;
    void remove(T key) override;
};
template<typename T, typename U>
GenericFKSDictionary<T, U>::GenericFKSDictionary(int num_buckets, IHashFamily<T>* hash_family) {
    this->num_buckets = num_buckets;
    this->hash_family = hash_family;
    buckets = new GenericCollisionFree<T, U>*[num_buckets];
    for (int i = 0; i < num_buckets; i++) {
        buckets[i] = new GenericCollisionFree<T, U>(4, hash_family);
    }
}
template<typename T, typename U>
U *GenericFKSDictionary<T, U>::add(T key, U val) {
    uint64_t index = hash_family->hash(key, num_buckets);
    return buckets[index]->add(std::move(key), std::move(val));
}
template<typename T, typename U>
U *GenericFKSDictionary<T, U>::get(T key) {
    uint64_t index = hash_family->hash(key, num_buckets);
    return buckets[index]->get(key);
}

template<typename T, typename U>
void GenericFKSDictionary<T, U>::remove(T key) {
    uint64_t index = hash_family->hash(key, num_buckets);
    buckets[index]->remove(key);
}


