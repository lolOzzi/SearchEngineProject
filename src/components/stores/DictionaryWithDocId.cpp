#include <memory>
#include "../../core/interfaces.h"
#include "../../extras/basic/ChaniedHashDictionary.h"
#include "../../extras/basic/RadixTree.h"
#include "../../extras/basic/GenericFKSDictionary.h"
#include "../../extras/basic/DynamicArray.h"
#include "../../extras/basic/CuckooHasingDictionary.h"
#include "../../extras/basic/bursttrie/Record.h"

class DictionaryWithDocId : public IStore {
private:
    class DictWord {
    public:
        Record word;
        DynamicArray<int> documents_in;
        DictWord() : word(""), documents_in(2) {}
        explicit DictWord(const std::string &w) : word(w), documents_in(2) {}

        bool operator==(const DictWord& other) const {
            return word == other.word;
        }
    };

    class DictDoc {
    public:
        Record word;
        long long start_loc;

        DictDoc() : word(""), start_loc(-1) { }
        explicit DictDoc(const std::string &w, const long long loc) : word(w), start_loc(loc) { }

        bool operator==(const DictDoc& other) const {
            return word == other.word;
        }
    };

    StringHashFamily hash_family = StringHashFamily();
    CuckooHashingDictionary<std::string, DictWord> dict;
    DynamicArray<DictDoc> doc_arr;
    std::string last_document_title_added;
    int last_docid_added;
public:
    ~DictionaryWithDocId() override = default;
    DictionaryWithDocId(int n);
    void add(std::string word, Doc document) override;
    void add_document(Doc document) override;
    int get_num_docs() override;
    std::vector<Doc> get(std::string word) override;
    int words_added = 0;
    int documents_added = 0;
    void print_spaced_used();
};

DictionaryWithDocId::DictionaryWithDocId(int n) :
    dict(n, &hash_family),
    doc_arr(128)
{
}
int DictionaryWithDocId::get_num_docs() {
    return words_added;
}

void DictionaryWithDocId::add_document(Doc document) {
    DictDoc dict_doc = DictDoc(document.title, document.start_loc);
    doc_arr.add(dict_doc);
    last_docid_added = doc_arr.n - 1;
    documents_added++;
}

void DictionaryWithDocId::add(std::string word, Doc document) {
    if (word == "") return;
    DictWord* word_in_document = dict.get(word);

    if (!word_in_document || word_in_document->word == nullptr) {
        words_added++;
        DictWord new_word = DictWord(word);
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

std::vector<Doc> DictionaryWithDocId::get(std::string word) {
    DictWord* res = dict.get(word);
    if (res == nullptr) return std::vector<Doc>{};

    std::vector<Doc> temp = std::vector<Doc>{};

    for (int i = 0; i < res->documents_in.n; i++) {
        Doc new_doc;
        new_doc.title = doc_arr[res->documents_in[i]].word.get_string();
        new_doc.start_loc = doc_arr[res->documents_in[i]].start_loc;
        temp.push_back(new_doc);
    }

    return temp;
}

void DictionaryWithDocId::print_spaced_used() {
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

    printf("Total space used is %llu\n", doc_holder_space_used + dict_space_used +
                                               (doc_arr_size_count * sizeof(int)));
}
