#pragma once
#include "../../core/interfaces.h"

class BasicHasher : public IHash {
public:
    ~BasicHasher() override = default;
    std::uint64_t hash(std::string key, int max_val) override;
};
