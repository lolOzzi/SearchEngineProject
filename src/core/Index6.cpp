#include "Interfaces.h"
const static string END = "---END.OF.DOCUMENT---";

WikiItem::WikiItem() {
    str = "";
    articles = nullptr;
    next = nullptr;
}
WikiItem::~WikiItem() {
}

WikiItem::WikiItem(string s, WikiItem* n) {
    str = s;
    articles = nullptr;
    next = n;
}
void WikiItem::addDoc(WikiItem* article) {
    WikiItem* curr = articles;
    while (curr) {
        if (curr->str == article->str) return;
        curr = curr->next;
    }
    WikiItem* new_article = new WikiItem(article->str, nullptr);
    new_article->next = articles;
    articles = new_article;
}

WikiItemMap::WikiItemMap(int n) {
    numBuckets = n;
    buckets = new WikiItem*[n];
}

WikiItem* WikiItemMap::get(string s) {
    WikiItem* item = buckets[this->hash(s)];

    while (item) {
        if (!item->str.compare(s)) return item;
        item = item->next;
    }
    return item;
}

void WikiItemMap::add(WikiItem* new_item) {
    int index = this->hash(new_item->str);
    /*if (buckets[index]->str == "") {
        buckets[index]->str = new_item->str;
        buckets[index]->articles = new_item->articles;
        return;
    }*/
    new_item->next = buckets[index];
    buckets[index] = new_item;
}

int WikiItemMap::hash(string s) {
    size_t raw_hash = std::hash<std::string>{}(s);
    return (raw_hash < 0 ? -raw_hash : raw_hash) % numBuckets;
}

WikiItemMap::~WikiItemMap() {

    for (int i = 0; i < numBuckets; i++) {
        WikiItem* curr = buckets[i];
        while (curr) {
            WikiItem* next = curr->next;
            delete curr;
            curr = next;
        }
    }

    delete[] buckets;
}

Index5::Index5(): map(300'000) {};

void Index5::preprocess(string filename) {
    ifstream file;
    file.open (filename);
    if (!file.good()) {
        printf("Error reading file");
        return;
    }
    if (!file.is_open()) {
        printf("No file is open");
        return;
    };

    WikiItem article{};

    string word;
    WikiItem* tmp_item;
    WikiItem* current = new WikiItem("START", nullptr);
    short take_next = 1;
    printf("ayoo");

    while (file >> word)
    {
        if (word == "associated") {
            int i = 1;
        }
        // cout<< word << '\n';
        if (take_next) {
            article.str = word;
        }

        tmp_item = map.get(word);
        if (!tmp_item) {
            tmp_item = new WikiItem(word, nullptr);
            map.add(tmp_item);
        }
        tmp_item->addDoc(&article);
        take_next = word.compare(END) == 0;
    }
    printf("finished while loop");
    file.close();
    printf("close");
}

WikiItem* Index5::search(string query) {
    WikiItem* item = map.get(query);
    if (item) return item->articles;
    return nullptr;
}

