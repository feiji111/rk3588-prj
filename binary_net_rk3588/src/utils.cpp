#include <algorithm>
#include <iterator>
#include <vector>
#include "utils.h"

int argmax(std::vector<double> result) {
    if(result.empty()) {
        return -1;
    } else {
        auto max_it = std::max_element(result.begin(), result.end());
        return std::distance(result.begin(), max_it); 
    }
}