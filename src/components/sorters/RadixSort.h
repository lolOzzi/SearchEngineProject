#pragma once

#include "../../core/interfaces.h"
#include <list>
class RadixSort : public ISorter {
public:
    ~RadixSort() = default;
    void sort(std::vector<ScoredDoc> &A) override;
};