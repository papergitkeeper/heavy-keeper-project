#ifndef _SC_H
#define _SC_H

//#include "params.h"
#include "BOBHash32.h"
#include "SPA.h"
#include <cstring>
#include <algorithm>
#include <immintrin.h>
#include <stdexcept>
#ifdef UNIX
#include <x86intrin.h>
#else
#include <intrin.h>
#endif 
using namespace std;

#define MAX_HASH_NUM 4

template<int memory_in_bytes, int bucket_num = 1000, int counter_num = 16, int threshold = 240, int l1_ratio = 65>
class StreamClassifier
{
//    static constexpr int bucket_num = 1000;
//    static constexpr int counter_num = 16;

    static constexpr int buffer_size = bucket_num * counter_num * 8;
    static constexpr int remained = memory_in_bytes - buffer_size;

    static constexpr int d1 = 3;
    static constexpr int m1_in_bytes = int(remained * l1_ratio / 100.0);
    static constexpr int d2 = 3;
    static constexpr int m2_in_bytes = int(remained * (100 - l1_ratio) / 100.0);

    uint32_t ID[bucket_num][counter_num] __attribute__ ((aligned (16)));
    int counter[bucket_num][counter_num];
    int cur_pos[bucket_num];

    static constexpr int w1 = m1_in_bytes * 8 / 4;
    static constexpr int w_word = m1_in_bytes * 8 / 4 / 16;
    static constexpr int w2 = m2_in_bytes * 8 / 16;

    uint64_t L1[m1_in_bytes * 8 / 4 / 16]; // Layer 1 is organized as word, one word contains 16 counter, one counter consist of 4 bit
    uint16_t L2[m2_in_bytes * 8 / 16]; // Layer 2 is organized as counter, one counter consist of 16 bit

    SPA * spa;

    BOBHash32 * bobhash1;
    BOBHash32 * bobhash2[d2];

    int cur_kick;

    void clear_data()
    {
        cur_kick = 0;
        memset(ID, 0, sizeof(ID));
        memset(counter, 0, sizeof(counter));
        memset(cur_pos, 0, sizeof(cur_pos));
        memset(L1, 0, sizeof(L1));
        memset(L2, 0, sizeof(L2));
    }
public:
    StreamClassifier()
    {
        bobhash1 = new BOBHash32(500);
        for (int i = 0; i < d2; i++) {
            bobhash2[i] = new BOBHash32(1000 + i);
        }
        clear_data();
        spa = NULL;
    }

    void print_basic_info()
    {
        printf("Stream Classifer\n");
        printf("\tSIMD buffer: %d counters, %.4lf MB occupies\n", bucket_num * counter_num, bucket_num * counter_num * 8.0 / 1024 / 1024);
        printf("\tL1: %d counters, %.4lf MB occupies\n", w1, w1 * 0.5 / 1024 / 1024);
        printf("\tL2: %d counters, %.4lf MB occupies\n", w2, w2 * 2.0 / 1024 / 1024);
    }

    ~StreamClassifier()
    {
        delete bobhash1;
        for (int i = 0; i < d2; i++)
            delete bobhash2[i];
    }

    //periodical refreshing for continuous top-k;
    void init_array_period()
    {
        for (int i = 0; i < w_word; i++) {
            uint64_t temp = L1[i];

            temp = (temp & (0xF)) == 0xF ? temp : (temp & 0xFFFFFFFFFFFFFFF0);
            temp = (temp & (0xF0)) == 0xF0 ? temp : (temp & 0xFFFFFFFFFFFFFF0F);
            temp = (temp & (0xF00)) == 0xF00 ? temp : (temp & 0xFFFFFFFFFFFFF0FF);
            temp = (temp & (0xF000)) == 0xF000 ? temp : (temp & 0xFFFFFFFFFFFF0FFF);

            temp = (temp & (0xF0000)) == 0xF0000 ? temp : (temp & 0xFFFFFFFFFFF0FFFF);
            temp = (temp & (0xF00000)) == 0xF00000 ? temp : (temp & 0xFFFFFFFFFF0FFFFF);
            temp = (temp & (0xF000000)) == 0xF000000 ? temp : (temp & 0xFFFFFFFFF0FFFFFF);
            temp = (temp & (0xF0000000)) == 0xF0000000 ? temp : (temp & 0xFFFFFFFF0FFFFFFF);

            temp = (temp & (0xF00000000)) == 0xF00000000 ? temp : (temp & 0xFFFFFFF0FFFFFFFF);
            temp = (temp & (0xF000000000)) == 0xF000000000 ? temp : (temp & 0xFFFFFF0FFFFFFFFF);
            temp = (temp & (0xF0000000000)) == 0xF0000000000 ? temp : (temp & 0xFFFFF0FFFFFFFFFF);
            temp = (temp & (0xF00000000000)) == 0xF00000000000 ? temp : (temp & 0xFFFF0FFFFFFFFFFF);

            temp = (temp & (0xF000000000000)) == 0xF000000000000 ? temp : (temp & 0xFFF0FFFFFFFFFFFF);
            temp = (temp & (0xF0000000000000)) == 0xF0000000000000 ? temp : (temp & 0xFF0FFFFFFFFFFFFF);
            temp = (temp & (0xF00000000000000)) == 0xF00000000000000 ? temp : (temp & 0xF0FFFFFFFFFFFFFF);
            temp = (temp & (0xF000000000000000)) == 0xF000000000000000 ? temp : (temp & 0x0FFFFFFFFFFFFFFF);

            L1[i] = temp;
        }

        for (int i = 0; i < w2; i++) {
            short int temp = L2[i];
            L2[i] = (temp == threshold) ? temp : 0;
        }
    }

