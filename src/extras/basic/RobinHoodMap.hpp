#pragma once
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <new>
#include <utility>
#include <vector>

template<class Key, class T, class Hash = std::hash<Key>, class Equal = std::equal_to<Key>>
class RobinHoodMap {
    using Entry = std::pair<Key, T>;

    struct Slot {
        std::uint32_t distance_plus_one = 0;
        alignas(Entry) std::byte storage[sizeof(Entry)];
    };

    std::vector<Slot> slots;
    std::size_t count = 0;
    Hash hasher;
    Equal equal;

    static Entry& entry(Slot& slot) {
        return *std::launder(reinterpret_cast<Entry*>(&slot.storage));
    }

    static const Entry& entry(const Slot& slot) {
        return *std::launder(reinterpret_cast<const Entry*>(&slot.storage));
    }

    bool occupied(const Slot& slot) const {
        return slot.distance_plus_one != 0;
    }

    std::size_t mask() const {
        return slots.size() - 1;
    }

    std::size_t find_slot(const Key& key) const {
        if (slots.empty())
            return slots.size();

        std::size_t hash = hasher(key);
        std::size_t index = hash & mask();
        std::size_t distance = 0;

        while (true) {
            const Slot& slot = slots[index];
            if (!occupied(slot))
                return slots.size();

            std::size_t resident_distance = slot.distance_plus_one - 1;
            if (resident_distance < distance)
                return slots.size();

            if (equal(entry(slot).first, key))
                return index;

            index = (index + 1) & mask();
            ++distance;
        }
    }

    bool needs_growth_for_one_more() const {
        return slots.empty() || (count + 1) * 10 > slots.size() * 7;
    }

    bool insert_unique(Entry entr) {
        std::size_t hash = hasher(entr.first);
        std::size_t index = hash & mask();
        std::size_t distance = 0;
    
        while (true) {
            Slot& slot = slots[index];
    
            if (!occupied(slot)) {
                std::construct_at(&entry(slot), std::move(entr));
                slot.distance_plus_one = static_cast<std::uint32_t>(distance + 1);
                ++count;
                return true;
            }
    
            if (equal(entry(slot).first, entr.first))
                return false;
    
            std::size_t resident_distance = slot.distance_plus_one - 1;
            if (resident_distance < distance) {
                std::swap(entr, entry(slot));
                slot.distance_plus_one = static_cast<std::uint32_t>(distance + 1);
                distance = resident_distance + 1;
            } else {
                ++distance;
            }
    
            index = (index + 1) & mask();
        }
    }

    void rehash(std::size_t new_capacity) {
        std::vector<Slot> old_slots = std::move(slots);
        slots = std::vector<Slot>(new_capacity);
        count = 0;

        for (Slot& slot : old_slots) {
            if (!occupied(slot))
                continue;

            Entry moved = std::move(entry(slot));
            std::destroy_at(&entry(slot));
            slot.distance_plus_one = 0;
            insert_unique(std::move(moved));
        }
    }

public:
    RobinHoodMap() = default;
    RobinHoodMap(RobinHoodMap&&) noexcept = default;
    RobinHoodMap& operator=(RobinHoodMap&&) noexcept = default;
    ~RobinHoodMap() { clear(); }

    RobinHoodMap(const RobinHoodMap&) = delete;
    RobinHoodMap& operator=(const RobinHoodMap&) = delete;

    std::size_t size() const { return count; }
    bool empty() const { return count == 0; }

    void clear() {
        for (Slot& slot : slots) {
            if (!occupied(slot))
                continue;

            std::destroy_at(&entry(slot));
            slot.distance_plus_one = 0;
        }
        count = 0;
    }

    T* find(const Key& key) {
        std::size_t index = find_slot(key);
        return index == slots.size() ? nullptr : &entry(slots[index]).second;
    }

    const T* find(const Key& key) const {
        std::size_t index = find_slot(key);
        return index == slots.size() ? nullptr : &entry(slots[index]).second;
    }

    bool contains(const Key& key) const {
        return find_slot(key) != slots.size();
    }

    bool insert(Key key, T value) {
        if (needs_growth_for_one_more())
            rehash(slots.empty() ? 8 : slots.size() * 2);

        return insert_unique(Entry(std::move(key), std::move(value)));
    }

    bool set(Key key, T value) {
        if (std::size_t index = find_slot(key); index != slots.size()) {
            entry(slots[index]).second = std::move(value);
            return false;
        }

        if (needs_growth_for_one_more())
            rehash(slots.empty() ? 8 : slots.size() * 2);

        insert_unique(Entry(std::move(key), std::move(value)));
        return true;
    }

    bool remove(const Key& key) {
        std::size_t index = find_slot(key);
        if (index == slots.size())
            return false;

        std::destroy_at(&entry(slots[index]));
        slots[index].distance_plus_one = 0;
        --count;

        std::size_t hole = index;
        std::size_t next = (hole + 1) & mask();

        while (occupied(slots[next]) && slots[next].distance_plus_one > 1) {
            std::construct_at(&entry(slots[hole]), std::move(entry(slots[next])));
            slots[hole].distance_plus_one = slots[next].distance_plus_one - 1;

            std::destroy_at(&entry(slots[next]));
            slots[next].distance_plus_one = 0;

            hole = next;
            next = (next + 1) & mask();
        }

        return true;
    }
};
