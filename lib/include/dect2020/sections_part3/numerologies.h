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

#ifndef DECT2020_NUMEROLOGIES_H
#define DECT2020_NUMEROLOGIES_H

#include <cstdint>
#include <cstdio>

// ETSI TS 103 636-3 V1.1.1 (2020-07), 4.3

namespace dect2020 {

typedef struct{
    uint32_t u;
    uint32_t b;

    uint32_t delta_u_f;
    double T_u_symb;
    uint32_t N_SLOT_u_symb;
    uint32_t N_SLOT_u_subslot;
    double GI_u;

    uint32_t B_u_b_DFT;
    double T_u_b_s;
    uint32_t N_b_DFT;
    uint32_t N_b_CP;
    uint32_t N_b_OCC;
    uint32_t B_u_b_TX;

    // also calculate the guards
    uint32_t n_guards_top;
    uint32_t n_guards_bottom;

} numerologies_t;

void get_numerologies(numerologies_t& q, uint32_t u, uint32_t b);
void print_numerologies(numerologies_t& q);

}

#endif
