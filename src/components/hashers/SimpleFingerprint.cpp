#include "SimpleFingerprint.h"
#include <cstdlib>

void SimpleFingerprint::init() {
    z = rand() % p;
}

SimpleFingerprint::SimpleFingerprint() {
    p = 2147483647;
    init();
}

static int hash_individual(char c) {
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
