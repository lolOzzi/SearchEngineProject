#include <cstdint>

class SignatureHasher {
public:
    std::uint64_t M;
    std::uint64_t m;
    std::uint64_t seed_index;

    SignatureHasher(std::uint64_t max_val) {
        M = max_val;
        std::uint64_t temp = M - 1;
        m = 0;
        while (temp > 0) {
            temp >>= 1;
            m++;
        }
        seed_index = 0;
    }

    void get_new_hash() {
        seed_index++;
    }

    std::uint64_t hash(unsigned __int128 key) {
        std::uint64_t lower_64_bits = static_cast<std::uint64_t>(key);
        std::uint64_t z = lower_64_bits ^ seed_index;

        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
        z = z ^ (z >> 31);

        return static_cast<std::uint64_t>((static_cast<unsigned __int128>(z) * M) >> 64);
    }
};