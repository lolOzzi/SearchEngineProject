#include "../../HashFamily.h"

class SignatureCheckHasher : public IHashFamily<unsigned __int128> {
private:
public:
    std::uint64_t m = 0xc6a4a7935bd1e995ULL;
    int r = 47;
    SignatureCheckHasher();
    SignatureCheckHasher(const SignatureCheckHasher& other) {
        this->m = other.m;
        this->r = other.r;
    };
    std::uint64_t hash(unsigned __int128, std::uint64_t max_val) override;
    void get_new_hash() override;
    IHashFamily<unsigned __int128>* clone() const override {
        return new SignatureCheckHasher(*this);
    }
};

inline SignatureCheckHasher::SignatureCheckHasher() {
    r = 47;
}

inline void SignatureCheckHasher::get_new_hash() {
}

inline std::uint64_t SignatureCheckHasher::hash(unsigned __int128 key, uint64_t max_val) {
    std::uint64_t low = static_cast<std::uint64_t>(key);
    std::uint64_t high = static_cast<std::uint64_t>(key >> 64);
    std::uint64_t hash = high ^ (low * m);
    hash ^= hash >> r;
    hash *= m;
    hash ^= hash >> r;

    return hash % max_val;
}