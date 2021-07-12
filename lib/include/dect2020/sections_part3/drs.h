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

#ifndef DECT2020_DRS_H
#define DECT2020_DRS_H

#include <cstdint>
#include <complex>
#include <vector>

#include "dect2020/sections_part3/physical_resources.h"

// ETSI TS 103 636-3 V1.2.1 (2021-04), 5.2.3

namespace dect2020 {

typedef struct{
    uint32_t b = 0;
    uint32_t N_PACKET_symb = 0;
    uint32_t N_TS = 0;
    uint32_t N_eff_TX = 0;

    std::vector<int32_t> t;
    std::vector<std::vector<int32_t>> k_i;
    std::vector<std::vector<uint32_t>> k_i_linear;
    std::vector<std::vector<uint32_t>> l;
    std::vector<std::vector < std::complex<float> > > y_DRS_i;
} drs_t;

int get_drs(drs_t& drs, const k_b_OCC_vec_t& k_b_OCC_vec, uint32_t b, uint32_t N_PACKET_symb, uint32_t N_TS, uint32_t N_eff_TX);

}

#endif