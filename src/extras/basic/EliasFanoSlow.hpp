#include <vector>
#include <cstdint>
#include <cmath>
#include <cassert>
#include <stdexcept>
#include <algorithm>

namespace EliasFanoNS {

class EliasFano {
private:
    std::vector<uint64_t> L;
    std::vector<uint64_t> H;
    uint32_t l;
    uint32_t lower_mask;
    size_t n;

    void setBit(std::vector<uint64_t>& arr, size_t pos) {
        arr[pos / 64] |= (1ULL << (pos % 64));
    }

    bool getBit(const std::vector<uint64_t>& arr, size_t pos) const {
        return (arr[pos / 64] >> (pos % 64)) & 1;
    }

public:
    EliasFano(std::vector<uint32_t> nums) {
        n = nums.size();
        if (n == 0) return;

        uint32_t n_max = nums.back();
        if (n_max >= n)
            l = std::floor(std::log2(static_cast<float>(n_max)/ n));
        else 
            l = 0;

        lower_mask = (l < 32) ? ((1u << l) - 1) : ~0u;

        L.assign((n * l + 63) / 64, 0ULL);

        size_t h_bits = n + (n_max >> l) + 2;
        H.assign((h_bits + 63) / 64, 0ULL);

        for (size_t i = 0; i < n; i++) {
            uint32_t lower = nums[i] & lower_mask;
            for (uint32_t b = 0; b < l; b++)
                if ((lower >> b) & 1)
                    setBit(L, i * l + b);

            uint32_t upper = nums[i] >> l;
            setBit(H, upper + i); 
        }

    }

    uint32_t access(size_t i) const {
        assert(i < n);

        uint32_t lower = 0;
        for (uint32_t b = 0; b < l; b++) {
            if (getBit(L, i * l + b))
                lower |= (1u << b);
        }

        size_t rank = 0;
        size_t pos = 0;
        while (rank <= i) {
            if (getBit(H, pos)) rank++;
            if (rank <= i) pos++;
        }
        uint32_t upper = static_cast<uint32_t>(pos - i);

        return (upper << l) | lower;
    }

    size_t size() const { return n; }
};

} 
