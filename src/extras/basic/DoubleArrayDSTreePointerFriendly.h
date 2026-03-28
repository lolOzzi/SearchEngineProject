#pragma once
#include <string>
#include <algorithm>
#include <memory>

#include "AlphaMap.h"

#define DEFAULT_BASE 0
#define DEFAULT_CHECK -1

struct Edge{
    int base = DEFAULT_BASE;
    int check = DEFAULT_CHECK;
    bool is_terminal = false;
    char label = ' ';
};

class DoubleArrayDSTreePointerFriendly {
private:
public:
    std::unique_ptr<Edge>* arr;
    int root;
    int size;
    AlphaMap alpha_map;

    DoubleArrayDSTreePointerFriendly(int start_size) : size(start_size), alpha_map(AlphaMap(32)) {
        arr = new std::unique_ptr<Edge>[size]();

        for (size_t i = 0; i < size; ++i) {
            arr[i] = std::make_unique<Edge>();
        }

        root = 1;
        arr[root]->base = 1;
        arr[root]->check = 0;
    }

    void resize(int new_t);
    void get_children(DynamicArray<int>* child_codes_holder, int parent_val);
    int find_base(DynamicArray<int> child_codes, int parent_val);
    Edge* add(std::string s);
    bool search(const std::string& word);
    std::string reconstruct(int t);
    int get_node(const std::string&);
    std::string get_string_from_edge(Edge* e);
};


void DoubleArrayDSTreePointerFriendly::resize(int new_t) {
    int min_size = new_t+1;
    if (min_size <= size) return;
    int old_size = size;
    int new_size = old_size*2;
    while (new_size < min_size) {
        new_size *= 2;
    }

    std::unique_ptr<Edge>* new_arr = new std::unique_ptr<Edge>[new_size];
    for (int i = 0; i < old_size; i++) {
        new_arr[i] = std::move(arr[i]);
    }
    for (size_t i = old_size; i < new_size; i++) {
        new_arr[i] = std::make_unique<Edge>();
    }

    delete[] arr;
    arr = new_arr;
    size = new_size;
}

void DoubleArrayDSTreePointerFriendly::get_children(DynamicArray<int> *child_codes_holder, int parent_val) {
    for (int i = 0; i < size; i++) {
        if (arr[i]->check == parent_val) {
            int child_code = i - arr[parent_val]->base;
            child_codes_holder->add(child_code);
        }
    }
}

int DoubleArrayDSTreePointerFriendly::find_base(DynamicArray<int> child_codes, int parent_val) {
    int current_base = 0;
    bool has_conflict = true;

    while (has_conflict) {
        current_base++;
        has_conflict = false;
        for (int i = 0; i < child_codes.n; i++) {
            int new_pos = current_base + child_codes[i];
            if (new_pos >= size) {
                resize(new_pos);
            }
            if (arr[new_pos]->check != DEFAULT_CHECK /*&& check[new_pos] != parent_val*/) {
                has_conflict = true;
                break;
            }
        }
    }

    return current_base;
}

Edge* DoubleArrayDSTreePointerFriendly::add(std::string word) {
    int s = root;
    for (int i = 0; i < word.size(); i++) {
        char c = word[i];
        int code = alpha_map.get(c);
        int t = arr[s]->base + code;

        if (t >= size) resize(t);

        // Transition already exists
        if (arr[t]->check == s) {
            s = t;
            continue;
        }

        // Transition does not exist, and spot is not taken
        if (arr[t]->check == DEFAULT_CHECK) {
            arr[t]->check = s;
            arr[t]->base = DEFAULT_BASE;
            arr[t]->label = c;
            s = t;
            continue;
        }

        // Transition is taken by other node
        if (arr[t]->check != s) {
            // Find children of parent char
            DynamicArray<int> children = DynamicArray<int>();
            get_children(&children, s);
            children.add(code);

            // Relocate
            int old_base = arr[s]->base;
            int new_base = find_base(children, s);

            for (int j = 0; j < children.n; j++) {
                int old_t = old_base + children[j];
                if (arr[old_t]->check != s) continue;

                int new_t = new_base + children[j];
                if (new_t >= size) resize(new_t);

                arr[new_t]->base = arr[old_t]->base;
                arr[new_t]->check = s;
                arr[new_t]->label = arr[old_t]->label;

                arr[new_t]->is_terminal = arr[old_t]->is_terminal;
                arr[old_t]->is_terminal = false;

                // Update grandchildren
                for (int k = 0; k < size; k++) {
                    if (arr[k]->check == old_t) {
                        arr[k]->check = new_t;
                    }
                }

                arr[old_t]->base = DEFAULT_BASE;
                arr[old_t]->check = DEFAULT_CHECK;
                arr[old_t]->label = ' ';
            }
            arr[s]->base = new_base;
            t = arr[s]->base + code;

            arr[t]->base = DEFAULT_BASE;
            arr[t]->check = s;
            arr[t]->label = c;
            s = t;
        }
    }
    arr[s]->is_terminal = true;
    return arr[s].get();
}

bool DoubleArrayDSTreePointerFriendly::search(const std::string& word) {
    int s = root;

    for (int i = 0; i < word.size(); i++) {
        char c = word[i];
        int code = alpha_map.get(c);

        int t = arr[s]->base + code;
        if (t >= size || arr[t]->check != s) {
            return false;
        }
        s = t;
    }

    return arr[s]->is_terminal;
}

std::string DoubleArrayDSTreePointerFriendly::reconstruct(int t) {
    std::string result;

    while (t != root) {
        int parent = arr[t]->check;
        result.push_back(arr[t]->label);
        t = parent;
    }

    std::reverse(result.begin(), result.end());
    return result;
}

int DoubleArrayDSTreePointerFriendly::get_node(const std::string& word) {
    int s = root;

    for (char c : word) {
        int code = alpha_map.get(c);
        int t = arr[s]->base + code;

        if (t >= size || arr[t]->check != s) {
            return -1;
        }
        s = t;
    }
    return s;
}

std::string DoubleArrayDSTreePointerFriendly::get_string_from_edge(Edge *e) {
    std::string result;
    result.push_back(e->label);
    int t = e->check;
    while (t != root) {
        int parent = arr[t]->check;
        result.push_back(arr[t]->label);
        t = parent;
    }

    std::reverse(result.begin(), result.end());
    return result;
}
