#include <cstdint>
#include <vector>
#include <cassert>
#include <cstdio>

static int floor_log2(uint64_t value) {
    return value ? 63 - __builtin_clzll(value) : 0;
}

static uint64_t bitmask(int length) {
    if (length <= 0) return 0ULL;
    if (length >= 64) return ~0ULL;
    return (1ULL << length) - 1;
}

static void bits_write(std::vector<uint64_t>& bit_vector, size_t position, int length, uint64_t value) {
    if (!length) return;
    bit_vector[position >> 6] |= value << (position & 63); // pos >> 6 is funny divison by 64, pos & 63 is pos mod 64 
    if ((position & 63) + (unsigned)length > 64) {
        bit_vector[(position >> 6) + 1] |= value >> (64 - (position & 63));
    }
}

static uint64_t bits_read(const std::vector<uint64_t>& bit_vector, size_t position, int length) {
    if (!length) return 0;
    uint64_t result = bit_vector[position >> 6] >> (position & 63);
    if ((position & 63) + (unsigned)length > 64) {
        result |= bit_vector[(position >> 6) + 1] << (64 - (position & 63));
    }
    return result & bitmask(length);
}

static int select1_in_block(const std::vector<uint64_t>& bit_vector, size_t word_offset, uint32_t word_count, uint32_t high_bits_length, int rank) {
    for (uint32_t word_index = 0; word_index < word_count; ++word_index) {
        uint64_t word = bit_vector[word_offset + word_index];
        const int tail_length = high_bits_length & 63;
        if (word_index + 1 == word_count && tail_length) {
            word &= bitmask(tail_length);
        }
        int population_count = __builtin_popcountll(word);
        if (rank < population_count) {
            for (int bit_index = 0; bit_index < 64; ++bit_index) {
                if ((word >> bit_index) & 1) {
                    if (rank-- == 0) {
                        return (int)(word_index * 64 + bit_index);
                    }
                }
            }
        }
        rank -= population_count;
    }
    assert(false);
    return 0;
}

class ExtensibleEliasFano {
public:
    explicit ExtensibleEliasFano(uint32_t universe_maximum)
        : universe_maximum_(universe_maximum), block_capacity_(calc_block_capacity(universe_maximum)) {
        //buffer_.reserve(block_capacity_);
    }

    void add(uint32_t value) {
        assert(!has_last_value_ || value >= last_value_);
        last_value_ = value;
        has_last_value_ = true;
        buffer_.push_back(value);
        if ((int)buffer_.size() == block_capacity_) {
            flush_buffer();
        }
    }
     // not O(1) will need to figure out if needed later, would need a select/rank datastructure on top like 3* (so more mem)
    uint32_t access(int index) const {
        assert(index >= 0 && index < size());
        const int block_index = index / block_capacity_;
        const int remainder_index = index % block_capacity_;
        if (block_index < (int)block_metadata_.size()) {
            return block_access(block_index, remainder_index);
        }
        return buffer_[remainder_index];
    }

    int size() const { 
        return (int)block_metadata_.size() * block_capacity_ + (int)buffer_.size(); 
    }
    
    int block_size() const { 
        return block_capacity_; 
    }
    
    void copy_elements_to_vector(std::vector<uint32_t> &res) {
        int count = 0;

        for (int i = 0; i < (int)block_metadata_.size(); ++i) {
            BlockMetadata block_metadata = block_metadata_[i];

            int high_bit_length = block_metadata.high_bit_length;
            int low_bit_size = block_metadata.low_bit_size;

            int word_count = (high_bit_length + 63) / 64;
            int tail_length = high_bit_length & 63;

            size_t current_position_low = block_metadata.low_bit_offset;
            int remainder_index = 0;

            for (int word_index = 0; word_index < word_count; ++word_index) {
                uint64_t word = high_bits_vector_[block_metadata.high_word_offset + word_index];

                if (word_index + 1 == word_count && tail_length) {
                    word &= bitmask(tail_length);
                }

                while (word) {
                    int bit_index = __builtin_ctzll(word);
                    uint32_t high_bits_value = (uint64_t)(word_index * 64 + bit_index - remainder_index);
                    uint32_t low_bits_value = bits_read(low_bits_vector_, current_position_low, low_bit_size);
                    uint32_t value = block_metadata.base_value + ((high_bits_value << low_bit_size) | low_bits_value);

                    assert(value <= UINT32_MAX);
                    res.push_back((uint32_t)value);

                    current_position_low += low_bit_size;
                    remainder_index++;
                    count++;

                    word &= word - 1;
                }
            }

            assert(remainder_index == block_capacity_);
        }

        for (int i = 0; i < buffer_.size(); ++i) {
            assert(buffer_[i] <= UINT32_MAX);
            res.push_back((uint32_t)buffer_[i]);
        }
}

private:

    struct BlockMetadata {
        uint32_t base_value;
        uint32_t high_word_offset;
        uint32_t high_bit_length;
        uint32_t low_bit_offset;
        uint8_t low_bit_size;
    };

    uint32_t universe_maximum_;
    int block_capacity_;
    std::vector<BlockMetadata> block_metadata_;
    std::vector<uint64_t> high_bits_vector_;
    std::vector<uint64_t> low_bits_vector_;
    std::vector<uint32_t> buffer_;
    bool has_last_value_ = false;
    uint32_t last_value_ = 0;
    
    static int calc_block_capacity(uint32_t universe_maximum) {
        //if (universe_maximum <= 1) return 1;
        //int log2_value = floor_log2(universe_maximum);
        //int computed_capacity = log2_value * log2_value;
        //return computed_capacity;
        return 32;
    }

    void flush_buffer() {
        assert((int)buffer_.size() == block_capacity_);
        int element_count = block_capacity_;
        uint32_t base_value = buffer_[0];
        uint64_t universe_block_size = uint64_t(buffer_[element_count - 1]) - base_value + 1;
        int low_bit_size = (universe_block_size > (uint32_t)element_count) ? floor_log2(universe_block_size / element_count) : 0;
        uint64_t high_bits_length = (uint64_t)element_count + ((universe_block_size + bitmask(low_bit_size)) >> low_bit_size);
        int high_words_count = (int)((high_bits_length + 63) / 64);
        int low_words_count = (int)(((uint64_t)element_count * low_bit_size + 63) / 64) + 1;

        BlockMetadata metadata;
        metadata.base_value = base_value;
        metadata.high_word_offset = high_bits_vector_.size();
        metadata.high_bit_length = (uint32_t)high_bits_length;
        metadata.low_bit_offset = low_bits_vector_.size() * 64;
        metadata.low_bit_size = (uint8_t)low_bit_size;

        high_bits_vector_.resize(high_bits_vector_.size() + high_words_count, 0);
        low_bits_vector_.resize(low_bits_vector_.size() + low_words_count, 0);

        for (int current_index = 0; current_index < element_count; ++current_index) {
            uint32_t relative_value = buffer_[current_index] - base_value;
            bits_write(low_bits_vector_, (size_t)metadata.low_bit_offset + (size_t)current_index * low_bit_size, low_bit_size, relative_value & bitmask(low_bit_size));
            size_t high_position = (size_t)metadata.high_word_offset * 64 + (size_t)(relative_value >> low_bit_size) + (size_t)current_index;
            high_bits_vector_[high_position >> 6] |= 1ULL << (high_position & 63);
        }

        block_metadata_.push_back(metadata);
        buffer_.clear();
    }

    uint32_t block_access(int block_index, int remainder_index) const {
        const BlockMetadata& metadata = block_metadata_[block_index];
        uint64_t low_bits_value = bits_read(low_bits_vector_, (size_t)metadata.low_bit_offset + (size_t)remainder_index * metadata.low_bit_size, metadata.low_bit_size);
        uint32_t word_count = (metadata.high_bit_length + 63) / 64;
        int selected_position = select1_in_block(high_bits_vector_, metadata.high_word_offset, word_count, metadata.high_bit_length, remainder_index);
        uint64_t high_bits_value = (uint64_t)(selected_position - remainder_index);
        return metadata.base_value + ((high_bits_value << metadata.low_bit_size) | low_bits_value);
    }


};
