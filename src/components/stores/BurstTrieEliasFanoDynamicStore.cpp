#include <cstdint>
#include <memory>
#include <string>
#include <sys/types.h>
#include <variant>
#include <vector>
#include "../../core/interfaces.h"
#include "../../extras/basic/DynamicArray.h"
#include "../../extras/basic/bursttrie/BurstTrie.h"
#include "../../extras/basic/EliasFanoBuffer.hpp"

namespace BurstTrieEliasFanoDynamicStoreNS {

    class WordPostings {
    private:
        EliasFanoBufferNS::EliasFanoBuffer document_ids;

    public:
        WordPostings() : document_ids(EliasFanoBufferNS::EliasFanoBuffer(2)) {}

        void append(uint32_t document_id) {
            if (document_ids.size() != 0 && !document_ids.is_higher_than_last_added_value(document_id)) {
                return;
            }
            document_ids.add(document_id);
        }


        std::size_t size() const {
            return document_ids.size();
        }

        void copy_document_ids_to(std::vector<uint32_t>& output) {
            document_ids.copy_elements_to_vector(output);
    
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

    class BurstTrieEliasFanoDynamicStore : public IStore {
    private:
        BurstTrie<uint32_t> word_dictionary;
        std::vector<WordPostings> postings_lists;
        DynamicArray<DocumentRecord> documents;
        uint32_t last_added_document_id;

        Doc build_document_from_id(uint32_t document_id);

    public:
        BurstTrieEliasFanoDynamicStore();
        ~BurstTrieEliasFanoDynamicStore() override = default;

        void add(std::string word, Doc document) override;
        void add_document(Doc document) override;
        std::vector<Doc> get(std::string word) override;
        int get_num_docs() override;
    };

    BurstTrieEliasFanoDynamicStore::BurstTrieEliasFanoDynamicStore()
        : word_dictionary(),
          postings_lists(),
          documents(2),
          last_added_document_id(0) {}

    void BurstTrieEliasFanoDynamicStore::add_document(Doc document) {
        documents.add(DocumentRecord(std::move(document.title), document.start_loc));
        last_added_document_id = static_cast<uint32_t>(documents.n - 1);
    }

    void BurstTrieEliasFanoDynamicStore::add(std::string word, Doc document) {
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

    std::vector<Doc> BurstTrieEliasFanoDynamicStore::get(std::string word) {
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


    int BurstTrieEliasFanoDynamicStore::get_num_docs() {
        return documents.n;
    }

    Doc BurstTrieEliasFanoDynamicStore::build_document_from_id(uint32_t document_id) {
        Doc document;
        document.title = documents[document_id].title;
        document.start_loc = documents[document_id].start_location;
        return document;
    }
}
