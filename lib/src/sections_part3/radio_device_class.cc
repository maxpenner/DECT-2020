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

#include "dect2020/sections_part3/radio_device_class.h"
#include "dect2020/utils/debug.h"

namespace dect2020 {

int get_radio_device_class(radio_device_class_t& q, std::string radio_device_class_string){

    if (radio_device_class_string.compare("1.1.1.A") == 0){
        q.u_min = 1;
        q.b_min = 1;
        q.N_ant_min = 1;
        q.mcs_index_min = 4;
        q.M_DL_HARQ_min = 8;
        q.M_connection_DL_HARQ_min = 2;
        q.N_soft_min = 25344;
        q.Z_min = 2048;
    }
    // max settings
    else if (radio_device_class_string.compare("8.16.8.B") == 0){
        q.u_min = 8;
        q.b_min = 16;
        q.N_ant_min = 8;
        q.mcs_index_min = 11;

        // there is no limit for these values
        q.M_DL_HARQ_min = 8;
        q.M_connection_DL_HARQ_min = 2;
        q.N_soft_min = 25344;

        q.Z_min = 6144;
    }
    // reasonable settings
    else if (radio_device_class_string.compare("4.2.2.C") == 0){
        q.u_min = 4;
        q.b_min = 2;
        q.N_ant_min = 2;
        q.mcs_index_min = 6;

        // there is no limit for these values
        q.M_DL_HARQ_min = 8;
        q.M_connection_DL_HARQ_min = 2;
        q.N_soft_min = 25344;

        q.Z_min = 6144;
    }
    else{
        ERROR("Unknown Radio Device Class:");
        std::cout << radio_device_class_string << std::endl;
        return DECT2020_ERROR;
    }

    return DECT2020_SUCCESS;
}

void print_radio_device_class(radio_device_class_t& q){
    std::cout << "Radio Device Configuration: " << q.radio_device_class_string << std::endl;
    std::cout << "u_min:                      " << q.u_min << std::endl;
    std::cout << "b_min:                      " << q.b_min << std::endl;
    std::cout << "N_ant_min:                  " << q.N_ant_min  << std::endl;
    std::cout << "mcs_index_min:              " << q.mcs_index_min << std::endl;
    std::cout << "M_DL_HARQ_min:              " << q.M_DL_HARQ_min << std::endl;
    std::cout << "M_connection_DL_HARQ_min:   " << q.M_connection_DL_HARQ_min << std::endl;
    std::cout << "N_soft_min:                 " << q.N_soft_min << std::endl;
    std::cout << "Z_min:                      " << q.Z_min << std::endl;
    std::cout << std::endl;
}

}
