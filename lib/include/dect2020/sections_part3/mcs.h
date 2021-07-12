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

#ifndef DECT2020_MCS_H
#define DECT2020_MCS_H

#include <cstdint>
#include <cstdio>

// ETSI TS 103 636-3 V1.1.1 (2020-07), 4.3

namespace dect2020 {

typedef struct{
    uint32_t index;
    uint32_t N_bps;
    uint32_t R_numerator;
    uint32_t R_denominator;
} mcs_t;

void get_mcs(mcs_t& q, uint32_t index);

}

#endif
