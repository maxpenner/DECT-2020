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

#include "dect2020/sections_part3/mcs.h"

namespace dect2020 {

void get_mcs(mcs_t& q, uint32_t index){

    q.index = index;

    switch (index){
        case 0:
            q.N_bps = 1;
            q.R_numerator = 1;
            q.R_denominator = 2;
            break;

        case 1:
            q.N_bps = 2;
            q.R_numerator = 1;
            q.R_denominator = 2;
            break;

        case 2:
            q.N_bps = 2;
            q.R_numerator = 3;
            q.R_denominator = 4;
            break;

        case 3:
            q.N_bps = 4;
            q.R_numerator = 1;
            q.R_denominator = 2;
            break;

        case 4:
            q.N_bps = 4;
            q.R_numerator = 3;
            q.R_denominator = 4;
            break;

        case 5:
            q.N_bps = 6;
            q.R_numerator = 2;
            q.R_denominator = 3;
            break;

        case 6:
            q.N_bps = 6;
            q.R_numerator = 3;
            q.R_denominator = 4;
            break;

        case 7:
            q.N_bps = 6;
            q.R_numerator = 5;
            q.R_denominator = 6;
            break;

        case 8:
            q.N_bps = 8;
            q.R_numerator = 3;
            q.R_denominator = 4;
            break;

        case 9:
            q.N_bps = 8;
            q.R_numerator = 5;
            q.R_denominator = 6;
            break;

        case 10:
            q.N_bps = 10;
            q.R_numerator = 3;
            q.R_denominator = 4;
            break;

        case 11:
            q.N_bps = 10;
            q.R_numerator = 5;
            q.R_denominator = 6;
            break;
    }
}

}
