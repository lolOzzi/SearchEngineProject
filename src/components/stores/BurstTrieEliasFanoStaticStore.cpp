#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include "../../core/interfaces.h"
#include "../../extras/basic/DynamicArray.h"
#include "../../extras/basic/bursttrie/BurstTrie.h"
#include "../../extras/basic/EliasFanoBuffer.hpp"

namespace BurstTrieEliasFanoStaticStoreNS {

class DocumentTable {
    DynamicArray<std::string> titles;
    DynamicArray<long long> raw_locations;
public:
    DocumentTable() : titles(2), raw_locations(2) {}

    void append(std::string title, long long loc) {
        titles.add(std::move(title));
        raw_locations.add(loc);
    }

    std::size_t size() const { return titles.n; }

    const std::string& title_at(uint32_t id) const { return titles[id]; }

    long long start_location_at(uint32_t id) const {
        return raw_locations[id];
    }
};

class BurstTrieEliasFanoStaticStore : public IStore, public ICompressible {
    BurstTrie<uint32_t> word_dictionary;
    std::vector<DynamicArray<uint32_t>> raw_postings;
    std::vector<std::unique_ptr<EliasFanoBuffer>> postings;
    DocumentTable documents;
    uint32_t last_added_document_id = 0;

    Doc build_document_from_id(uint32_t document_id) const;
public:
    BurstTrieEliasFanoStaticStore() = default;
    ~BurstTrieEliasFanoStaticStore() override = default;
    void add(std::string word, Doc document) override;
    void add_document(Doc document) override;
    std::vector<Doc> get(std::string word) override;
    int get_num_docs() override;
    void compress() override;
};

void BurstTrieEliasFanoStaticStore::add_document(Doc document) {
    documents.append(std::move(document.title), document.start_loc);
    last_added_document_id = static_cast<uint32_t>(documents.size() - 1);
}

void BurstTrieEliasFanoStaticStore::add(std::string word, Doc /*document*/) {
    if (word.empty()) return;

    uint32_t* existing = word_dictionary.get(word);
    uint32_t word_id;
    if (!existing) {
        word_id = static_cast<uint32_t>(raw_postings.size());
        raw_postings.emplace_back(2);
        word_dictionary.add(word, word_id);
    } else {
        word_id = *existing;
    }

    DynamicArray<uint32_t>& list = raw_postings[word_id];
    if (list.n == 0 || list[list.n - 1] != last_added_document_id)
        list.add(last_added_document_id);
}

void BurstTrieEliasFanoStaticStore::compress() {
    const uint32_t universe = static_cast<uint32_t>(documents.size());
    postings.reserve(raw_postings.size());
    for (DynamicArray<uint32_t>& raw : raw_postings) {
        auto buf = std::make_unique<EliasFanoBuffer>(universe);
        for (int i = 0; i < raw.n; i++) buf->add(raw[i]);
        postings.push_back(std::move(buf));
    }
    std::vector<DynamicArray<uint32_t>>().swap(raw_postings);
}

std::vector<Doc> BurstTrieEliasFanoStaticStore::get(std::string word) {
    uint32_t* word_id = word_dictionary.get(word);
    if (!word_id) return {};

    std::vector<uint32_t> document_ids;
    postings[*word_id]->copy_elements_to_vector(document_ids);

    std::vector<Doc> results;
    results.reserve(document_ids.size());
    for (uint32_t id : document_ids) results.push_back(build_document_from_id(id));
    return results;
}

int BurstTrieEliasFanoStaticStore::get_num_docs() {
    return static_cast<int>(documents.size());
}

Doc BurstTrieEliasFanoStaticStore::build_document_from_id(uint32_t document_id) const {
    Doc document;
    document.title     = documents.title_at(document_id);
    document.start_loc = documents.start_location_at(document_id);
    return document;
}

}
