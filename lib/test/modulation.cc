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
#include <cstdint>
#include <cstring>
#include <sys/time.h>
#include <iomanip>
#include <fstream>
#include <cmath>

#include "dect2020/config.h"
#include "dect2020/sections_part3/beamforming_and_antenna_port_mapping.h"
#include "dect2020/sections_part3/transmit_diversity_precoding.h"
#include "dect2020/utils/debug.h"

int main(int argc, char** argv)
{
    std::cout << "Loading Modulation Matrices" << std::endl;

    dect2020::y_i_t y_i;
    dect2020::get_y_i(y_i, 0.52f);

    dect2020::w_t w;
    dect2020::get_w(w, 3.32f);

    std::cout << "Done!" << std::endl;
    return 0;
}
