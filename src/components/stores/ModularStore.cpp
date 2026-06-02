#include <memory>

#include <vector>
#include <string>
#include <cstdint>
#include "../../core/interfaces.h"
//#include "../../extras/basic/CompressedHashTrie.h"
#include "../../extras/basic/CompressedTrie.h"
#include "../../extras/basic/DynamicArray.h"
#include "../../extras/basic/DynamicPackedArray.h"
#include "../../extras/basic/GenericFKSDictionary.h"
#include "../../extras/basic/HashFamily.h"

// First greek guy or smth
namespace ModularStoreNS {

    class ModularStoreDictWord {
    public:
        Trie::Node* word;
        std::vector<uint32_t> documents_in;
        std::vector<ModularStoreDictWord*> originals;

        ModularStoreDictWord(){
            this->word = nullptr;
            documents_in = std::vector<uint32_t>{};
            originals = std::vector<ModularStoreDictWord*>{}; // Fuzzy search not actually implemented
        }
        explicit ModularStoreDictWord(Trie::Node* node) {
            this->word = node;
            documents_in = std::vector<uint32_t>{};
            originals = std::vector<ModularStoreDictWord*>{};
        }
    };

    class ModularStore : public IStore {
    private:
        // Words and titles yk
        Trie::CompressedTrie tree;
        //Doc ID to doc node
        DynamicArray<Trie::Node*> doc_nodes;
        int id_for_last_added_doc = 0;
        void map_id_to_doc_nodes(std::vector<Doc> &res, std::vector<uint32_t> &ids );
        // Dictionary for things
        StringHashFamily* hasher;
        GenericFKSDictionary<std::string, ModularStoreDictWord> word_node_store;


    public:
        ModularStore(StringHashFamily* _hasher, int size) : tree(), doc_nodes(100), word_node_store(size, _hasher) {hasher = _hasher;};
        ~ModularStore() override = default;
		void add(std::string word, Doc document) override;
    	void add_document(Doc document) override;
    	std::vector<Doc> get(std::string word) override;
    	int get_num_docs() override;

    };


    void ModularStore::add_document(Doc document) {
		auto doc_node = tree.add(document.title);
        //if (doc_nodes[doc_nodes.n-1] == doc_node) return;
		doc_nodes.add(doc_node);
		id_for_last_added_doc = doc_nodes.n-1;
    }
    void ModularStore::add(std::string word, Doc document) {
		auto word_node = tree.add(word);
    	ModularStoreDictWord* existing = word_node_store.get(word);
    	if (existing) {
            uint32_t cur_id = static_cast<uint32_t>(id_for_last_added_doc);
            auto& docs = existing->documents_in;
            if (docs.empty() || docs.back() != cur_id) {
               docs.push_back(cur_id);
            }
    	} else {
    		ModularStoreDictWord tdw(word_node);
    		tdw.documents_in.push_back(id_for_last_added_doc);
    		word_node_store.add(word, tdw);
    	}
    }
    std::vector<Doc> ModularStore::get(std::string word) {
		ModularStoreDictWord* tdw = word_node_store.get(word);
    	if (!tdw) return {};
		std::vector<uint32_t> ids = tdw->documents_in;
		std::vector<Doc> res;
    	res.reserve(ids.size());
		map_id_to_doc_nodes(res, ids);
		return res;
    }

    void ModularStore::map_id_to_doc_nodes(std::vector<Doc> &res, std::vector<uint32_t> &ids) {
    	for (int i = 0; i < ids.size(); ++i) {
        	auto doc_node = doc_nodes[ids[i]];
            Doc doc = Doc{Trie::get_string_from_node(doc_node)};
        	res.push_back(doc);
    	}
    }

    int ModularStore::get_num_docs() {
      return doc_nodes.n;
    }
}
