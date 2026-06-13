#pragma once
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
