#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <sys/types.h>
#include <variant>
#include <vector>
#include "../../core/interfaces.h"
#include "../../extras/basic/DynamicArray.h"
#include "../../extras/basic/bursttrie/BurstTrie.h"
#include "../../extras/basic/DeltaEncodedArray.h"
#include "../stores/RegexStoreComp.cpp"


namespace BurstTrieDeltaDynamicStoreRegexNS {

    class WordPostings {
    private:
         DeltaEncodedArray document_ids;

    public:
        WordPostings() : document_ids() {}

        void append(uint32_t document_id) {
            if (document_ids.get_elem_count() != 0 && !document_ids.is_higher_than_last_added_value(document_id)) {
                return;
            }
            document_ids.add(document_id);
        }


        std::size_t size() {
            return document_ids.get_elem_count();
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

    class BurstTrieDeltaDynamicStoreRegex : public IStore {
    private:
        BurstTrie<uint32_t> word_dictionary;
        std::vector<WordPostings> postings_lists;
        DynamicArray<DocumentRecord> documents;
        uint32_t last_added_document_id;

        Doc build_document_from_id(uint32_t document_id);
        RegexStoreComp reg;


    public:
        BurstTrieDeltaDynamicStoreRegex();
        ~BurstTrieDeltaDynamicStoreRegex() override = default;

        void add(std::string word, Doc document) override;
        void add_document(Doc document) override;
        std::vector<Doc> get(std::string word) override;
        int get_num_docs() override;
        std::vector<std::string> getWordsFromTrigram(std::string tri);

    };
}
