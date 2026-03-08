#pragma once
#include <cstring>
#include <type_traits>
#include <utility>

template <typename T>
class DynamicArray {
private:
    int size;
    T* arr;
    void double_up();
public:
    int n;
    void add(T key);
    DynamicArray(int base_size);
    //~DynamicArray();
    T& operator[](int index) { return arr[index]; }
    const T& operator[](int index) const { return arr[index]; }
};

/*template<typename T>
DynamicArray<T>::~DynamicArray() {
    delete[] arr;
}*/

template<typename T>
DynamicArray<T>::DynamicArray(int base_size) {
    size = base_size;
    n = 0;
    arr = new T[size];
}

template<typename T>
void DynamicArray<T>::add(T key) {
    if (n >= size) {
        double_up();
    }
    arr[n] = key;
    n++;
}

template<typename T>
void DynamicArray<T>::double_up() {
    T* new_arr = new T[size * 2];
    if constexpr (std::is_trivially_copyable_v<T>) {
        std::memcpy(new_arr, arr, n * sizeof(T));
    } else {
        for (int i = 0; i < n; i++) {
            new (new_arr + i) T(std::move(arr[i]));
        }
    }
    delete[] arr;
    arr = new_arr;
    size = size * 2;
}