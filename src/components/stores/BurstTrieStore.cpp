#include "BurstTrieStore.h"
#include <cassert>

using namespace BurstTrieStoreNS;

BurstTrieStore::BurstTrieStore() :
    tree(),
    doc_arr(128)
{
}

int BurstTrieStore::get_num_docs() {
    return words_added;
}

void BurstTrieStore::add_document(Doc document) {
    DictDoc dict_doc = DictDoc(document.title, document.start_loc);
    doc_arr.add(dict_doc);
    last_docid_added = doc_arr.n - 1;
    documents_added++;
}

void BurstTrieStore::add(std::string word, Doc document) {
    if (word == "") return;
    TreeWord* word_in_document = tree.get(word);

    if (word_in_document == nullptr) {
        words_added++;
        word_in_document = tree.add(word, TreeWord());
        assert(word_in_document != nullptr);
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

std::vector<Doc> BurstTrieStore::get(std::string word) {
    TreeWord* res = tree.get(word);
    if (res == nullptr) return std::vector<Doc>{};
    std::vector<Doc> temp = std::vector<Doc>{};
    for (int i = 0; i < res->documents_in.n; i++) {
        Doc new_doc;
        new_doc.title = (doc_arr[res->documents_in[i]].word);
        new_doc.start_loc = doc_arr[res->documents_in[i]].start_loc;
        temp.push_back(new_doc);
    }
    return temp;
}
