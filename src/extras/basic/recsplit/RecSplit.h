#pragma once

#include "hashfunctions/StringHasher.h"
#include "../DynamicArray.h"
#include "../SortedDynamicArray.h"
#include "../ChaniedHashDictionary.h"
#include <cmath>
#include <variant>

#include "hashfunctions/SignatureHasher.h"
#include "hashfunctions/SignatureCheckHasher.h"

#define LEAF_SIZE 8
#define BUCKET_SIZE 1000
#define NUM_UPPER_BITS 64

template <typename T>
class RecSplit {
private:
    uint64_t HashSignature(unsigned __int128 signature);
    DynamicArray<unsigned __int128>* AssignSignaturesToBuckets(const DynamicArray<unsigned __int128> &signatures);
    DynamicArray<unsigned __int128> CreateSignatures(const DynamicArray<std::string> &keys);

    void ProcessBucket(DynamicArray<unsigned __int128> *signatures);
    void ProcessLeaf(const DynamicArray<unsigned __int128> *signatures);
    bool VerifyHashfunction(const DynamicArray<unsigned __int128> *signatures, uint64_t splitting_unit, SignatureHasher *hasher, uint64_t *
                            amount_in_buckets, uint64_t num_new_buckets);
    uint64_t GetSplittingUnit(uint64_t m);
    uint64_t GetTreeSize(uint64_t m);

    void SaveSeedToFinalOutput(uint64_t seed);
    uint64_t Hash(const std::string& key);

    DynamicArray<uint64_t> seeds;
    DynamicArray<uint64_t> prefix_sums;
    DynamicArray<uint64_t> offsets;
    DynamicArray<uint64_t> bucket_sizes;
    DynamicArray<T> values_array;

    StringHasher string_hasher;
    uint64_t num_buckets;

public:
    RecSplit();
    void CreateMPHF(const DynamicArray<std::string> &keys, const DynamicArray<T> &values);
    T* LookUp(const std::string &word);
};

template<typename T>
RecSplit<T>::RecSplit() : seeds(4), prefix_sums(4), string_hasher(), num_buckets(0) {
}

template<typename T>
void RecSplit<T>::SaveSeedToFinalOutput(const uint64_t seed) {
    seeds.add(seed);
}

template<typename T>
T *RecSplit<T>::LookUp(const std::string &key) {
    uint64_t global_hash = Hash(key);
    if (global_hash == static_cast<uint64_t>(-1)) {
        return nullptr;
    }
    return &values_array[global_hash];
}

template<typename T>
uint64_t RecSplit<T>::Hash(const std::string& key) {
    unsigned __int128 signature = string_hasher.hash(key);
    auto bucket_index = HashSignature(signature);

    uint64_t current_m = bucket_sizes[bucket_index];
    if (current_m == 0)
        return -1;

    uint64_t array_index = offsets[bucket_index];
    uint64_t local_offset = 0;

    while (current_m > LEAF_SIZE) {
        uint64_t current_seed = seeds[array_index];
        uint64_t split_unit = GetSplittingUnit(current_m);

        SignatureHasher hasher(current_m);
        hasher.seed_index = current_seed;

        uint64_t child_index = hasher.hash(signature) / split_unit;

        ++array_index;
        uint64_t tree_size = GetTreeSize(split_unit);
        for (uint64_t i = 0; i < child_index; i++) {
            local_offset += split_unit;
            array_index += tree_size;
        }

        if (child_index == (current_m + split_unit - 1) / split_unit - 1 && current_m % split_unit != 0) {
            current_m = current_m % split_unit;
        } else {
            current_m = split_unit;
        }
    }

    uint64_t leaf_seed = seeds[array_index];
    SignatureHasher hasher(current_m);
    hasher.seed_index = leaf_seed;

    uint64_t bijection_value = hasher.hash(signature);
    local_offset += bijection_value;

    uint64_t global_hash = prefix_sums[bucket_index] + local_offset;
    return global_hash;
}

