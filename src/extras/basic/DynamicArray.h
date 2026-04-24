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

    DynamicArray() : size(4), arr(new T[4]), n(0) {}
    explicit DynamicArray(int base_size);

    // 2. Copy constructor (your version)
    DynamicArray(const DynamicArray& other) : size(other.size), arr(new T[other.size]), n(other.n) { for (int i = 0; i < other.n; i++) {arr[i] = other.arr[i]; } }
    // 3. Copy assignment
    DynamicArray& operator=(const DynamicArray& other) {
        if (this != &other) {
            delete[] arr;
            size = other.size;
            n = other.n;
            arr = new T[size];
            for (int i = 0; i < n; i++) arr[i] = other.arr[i];
        }
        return *this;
    }
    // 4. Move constructor
    DynamicArray(DynamicArray&& other)  noexcept : size(other.size), arr(other.arr), n(other.n) { other.arr = nullptr; other.size = 0; other.n = 0; }
    // 5. Move assignment
    DynamicArray& operator=(DynamicArray&& other) noexcept {
        if (this != &other) {
            delete[] arr;
            size = other.size;
            n = other.n;
            arr = other.arr;
            other.arr = nullptr; other.size = 0; other.n = 0;
        }
        return *this;
    }

    ~DynamicArray(){ delete[] arr; }

    T& operator[](int index) { return arr[index]; }
    const T& operator[](int index) const { return arr[index]; }
};

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
    arr[n] = std::move(key);
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
