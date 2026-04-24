#include "extras/basic/DynamicPackedArray.h"
#include <vector>
#include <cstdint>
#include <string>
#include <iostream>

int TestDynamicPackedArray() {

    DynamicPackedArray dpa(2);

    // add a range of values (edge cases + typical)
    vector<uint32_t> vals = {0u, 1u, 127u, 128u, 30000u, 0xFFFFFFu, 4294967295u};
    for (int i = 0; i < vals.size(); ++i) {
        dpa.add(vals[i]);
    }



    // extract into a vector and print
    std::vector<std::uint32_t> out;
    dpa.copy_elements_to_vector(out);

    std::cout << "Extracted " << out.size() << " elements:\n";
    for (std::size_t i = 0; i < out.size(); ++i) {
        std::cout << i << ": " << out[i] << " correct: " << vals[i] << " succes? "<<(out[i] == vals[i]) << '\n';
    }

    return 0;
}