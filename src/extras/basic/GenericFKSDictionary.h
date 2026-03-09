#pragma once
#include "./HashFamily.h"
#include <utility>
#include <cassert>

template <typename T, typename U>
class GenericCollisionFree {
private:
    struct Item{
        T key;
        U val;
        bool used = false;

        bool operator==(const Item& other) const {
            return used == other.used && key == other.key;
        }
    };
    int n;
    int size;
    int arr_size;
    Item* arr;
    IHashFamily<T>* hash_family;
    void do_new(int old_arr_size, Item* new_item);
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
    this->arr = new Item[arr_size];
    for (int i = 0; i < arr_size; i++) {
        arr[i] = Item();
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
void GenericCollisionFree<T, U>::do_new(int old_arr_size, Item* new_item) {

    bool did_new_success = false;
    Item* new_arr = new Item[arr_size];
    while (!did_new_success) {
        for (int i = 0; i < arr_size; i++) {
            new_arr[i] = Item();
        }
        hash_family->get_new_hash();
        bool continue_after = false;

        for (int i = 0; i < old_arr_size; i++) {
            if (!arr[i].used) continue;
            uint64_t new_index = hash_family->hash(arr[i].key, arr_size);
            assert(new_index < static_cast<uint64_t>(arr_size));

            if (new_arr[new_index].used) {
                continue_after = true;
                break;
            }
            new_arr[new_index].used = true;
        }

        if (continue_after) {
            continue;
        }
        int new_index = hash_family->hash(new_item->key, arr_size);
        if (new_arr[new_index].used) continue;
        assert(new_index >= 0 && new_index < arr_size);

        new_arr[new_index] = std::move(*new_item);
        did_new_success = true;
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
class GenericFKSDictionary {
private:
    int num_buckets;
    IHashFamily<T>* hash_family;
    GenericCollisionFree<T, U>** buckets;
public:
    GenericFKSDictionary(int num_buckets, IHashFamily<T>* hash_family);
    U* add(T key, U val);
    U* get(T key);
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
    return buckets[index]->add(key, val);
}
template<typename T, typename U>
U *GenericFKSDictionary<T, U>::get(T key) {
    uint64_t index = hash_family->hash(key, num_buckets);
    return buckets[index]->get(key);
}