#pragma once
#include <cstring>
#include <type_traits>
#include <utility>
#include <vector>
#define MSB 0x8000000000000000
template <typename T>
class DynamicPackedArray {
private:
    int size;
    T* arr;
    void double_up();
public:
    int n;
    void add(T key);
    void copy_elements_to_vector(std::vector<T> &res);
    DynamicPackedArray(int base_size);
    ~DynamicPackedArray();
    DynamicPackedArray(const DynamicPackedArray&) = delete;
	DynamicPackedArray& operator=(const DynamicPackedArray&) = delete;

};
template<typename T>
DynamicPackedArray<T>::~DynamicPackedArray() {
	delete[] arr;
}

template<typename T>
DynamicPackedArray<T>::DynamicPackedArray(int base_size) {
    size = base_size;
    n = 0;
    arr = new T[size];
}

template<typename T>
void DynamicPackedArray<T>::add(T key) {
    if (n >= size) {
        double_up();
    }
    arr[n] = key;
    n++;
}


template<typename T>
void copy_elements_to_vector(std::vector<T> &res) {
  int curr_start = 0;
  for (int i = 0; i < n; ++i)
}


template<typename T>
void DynamicPackedArray<T>::double_up() {
    T* new_arr = new T[size*2];
    if constexpr (std::is_trivially_copyable_v<T>) {
        std::memcpy(new_arr, arr, n * sizeof(T));
    } else {
        for (int i = 0; i < n; i++) {
            new (new_arr + i) T(std::move(arr[i]));
        }
    }
    delete[] arr;
    arr = new_arr;
    size = size*2;
}

