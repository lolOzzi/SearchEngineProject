#include "BasicHashTable.h"

using namespace BasicHashTableNS;

BasicHashTable::BasicHashTable(int n, IHash* hash_function) {
    numBuckets = n;
    buckets = new Word*[n];
    for (int i = 0; i < numBuckets; ++i) buckets[i] = nullptr;
    this->hash_function = hash_function;
}

BasicHashTable::~BasicHashTable() {
    if (!buckets) return;
    for (int i = 0; i < numBuckets; ++i) {
        Word* cur = buckets[i];
        while (cur) {
            Word* next = cur->next;
            delete cur;
            cur = next;
        }
    }
    delete [] buckets;
    buckets = nullptr;
}

Word *BasicHashTable::get_word(std::string word) {
    std::uint64_t index = hash_function->hash(word, numBuckets);
    Word* curr_word = buckets[index];
    while (curr_word) {
        if (curr_word->word == word) {
            return curr_word;
        }
        curr_word = curr_word->next;
    }
    return nullptr;
}


void BasicHashTable::add(const std::string word, const Doc document) {
    std::uint64_t index = hash_function->hash(word, numBuckets);
    Word* word_in_document = get_word(word);

    if (!word_in_document) {
        word_in_document = new Word(word);
        word_in_document->next = buckets[index];
        buckets[index] = word_in_document;
    }

    if (!word_in_document->documents_in.empty()) {
        if (word_in_document->documents_in[word_in_document->documents_in.size() - 1].title == document.title) {
            return;
        }
    }
    word_in_document->documents_in.push_back(document);
}

void BasicHashTable::add_document(Doc document) {
    this->num_docs++;
}

int BasicHashTable::get_num_docs() {
  return this->num_docs;
}

std::vector<Doc> BasicHashTable::get(std::string word) {
    Word* result = get_word(word);
    return result ? result->documents_in : std::vector<Doc> {};
}