template<typename T>
void RecSplit<T>::CreateMPHF(const DynamicArray<std::string>& keys, const DynamicArray<T>& values) {
    std::cout << "Creating MPHF of " << values.n  << " keys" << std::endl;

    std::cout << "Creating Signatures" << std::endl;
    DynamicArray<unsigned __int128> signatures = CreateSignatures(keys);

    std::cout << "Assigning To Buckets" << std::endl;
    auto buckets = AssignSignaturesToBuckets(signatures);

    std::cout << "Processing Buckets" << std::endl;
    uint64_t current_key_sum = 0;
    for (uint64_t i = 0; i < num_buckets; i++) {
        prefix_sums.add(current_key_sum);
        offsets.add(seeds.n);
        bucket_sizes.add(buckets[i].n);

        if (buckets[i].n > 0) {
            ProcessBucket(&buckets[i]);
        }

        current_key_sum += buckets[i].n;
    }
    delete[] buckets;

    std::cout << "Created MPHF" << std::endl;
    values_array = DynamicArray<T>(values.n);
    for(int i = 0; i < keys.n; i++) {
        uint64_t global_hash = Hash(keys[i]);
        values_array[global_hash] = std::move(values[i]);
    }
    std::cout << "Rec Split Done" << std::endl;
    printf("sizes %d %d %d %d \n", prefix_sums.n, offsets.n, bucket_sizes.n, seeds.n);
}

template<typename T>
void RecSplit<T>::ProcessBucket(DynamicArray<unsigned __int128>* signatures) {
    if (signatures->n <= LEAF_SIZE) {
        ProcessLeaf(signatures);
        return;
    }

    auto splitting_unit = GetSplittingUnit(signatures->n);
    uint64_t num_new_buckets = (signatures->n + splitting_unit - 1)  / splitting_unit;

    SignatureHasher hasher = SignatureHasher(signatures->n);
    uint64_t seed = -1;
    bool foundHashFunction = false;

    uint64_t* amount_in_buckets = new uint64_t[num_new_buckets];

    while (!foundHashFunction) {
        if (VerifyHashfunction(signatures, splitting_unit, &hasher, amount_in_buckets, num_new_buckets)) {
            foundHashFunction = true;
            seed = hasher.seed_index;
        }
        else {
            hasher.get_new_hash();
        }
    }

    delete[] amount_in_buckets;

    SaveSeedToFinalOutput(seed);

    DynamicArray<unsigned __int128>* new_signatures_buckets = new DynamicArray<unsigned __int128>[num_new_buckets];
    for (uint64_t i = 0; i < num_new_buckets - 1; i++)
        new_signatures_buckets[i].reserve(splitting_unit);

    for (int i = 0; i < signatures->n; i++) {
        uint64_t bucket_index = hasher.hash(signatures->arr[i]) / splitting_unit;
        new_signatures_buckets[bucket_index].add(signatures->arr[i]);
    }

    for (uint64_t i = 0; i < num_new_buckets; i++) {
        ProcessBucket(&new_signatures_buckets[i]);
    }
    delete[] new_signatures_buckets;
}

template<typename T>
bool RecSplit<T>::VerifyHashfunction(const DynamicArray<unsigned __int128> *signatures, const uint64_t splitting_unit, SignatureHasher* hasher,
    uint64_t* amount_in_buckets, uint64_t num_new_buckets) {

    for (uint64_t i = 0; i < num_new_buckets; i++) {
        amount_in_buckets[i] = 0;
    }

    for (int i = 0; i < signatures->n; i++) {
        uint64_t bucket_index = hasher->hash(signatures->arr[i]) / splitting_unit;
        amount_in_buckets[bucket_index]++;
        if (amount_in_buckets[bucket_index] > splitting_unit) {
            return false;
        }
    }

    uint64_t remainder = signatures->n % splitting_unit;
    uint64_t allowed_in_last = (remainder == 0) ? splitting_unit : remainder;
    bool ret_val = amount_in_buckets[num_new_buckets - 1] <= allowed_in_last;
    return ret_val;
}

template<typename T>
void RecSplit<T>::ProcessLeaf(const DynamicArray<unsigned __int128>* signatures) {
    int n = signatures->n;
    if (n == 1) {
        SaveSeedToFinalOutput(0);
        return;
    }
    bool found_perfect_seed = false;
    SignatureHasher hasher = SignatureHasher(n);
    while (!found_perfect_seed) {
        uint32_t used_mask = 0;
        found_perfect_seed = true;

        for (int i = 0; i < n; i++) {
            uint32_t bit = 1u << hasher.hash(signatures->arr[i]);
            if (used_mask & bit) {
                hasher.get_new_hash();
                found_perfect_seed = false;
                break;
            }
            used_mask |= bit;
        }
    }
    SaveSeedToFinalOutput(hasher.seed_index);
}

