#include <memory>
#include <vector>
#include <string>
#include <cstdint>
#include "../../core/interfaces.h"
#include "../../extras/basic/CompressedTrie.h"
#include "../../extras/basic/DynamicArray.h"
#include "../../extras/basic/DynamicPackedArray.h"
#include "../../extras/basic/GenericFKSDictionary.h"
#include "../../extras/basic/HashFamily.h"

// Important greek guy or smth
namespace ModularStorePackedNS {

    class ModularStorePackedDictWord {
    public:
        Trie::Node* word;
        uint32_t last_added_id;
        DynamicPackedArray documents_in;
        std::vector<ModularStorePackedDictWord*> originals;

        ModularStorePackedDictWord(){
            this->word = nullptr;
            documents_in = DynamicPackedArray{};
            last_added_id = 0;
            originals = std::vector<ModularStorePackedDictWord*>{}; // Fuzzy search not actually implemented
        }
        explicit ModularStorePackedDictWord(Trie::Node* node) {
            this->word = node; 
            documents_in = DynamicPackedArray{};
            last_added_id = 0;
            originals = std::vector<ModularStorePackedDictWord*>{};
        }
    };

    class ModularStorePacked : public IStore {
    private:
        // Words and titles yk
        Trie::CompressedTrie tree;
        //Doc ID to doc node
        DynamicArray<Trie::Node*> doc_nodes;
        uint32_t id_for_last_added_doc = 0;
        void map_id_to_doc_nodes(std::vector<Doc> &res, std::vector<uint32_t> &ids );
        // Dictionary for things
        StringHashFamily* hasher;
        GenericFKSDictionary<std::string, ModularStorePackedDictWord> word_node_store;


    public:
        ModularStorePacked(StringHashFamily* _hasher, int size) : tree(), doc_nodes(100), word_node_store(size, _hasher) {hasher = _hasher;};
        ~ModularStorePacked() override = default;
		void add(std::string word, Doc document) override;
    	void add_document(Doc document) override;
    	std::vector<Doc> get(std::string word) override;
    	int get_num_docs() override;

    };


    void ModularStorePacked::add_document(Doc document) {
		auto doc_node = tree.add(document.title);
        //if (doc_nodes[doc_nodes.n-1] == doc_node) return;
		doc_nodes.add(doc_node);
		id_for_last_added_doc = doc_nodes.n-1;
    }
    void ModularStorePacked::add(std::string word, Doc document) {
		auto word_node = tree.add(word);
    	ModularStorePackedDictWord* existing = word_node_store.get(word);
    	if (existing) {
            uint32_t cur_id = static_cast<uint32_t>(id_for_last_added_doc);
            auto& docs = existing->documents_in;
            if (docs.get_elem_count() == 0 || existing->last_added_id != cur_id) {
                docs.add(cur_id);
                existing->last_added_id = cur_id;
            }
    	} else {
    		ModularStorePackedDictWord pdw(word_node);
    		pdw.documents_in.add(id_for_last_added_doc);
            pdw.last_added_id = id_for_last_added_doc;
    		word_node_store.add(word, std::move(pdw));
    	}
    }
    std::vector<Doc> ModularStorePacked::get(std::string word) {
		ModularStorePackedDictWord* tdw = word_node_store.get(word);
    	if (!tdw) return {};
		std::vector<uint32_t> ids;
        ids.reserve(tdw->documents_in.get_elem_count());
        tdw->documents_in.copy_elements_to_vector(ids);
		std::vector<Doc> res;
    	res.reserve(ids.size());
		map_id_to_doc_nodes(res, ids);
		return res;
    }

    void ModularStorePacked::map_id_to_doc_nodes(std::vector<Doc> &res, std::vector<uint32_t> &ids) {
    	for (int i = 0; i < ids.size(); ++i) {
        	auto doc_node = doc_nodes[ids[i]];
            Doc doc = Doc{Trie::get_string_from_node(doc_node)};
        	res.push_back(doc);
    	}
    }

    int ModularStorePacked::get_num_docs() {
      return doc_nodes.n;
    }
}
