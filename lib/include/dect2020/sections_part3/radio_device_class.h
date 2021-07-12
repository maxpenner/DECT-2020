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

#ifndef DECT2020_RADIO_DEVICE_CLASS_H
#define DECT2020_RADIO_DEVICE_CLASS_H

#include <cstdint>
#include <string>

namespace dect2020 {

// ETSI TS 103 636-3 V1.1.1 (2020-07), ANNEX B
typedef struct{

    std::string radio_device_class_string;

    uint32_t u_min;
    uint32_t b_min;
    uint32_t N_ant_min;
    uint32_t mcs_index_min;
    uint32_t M_DL_HARQ_min;
    uint32_t M_connection_DL_HARQ_min;
    uint32_t N_soft_min;
    uint32_t Z_min;
} radio_device_class_t;

int get_radio_device_class(radio_device_class_t& q, std::string radio_device_class_string);

void print_radio_device_class(radio_device_class_t& q);

}

#endif
