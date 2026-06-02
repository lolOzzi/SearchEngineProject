#pragma once

#include <cstdint>
#include <optional>
#include <vector>
#include "XFastTrie.hpp"
#include "RBTree.hpp"
#include "RobinHoodMap.hpp"

class YFastTrie {
private:
    struct Bucket {
        RBTreeNS::RBTree<uint32_t> tree;
        uint32_t size = 0;
    };

    XFastTrie rep_trie;
    RobinHoodMap<uint32_t, uint32_t> rep_to_idx;
    std::vector<Bucket> buckets;

public:
    void insert(uint32_t x) {
        if (buckets.empty()) {
            buckets.push_back(Bucket{});
            buckets[0].tree.insert(x);
            buckets[0].size = 1;
            rep_to_idx.insert(x, 0);
            rep_trie.insert(x);
            return;
        }

        auto rep_opt = rep_trie.successor(x);
        uint32_t rep = rep_opt.has_value() ? *rep_opt : *rep_trie.predecessor(UINT32_MAX);

        const uint32_t* b_idx_ptr = rep_to_idx.find(rep);
        if (!b_idx_ptr) return;
        uint32_t b_idx = *b_idx_ptr;

        if (buckets[b_idx].tree.contains(x)) return;

        buckets[b_idx].tree.insert(x);
        buckets[b_idx].size++;

        if (x > rep) {
            rep_to_idx.remove(rep);
            rep_to_idx.insert(x, b_idx);
            rep_trie.remove(rep);
            rep_trie.insert(x);
            rep = x;
        }

        if (buckets[b_idx].size > 64) {
            uint32_t new_b_idx = static_cast<uint32_t>(buckets.size());
            buckets.push_back(Bucket{});
            for (int i = 0; i < 32; ++i) {
                uint32_t min_val = *buckets[b_idx].tree.min();
                buckets[b_idx].tree.remove(min_val);
                buckets[b_idx].size--;
                buckets[new_b_idx].tree.insert(min_val);
                buckets[new_b_idx].size++;
            }
            uint32_t new_rep = *buckets[new_b_idx].tree.max();
            rep_to_idx.insert(new_rep, new_b_idx);
            rep_trie.insert(new_rep);
        }
    }

    bool contains(uint32_t x) const {
        auto rep_opt = rep_trie.successor(x);
        if (!rep_opt) return false;
        
        const uint32_t* b_idx = rep_to_idx.find(*rep_opt);
        return b_idx ? buckets[*b_idx].tree.contains(x) : false;
    }

    std::optional<uint32_t> predecessor(uint32_t x) const {
        auto rep_opt = rep_trie.successor(x);
        if (rep_opt) {
            const uint32_t* b_idx = rep_to_idx.find(*rep_opt);
            if (b_idx) {
                auto ans = buckets[*b_idx].tree.predecessor(x);
                if (ans.has_value()) return ans;
            }
        }
        return rep_trie.predecessor_strict(x);
    }

    std::optional<uint32_t> successor(uint32_t x) const {
        auto rep_opt = rep_trie.successor(x);
        if (!rep_opt) return std::nullopt;
        
        const uint32_t* b_idx = rep_to_idx.find(*rep_opt);
        return b_idx ? buckets[*b_idx].tree.successor(x) : std::nullopt;
    }

    std::optional<uint32_t> predecessor_strict(uint32_t x) const {
        auto rep_opt = rep_trie.successor(x);
        if (rep_opt) {
            const uint32_t* b_idx = rep_to_idx.find(*rep_opt);
            if (b_idx) {
                auto ans = buckets[*b_idx].tree.predecessor_strict(x);
                if (ans.has_value()) return ans;
            }
        }
        return rep_trie.predecessor_strict(x);
    }

    std::optional<uint32_t> successor_strict(uint32_t x) const {
        auto rep_opt = rep_trie.successor(x);
        if (rep_opt) {
            const uint32_t* b_idx = rep_to_idx.find(*rep_opt);
            if (b_idx) {
                auto ans = buckets[*b_idx].tree.successor_strict(x);
                if (ans.has_value()) return ans;
            }
        }
        
        auto next_rep = rep_trie.successor_strict(x);
        if (!next_rep) return std::nullopt;
        
        const uint32_t* next_b_idx = rep_to_idx.find(*next_rep);
        return next_b_idx ? buckets[*next_b_idx].tree.min() : std::nullopt;
    }
};