template<typename T>
uint64_t RecSplit<T>::GetTreeSize(const uint64_t m) {
    if (m <= LEAF_SIZE) {
        return 1;
    }

    uint64_t split_unit = GetSplittingUnit(m);
    uint64_t total_nodes = 1;

    uint64_t full_children = m / split_unit;
    total_nodes += full_children * GetTreeSize(split_unit);

    uint64_t remainder = m % split_unit;
    if (remainder > 0) {
        total_nodes += GetTreeSize(remainder);
    }

    return total_nodes;
}

template<typename T>
uint64_t RecSplit<T>::GetSplittingUnit(const uint64_t m) {
    if (m <= LEAF_SIZE) {
        return m;
    }
    // Lvl 1
    uint64_t s = std::max<uint64_t>(2, static_cast<uint64_t>(std::ceil(0.35 * LEAF_SIZE + 0.5)));
    if (m <= s * LEAF_SIZE) {
        return LEAF_SIZE;
    }
    // Lvl 2
    uint64_t t = static_cast<uint64_t>(std::ceil(0.21 * LEAF_SIZE + 0.9));
    uint64_t stl = s * t * LEAF_SIZE;
    if (m <= stl) {
        return s * LEAF_SIZE;
    }
    // Lvl > 2
    uint64_t half_m = m / 2;
    uint64_t ceil_div = (half_m + stl - 1) / stl;
    return ceil_div * stl;
}

template<typename T>
uint64_t RecSplit<T>::HashSignature(const unsigned __int128 signature) {
    uint64_t u_x = static_cast<uint64_t>(signature >> (128 - NUM_UPPER_BITS));
    uint64_t bucket_index = static_cast<uint64_t>((static_cast<unsigned __int128>(u_x) * num_buckets) >> NUM_UPPER_BITS);
    return bucket_index;
}

template<typename T>
DynamicArray<unsigned __int128>* RecSplit<T>::AssignSignaturesToBuckets(const DynamicArray<unsigned __int128>& signatures) {
    num_buckets = static_cast<uint64_t>((signatures.n + BUCKET_SIZE - 1) / BUCKET_SIZE);
    DynamicArray<unsigned __int128>* buckets = new DynamicArray<unsigned __int128>[num_buckets];
    for (int i = 0; i < signatures.n; i++) {
        uint64_t bucket_index = HashSignature(signatures[i]);
        buckets[bucket_index].add(signatures[i]);
    }
    return buckets;
}

template<typename T>
DynamicArray<unsigned __int128> RecSplit<T>::CreateSignatures(const DynamicArray<std::string>& keys) {
    DynamicArray<unsigned __int128> signatures = DynamicArray<unsigned __int128>(keys.get_size());

    SignatureCheckHasher hasher;
    ChainedHashDictionary<unsigned __int128, unsigned __int128> signature_dict = ChainedHashDictionary<unsigned __int128, unsigned __int128>(keys.n, &hasher);

    //SortedDynamicArray<unsigned __int128> sorted_signatures = SortedDynamicArray<unsigned __int128>(keys.get_size());
    string_hasher = StringHasher();
    for (int i = 0; i < keys.n; i++) {
        signatures.add(string_hasher.hash(keys[i]));

        /*
        int idx = sorted_signatures.find_insert_index(signatures[i]);
        int candidates[2] = { idx, idx - 1 };
        if (candidates[1] < 0)
            candidates[1] = 0;

        if ((sorted_signatures.size() > candidates[0] && sorted_signatures[candidates[0]] == signatures[i]) ||
            (sorted_signatures.size() > candidates[1] && sorted_signatures[candidates[1]] == signatures[i]))
        */
        if (signature_dict.get(signatures[i]) != nullptr)
        {
            std::cout << "Reset" << std::endl;
            signatures.clear();
            signature_dict = ChainedHashDictionary<unsigned __int128, unsigned __int128>(keys.n, &hasher);
            //sorted_signatures.clear();
            string_hasher.get_new_hash();
            i = -1;
            continue;
        }
        signature_dict.add(signatures[i], signatures[i]);
        //sorted_signatures.push_back(signatures[i]);
    }
    return signatures;
}