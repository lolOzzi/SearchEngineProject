#include <memory>
#include <functional>
#include <stdexcept>
#include <utility>
#include <iostream>

template<typename T, typename Compare = std::less<T>>
class SortedDynamicArray {
private:
    int size_;
    int capacity_;
    T* arr_;
    Compare cmp_;

    void reserve(int new_capacity) {
        if (new_capacity <= capacity_) return;
        T* new_arr = new T[new_capacity];
        for (int i = 0; i < size_; ++i)
            new_arr[i] = std::move(arr_[i]);
        delete[] arr_;
        arr_ = new_arr;
        capacity_ = new_capacity;
    }


    void insert_at_index(int index, T elem) {
        for (int i = size_; i > index; --i)
            arr_[i] = std::move(arr_[i - 1]);
        arr_[index] = std::move(elem);
        ++size_;
    }

public:
    explicit SortedDynamicArray(int init_capacity = 8, Compare cmp = Compare())
        : size_(0),
          capacity_(init_capacity > 0 ? init_capacity : 1),
          arr_(new T[capacity_]),
          cmp_(std::move(cmp))
    {}

    ~SortedDynamicArray() { delete[] arr_; }

    SortedDynamicArray(const SortedDynamicArray&) = delete;
    SortedDynamicArray& operator=(const SortedDynamicArray&) = delete;

    SortedDynamicArray(SortedDynamicArray&& other) noexcept
        : size_(other.size_), capacity_(other.capacity_), arr_(other.arr_), cmp_(std::move(other.cmp_))
    {
        other.arr_ = nullptr;
        other.size_ = other.capacity_ = 0;
    }
    int find_insert_index(const T& elem) const {
        int lo = 0;
        int hi = size_;
        while (lo < hi) {
            int mid = lo + (hi - lo) / 2;
            if (cmp_(elem, arr_[mid])) hi = mid;
            else lo = mid + 1;
        }
        return lo;
    }

    SortedDynamicArray& operator=(SortedDynamicArray&& other) noexcept {
        if (this == &other) return *this;
        delete[] arr_;
        arr_ = other.arr_;
        size_ = other.size_;
        capacity_ = other.capacity_;
        cmp_ = std::move(other.cmp_);
        other.arr_ = nullptr;
        other.size_ = other.capacity_ = 0;
        return *this;
    }

    void push_back(T elem) {
        if (size_ + 1 > capacity_) reserve(capacity_ ? capacity_ * 2 : 1);
        if (size_ == 0) {
            arr_[0] = std::move(elem);
            ++size_;
            return;
        }
        int idx = find_insert_index(elem);
        insert_at_index(idx, std::move(elem));
    }

    const T& operator[](int index) const {
        if (index < 0 || index >= size_) throw std::out_of_range("Index out of bounds");
        return arr_[index];
    }

    T& operator[](int index) {
        if (index < 0 || index >= size_) throw std::out_of_range("Index out of bounds");
        return arr_[index];
    }

    int size() const noexcept { return size_; }
    int capacity() const noexcept { return capacity_; }
};