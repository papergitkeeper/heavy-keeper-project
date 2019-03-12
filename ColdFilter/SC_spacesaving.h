#ifndef STREAMCLASSIFIER_SC_SPACESAVING_H
#define STREAMCLASSIFIER_SC_SPACESAVING_H

#include "SC.h"
#include <algorithm>

template<uint32_t ss_capacity, uint64_t sc_memory_in_bytes, uint32_t threshold, uint32_t bucket_num = 96>
class SC_SpaceSaving
{
    StreamClassifier<sc_memory_in_bytes, bucket_num, 16, ((threshold - 15) > 65535 ? 65535 : (threshold - 15)), 35> sc;
    SpaceSaving<ss_capacity> ss;
public:
    SC_SpaceSaving() {
        sc.init_spa(&ss);
//        sc.print_basic_info();
    }

    inline void build(uint32_t * items, int n)
    {
        for (int i = 0; i < n; ++i) {
            sc.insert(items[i]);
        }

        sc.refresh();
    }

    void get_top_k(uint32_t k, uint32_t items[])
    {
        ss.get_top_k(k, items);
    }

    void get_top_k_with_frequency(uint32_t k, vector<pair<uint32_t, uint32_t>> & result)
    {
        ss.get_top_k_with_frequency(k, result);
//        for (auto itr: result) {
        for (int i = 0; i < k; ++i) {
            result[i].second += std::min(65535u + 15u, threshold);
        }
    }
};

#endif //STREAMCLASSIFIER_SC_SPACESAVING_H
