#include "RoutingRecord.h"
#include <vector>

class Filter {
public:
    template <typename structure, typename pred>
    static std::vector<RoutingRecord> filter(structure begin, structure end, pred p) {
        std::vector<RoutingRecord> result;
        for (auto it = begin; it != end; ++it) {
            if (p(*it)) {
                result.push_back(*it);
            }
        }
        return result;
    }
};