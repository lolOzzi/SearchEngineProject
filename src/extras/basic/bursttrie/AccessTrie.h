#pragma once
#include <array>
#include <string>
#include <variant>
#include "BinarySearchTree.h"

#define CHARNUM 256
#define A_ASCII_START 97
#define LIMIT 100

template <typename T>
class AccessTrie {
private:
    struct Container {
        int records = 0;
        BinarySearchTree<T> BST;
    };

    using ATNode = std::variant<AccessTrie<T>*, Container*>;
    std::array<ATNode, CHARNUM> arr;
    uint8_t depth;
public:
    AccessTrie(uint8_t d) : depth(d) {
        for (auto& node : arr) {
            node = new Container{0, BinarySearchTree<T>()};
        }
    }
    T* search(Record word);
    T* insert(Record word, T value);
    void burst(int index);
};

template <typename T>
inline T* AccessTrie<T>::search(const Record word) {
    int index = static_cast<unsigned char>(word[0]);
    if (index == 0) {
        Container* container = std::get<Container*>(arr[index]);
        return container->BST.get(word);
    }
    if (std::holds_alternative<Container*>(arr[index])) {
        Container* container = std::get<Container*>(arr[index]);
        return container->BST.get(word.substring(1));
    }
    else if (std::holds_alternative<AccessTrie<T>*>(arr[index])) {
        AccessTrie<T>* at = std::get<AccessTrie<T>*>(arr[index]);
        return at->search(word.substring(1));
    }
    return nullptr;
}

template <typename T>
inline T* AccessTrie<T>::insert(const Record word, const T value) {
    const char* str = word.get_string();
    int index = static_cast<unsigned char>(str[0]);
    if (index == 0) {
        Container* container = std::get<Container*>(arr[index]);
        return container->BST.add(word, value);
    }
    else if (std::holds_alternative<Container*>(arr[index])) {
        Container* container = std::get<Container*>(arr[index]);
        if (container->records > LIMIT) {
            burst(index);
            AccessTrie<T>* at = std::get<AccessTrie<T>*>(arr[index]);
            return at->insert(word.substring(1), value);
        }

        ++container->records;
        return container->BST.add(word.substring(1), value);
    }
    else if (std::holds_alternative<AccessTrie<T>*>(arr[index])) {
        AccessTrie<T>* at = std::get<AccessTrie<T>*>(arr[index]);
        return at->insert(word.substring(1), value);
    }
    return nullptr;
}

template <typename T>
inline void AccessTrie<T>::burst(int index) {
    if (std::holds_alternative<Container*>(arr[index])) {
        Container* container = std::get<Container*>(arr[index]);
        DynamicArray<ValuePairs<T>> values = DynamicArray<ValuePairs<T>>(4);
        container->BST.BFSAndDelete(&values);
        AccessTrie* at = new AccessTrie(depth+1);
        for (int j = 0; j < values.n; j++) {
            at->insert(values[j].word, values[j].val);
        }
        delete container;
        arr[index] = at;
    }
}


