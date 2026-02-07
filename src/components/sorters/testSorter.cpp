#include "MergeSort.cpp"
#include "RadixSort.cpp"
#include "../../core/interfaces.h"
#include <iostream>

int main() {
  std::vector<ScoredDoc> docs(6);
  docs[0].title = "abe";
  docs[0].score = 110;
  docs[1].title = "kat";
  docs[1].score = 200;
  docs[2].title = "lak";
  docs[2].score = 220;
  docs[3].title = "kak";
  docs[3].score = 511;
  docs[4].title = "nak";
  docs[4].score = 521;
  docs[5].title = "sak";
  docs[5].score = 323;

  RadixSort(docs);

  for (int i = 0; i < docs.size(); i++) {
    std::cout << docs[i].title << " " << docs[i].score << "\n";

  }
  return 0;
}