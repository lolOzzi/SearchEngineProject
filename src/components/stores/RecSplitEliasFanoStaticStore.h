#pragma once
#include <cstdint>
#include <memory>
#include <string>
#include <variant>
#include <vector>
#include "../../core/interfaces.h"
#include "../../extras/basic/CuckooHasingDictionary.h"
#include "../../extras/basic/DynamicArray.h"
#include "../../extras/basic/EliasFanoStatic.hpp"
#include "../../extras/basic/recsplit/RecSplit.h"

namespace RecSplitEliasFanoStaticStoreNS {

    class WordPostings {
    private:
        std::variant<DynamicArray<uint32_t>, std::unique_ptr<EliasFanoStaticNS::EliasFanoStatic>> document_ids;

    public:
        WordPostings() : document_ids(DynamicArray<uint32_t>(2)) {}

        bool is_compressed() const {
            return std::holds_alternative<std::unique_ptr<EliasFanoStaticNS::EliasFanoStatic>>(document_ids);
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
            std::vector<uint32_t> res;
            uncompressed.copy_elements_to_vector(res);
            std::unique_ptr<EliasFanoStaticNS::EliasFanoStatic> compressed = std::make_unique<EliasFanoStaticNS::EliasFanoStatic>(res);
            document_ids = std::move(compressed);
        }

        std::size_t size() const {
            if (is_compressed()) {
                return std::get<std::unique_ptr<EliasFanoStaticNS::EliasFanoStatic>>(document_ids)->get_elem_count();
            }
            return std::get<DynamicArray<uint32_t>>(document_ids).n;
        }

        void copy_document_ids_to(std::vector<uint32_t>& output) const {
            if (is_compressed()) {
                std::get<std::unique_ptr<EliasFanoStaticNS::EliasFanoStatic>>(document_ids)->copy_elements_to_vector(output);
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

    class RecSplitEliasFanoStaticStore : public IStore, public ICompressible {
    private:
        StringHashFamily hash_family = StringHashFamily();
        CuckooHashingDictionary<std::string, uint32_t> word_dictionary;
        RecSplit<uint32_t> static_word_dictionary;
        std::vector<WordPostings> postings_lists;
        DynamicArray<DocumentRecord> documents;
        uint32_t last_added_document_id;

        Doc build_document_from_id(uint32_t document_id);

    public:
        RecSplitEliasFanoStaticStore();
        ~RecSplitEliasFanoStaticStore() override = default;

        void add(std::string word, Doc document) override;
        void add_document(Doc document) override;
        std::vector<Doc> get(std::string word) override;
        int get_num_docs() override;
        void compress() override;
    };
}
