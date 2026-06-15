#include "RecSplitEliasFanoStaticStore.h"

namespace RecSplitEliasFanoStaticStoreNS {
    RecSplitEliasFanoStaticStore::RecSplitEliasFanoStaticStore()
        : word_dictionary(4, &hash_family),
          postings_lists(),
          documents(2),
          last_added_document_id(0) {}

    void RecSplitEliasFanoStaticStore::add_document(Doc document) {
        documents.add(DocumentRecord(std::move(document.title), document.start_loc));
        last_added_document_id = static_cast<uint32_t>(documents.n - 1);
    }

    void RecSplitEliasFanoStaticStore::add(std::string word, Doc document) {
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

    std::vector<Doc> RecSplitEliasFanoStaticStore::get(std::string word) {
        //uint32_t* word_id = word_dictionary.get(word);
        uint32_t* word_id = static_word_dictionary.LookUp(word);
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

    void RecSplitEliasFanoStaticStore::compress() {
        uint32_t universe = static_cast<uint32_t>(documents.n);
        for (WordPostings& postings : postings_lists) {
            if (!postings.is_compressed()) {
                postings.compress(universe);
            }
        }

        auto items = word_dictionary.move_all_items();
        int n = word_dictionary.get_num_items();
        word_dictionary.informal_delete();
        DynamicArray<std::string> keys = DynamicArray<std::string>(n);
        DynamicArray<uint32_t> values = DynamicArray<uint32_t>(n);
        for (int i = 0; i < n; i++) {
            keys.add(std::move(items[i].key));
            values.add(items[i].val);
        }
        std::cout << n << std::endl;
        static_word_dictionary.CreateMPHF(keys, values);

        delete[] items;

    }

    int RecSplitEliasFanoStaticStore::get_num_docs() {
        return documents.n;
    }

    Doc RecSplitEliasFanoStaticStore::build_document_from_id(uint32_t document_id) {
        Doc document;
        document.title = documents[document_id].title;
        document.start_loc = documents[document_id].start_location;
        return document;
    }
}
