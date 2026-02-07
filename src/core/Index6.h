using namespace std;
#include <string>
#include <fstream>
#include <iostream>

class WikiItem {
    public:
        WikiItem();
        ~WikiItem();
        WikiItem(string s, WikiItem* n);
        void addDoc(WikiItem* article);
        string str;
        WikiItem* next;
        WikiItem* articles;

    private:


};

class WikiItemMap {
    public:
        WikiItemMap(int n);
        ~WikiItemMap();
        WikiItem* get(string s);
        void add(WikiItem* new_item);
        int hash(string s);

    private:
        int numBuckets;
        WikiItem** buckets;
};


class Index5 {
public:
    Index5();
    void preprocess(string filename);
    WikiItem* search(string query);
private:
    WikiItemMap map;
};