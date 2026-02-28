#include "../../core/interfaces.h"

#include <string>
#include <vector>

class BlockTable {
  private:
      vector<vector<int>> abe;
    public:

}

class Word {
public:
    Word(std::string word) {
        this->word = word;
        documents_in = std::vector<Doc>{};
        next = nullptr;
    }
    std::string word;
    std::vector<Doc> documents_in;
    Word* next;
};

class DocBlock : public IStore {
private:
    int numBuckets;
    Word** buckets;
    IHash* hash_function;
    Word* get_word(std::string word);
    int num_docs;
public:
    ~DocBlock() override;
    DocBlock(int n, IHash* hash_function);
    void add(std::string word, Doc document) override;
    void add_document(Doc document) override;
    int get_num_docs() override;
    std::vector<Doc> get(std::string word) override;
};


DocBlock::DocBlock(int n, IHash* hash_function) {
    numBuckets = n;
    buckets = new Word*[n];
    for (int i = 0; i < numBuckets; ++i) buckets[i] = nullptr;
    this->hash_function = hash_function;
}

DocBlock::~DocBlock() {
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

Word *DocBlock::get_word(std::string word) {
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


void DocBlock::add(const std::string word, const Doc document) {
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

void DocBlock::add_document(Doc document) {
    this->num_docs++;
}

int DocBlock::get_num_docs() {
    return this->num_docs;
}

std::vector<Doc> DocBlock::get(std::string word) {
    Word* result = get_word(word);
    return result ? result->documents_in : std::vector<Doc> {};
}

