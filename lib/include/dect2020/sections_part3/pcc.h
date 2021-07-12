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

#ifndef DECT2020_PCC_H
#define DECT2020_PCC_H

#include <cstdint>
#include <complex>
#include <vector>

// ETSI TS 103 636-3 V1.2.1 (2021-04), 5.2.4

namespace dect2020 {

typedef struct{
    /*
     * vector k_i_linear will have 6 rows, each containing one matrix, total size is [6][4][98]
     *
     * Linear indices start at bottom left corner in frame according to Figure 4.5-2
     *  linear index 0 at position [-N_DFT/2    ][0]
     *  linear index 1 at position [-N_DFT/2 +1 ][0]
     *
     * row 0:   [4][98] (b=1,  N_DFT=64)
     * row 1:   [4][98] (b=2,  N_DFT=128)
     * row 2:   [4][98] (b=4,  N_DFT=256)
     * row 3:   [4][98] (b=8,  N_DFT=512)
     * row 4:   [4][98] (b=12, N_DFT=768)
     * row 5:   [4][98] (b=16, N_DFT=1024)
     */
    std::vector< std::vector< std::vector < uint32_t > > > k_i_linear;
} pcc_t;

void get_pcc(pcc_t& pcc);
}

#endif