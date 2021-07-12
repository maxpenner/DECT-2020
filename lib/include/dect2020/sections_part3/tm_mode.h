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

#ifndef DECT2020_TM_MODE_H
#define DECT2020_TM_MODE_H

#include <cstdint>

// ETSI TS 103 636-3 V1.1.1 (2020-07), 7.2

namespace dect2020 {

typedef struct{
    uint32_t tm_mode_index;
    uint32_t N_eff_TX;
    uint32_t N_SS;
    bool cl;
    bool bf;
    uint32_t N_TS;
    uint32_t N_TX;
} tm_mode_t;

void get_tm_mode(tm_mode_t& q, uint32_t index);

uint32_t get_max_tm_mode_index_depending_on_N_ant(uint32_t N_ant);

}

#endif
