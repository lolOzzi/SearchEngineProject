#pragma once

#include <cstdint>
#include <optional>
#include <vector>

namespace RBTreeNS {
template <typename Key>
class RBTree {
    static constexpr uint32_t NIL = 0;

    struct Node {
        uint32_t left, right;
        uint32_t parent_color;  // bit 1 til 32 is index, bit 0 = color (1=red, 0=black)
        Key key;
        uint32_t parent() const {
            return parent_color >> 1;
        }
        bool is_red() const {
            return parent_color & 1;
        }
        void set_parent(uint32_t p) {
            parent_color = (p << 1) | (parent_color & 1);
        }
        void set_color(bool red) {
            parent_color = (parent_color & ~1u) | (uint32_t) red;
        }
    };

    std::vector<Node> nodes;
    uint32_t root = NIL;
    uint32_t free_head = NIL;

    uint32_t alloc(Key key) {
        uint32_t idx;
        if (free_head != NIL) {
            idx = free_head;
            free_head = nodes[idx].left;
        } else {
            idx = (uint32_t) nodes.size();
            nodes.push_back({});
        }
        nodes[idx] = {
            NIL,
            NIL,
            1u,
            key
        }; // red, parent = NIL
        return idx;
    }

    void release(uint32_t idx) {
        nodes[idx].left = free_head;
        free_head = idx;
    }

    void rotate_left(uint32_t x) {
        uint32_t y = nodes[x].right;
        nodes[x].right = nodes[y].left;
        if (nodes[y].left != NIL) nodes[nodes[y].left].set_parent(x);
        uint32_t px = nodes[x].parent();
        nodes[y].set_parent(px);
        if (px == NIL) root = y;
        else if (nodes[px].left == x) nodes[px].left = y;
        else nodes[px].right = y;
        nodes[y].left = x;
        nodes[x].set_parent(y);
    }

    void rotate_right(uint32_t x) {
        uint32_t y = nodes[x].left;
        nodes[x].left = nodes[y].right;
        if (nodes[y].right != NIL) nodes[nodes[y].right].set_parent(x);
        uint32_t px = nodes[x].parent();
        nodes[y].set_parent(px);
        if (px == NIL) root = y;
        else if (nodes[px].right == x) nodes[px].right = y;
        else nodes[px].left = y;
        nodes[y].right = x;
        nodes[x].set_parent(y);
    }

    void insert_fixup(uint32_t z) {
        while (nodes[nodes[z].parent()].is_red()) {
            uint32_t p = nodes[z].parent();
            uint32_t g = nodes[p].parent();
            if (p == nodes[g].left) {
                uint32_t uncle = nodes[g].right;
                if (nodes[uncle].is_red()) {
                    nodes[p].set_color(false);
                    nodes[uncle].set_color(false);
                    nodes[g].set_color(true);
                    z = g;
                } else {
                    if (z == nodes[p].right) {
                        z = p;
                        rotate_left(z);
                        p = nodes[z].parent();
                        g = nodes[p].parent();
                    }
                    nodes[p].set_color(false);
                    nodes[g].set_color(true);
                    rotate_right(g);
                }
            } else {
                uint32_t uncle = nodes[g].left;
                if (nodes[uncle].is_red()) {
                    nodes[p].set_color(false);
                    nodes[uncle].set_color(false);
                    nodes[g].set_color(true);
                    z = g;
                } else {
                    if (z == nodes[p].left) {
                        z = p;
                        rotate_right(z);
                        p = nodes[z].parent();
                        g = nodes[p].parent();
                    }
                    nodes[p].set_color(false);
                    nodes[g].set_color(true);
                    rotate_left(g);
                }
            }
        }
        nodes[root].set_color(false);
    } // move lower tree from one node to other node

    void transplant(uint32_t u, uint32_t v) {
        uint32_t pu = nodes[u].parent();
        if (pu == NIL) root = v;
        else if (nodes[pu].left == u) nodes[pu].left = v;
        else nodes[pu].right = v;
        nodes[v].set_parent(pu);
    }

    uint32_t subtree_min(uint32_t x) const {
        while (nodes[x].left != NIL) x = nodes[x].left;
        return x;
    }
    
