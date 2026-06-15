#include "BurstTrieEliasFanoDynamicStore.h"

namespace BurstTrieEliasFanoDynamicStoreNS {
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
