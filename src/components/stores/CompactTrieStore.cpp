#include "CompactTrieStore.h"

using namespace CompactTrieStoreNS;

CompactTrieStore::CompactTrieStore() :
    tree(),
    doc_arr(128)
{
}

int CompactTrieStore::get_num_docs() {
    return words_added;
}

void CompactTrieStore::add_document(Doc document) {
    DictDoc dict_doc = DictDoc(document.title, document.start_loc);
    doc_arr.add(dict_doc);
    last_docid_added = doc_arr.n - 1;
    documents_added++;
}

void CompactTrieStore::add(std::string word, Doc document) {
    if (word == "") return;
    RadixTreeWithPostingsNS::Node* treeNode = tree.find(word);

    if (treeNode == nullptr) {
        words_added++;
        treeNode = tree.add(word);
        treeNode->postings.push_back(last_docid_added);
        return;
    }

    if (treeNode->postings.size() != 0) {
        if (treeNode->postings[treeNode->postings.size() - 1] == last_docid_added) {
            return;
        }
    }
    treeNode->postings.push_back(last_docid_added);
}

std::vector<Doc> CompactTrieStore::get(std::string word) {
    RadixTreeWithPostingsNS::Node* treeNode = tree.find(word);

    if (treeNode == nullptr)
        return std::vector<Doc>{};

    std::vector<Doc> temp = std::vector<Doc>{};
    for (int i = 0; i < treeNode->postings.size(); i++) {
        Doc new_doc;
        new_doc.title = (doc_arr[treeNode->postings[i]].word);
        new_doc.start_loc = doc_arr[treeNode->postings[i]].start_loc;
        temp.push_back(new_doc);
    }
    return temp;
}
