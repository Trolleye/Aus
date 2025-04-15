#include "RoutingRecord.h"
#include <vector>

class Filter {
public:
    template <typename structure, typename pred, typename oper>
    static void filter(structure begin, structure end, pred p, oper op) {
        for (auto it = begin; it != end; ++it) {
            if (p(*it)) {
                op(*it);
            }
        }
    }
};