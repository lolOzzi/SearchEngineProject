#include <string>

class StringHasher {
private:
public:
    unsigned __int128 p;
    unsigned __int128 z;
    StringHasher();
    StringHasher(const StringHasher& other) {
        this->p = other.p;
        this->z = other.z;
    };
    unsigned __int128 hash(const std::string &key);
    void get_new_hash();
    StringHasher* clone() const {
        return new StringHasher(*this);
    }
};

inline StringHasher::StringHasher() {
    p = ~((unsigned __int128)0) - 158;;
    StringHasher::get_new_hash();
}

inline void StringHasher::get_new_hash() {
    z = rand() % p;
}

inline unsigned __int128 StringHasher::hash(const std::string &key) {
    unsigned __int128 sum = 0;
    int n = key.length();
    for (int i = 0; i < n; i++) {
        sum = (sum * z + key[i]); // % p;
    }
    unsigned __int128 ret_val = sum % p;
    return ret_val;
}
