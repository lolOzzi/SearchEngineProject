#include "ChainSet.h"

#include <string>
#include <cstdint>
#include <iostream>
#include "../../core/interfaces.h"
// Extremely simple chained-hashing set

class IHash;
template<typename T>
concept has_str = requires(const T& t) {
    { t.str } -> std::convertible_to<std::string>;
};
template<typename T>
concept has_str_point = requires(const T& t) {
    { t->str } -> std::convertible_to<std::string>;
};
template<typename T>
concept can_be_converted_to_str = requires(const T& t) {
    { t } -> std::convertible_to<std::string>;
};
template<typename T>
concept has_title = requires(const T& t) {
    { t.title } -> std::convertible_to<std::string>;
};

template<typename T>
concept StringLike = has_str<T> || can_be_converted_to_str<T> || has_str_point<T> || has_title<T>;

template<StringLike T>
std::string to_string(const T& t) {
    if constexpr (has_str<T>) {
        return t.str;
    } else if constexpr (has_str_point<T>) {
        return t->str;
    } else if constexpr (has_title<T>) {
        return t.title;
    } else {
        return static_cast<std::string>(t);
    }
}

template<StringLike T>
class SimpleSet {
public:
    SimpleSet(IHash* hasher, int buckets = 16): hasher_(hasher), bucket_count_(buckets > 0 ? buckets : 16), size_(0)
    {
        buckets_ = new Node*[bucket_count_];
        for (int i = 0; i < bucket_count_; ++i) buckets_[i] = nullptr;
    }

    ~SimpleSet() {
        clear();
        delete[] buckets_;
    }

    void insert(const T key) requires StringLike<T> {
        int index = index_for(key);
        Node* curr = buckets_[index];
        while (curr) {
            if (to_string(curr->key) == to_string(key)) return;
            curr = curr->next;
        }
        Node* n = new Node{key, buckets_[index]};
        buckets_[index] = n;
        size_++;
    }

    bool contains(const std::string& key) requires StringLike<T>  {
        int index = index_for(key);
        Node* curr = buckets_[index];
        while (curr) {
            if (to_string(curr->key) == to_string(key)) return true;
            curr = curr->next;
        }
        return false;
    }

    void remove(const T key) requires StringLike<T>  {
        int index = index_for(key);
        if (index == -1) return;
        Node* current = buckets_[index];
        Node* last = nullptr;
        while (current) {
            if (to_string(current->key) == to_string(key)) {
              if (last) {
                last->next = current->next;
              } else {
                  buckets_[index] = nullptr;
              }
              size_--;
              delete current;
              return;
            }
            Node* next = current->next;
            if (!next) break;
            last = current;
            current = next;

        }
        return;

    }

    void clear() {
        for (int i = 0; i < bucket_count_; ++i) {
            Node* cur = buckets_[i];
            while (cur) {
                Node* next = cur->next;
                delete cur;
                cur = next;
            }
            buckets_[i] = nullptr;
        }
        size_ = 0;
    }

    int size() const { return size_; }
    int bucket_count() const { return bucket_count_; }

    void copy_elements_to_vector(std::vector<T> &res) {
    	for (int i = 0; i < bucket_count_; ++i) {
          	Node* curr = buckets_[i];
			while (curr) {
				res.push_back(curr->key);
				curr = curr->next;
			}
    	}
    }

private:
    struct Node {
        T key;
        Node* next;
    };

    IHash* hasher_;
    int bucket_count_;
    Node** buckets_;
    int size_;

    int index_for(const T key) requires StringLike<T>  {
        if (bucket_count_ < 0) return -1;
        std::uint64_t h = hasher_->hash(to_string(key), bucket_count_);
        return static_cast<int>(h % static_cast<std::uint64_t>(bucket_count_));
    }
    SimpleSet(const SimpleSet&) = delete;
    SimpleSet& operator=(const SimpleSet&) = delete;
};

