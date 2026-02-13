#include <cassert>

#include "../../core/interfaces.h"
#include <cmath>
#define AMT_POWERS 30

class SimpleFingerprint : public IHash {
private:
    std::uint64_t p;
public:
    std::uint64_t z;
    ~SimpleFingerprint() override = default;
    SimpleFingerprint();
    std::uint64_t hash(std::string key, int max_val) override;
    void init();
};

void SimpleFingerprint::init() {
    z = rand() % p;
}

SimpleFingerprint::SimpleFingerprint() {
    p = 2147483647;
    init();
}

int hash_individual(char c) {
    return (int)c;
}

std::uint64_t SimpleFingerprint::hash(std::string key, int max_val) {
    if (max_val == 0) return 0;
    assert(max_val > 0);
    std::uint64_t sum = 0;
    int n = key.length();
    for (int i = 0; i < n; i++) {
        sum = (sum * z + hash_individual(key[i])) % p;
    }
    std::uint64_t ret_val = sum % max_val;
    assert(ret_val < max_val);
    return ret_val;
}
