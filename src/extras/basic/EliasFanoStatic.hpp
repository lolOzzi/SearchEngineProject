#pragma once
#include <memory>
#include <vector>
#include <cstdint>
#include <cmath>
#include <cassert>

namespace EliasFanoStaticNS {

class EliasFanoStatic {
private:
    std::unique_ptr<uint64_t[]> data;
    uint32_t n       = 0;
    //   bits [4:0]  = l        (needs 5 bits: log2(31) < 5)
    //   bits [31:5] = h_words_ (needs ≤27 bits: max ≈ (2·2^32+65)/64 ≈ 2^27)
    uint32_t packed_ = 0;

    uint8_t  l_bits()        const { return  static_cast<uint8_t>(packed_ & 0x1Fu); }
    uint32_t h_words()       const { return  packed_ >> 5; }
    void     set_packed(uint8_t l, uint32_t hw) {
        packed_ = (hw << 5) | (static_cast<uint32_t>(l) & 0x1Fu);
    }

    size_t l_words() const {
        return (static_cast<size_t>(n) * l_bits() + 63) / 64;
    }
    size_t total_words() const { return l_words() + h_words(); }

    void setBit(size_t base_word, size_t pos) {
        data[base_word + pos / 64] |= (1ULL << (pos % 64));
    }
    bool getBit(size_t base_word, size_t pos) const {
        return (data[base_word + pos / 64] >> (pos % 64)) & 1;
    }

    void build(const std::vector<uint32_t>& nums) {
        n = static_cast<uint32_t>(nums.size());
        if (n == 0) { data.reset(); packed_ = 0; return; }

        const uint32_t n_max = nums.back();
        const uint8_t  l     = (n_max >= n)
            ? static_cast<uint8_t>(std::floor(std::log2(static_cast<float>(n_max) / n)))
            : 0;

        const uint32_t lower_mask = (l < 32) ? ((1u << l) - 1) : ~0u;
        const size_t   lw         = (static_cast<size_t>(n) * l + 63) / 64;
        const size_t   h_bits     = n + (n_max >> l) + 2;
        const uint32_t hw         = static_cast<uint32_t>((h_bits + 63) / 64);

        set_packed(l, hw);
        data = std::make_unique<uint64_t[]>(lw + hw);   // zero-initialised

        const size_t H_BASE = lw;
        for (size_t i = 0; i < n; i++) {
            const uint32_t lower = nums[i] & lower_mask;
            for (uint8_t b = 0; b < l; b++)
                if ((lower >> b) & 1)
                    setBit(0, i * l + b);
            setBit(H_BASE, (nums[i] >> l) + i);
        }
    }

    void build_with_insert(uint32_t num) {
        std::vector<uint32_t> tmp;
        tmp.reserve(static_cast<size_t>(n) + 1);
        copy_elements_to_vector(tmp);

        tmp.push_back(num);
        data.reset();
        build(tmp);
    }

public:
    EliasFanoStatic() = default;
    explicit EliasFanoStatic(const std::vector<uint32_t>& nums) { build(nums); }

    void add(uint32_t num) { build_with_insert(num); }

    uint32_t access(size_t i) const {
        assert(i < n);
        const uint8_t  l      = l_bits();
        const size_t   H_BASE = l_words();

        uint32_t lower = 0;
        for (uint8_t b = 0; b < l; b++)
            if (getBit(0, i * l + b))
                lower |= (1u << b);

        size_t rank = 0, pos = 0;
        while (rank <= i) {
            if (getBit(H_BASE, pos)) rank++;
            if (rank <= i) pos++;
        }
        return (static_cast<uint32_t>(pos - i) << l) | lower;
    }

    void copy_elements_to_vector(std::vector<uint32_t>& res) const {
        if (n == 0) return;
        const uint8_t l      = l_bits();
        const size_t  H_BASE = l_words();

        const size_t base = res.size();
        for (size_t i = 0; i < n; i++) {
            uint32_t lower = 0;
            for (uint8_t b = 0; b < l; b++)
                if (getBit(0, i * l + b))
                    lower |= (1u << b);
            res.push_back(lower);
        }
        size_t idx = 0;
        uint32_t upper = 0;
        for (size_t pos = 0; idx < n; pos++) {
            if (getBit(H_BASE, pos)) res[base + idx++] |= (upper << l);
            else upper++;
        }
    }

    uint32_t get_elem_count() const { return n; }

    // Returns the number of uint64_t words currently allocated.
    // Useful for memory profiling; also doubles as a free bounds-check proxy.
    size_t allocated_words() const { return total_words(); }
};

} // namespace EliasFanoStaticNS