    void init_array_all()
    {
        memset(L1, 0, sizeof(uint64_t) * w_word);
        memset(L2, 0, sizeof(short int) * w2);
    }

    void init_spa(SPA * _spa)
    {
        spa = _spa;
    }

    void insert(uint32_t key)
    {
        int bucket_id = key % bucket_num;
        // int bucket_id = key & 0x2FF;

        // the code below assume counter per buckets is 16

        const __m128i item = _mm_set1_epi32((int)key);
        int matched;

        if (counter_num == 16) {
            __m128i *keys_p = (__m128i *)ID[bucket_id];

            __m128i a_comp = _mm_cmpeq_epi32(item, keys_p[0]);
            __m128i b_comp = _mm_cmpeq_epi32(item, keys_p[1]);
            __m128i c_comp = _mm_cmpeq_epi32(item, keys_p[2]);
            __m128i d_comp = _mm_cmpeq_epi32(item, keys_p[3]);

            a_comp = _mm_packs_epi32(a_comp, b_comp);
            c_comp = _mm_packs_epi32(c_comp, d_comp);
            a_comp = _mm_packs_epi32(a_comp, c_comp);

            matched = _mm_movemask_epi8(a_comp);
        } else if (counter_num == 4) {
            __m128i *keys_p = (__m128i *)ID[bucket_id];
            __m128i a_comp = _mm_cmpeq_epi32(item, keys_p[0]);
            matched = _mm_movemask_ps(*(__m128 *)&a_comp);
        } else {
            throw std::logic_error("Not implemented.");
        }

        if (matched != 0) {
            //return 32 if input is zero;
            int matched_index = _tzcnt_u32((uint32_t)matched);

            ++counter[bucket_id][matched_index];
            return;
        }

        int cur_pos_now = cur_pos[bucket_id];
        if (cur_pos_now != counter_num) {
            // printf("%d\n", cur_pos_now);
            ID[bucket_id][cur_pos_now] = key;
            ++counter[bucket_id][cur_pos_now] ;
            ++cur_pos[bucket_id];
            return;
        }

        /****************randomly choose one counter to kick!******************/
        insert_SC(ID[bucket_id][cur_kick], counter[bucket_id][cur_kick]);
        ID[bucket_id][cur_kick] = key;
        counter[bucket_id][cur_kick] = 1;

        cur_kick = (cur_kick + 1) % counter_num;
    }

    void insert_SC(uint32_t kick_ID, int kick_f)
    {
        int v1 = 1 << 30;

        int value[MAX_HASH_NUM];
        int index[MAX_HASH_NUM];
        int offset[MAX_HASH_NUM];

        uint64_t hash_value = (bobhash1->run((const char *)&kick_ID, 4));
        int word_index = hash_value % w_word;
        hash_value >>= 16;

        uint64_t temp = L1[word_index];
        for (int i = 0; i < d1; i++) {
            offset[i] = (hash_value & 0xF);
            value[i] = (temp >> (offset[i] << 2)) & 0xF;
            v1 = std::min(v1, value[i]);

            hash_value >>= 4;
        }

        int temp2 = v1 + kick_f;
        if (temp2 <= 15) { // maybe optimized use SIMD?
            for (int i = 0; i < d1; i++) {
                int temp3 = ((temp >> (offset[i] << 2)) & 0xF);
                if (temp3 < temp2) {
                    temp += ((uint64)(temp2 - temp3) << (offset[i] << 2));
                }
            }
            L1[word_index] = temp;
            return;
        }

        for (int i = 0; i < d1; i++) {
            temp |= ((uint64_t)0xF << (offset[i] << 2));
        }
        L1[word_index] = temp;

        int delta1 = 15 - v1;
        kick_f -= delta1;

        int v2 = 1 << 30;
        for (int i = 0; i < d2; i++) {
            index[i] = (bobhash2[i]->run((const char *)&kick_ID, 4)) % w2;
            value[i] = L2[index[i]];
            v2 = std::min(value[i], v2);
        }

        temp2 = v2 + kick_f;
        if (temp2 <= threshold) {
            for (int i = 0; i < d2; i++) {
                L2[index[i]] = (L2[index[i]] > temp2) ? L2[index[i]] : temp2;
            }
            return;
        }

        for (int i = 0; i < d2; i++) {
            L2[index[i]] = threshold;
        }

        int delta2 = threshold - v2;
        kick_f -= delta2;

        spa->insert(kick_ID, kick_f);
    }

    void refresh()
    {
        for (int i = 0; i < bucket_num; i++) {
            for (int j = 0; j < counter_num; j++) {
                insert_SC(ID[i][j], counter[i][j]);
                ID[i][j] = counter[i][j] = 0;
            }
            cur_pos[i] = 0;
        }
        return;
    }

    int query(uint32_t key)
    {
        int v1 = 1 << 30;

//        constexpr int max_d = d1 > d2 ? d1 : d2;
//        int value[max_d];
//        int index[max_d];
//        int offset[max_d];

        uint32_t hash_value = (bobhash1->run((const char *)&key, 4));
        int word_index = hash_value % w_word;
        hash_value >>= 16;

        uint64_t temp = L1[word_index];
        for (int i = 0; i < d1; i++) {
            int of, val;
            of = (hash_value & 0xF);
            val = (temp >> (of << 2)) & 0xF;
            v1 = std::min(val, v1);
            hash_value >>= 4;
        }

        if (v1 != 15)
            return v1;

        int v2 = 1 << 30;
        for (int i = 0; i < d2; i++) {
            int index = (bobhash2[i]->run((const char *)&key, 4)) % w2;
            int value = L2[index];
            v2 = std::min(value, v2);
        }

        return v1 + v2;
    }
};

#endif//_SC_H
