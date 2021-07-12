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

#include "dect2020/sections_part3/transport_block_size.h"
#include "dect2020/utils/debug.h"
#include "dect2020/utils/math_functions.h"

// ETSI TS 103 636-3 V1.1.1 (2020-07), 5.3

namespace dect2020 {

uint32_t get_N_PDC_bits_raw(uint32_t N_SS,
                            uint32_t N_PDC_subc,
                            uint32_t N_bps)
{
    return N_SS * N_PDC_subc * N_bps;
}

uint32_t get_N_PDC_bits(uint32_t N_SS,
                        uint32_t N_PDC_subc,
                        uint32_t N_bps,
                        uint32_t R_numerator,
                        uint32_t R_denominator)
{
    return (get_N_PDC_bits_raw(N_SS, N_PDC_subc, N_bps) * R_numerator) / R_denominator;
}

int get_N_TB_bits(uint32_t& N_TB_bits,
                    uint32_t N_SS,
                    uint32_t N_PDC_subc,
                    uint32_t N_bps,
                    uint32_t R_numerator,
                    uint32_t R_denominator,
                    uint32_t Z)
{
    uint32_t N_PDC_bits = get_N_PDC_bits(N_SS, N_PDC_subc, N_bps, R_numerator, R_denominator);

    uint32_t L = 24;

    uint32_t M;
    if (N_PDC_bits <= 512)
        M = 8;
    else if (N_PDC_bits <= 1024)
        M = 16;
    else if (N_PDC_bits <= 2048)
        M = 32;
    else
        M = 64;

    uint32_t N_M = (N_PDC_bits/M)*M;

    // sanity checks
    if(N_M == 0){
        ERROR("Frame too small. N_M is %d", N_M);
        return DECT2020_ERROR;
    }
    if(N_M <= L){
        ERROR("Frame too small. N_M is %d and therefore <= L=24", N_M);
        return DECT2020_ERROR;
    }

    if (N_M <= Z){
        N_TB_bits = N_M - L;
    }
    else{
        uint32_t C = ceil_divide_integer<uint32_t>(N_M - L, Z);
        N_TB_bits = N_M - (C+1)*L;
    }

    // sanity check
    if(N_TB_bits == 0){
        ERROR("N_TB_bits is 0");
        return DECT2020_ERROR;
    }

    return DECT2020_SUCCESS;
}

}
