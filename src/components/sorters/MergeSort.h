#pragma once

#include "../../core/interfaces.h"

class MergeSort : public ISorter {
public:
    ~MergeSort() = default;
    void sort(std::vector<ScoredDoc> &A) override;
};
