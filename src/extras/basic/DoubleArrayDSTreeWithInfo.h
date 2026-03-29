#pragma once
#include <string>
#include <algorithm>
#include "AlphaMap.h"

#define DEFAULT_BASE 0
#define DEFAULT_CHECK 0
#define END_CHECK_VAL 2147483647

class DoubleArrayDSTreeWithInfo {
private:
    int* base;
    int* check;
    bool* terminal;
    char* label;
    int root;
    AlphaMap alpha_map;

    void remove_free_base(int pos);
    void add_free_base(int pos);
    void resize(int new_t);
    void get_children(DynamicArray<int>* child_codes_holder, int parent_val);
    int find_base(DynamicArray<int> child_codes, int parent_val);
public:
    DynamicArray<int>* documents_in;
    int size;

    DoubleArrayDSTreeWithInfo(int start_size);
    int add(const std::string& word);
    int search(const std::string& word);
    std::string reconstruct(int t);
    int get_node(const std::string&);
};

DoubleArrayDSTreeWithInfo::DoubleArrayDSTreeWithInfo(int start_size) : alpha_map(AlphaMap(32)), size(start_size) {
    base = new int[size];
    check = new int[size];
    terminal = new bool[size];
    label = new char[size];
    documents_in = new DynamicArray<int>[size];

    for (int i = 0; i < size; i++) {
        base[i] = DEFAULT_BASE;
        check[i] = (i + 1)*(-1);
        terminal[i] = false;
        label[i] = ' ';
        documents_in[i] = DynamicArray<int>(4);
    }
    root = 1;
    base[root] = 1;
    check[root] = 0;

    check[0] = (root+1)*(-1);
    check[size-1] = END_CHECK_VAL;
}

void DoubleArrayDSTreeWithInfo::resize(int new_t) {
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
    DynamicArray<int>* new_documents_in = new DynamicArray<int>[new_size];

    copy(base, base + old_size, new_base);
    copy(check, check + old_size, new_check);
    copy(terminal, terminal + old_size, new_terminal);
    copy(label, label + old_size, new_label);
    copy(documents_in, documents_in + old_size, new_documents_in);

    for (int i = old_size; i < new_size; i++) {
        new_base[i] = DEFAULT_BASE;
        new_check[i] = (i + 1)*(-1);
        new_terminal[i] = false;
        new_label[i] = ' ';
        new_documents_in[i] = DynamicArray<int>(4);
    }

    delete[] base;
    delete[] check;
    delete[] terminal;
    delete[] label;
    delete[] documents_in;

    base = new_base;
    check = new_check;
    terminal = new_terminal;
    label = new_label;
    documents_in = new_documents_in;

    size = new_size;
    check[size-1] = END_CHECK_VAL;
    if (check[old_size-1] == END_CHECK_VAL)
    {
        check[old_size-1] = old_size*(-1);
    } else
    {
        add_free_base(old_size);
    }
}

inline void DoubleArrayDSTreeWithInfo::remove_free_base(int pos)
{
    if (check[pos] > 0) return;
    int cur = 0;
    int search_pos = pos*(-1);
    while (check[cur] > search_pos && check[cur] != END_CHECK_VAL)
    {
        cur = check[cur]*(-1);
    }
    if (check[cur] == search_pos) {
        check[cur] = check[pos];
    }
    else
    {
        int idk = 1;
    }
}

inline void DoubleArrayDSTreeWithInfo::add_free_base(int pos)
{
    //if (check[pos] < 0) return;
    int cur = 0;
    int search_pos = pos*(-1);
    while (check[cur] > search_pos && check[cur] != END_CHECK_VAL)
    {
        cur = check[cur]*(-1);
    }

    if (cur < pos && check[pos] < 0)
    {
        check[cur] = search_pos;
        return;
    }

    check[pos] = check[cur];
    check[cur] = search_pos;
}

void DoubleArrayDSTreeWithInfo::get_children(DynamicArray<int> *child_codes_holder, int parent_val) {
    int start_val = base[parent_val];
    int end_val = start_val + alpha_map.n + 1;
    for (int i = start_val; i < end_val; i++) {
        if (check[i] == parent_val) {
            int child_code = i - base[parent_val];
            child_codes_holder->add(child_code);
        }
    }
}

int DoubleArrayDSTreeWithInfo::find_base(DynamicArray<int> child_codes, int parent_val) {
    int current_base = 0;
    bool has_conflict = true;

    while (has_conflict) {
        if (check[current_base] == END_CHECK_VAL)
            resize(size+1);
        current_base = check[current_base]*(-1);

        has_conflict = false;
        for (int i = 0; i < child_codes.n; i++) {
            int new_pos = current_base + child_codes[i];
            if (new_pos >= size) {
                resize(new_pos);
            }
            if (check[new_pos] >= 0) {
                has_conflict = true;
                break;
            }
        }
    }

    if (current_base < 0)
    {
        int idk = 1;
    }
    return current_base;
}

int DoubleArrayDSTreeWithInfo::add(const std::string& word) {
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
        if (check[t] < 0 || check[t] == END_CHECK_VAL) {
            remove_free_base(t);
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
                remove_free_base(new_t);
                check[new_t] = s;
                label[new_t] = label[old_t];

                terminal[new_t] = terminal[old_t];
                terminal[old_t] = false;

                documents_in[new_t] = documents_in[old_t];
                documents_in[old_t] = DynamicArray<int>(4);

                // Update grandchildren
                for (int k = 0; k < size; k++) {
                    if (check[k] == old_t) {
                        check[k] = new_t;
                    }
                }

                base[old_t] = DEFAULT_BASE;
                add_free_base(old_t);
                label[old_t] = ' ';
            }
            base[s] = new_base;
            t = base[s] + code;

            base[t] = DEFAULT_BASE;
            remove_free_base(t);
            check[t] = s;
            label[t] = c;
            s = t;
        }
    }
    terminal[s] = true;
    return s;
}

int DoubleArrayDSTreeWithInfo::search(const std::string& word) {
    int s = root;

    for (int i = 0; i < word.size(); i++) {
        char c = word[i];
        int code = alpha_map.get(c);

        int t = base[s] + code;
        if (t >= size || check[t] != s) {
            return -1;
        }
        s = t;
    }

    return s;
}

std::string DoubleArrayDSTreeWithInfo::reconstruct(int t) {
    std::string result;

    while (t != root) {
        int parent = check[t];
        result.push_back(label[t]);
        t = parent;
    }

    std::reverse(result.begin(), result.end());
    return result;
}

int DoubleArrayDSTreeWithInfo::get_node(const std::string& word) {
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