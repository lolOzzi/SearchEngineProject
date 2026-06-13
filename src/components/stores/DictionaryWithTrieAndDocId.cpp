#include "DictionaryWithTrieAndDocId.h"
#include <cassert>

DictionaryWithTrieAndDocId::DictionaryWithTrieAndDocId(int n, IHash* hash_function) :
    tree(),
    dict(n, &hash_family),
    doc_arr(128)
{
}
int DictionaryWithTrieAndDocId::get_num_docs() {
    return words_added;
}

void DictionaryWithTrieAndDocId::add_document(Doc document) {
    Node* node = tree.add(document.title);
    DictDoc dict_doc = DictDoc(node, document.start_loc);
    doc_arr.add(dict_doc);
    last_docid_added = doc_arr.n - 1;
    documents_added++;
    node->label.set_to_document();
}

void DictionaryWithTrieAndDocId::add(std::string word, Doc document) {
    if (word == "") return;

    DictWord* word_in_document = dict.get(word);

    if (!word_in_document || word_in_document->word == nullptr) {
        words_added++;

        Node* node = tree.add(word);
        DictWord new_word = DictWord(node);

        word_in_document = dict.add(word, new_word);
        assert(word_in_document->word != nullptr);
        word_in_document->documents_in.add(last_docid_added);
        return;
    }

    if (word_in_document->documents_in.n != 0) {
        if (word_in_document->documents_in[word_in_document->documents_in.n - 1] == last_docid_added) {
            return;
        }
    }
    word_in_document->documents_in.add(last_docid_added);
}

std::vector<Doc> DictionaryWithTrieAndDocId::get(std::string word) {
    DictWord* res = dict.get(word);
    if (res == nullptr) return std::vector<Doc>{};

    std::vector<Doc> temp = std::vector<Doc>{};

    for (int i = 0; i < res->documents_in.n; i++) {
        Doc new_doc;
        new_doc.title = get_string_from_node(doc_arr[res->documents_in[i]].word);
        new_doc.start_loc = doc_arr[res->documents_in[i]].start_loc;
        temp.push_back(new_doc);
    }

    return temp;
}


static void prefix_search_from_node(Node* node, std::vector<Node*>* document_nodes) {
    for (auto& node_ptr : node->targets) {
        Node* loop_node = node_ptr.get();
        prefix_search_from_node(loop_node, document_nodes);

        if (loop_node->label.is_document()) {
            document_nodes->push_back(loop_node);
        }
    }
}

std::vector<Doc> DictionaryWithTrieAndDocId::prefix_search(std::string word) {
    Node* best = find_best_node(&tree.root, word);
    std::vector<Doc> temp = std::vector<Doc>{};
    std::vector<Node*> docs = std::vector<Node*>{};
    prefix_search_from_node(best, &docs);

    for (int i = 0; i < doc_arr.n; i++) {
        for (auto& node_ptr : docs) {
            if (doc_arr[i].word == node_ptr) {
                Doc new_doc;
                new_doc.title = get_string_from_node(doc_arr[i].word);
                new_doc.start_loc = doc_arr[i].start_loc;
                temp.push_back(new_doc);
            }
        }
    }
    return temp;
}

void DictionaryWithTrieAndDocId::print_spaced_used() {
    unsigned long long doc_holder_num_items = static_cast<unsigned long long>(doc_arr.n);
    unsigned long long doc_holder_space_used = doc_arr.get_size() * sizeof(DictDoc);
    printf("Documents holder has %llu items and uses %llu bytes.\n", doc_holder_num_items, doc_holder_space_used);

    unsigned long long dict_num_items = static_cast<unsigned long long>(dict.get_num_items());
    unsigned long long dict_space_used = dict.get_size() * dict.get_item_size();
    printf("Word dictionary has %llu items and uses %llu bytes.\n", dict_num_items, dict_space_used);

    auto dict_items = dict.get_all_items();
    unsigned long long doc_arr_item_count = 0;
    unsigned long long doc_arr_size_count = 0;
    for (int i = 0; i < dict.get_num_items(); i++) {
        doc_arr_item_count += static_cast<unsigned long long>(dict_items[i]->val.documents_in.n);
        doc_arr_size_count += static_cast<unsigned long long>(dict_items[i]->val.documents_in.get_size());
    }
    printf("Words documents_in has %llu items and uses %llu bytes.\n", doc_arr_item_count, doc_arr_size_count * sizeof(int));

    unsigned long long tree_num_nodes = 0;
    unsigned long long tree_string_space_used = 0;
    tree.get_space_used(&tree.root, &tree_num_nodes, &tree_string_space_used);
    printf("Compressed trie has %llu items and uses %llu bytes.\n", tree_num_nodes, tree_string_space_used);

    printf("Total space used is %llu\n", doc_holder_space_used + dict_space_used +
                                               (doc_arr_size_count* sizeof(int)) + tree_string_space_used);
}
