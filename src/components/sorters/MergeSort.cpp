#include "../../core/interfaces.h"

void merge (std::vector<ScoredDoc>& A, int l, int m, int r, std::vector<ScoredDoc>& tmp) {
  int i = l;
  int j = m + 1;
  int k = l;

  while (i <= m && j <= r) {
    if (A[i].score >= A[j].score) {
      tmp[k] = A[i];
      i++;
    }
    else {
      tmp[k] = A[j];
      j++;
    }
    k++;
  }
  while (j <= r) {
    tmp[k] = A[j];
    k++;
    j++;
  };

  while (i <= m) {
    tmp[k] = A[i];
    k++;
    i++;
  }

  for (int it = l; it <= r; ++it) {
    A[it] = tmp[it];
  }
}

void sort(std::vector<ScoredDoc>& A, int l, int r, std::vector<ScoredDoc>& tmp) {
  if (l >= r) {
    return;
  }
  int mid = (l + r) / 2; // hopefully it rounds down
  sort(A, l, mid, tmp);
  sort(A, mid+1, r, tmp);
  return merge(A, l, mid, r, tmp);
}


void MergeSort(std::vector<ScoredDoc> &A) {
  int n = A.size();
  std::vector<ScoredDoc> tmp(n);
  sort(A, 0, n-1, tmp);
}