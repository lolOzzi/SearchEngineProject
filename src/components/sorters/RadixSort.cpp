#include "../../core/interfaces.h";
#include <list>;
class RadixSort : public ISorter {
public:
    ~RadixSort() = default;
    void sort(std::vector<ScoredDoc> &A) override;
};

int getDigit(const int &d, const int &num) {
    int digit = num;
    switch(d) {
        case 0: break;
        case 1: digit /= 10; break;
        case 2: digit /= 100; break;
        case 3: digit /= 1000; break;
        case 4: digit /= 10000; break;
        case 5: digit /= 100000; break;
        case 6: digit /= 1000000; break;
        case 7: digit /= 10000000; break;
        case 8: digit /= 100000000; break;
        default: abort();
    }
    return digit % 10;
}

void RadixSort::sort(std::vector<ScoredDoc> &A) {
    int num_digits = 3;
    for (int d = 0; d < num_digits; ++d) {
        std::list<ScoredDoc> radix[10];
        for (int i = 0; i < A.size(); ++i) {
            int index = getDigit(d,A[i].score);
            radix[index].push_back(A[i]);
        }
        int c = 0;
        for (int i = 9; i >= 0; --i) {
            while(!radix[i].empty()) {
                A[c] = radix[i].front();
                radix[i].pop_front();
                c++;
            }
        }
    }
}

