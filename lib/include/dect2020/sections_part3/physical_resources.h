/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Maxim Penner <maxim.penner@ikt.uni-hannover.de>
 */

#ifndef DECT2020_PHYSICAL_RESOURCES_H
#define DECT2020_PHYSICAL_RESOURCES_H

#include <cstdint>
#include <array>
#include <vector>

// ETSI TS 103 636-3 V1.1.1 (2020-07), 4.5

namespace dect2020 {

typedef struct{
    const std::array<uint32_t, 6> N_b_OCC= {56,112,224,448,672,896};
    int32_t b_1[56];
    int32_t b_2[112];
    int32_t b_4[224];
    int32_t b_8[448];
    int32_t b_12[672];
    int32_t b_16[896];
} k_b_OCC_t;

typedef struct{                                  //  1 2   4       8       12       16
    const std::array<uint32_t,17> b2b_idx       = {0,0,1,0,2,0,0,0,3,0,0,0, 4,0,0,0, 5};
    const std::array<uint32_t, 6> b_idx2b       = {1,2,4,8,12,16};
    const std::array<uint32_t, 6> N_b_DFT       = {64,128,256,512,768,1024};
    const std::array<uint32_t, 6> N_b_OCC       = {56,112,224,448,672,896};
    const std::array<uint32_t, 6> guards_top    = {3,7,15,31,47,63};
    const std::array<uint32_t, 6> guards_bottom = {4,8,16,32,48,64};

    /*
     * vector b_x will have 6 rows
     *
     * row 0:   56 elements  (b=1)
     * row 1:   112 elements (b=2)
     * row 2:   224 elements (b=4)
     * row 3:   448 elements (b=8)
     * row 4:   672 elements (b=12)
     * row 5:   896 elements (b=16)
     */
    std::vector<std::vector<int32_t>> b_x;
} k_b_OCC_vec_t;

void get_k_b_OCC(k_b_OCC_t& k_b_OCC);
void get_k_b_OCC(k_b_OCC_vec_t& k_b_OCC_vec);

}

#endif