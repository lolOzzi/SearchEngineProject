#pragma once
#include "Record.h"

template <typename T>
struct ValuePairs{
    Record word;
    T val;
};

template <typename T>
class BinarySearchTree {
private:
    struct BSTNode{
        Record word;
        T val;
        BSTNode* left;
        BSTNode* right;
    };
    BSTNode* root;
    void BFSAndDeleteHelper(DynamicArray<ValuePairs<T>> *item_holder, BSTNode* node);
public:
    BinarySearchTree() : root(nullptr) {}
    T* add(Record word, T val);
    T* get(Record word);


    void BFSAndDelete(DynamicArray<ValuePairs<T>> *item_holder);
};


template<typename T>
T* BinarySearchTree<T>::add(Record word, T val) {
    BSTNode* currnode = root;
    BSTNode* new_node = new BSTNode{std::move(word), std::move(val), nullptr, nullptr};

    if (currnode == nullptr) {
        root = new_node;
        return &new_node->val;
    }

    while (currnode != nullptr) {
        if (word == currnode->word) {
            return &currnode->val;
        }
        else if (word < currnode->word) {
            if (currnode->left == nullptr) {
                currnode->left = new_node;;
                return &new_node->val;
            }
            currnode = currnode->left;
        }
        else {
            if (currnode->right == nullptr) {
                currnode->right = new_node;
                return &new_node->val;
            }
            currnode = currnode->right;
        }
    }
    return nullptr;
}

template<typename T>
T *BinarySearchTree<T>::get(Record word) {
    BSTNode* currnode = root;

    while (currnode != nullptr) {
        if (word == currnode->word) {
            return &currnode->val;
        }
        else if (word < currnode->word) {
            if (currnode->left == nullptr) {
                return nullptr;
            }
            currnode = currnode->left;
        }
        else {
            if (currnode->right == nullptr) {
                return nullptr;
            }
            currnode = currnode->right;
        }
    }
    return nullptr;
}

template<typename T>
void BinarySearchTree<T>::BFSAndDeleteHelper(DynamicArray<ValuePairs<T>> *item_holder, BSTNode *node) {
    item_holder->add(ValuePairs<T>{std::move(node->word), std::move(node->val)});
    if (node->left != nullptr)
        BFSAndDeleteHelper(item_holder, node->left);
    if (node->right != nullptr)
        BFSAndDeleteHelper(item_holder, node->right);
    delete node;
}

template<typename T>
void BinarySearchTree<T>::BFSAndDelete(DynamicArray<ValuePairs<T>> *item_holder) {
    BFSAndDeleteHelper(item_holder, root);
    //delete this;
}
