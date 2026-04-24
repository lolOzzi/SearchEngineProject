#pragma once

#include <cstring>
#include <type_traits>
#include <utility>
#include <cstdint>
#include <vector>
#include <limits>
#define MSB8 0x80 // 10000000
#define M7  0x7F // 01111111

//T neds to be unsigned integer
template <typename T>
concept UnsignedInt = std::numeric_limits<T>::is_integer && !std::numeric_limits<T>::is_signed;

class DynamicPackedArray {
private:
    int capacity; //Size in actual uints
    int n; // The # of elements inserted
    int front;
    uint8_t* arr;
    void double_up();
    template<UnsignedInt T> int calc_elem_size(T& elem);
    void format_elem();
    uint32_t extract_elem(int& pos);
public:
    template<UnsignedInt T> void add(T elem);
    void copy_elements_to_vector(std::vector<std::uint32_t > &res);
	int get_elem_count();
    DynamicPackedArray();
    explicit DynamicPackedArray(int base_cap);
    ~DynamicPackedArray();

	DynamicPackedArray(DynamicPackedArray&& other) noexcept
		: capacity(other.capacity), n(other.n), front(other.front), arr(other.arr)
	{
		other.arr      = nullptr;
		other.capacity = 0;
		other.n        = 0;
		other.front    = 0;
	}

	DynamicPackedArray& operator=(DynamicPackedArray&& other) noexcept {
		if (this != &other) {
			delete[] arr;
			arr            = other.arr;
			capacity       = other.capacity;
			n              = other.n;
			front          = other.front;
			other.arr      = nullptr;
			other.capacity = 0;
			other.n        = 0;
			other.front    = 0;
		}
		return *this;
	}

	DynamicPackedArray(const DynamicPackedArray&)            = delete;
	DynamicPackedArray& operator=(const DynamicPackedArray&) = delete;

};

inline DynamicPackedArray::~DynamicPackedArray() {
	delete[] arr;
}
inline DynamicPackedArray::DynamicPackedArray() {
    capacity = 10;
    n = 0;
    front = 0;
    arr = new uint8_t[capacity];
}


inline DynamicPackedArray::DynamicPackedArray(int base_cap) {
    capacity = base_cap;
    n = 0;
    front = 0;
    arr = new uint8_t[capacity];
}

template<UnsignedInt T>
void DynamicPackedArray::add(T elem) {
	int elem_size = calc_elem_size(elem);
	if (front+elem_size >= capacity) {
		double_up();

	}
    int c = front;
	while (elem >= MSB8) {
       arr[c] = (elem & M7) | MSB8;
       elem >>= 7;
       c++;
	}
    arr[c++] = elem;
    front = c;
	n++;
}

template<UnsignedInt T>
int DynamicPackedArray::calc_elem_size(T& elem) {
	return elem == 0 ? 1 : (std::bit_width((uint64_t)elem) + 6) / 7;
}

uint32_t DynamicPackedArray::extract_elem(int& pos) {
  	uint32_t res = 0;
    int c = 0;
	while (arr[pos] >= MSB8) {
          //res <<= c*7;
          res |= (arr[pos] & M7)<<c*7;
          c++;
          pos++;
	}
	//res <<= c*7;
	res |= arr[pos] << c*7;
    pos++;
	return res;
}

void DynamicPackedArray::copy_elements_to_vector(std::vector<uint32_t> &res) {
	int pos = 0;
	for (int i = 0; i < n; ++i) {
          res.push_back(extract_elem(pos));
	}
}

void DynamicPackedArray::double_up() {
    uint8_t* new_arr = new uint8_t[capacity*2];
	std::memcpy(new_arr, arr, front * sizeof(uint8_t));
    delete[] arr;
    arr = new_arr;
    capacity = capacity*2;
}
int DynamicPackedArray::get_elem_count() {
	return n;
}

