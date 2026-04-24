#include <array>
#include <cstdint>
#include <optional>
#include <sys/types.h>
#include <vector>
#include "RobinHoodMap.hpp"

class XFastTrie {
private:
    static constexpr int BitWidth = 32;
    static constexpr int MaxLevel = BitWidth;

    struct LeafNode {
        uint32_t value;
        int32_t previous_index = -1;
        int32_t next_index = -1;
    };

    struct InternalNode {
        int32_t min_leaf_index = -1;
        int32_t max_leaf_index = -1;
    };

    std::vector<LeafNode> leaves;
    std::array<RobinHoodMap<uint32_t, InternalNode>, MaxLevel> levels;
    RobinHoodMap<uint32_t, int32_t> leaf_level;

    uint32_t get_prefix(uint32_t value, int level) const {
        if (level == 0) return 0;
        return value >> (BitWidth - level);
    }

    const InternalNode* find_node(int level, uint32_t prefix) const {
        return levels[level].find(prefix);
    }

    InternalNode* find_node(int level, uint32_t prefix) {
        return levels[level].find(prefix);
    }

    int32_t search_predecessor_index(uint32_t value) const {
        int low = 0, high = MaxLevel - 1, match_level = 0;
        const InternalNode* matched_node = nullptr;
        while (low <= high) {
            int mid = low + (high - low) / 2;
            const InternalNode* node = find_node(mid, get_prefix(value, mid));
            if (node != nullptr) { matched_node = node; match_level = mid; low = mid + 1; }
            else                 { high = mid - 1; }
        }
        if (!matched_node) return -1;
        uint32_t mismatch_bit = (value >> (BitWidth - match_level - 1)) & 1;
        return (mismatch_bit == 1) ? matched_node->max_leaf_index
                                   : leaves[matched_node->min_leaf_index].previous_index;
    }

    int32_t search_successor_index(uint32_t value) const {
        int low = 0, high = MaxLevel - 1, match_level = 0;
        const InternalNode* matched_node = nullptr;
        while (low <= high) {
            int mid = low + (high - low) / 2;
            const InternalNode* node = find_node(mid, get_prefix(value, mid));
            if (node != nullptr) { matched_node = node; match_level = mid; low = mid + 1; }
            else                 { high = mid - 1; }
        }
        if (!matched_node) return -1;
        uint32_t mismatch_bit = (value >> (BitWidth - match_level - 1)) & 1;
        return (mismatch_bit == 1) ? leaves[matched_node->max_leaf_index].next_index
                                   : matched_node->min_leaf_index;
    }

    int32_t get_predecessor_index(uint32_t value) const {
        if (leaves.empty()) return -1;
        const int32_t* exact = leaf_level.find(value);
        if (exact) return leaves[*exact].previous_index;
        return search_predecessor_index(value);
    }

    int32_t get_successor_index(uint32_t value) const {
        if (leaves.empty()) return -1;
        const int32_t* exact = leaf_level.find(value);
        if (exact) return leaves[*exact].next_index;
        return search_successor_index(value);
    }

public:
    void insert(uint32_t value) {
        if (leaf_level.find(value) != nullptr) return;
        int32_t predecessor_index = get_predecessor_index(value);
        int32_t successor_index = -1;
        if (predecessor_index != -1) {
            successor_index = leaves[predecessor_index].next_index;
        } else if (!leaves.empty()) {
            const InternalNode* root = find_node(0, 0);
            successor_index = root ? root->min_leaf_index : -1;
        }
        int32_t new_leaf_index = static_cast<int32_t>(leaves.size());
        leaves.push_back({value, predecessor_index, successor_index});
        if (predecessor_index != -1) leaves[predecessor_index].next_index = new_leaf_index;
        if (successor_index != -1) leaves[successor_index].previous_index = new_leaf_index;
        leaf_level.insert(value, new_leaf_index);
        for (int level = 0; level < MaxLevel; ++level) {
            uint32_t prefix = get_prefix(value, level);
            InternalNode* node = find_node(level, prefix);
            if (node == nullptr) {
                levels[level].insert(prefix, InternalNode{new_leaf_index, new_leaf_index});
            } else {
                if (value < leaves[node->min_leaf_index].value) node->min_leaf_index = new_leaf_index;
                if (value > leaves[node->max_leaf_index].value) node->max_leaf_index = new_leaf_index;
            }
        }
    }

    void remove(uint32_t value) {
        const int32_t* leaf_entry = leaf_level.find(value);
        if (leaf_entry == nullptr) return;
        int32_t leaf_index = *leaf_entry;
        int32_t pred_index = leaves[leaf_index].previous_index;
        int32_t succ_index = leaves[leaf_index].next_index;
        if (pred_index != -1) leaves[pred_index].next_index = succ_index;
        if (succ_index != -1) leaves[succ_index].previous_index = pred_index;
        leaf_level.erase(value);
        for (int8_t c = MaxLevel - 1; c >= 0; --c) {
            uint32_t prefix = get_prefix(value, c);
            InternalNode* curr = find_node(c, prefix);
            bool min_deleted = (curr->min_leaf_index == leaf_index);
            bool max_deleted = (curr->max_leaf_index == leaf_index);
            if (min_deleted && max_deleted) {
                levels[c].erase(prefix);
            } else {
                if (min_deleted) curr->min_leaf_index = succ_index;
                if (max_deleted) curr->max_leaf_index = pred_index;
            }
        }
        int32_t removed_idx = leaf_index;
        int32_t last_idx = static_cast<int32_t>(leaves.size()) - 1;
        if (removed_idx != last_idx) {
            uint32_t moved_value = leaves[last_idx].value;
            int32_t moved_prev = leaves[last_idx].previous_index;
            int32_t moved_next = leaves[last_idx].next_index;
            leaves[removed_idx] = leaves[last_idx];
            if (moved_prev != -1) leaves[moved_prev].next_index = removed_idx;
            if (moved_next != -1) leaves[moved_next].previous_index = removed_idx;
            int32_t* moved_entry = leaf_level.find(moved_value);
            *moved_entry = removed_idx;
            for (int8_t level = MaxLevel - 1; level >= 0; --level) {
                uint32_t p = get_prefix(moved_value, level);
                InternalNode* node = find_node(level, p);
                bool patched = false;
                if (node->min_leaf_index == last_idx) { node->min_leaf_index = removed_idx; patched = true; }
                if (node->max_leaf_index == last_idx) { node->max_leaf_index = removed_idx; patched = true; }
                if (!patched) break;
            }
        }
        leaves.pop_back();
    }

    std::optional<uint32_t> predecessor(uint32_t value) const {
        if (leaves.empty()) return std::nullopt;
        const int32_t* exact = leaf_level.find(value);
        if (exact) return leaves[*exact].value;
        int32_t index = search_predecessor_index(value);
        return index != -1 ? std::optional(leaves[index].value) : std::nullopt;
    }

    std::optional<uint32_t> successor(uint32_t value) const {
        if (leaves.empty()) return std::nullopt;
        const int32_t* exact = leaf_level.find(value);
        if (exact) return leaves[*exact].value;
        int32_t index = search_successor_index(value);
        return index != -1 ? std::optional(leaves[index].value) : std::nullopt;
    }

    std::optional<uint32_t> predecessor_strict(uint32_t value) const {
        int32_t index = get_predecessor_index(value);
        return index != -1 ? std::optional(leaves[index].value) : std::nullopt;
    }

    std::optional<uint32_t> successor_strict(uint32_t value) const {
        int32_t index = get_successor_index(value);
        return index != -1 ? std::optional(leaves[index].value) : std::nullopt;
    }
};
