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

#ifndef DECT2020_STF_H
#define DECT2020_STF_H

#include <cstdint>
#include <complex>
#include <array>
#include <vector>

// ETSI TS 103 636-3 V1.2.1 (2021-04), 5.2.2

namespace dect2020 {

typedef struct{
    const std::array<uint32_t, 6> N_b_OCC = {56,112,224,448,672,896};

    int32_t k_i_b_1[14];    // 56/4
    int32_t k_i_b_2[28];    // 112/4
    int32_t k_i_b_4[56];    // 224/4
    int32_t k_i_b_8[112];   // 448/4
    int32_t k_i_b_12[168];  // 672/4
    int32_t k_i_b_16[224];  // 896/4

    // we have four possible values for N_eff_TX
    std::complex<float> y_STF_i_b_1[4][14];
    std::complex<float> y_STF_i_b_2[4][28];
    std::complex<float> y_STF_i_b_4[4][56];
    std::complex<float> y_STF_i_b_8[4][112];
    std::complex<float> y_STF_i_b_12[4][168];
    std::complex<float> y_STF_i_b_16[4][224];
} stf_t;

typedef struct{

    /*
     * vector k_i_b_x will have 6 rows, total size is [6][14|28|56|112|168|224]
     *
     * row 0:   14  elements (b=1)
     * row 1:   28  elements (b=2)
     * row 2:   56  elements (b=4)
     * row 3:   112 elements (b=8)
     * row 4:   168 elements (b=12)
     * row 5:   224 elements (b=16)
     */
    std::vector<std::vector<int32_t>> k_i_b_x;

    /*
     * vector y_STF_i_b_x will have 6 rows, each containing one matrix, total size is [6][4][14|28|56|112|168|224]
     *
     * row 0:   [4][14]  elements (1,2,4,8 effective TX antennas, b=1)
     * row 1:   [4][28]  elements (1,2,4,8 effective TX antennas, b=2)
     * row 2:   [4][56]  elements (1,2,4,8 effective TX antennas, b=4)
     * row 3:   [4][112] elements (1,2,4,8 effective TX antennas, b=8)
     * row 4:   [4][168] elements (1,2,4,8 effective TX antennas, b=12)
     * row 5:   [4][224] elements (1,2,4,8 effective TX antennas, b=16)
     */
    std::vector< std::vector< std::vector < std::complex<float> > > > y_STF_i_b_x;
} stf_vec_t;

void get_stf(stf_t& stf);
void get_stf(stf_vec_t& stf_vec);

}

#endif