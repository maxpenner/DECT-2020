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

#ifndef DECT2020_PACKET_SIZES_H
#define DECT2020_PACKET_SIZES_H

#include <cstdint>
#include <string>

#include "dect2020/sections_part3/mcs.h"
#include "dect2020/sections_part3/numerologies.h"
#include "dect2020/sections_part3/tm_mode.h"

namespace dect2020 {

typedef struct{
    // freely selected during operation (max values defined by radio device class)
    uint32_t u;
    uint32_t b;

    // freely selected during operation (max value for PacketLength is 16 (4 bit, value + 1))
    uint32_t PacketLengthType;
    uint32_t PacketLength;

    // freely selected during operation (max values defined by number of antennas of radio device class)
    // N_eff_TX
    // N_SS
    uint32_t tm_mode_index;

    // freely selected during operation (max value defined by radio device class)
    uint32_t mcs_index;

    // freely selected during operation (max values defined by radio device class)
    uint32_t Z;
} packet_sizes_required_input_t;

typedef struct{

    packet_sizes_required_input_t packet_sizes_required_input;

    // the following values will be derived from packet_sizes_required_input
    
    numerologies_t numerology = {};
    mcs_t mcs = {};
    tm_mode_t tm_mode = {};

    uint32_t N_PACKET_symb;
    uint32_t N_PDC_subc;
    uint32_t N_PDC_subc_bits;
    uint32_t N_PDC_bits;
    uint32_t N_TB_bits;
    uint32_t N_TB_byte;

    uint32_t Z;
    uint32_t N_code_blocks;
} packet_sizes_t;

int get_packet_sizes_t(packet_sizes_t& q, packet_sizes_required_input_t& qq);

int get_maximum_packet_sizes_t(packet_sizes_t& q, std::string radio_device_class_string, uint32_t maxPacketLengthType, uint32_t maxPacketLength);

int get_maximum_packet_sizes_t(packet_sizes_t& q, std::string radio_device_class_string);

void print_packet_sizes_t(packet_sizes_t& q);

}

#endif
