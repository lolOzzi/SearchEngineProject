#include <vector>
#include <string>
#include <cstdint>
#include "../../core/interfaces.h"
#include "../../extras/basic/CompressedHashTrie.h"
#include "../../extras/basic/DynamicArray.h"
#include "../../extras/basic/CuckooHasingDictionary.h"
#include "../../extras/basic/HashFamily.h"
#include "../../extras/basic/EliasFanoExtendableSlow.hpp"

// Paradox greek guy or smth
namespace ModularStoreEliasFanoNS {

    class ModularStoreEliasFanoDictWord {
    public:
        HashTrie::Node* word;
        uint32_t last_added_id;
        ExtensibleEliasFano documents_in;
        std::vector<ModularStoreEliasFanoDictWord*> originals;

        ModularStoreEliasFanoDictWord()
            : word(nullptr),
              last_added_id(0),
              documents_in(UINT32_MAX),
              originals() {}

        explicit ModularStoreEliasFanoDictWord(HashTrie::Node* node)
            : word(node),
              last_added_id(0),
              documents_in(UINT32_MAX),
              originals() {}
    };

    class ModularStoreEliasFano : public IStore {
    private:
        // Words and titles yk
        HashTrie::CompressedHashTrie tree;
        //Doc ID to doc node
        DynamicArray<HashTrie::Node*> doc_nodes;
        uint32_t id_for_last_added_doc = 0;
        void map_id_to_doc_nodes(std::vector<Doc> &res, std::vector<uint32_t> &ids );
        // Dictionary for things
        StringHashFamily* hasher;
        CuckooHashingDictionary<std::string, ModularStoreEliasFanoDictWord> word_node_store;


    public:
        ModularStoreEliasFano(StringHashFamily* _hasher, int size) : tree(), doc_nodes(100), word_node_store(size, _hasher) {hasher = _hasher;};
        ~ModularStoreEliasFano() override = default;
		void add(std::string word, Doc document) override;
    	void add_document(Doc document) override;
    	std::vector<Doc> get(std::string word) override;
    	int get_num_docs() override;

    };


    void ModularStoreEliasFano::add_document(Doc document) {
		auto doc_node = tree.add(document.title);
        //if (doc_nodes[doc_nodes.n-1] == doc_node) return;
		doc_nodes.add(doc_node);
		id_for_last_added_doc = doc_nodes.n-1;
    }
    void ModularStoreEliasFano::add(std::string word, Doc document) {
		auto word_node = tree.add(word);
    	ModularStoreEliasFanoDictWord* existing = word_node_store.get(word);
    	if (existing) {
            uint32_t cur_id = static_cast<uint32_t>(id_for_last_added_doc);
            auto& docs = existing->documents_in;
            if (docs.size() == 0 || existing->last_added_id != cur_id) {
                docs.add(cur_id);
                existing->last_added_id = cur_id;
            }
    	} else {
    		ModularStoreEliasFanoDictWord pdw(word_node);
    		pdw.documents_in.add(id_for_last_added_doc);
            pdw.last_added_id = id_for_last_added_doc;
    		word_node_store.add(word, std::move(pdw));
    	}
    }
    std::vector<Doc> ModularStoreEliasFano::get(std::string word) {
		ModularStoreEliasFanoDictWord* tdw = word_node_store.get(word);
    	if (!tdw) return {};
		std::vector<uint32_t> ids;
        ids.reserve(tdw->documents_in.size()); 
        tdw->documents_in.copy_elements_to_vector(ids);
		std::vector<Doc> res;
    	res.reserve(ids.size());
		map_id_to_doc_nodes(res, ids);
		return res;
    }

    void ModularStoreEliasFano::map_id_to_doc_nodes(std::vector<Doc> &res, std::vector<uint32_t> &ids) {
    	for (int i = 0; i < ids.size(); ++i) {
        	auto doc_node = doc_nodes[ids[i]];
            Doc doc = Doc{HashTrie::get_string_from_node(doc_node)};
        	res.push_back(doc);
    	}
    }

    int ModularStoreEliasFano::get_num_docs() {
      return doc_nodes.n;
    }
}