    void delete_fixup(uint32_t x) {
        while(x != root && !nodes[x].is_red()) {
            if (x == nodes[nodes[x].parent()].left) {
                uint32_t w = nodes[nodes[x].parent()].right;
                if (nodes[w].is_red()) {
                    nodes[w].set_color(false);
                    nodes[nodes[x].parent()].set_color(true);
                    rotate_left(nodes[x].parent());
                    w = nodes[nodes[x].parent()].right; 
                }
                if (!nodes[nodes[w].left].is_red() && !nodes[nodes[w].right].is_red()) {
                    nodes[w].set_color(true);
                    x = nodes[x].parent();
                }
                else {
                    if (!nodes[nodes[w].right].is_red()) {
                        nodes[nodes[w].left].set_color(false);
                        nodes[w].set_color(true);
                        rotate_right(w);
                        w = nodes[nodes[x].parent()].right;
                    }

                    nodes[w].set_color(nodes[nodes[x].parent()].is_red());
                    nodes[nodes[x].parent()].set_color(false);
                    nodes[nodes[w].right].set_color(false);
                    rotate_left(nodes[x].parent());
                    x = root;
                }

            }
            else {
                uint32_t w = nodes[nodes[x].parent()].left;
                if (nodes[w].is_red()) {
                    nodes[w].set_color(false);
                    nodes[nodes[x].parent()].set_color(true);
                    rotate_right(nodes[x].parent());
                    w = nodes[nodes[x].parent()].left; 
                }
                if (!nodes[nodes[w].right].is_red() && !nodes[nodes[w].left].is_red()) {
                    nodes[w].set_color(true);
                    x = nodes[x].parent();
                }
                else {
                    if (!nodes[nodes[w].left].is_red()) {
                        nodes[nodes[w].right].set_color(false);
                        nodes[w].set_color(true);
                        rotate_left(w);
                        w = nodes[nodes[x].parent()].left;
                    }

                    nodes[w].set_color(nodes[nodes[x].parent()].is_red());
                    nodes[nodes[x].parent()].set_color(false);
                    nodes[nodes[w].left].set_color(false);
                    rotate_right(nodes[x].parent());
                    x = root;
                }
            }
        }
        nodes[x].set_color(false);
    }

public:
    RBTree() {
        nodes.push_back({ Key {}, NIL, NIL, 0 });
    }

    void insert(Key key) {
        uint32_t z = alloc(key);
        uint32_t y = NIL, x = root;
        while (x != NIL) {
            y = x;
            if (key < nodes[x].key) x = nodes[x].left;
            else if (key > nodes[x].key) x = nodes[x].right;
            else {
                release(z);
                return;
            }
        }
        nodes[z].set_parent(y);
        if (y == NIL) root = z;
        else if (key < nodes[y].key) nodes[y].left = z;
        else nodes[y].right = z;
        insert_fixup(z);
    }

    void remove(Key key) {
        uint32_t curr = root;
        bool found = false;
        uint32_t z, y;
        while (curr != NIL) {
            if (key < nodes[curr].key) curr = nodes[curr].left;
            else if (key > nodes[curr].key) curr = nodes[curr].right;
            else {
                z = curr;
                found = true;
                break;
            }
        }
        if (!found) {
            return;
        }
        
        
        uint32_t x;
        y = z;
        bool y_orig_colour = nodes[y].is_red();
        if (nodes[z].left == NIL) {
            x = nodes[z].right;
            transplant(z,nodes[z].right);
        }
        else if (nodes[z].right == NIL) {
            x = nodes[z].left;
            transplant(z, nodes[z].left);
        }
        else {
            y = subtree_min(nodes[z].right);
            y_orig_colour = nodes[y].is_red();
            x = nodes[y].right;
            if (y != nodes[z].right) {
                transplant(y, nodes[y].right);
                nodes[y].right = nodes[z].right;
                nodes[nodes[y].right].set_parent(y);
            } 
            else {
                nodes[x].set_parent(y);
            }
            transplant(z, y);
            nodes[y].left = nodes[z].left;
            nodes[nodes[y].left].set_parent(y);
            nodes[y].set_color(nodes[z].is_red());
        }
        if (y_orig_colour == false) {
            delete_fixup(x);
        }
        release(z);
    }

    bool contains(Key key) const {
        uint32_t x = root;
        while (x != NIL) {
            if (key < nodes[x].key) x = nodes[x].left;
            else if (key > nodes[x].key) x = nodes[x].right;
            else return true;
        }
        return false;
    }

    std::optional<Key> predecessor(Key query) const {
        uint32_t result = NIL, x = root;
        while (x != NIL) {
            if (nodes[x].key <= query) {
                result = x;
                x = nodes[x].right;
            } else x = nodes[x].left;
        }
        return result == NIL ? std::nullopt : std::optional(nodes[result].key);
    }

    std::optional<Key> predecessor_strict(Key query) const {
        uint32_t result = NIL, x = root;
        while (x != NIL) {
            if (nodes[x].key < query) {
                result = x;
                x = nodes[x].right;
            } else x = nodes[x].left;
        }
        return result == NIL ? std::nullopt : std::optional(nodes[result].key);
    }

    std::optional<Key> successor_strict(Key query) const {
        uint32_t result = NIL, x = root;
        while (x != NIL) {
            if (nodes[x].key > query) {
                result = x;
                x = nodes[x].left;
            } else x = nodes[x].right;
        }
        return result == NIL ? std::nullopt : std::optional(nodes[result].key);
}

    std::optional<Key> successor(Key query) const {
        uint32_t result = NIL, x = root;
        while (x != NIL) {
            if (nodes[x].key >= query) {
                result = x;
                x = nodes[x].left;
            } else x = nodes[x].right;
        }
        return result == NIL ? std::nullopt : std::optional(nodes[result].key);
    }

    std::optional<Key> min() const {
        if (root == NIL) return std::nullopt;
        return nodes[subtree_min(root)].key;
    }

    std::optional<Key> max() const {
        if (root == NIL) return std::nullopt;
        uint32_t x = root;
        while (nodes[x].right != NIL) x = nodes[x].right;
        return nodes[x].key;
    }

    bool empty() const {
        return root == NIL;
    }
};
}
