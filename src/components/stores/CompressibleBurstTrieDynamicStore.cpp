#include <cstdint>
#include <memory>
#include <string>
#include <variant>
#include <vector>
#include "../../core/interfaces.h"
#include "../../extras/basic/DynamicArray.h"
#include "../../extras/basic/bursttrie/BurstTrie.h"
#include "../../extras/basic/EliasFanoBuffer.hpp"

namespace CompressibleBurstTrieStoreNS {

    class WordPostings {
    private:
        std::variant<DynamicArray<uint32_t>, std::unique_ptr<EliasFanoBufferNS::EliasFanoBuffer>> document_ids;

    public:
        WordPostings() : document_ids(DynamicArray<uint32_t>(2)) {}

        bool is_compressed() const {
            return std::holds_alternative<std::unique_ptr<EliasFanoBufferNS::EliasFanoBuffer>>(document_ids);
        }

        void append(uint32_t document_id) {
            DynamicArray<uint32_t>& uncompressed = std::get<DynamicArray<uint32_t>>(document_ids);
            if (uncompressed.n != 0 && uncompressed[uncompressed.n - 1] == document_id) {
                return;
            }
            uncompressed.add(document_id);
        }

        void compress(uint32_t universe) {
            DynamicArray<uint32_t>& uncompressed = std::get<DynamicArray<uint32_t>>(document_ids);
            std::unique_ptr<EliasFanoBufferNS::EliasFanoBuffer> compressed = std::make_unique<EliasFanoBufferNS::EliasFanoBuffer>(universe);
            for (int i = 0; i < uncompressed.get_size(); ++i) {
                compressed->add(uncompressed[i]);
            }
            document_ids = std::move(compressed);
        }

        std::size_t size() const {
            if (is_compressed()) {
                return std::get<std::unique_ptr<EliasFanoBufferNS::EliasFanoBuffer>>(document_ids)->size();
            }
            return std::get<DynamicArray<uint32_t>>(document_ids).n;
        }

        void copy_document_ids_to(std::vector<uint32_t>& output) const {
            if (is_compressed()) {
                std::get<std::unique_ptr<EliasFanoBufferNS::EliasFanoBuffer>>(document_ids)->copy_elements_to_vector(output);
                return;
            }
            const DynamicArray<uint32_t>& uncompressed = std::get<DynamicArray<uint32_t>>(document_ids);
            for (int i = 0; i < uncompressed.n; i++) {
                output.push_back(uncompressed[i]);
            }
        }
    };

    class DocumentRecord {
    public:
        std::string title;
        long long start_location;
        DocumentRecord() : title(), start_location(0) {}

        DocumentRecord(std::string document_title, long long document_start_location)
            : title(std::move(document_title)), start_location(document_start_location) {}
    };

    class CompressibleBurstTrieStore : public IStore, public ICompressible {
    private:
        BurstTrie<uint32_t> word_dictionary;
        std::vector<WordPostings> postings_lists;
        DynamicArray<DocumentRecord> documents;
        uint32_t last_added_document_id;

        Doc build_document_from_id(uint32_t document_id);

    public:
        CompressibleBurstTrieStore();
        ~CompressibleBurstTrieStore() override = default;

        void add(std::string word, Doc document) override;
        void add_document(Doc document) override;
        std::vector<Doc> get(std::string word) override;
        int get_num_docs() override;
        void compress() override;
    };

    CompressibleBurstTrieStore::CompressibleBurstTrieStore()
        : word_dictionary(),
          postings_lists(),
          documents(2),
          last_added_document_id(0) {}

    void CompressibleBurstTrieStore::add_document(Doc document) {
        documents.add(DocumentRecord(std::move(document.title), document.start_loc));
        last_added_document_id = static_cast<uint32_t>(documents.n - 1);
    }

    void CompressibleBurstTrieStore::add(std::string word, Doc document) {
        if (word.empty()) {
            return;
        }
        uint32_t* existing_word_id = word_dictionary.get(word);
        if (existing_word_id == nullptr) {
            uint32_t new_word_id = static_cast<uint32_t>(postings_lists.size());
            postings_lists.emplace_back();
            word_dictionary.add(word, new_word_id);
            postings_lists[new_word_id].append(last_added_document_id);
            return;
        }
        postings_lists[*existing_word_id].append(last_added_document_id);
    }

    std::vector<Doc> CompressibleBurstTrieStore::get(std::string word) {
        uint32_t* word_id = word_dictionary.get(word);
        if (word_id == nullptr) {
            return {};
        }
        WordPostings& postings = postings_lists[*word_id];
        std::vector<uint32_t> document_ids;
        document_ids.reserve(postings.size());
        postings.copy_document_ids_to(document_ids);
        std::vector<Doc> results;
        results.reserve(document_ids.size());
        for (uint32_t document_id : document_ids) {
            results.push_back(build_document_from_id(document_id));
        }
        return results;
    }

    void CompressibleBurstTrieStore::compress() {
        uint32_t universe = static_cast<uint32_t>(documents.n);
        for (WordPostings& postings : postings_lists) {
            if (!postings.is_compressed()) {
                postings.compress(universe);
            }
        }
    }

    int CompressibleBurstTrieStore::get_num_docs() {
        return documents.n;
    }

    Doc CompressibleBurstTrieStore::build_document_from_id(uint32_t document_id) {
        Doc document;
        document.title = documents[document_id].title;
        document.start_loc = documents[document_id].start_location;
        return document;
    }
}
