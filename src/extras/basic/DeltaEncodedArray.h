#pragma once
#include "DynamicPackedArray.h"

#include <chrono>
#include <limits>
#include <cstdint>
#include <vector>

template <typename T>
concept UnsignedIntDEA = std::numeric_limits<T>::is_integer && !std::numeric_limits<T>::is_signed;

class DeltaEncodedArray {
private:
    DynamicPackedArray arr;
    uint32_t last_added_elem = 0;
public:
    DeltaEncodedArray() {
        arr = DynamicPackedArray{};
    };
    ~DeltaEncodedArray() = default;

    DeltaEncodedArray(const DeltaEncodedArray&) = default;
    DeltaEncodedArray& operator=(const DeltaEncodedArray&) = default;
    DeltaEncodedArray(DeltaEncodedArray&&) = default;
    DeltaEncodedArray& operator=(DeltaEncodedArray&&) = default;
    uint32_t get_val(int index);
    template<UnsignedIntDEA T>void add(T elem);
    void copy_elements_to_vector(std::vector<uint32_t> &res);
    int get_elem_count();

};

uint32_t DeltaEncodedArray::get_val(int index) {
    std::vector<uint32_t> all_vals;
    arr.copy_elements_to_vector(all_vals);
    if (all_vals.size() <= index) return 0;
    uint32_t sum = 0;
    for (int i = 0; i < index+1; ++i) {
        sum += all_vals[i];
    }
    return sum;
}
template<UnsignedIntDEA T>
void DeltaEncodedArray::add(T elem) {
    //uint32_t last_elem = get_val(arr.get_elem_count()-1);
    assert(elem >= last_added_elem);
    arr.add(elem - last_added_elem);
    last_added_elem = elem;

}

void DeltaEncodedArray::copy_elements_to_vector(std::vector<uint32_t>& res) {
    std::vector<uint32_t> all_vals;
    arr.copy_elements_to_vector(all_vals);
    if (all_vals.empty()) return;
    uint32_t sum = 0;
    for (int i = 0; i < all_vals.size(); ++i) {
        sum += all_vals[i];
        res.push_back(sum);
    }
    return;
}

int DeltaEncodedArray::get_elem_count() {return arr.get_elem_count();}
