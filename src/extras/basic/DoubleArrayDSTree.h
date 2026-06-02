#pragma once
#include <string>
#include <algorithm>
#include "AlphaMap.h"

#define DEFAULT_BASE 0
#define DEFAULT_CHECK -1

class DoubleArrayDSTree {
public:
    int* base;
    int* check;
    bool* terminal;
    char* label;
    int root;
    int size;
    AlphaMap alpha_map;

    DoubleArrayDSTree(int start_size) : size(start_size), alpha_map(AlphaMap(32)) {
        base = new int[size];
        check = new int[size];
        terminal = new bool[size];
        label = new char[size];

        for (int i = 0; i < size; i++) {
            base[i] = DEFAULT_BASE;
            check[i] = DEFAULT_CHECK;
            terminal[i] = false;
            label[i] = ' ';
        }
        root = 1;
        base[root] = 1;
        check[root] = 0;
    }

    void resize(int new_t);
    void get_children(DynamicArray<int>* child_codes_holder, int parent_val);
    int find_base(DynamicArray<int> child_codes, int parent_val);
    void add(std::string s);
    bool search(const std::string& word);
    std::string reconstruct(int t);
    int get_node(const std::string&);
};


void DoubleArrayDSTree::resize(int new_t) {
    int min_size = new_t+1;
    if (min_size <= size) return;
    int old_size = size;
    int new_size = old_size*2;
    while (new_size < min_size) {
        new_size *= 2;
    }

    int* new_base = new int[new_size];
    int* new_check = new int[new_size];
    bool* new_terminal = new bool[new_size];
    char* new_label = new char[new_size];

    copy(base, base + old_size, new_base);
    copy(check, check + old_size, new_check);
    copy(terminal, terminal + old_size, new_terminal);
    copy(label, label + old_size, new_label);

    for (int i = old_size; i < new_size; i++) {
        new_base[i] = DEFAULT_BASE;
        new_check[i] = DEFAULT_CHECK;
        new_terminal[i] = false;
        new_label[i] = ' ';
    }

    delete[] base;
    delete[] check;
    delete[] terminal;
    delete[] label;

    base = new_base;
    check = new_check;
    terminal = new_terminal;
    label = new_label;

    size = new_size;
}

void DoubleArrayDSTree::get_children(DynamicArray<int> *child_codes_holder, int parent_val) {
    for (int i = 0; i < size; i++) {
        if (check[i] == parent_val) {
            int child_code = i - base[parent_val];
            child_codes_holder->add(child_code);
        }
    }
}

int DoubleArrayDSTree::find_base(DynamicArray<int> child_codes, int parent_val) {
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
            if (check[new_pos] != DEFAULT_CHECK /*&& check[new_pos] != parent_val*/) {
                has_conflict = true;
                break;
            }
        }
    }

    return current_base;
}

void DoubleArrayDSTree::add(std::string word) {
    int s = root;
    for (int i = 0; i < word.size(); i++) {
        char c = word[i];
        int code = alpha_map.get(c);
        int t = base[s] + code;

        if (t >= size) resize(t);

        // Transition already exists
        if (check[t] == s) {
            s = t;
            continue;
        }

        // Transition does not exist, and spot is not taken
        if (check[t] == DEFAULT_CHECK) {
            check[t] = s;
            base[t] = DEFAULT_BASE;
            label[t] = c;
            s = t;
            continue;
        }

        // Transition is taken by other node
        if (check[t] != s) {
            // Find children of parent char
            DynamicArray<int> children = DynamicArray<int>();
            get_children(&children, s);
            children.add(code);

            // Relocate
            int old_base = base[s];
            int new_base = find_base(children, s);

            for (int j = 0; j < children.n; j++) {
                int old_t = old_base + children[j];
                if (check[old_t] != s) continue;

                int new_t = new_base + children[j];
                if (new_t >= size) resize(new_t);

                base[new_t] = base[old_t];
                check[new_t] = s;
                label[new_t] = label[old_t];

                terminal[new_t] = terminal[old_t];
                terminal[old_t] = false;

                // Update grandchildren
                for (int k = 0; k < size; k++) {
                    if (check[k] == old_t) {
                        check[k] = new_t;
                    }
                }

                base[old_t] = DEFAULT_BASE;
                check[old_t] = DEFAULT_CHECK;
                label[old_t] = ' ';
            }
            base[s] = new_base;
            t = base[s] + code;

            base[t] = DEFAULT_BASE;
            check[t] = s;
            label[t] = c;
            s = t;
        }
    }
    terminal[s] = true;
}

bool DoubleArrayDSTree::search(const std::string& word) {
    int s = root;

    for (int i = 0; i < word.size(); i++) {
        char c = word[i];
        int code = alpha_map.get(c);

        int t = base[s] + code;
        if (t >= size || check[t] != s) {
            return false;
        }
        s = t;
    }

    return terminal[s];
}

std::string DoubleArrayDSTree::reconstruct(int t) {
    std::string result;

    while (t != root) {
        int parent = check[t];
        result.push_back(label[t]);
        t = parent;
    }

    std::reverse(result.begin(), result.end());
    return result;
}

int DoubleArrayDSTree::get_node(const std::string& word) {
    int s = root;

    for (char c : word) {
        int code = alpha_map.get(c);
        int t = base[s] + code;

        if (t >= size || check[t] != s) {
            return -1;
        }
        s = t;
    }

    return s;
}