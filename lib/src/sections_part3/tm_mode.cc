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

#include <iostream>

#include "dect2020/sections_part3/tm_mode.h"

namespace dect2020 {

void get_tm_mode(tm_mode_t& q, uint32_t index){

    q.tm_mode_index = index;

    q.cl = false;
    q.bf = false;

    switch (index){
        case 0:
            q.N_eff_TX         = 1;
            q.N_SS             = 1;
            q.N_TS             = 1;
            q.N_TX             = 1;
            break;

        case 1:
            q.N_eff_TX         = 2;
            q.N_SS             = 1;
            q.N_TS             = 2;
            q.N_TX             = 2;
            break;

        case 2:
            q.N_eff_TX         = 2;
            q.N_SS             = 2;
            q.N_TS             = 2;
            q.N_TX             = 2;
            break;

        case 3:
            q.N_eff_TX         = 1;
            q.N_SS             = 1;
            q.N_TS             = 1;
            q.N_TX             = 2;
            break;

        case 4:
            q.N_eff_TX         = 2;
            q.N_SS             = 2;
            q.N_TS             = 2;
            q.N_TX             = 2;
            break;

        case 5:
            q.N_eff_TX         = 4;
            q.N_SS             = 1;
            q.N_TS             = 4;
            q.N_TX             = 4;
            break;

        case 6:
            q.N_eff_TX         = 4;
            q.N_SS             = 4;
            q.N_TS             = 4;
            q.N_TX             = 4;
            break;

        case 7:
            q.N_eff_TX         = 1;
            q.N_SS             = 1;
            q.N_TS             = 1;
            q.N_TX             = 4;
            break;

        case 8:
            q.N_eff_TX         = 2;
            q.N_SS             = 2;
            q.N_TS             = 2;
            q.N_TX             = 4;
            break;

        case 9:
            q.N_eff_TX         = 4;
            q.N_SS             = 4;
            q.N_TS             = 4;
            q.N_TX             = 4;
            break;

        case 10:
            q.N_eff_TX         = 8;
            q.N_SS             = 1;
            q.N_TS             = 8;
            q.N_TX             = 8;
            break;

        case 11:
            q.N_eff_TX         = 8;
            q.N_SS             = 8;
            q.N_TS             = 8;
            q.N_TX             = 8;
            break;
    }
}

uint32_t get_max_tm_mode_index_depending_on_N_ant(uint32_t N_ant){

    uint32_t max_tm_mode_index = 0;

    switch (N_ant){
        case 1:
            max_tm_mode_index = 0;
            break;

        case 2:
            max_tm_mode_index = 4;
            break;

        case 4:
            max_tm_mode_index = 9;
            break;

        case 8:
            max_tm_mode_index = 11;
            break;
    }

    return max_tm_mode_index;
}

}
