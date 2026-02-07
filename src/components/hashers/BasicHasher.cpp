#include "../../core/interfaces.h"


class BasicHasher : public IHash {
public:
    ~BasicHasher() override = default;
    std::uint64_t hash(std::string key, int max_val) override;
};

std::uint64_t BasicHasher::hash(std::string key, int max_val) {
    size_t raw_hash = std::hash<std::string>{}(key);
    return (raw_hash < 0 ? -raw_hash : raw_hash) % max_val;
}
