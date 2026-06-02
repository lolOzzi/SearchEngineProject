#include "AccessTrie.h"

template <typename T>
class BurstTrie {
private:
    AccessTrie<T> root;
public:
    BurstTrie() : root(AccessTrie<T>(1)) {}
    T* get(const std::string &word);
    T* add(const std::string& word, const T &value);
};

template <typename T>
inline T* BurstTrie<T>::get(const std::string &word) {
    const Record rec = Record(word.c_str());
    return root.search(rec);
}

template <typename T>
inline T* BurstTrie<T>::add(const std::string& word, const T &value) {
    const Record rec = Record(word);
    return root.insert(rec, value);
}

