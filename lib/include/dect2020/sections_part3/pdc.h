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

#ifndef DECT2020_PDC_H
#define DECT2020_PDC_H

#include <cstdint>

#include "dect2020/sections_part3/pcc.h"
#include "dect2020/sections_part3/physical_resources.h"
#include "dect2020/sections_part3/drs.h"

// ETSI TS 103 636-3 V1.1.1 (2020-07), 5.2.5

namespace dect2020 {

typedef struct{
    /*
     * vector k_i_linear has only one dimension as the PDC subcarriers are the same for each transmit stream
     */
    std::vector<uint32_t> k_i_linear;
} pdc_t;

// there of a few configurations with N_PACKET_symb = 5 which fail, therefore always check if these functions return DECT2020_SUCCESS

int get_pdc(pdc_t& pdc, const k_b_OCC_vec_t& k_b_OCC_vec, const pcc_t& pcc, const drs_t& drs, uint32_t u, uint32_t b, uint32_t N_PACKET_symb, uint32_t N_TS);

int get_N_PDC_subc(uint32_t& N_PDC_subc, uint32_t N_PACKET_symb, uint32_t u, uint32_t N_eff_TX, uint32_t N_b_OCC);

}

#endif
