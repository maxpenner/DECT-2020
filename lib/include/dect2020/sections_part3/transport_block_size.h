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

#ifndef DECT2020_TRANSPORT_BLOCK_SIZE_H
#define DECT2020_TRANSPORT_BLOCK_SIZE_H

#include <cstdint>

// ETSI TS 103 636-3 V1.1.1 (2020-07), 5.3

namespace dect2020 {

uint32_t get_N_PDC_bits_raw(uint32_t N_SS,
                            uint32_t N_PDC_subc,
                            uint32_t N_bps);

uint32_t get_N_PDC_bits(uint32_t N_SS,
                            uint32_t N_PDC_subc,
                            uint32_t N_bps,
                            uint32_t R_numerator,
                            uint32_t R_denominator);

// there of a few configurations with N_PACKET_symb = 5 which fail, therefore always check if this function return DECT2020_SUCCESS
int get_N_TB_bits(uint32_t& N_TB_bits,
                    uint32_t N_SS,
                    uint32_t N_PDC_subc,
                    uint32_t N_bps,
                    uint32_t R_numerator,
                    uint32_t R_denominator,
                    uint32_t Z);

}

#endif
