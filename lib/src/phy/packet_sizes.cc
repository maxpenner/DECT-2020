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

#include "dect2020/phy/packet_sizes.h"
#include "dect2020/sections_part3/pdc.h"
#include "dect2020/sections_part3/radio_device_class.h"
#include "dect2020/sections_part3/transmission_packet_structure.h"
#include "dect2020/sections_part3/transport_block_size.h"
#include "dect2020/utils/debug.h"
#include "dect2020/utils/math_functions.h"

namespace dect2020 {

int get_packet_sizes_t(packet_sizes_t& q, packet_sizes_required_input_t& qq){

    uint32_t u                  = qq.u;
    uint32_t b                  = qq.b;

    uint32_t PacketLengthType   = qq.PacketLengthType;
    uint32_t PacketLength       = qq.PacketLength;

    uint32_t tm_mode_index      = qq.tm_mode_index;

    uint32_t mcs_index          = qq.mcs_index;

    uint32_t Z                  = qq.Z;

    // sanity checks
    if(u!=1 && u!=2 && u!=4 && u!=8){
        ERROR("u is %d", u);
        return DECT2020_ERROR;
    }
    if(b!=1 && b!=2 && b!=4 && b!=8 && b!=12 && b!=16){
        ERROR("b is %d", b);
        return DECT2020_ERROR;
    }
    if(PacketLengthType!=0 && PacketLengthType!=1){
        ERROR("PacketLengthType is %d", PacketLengthType);
        return DECT2020_ERROR;
    }
    if(PacketLength==0 || PacketLength>16){
        ERROR("PacketLength is %d", PacketLength);
        return DECT2020_ERROR;
    }
    if(tm_mode_index>11){
        ERROR("tm_mode_index is %d", tm_mode_index);
        return DECT2020_ERROR;
    }
    if(mcs_index>11){
        ERROR("mcs_index is %d", mcs_index);
        return DECT2020_ERROR;
    }
    if(Z!=2048 && Z!=6144){
        ERROR("Z is %d", Z);
        return DECT2020_ERROR;
    }

    // the following calculations depend only on the variables listed above

    numerologies_t numerology = {};
    get_numerologies(numerology, u, b);

    uint32_t N_PACKET_symb = get_N_PACKET_symb(PacketLengthType, PacketLength, numerology.N_SLOT_u_symb, numerology.N_SLOT_u_subslot);

    // sanity check
    if(N_PACKET_symb < 5 || N_PACKET_symb > 1280 || N_PACKET_symb%5 != 0){
        ERROR("N_PACKET_symb is %d", N_PACKET_symb);
        return DECT2020_ERROR;
    }

    tm_mode_t tm_mode;
    get_tm_mode(tm_mode, tm_mode_index);
    uint32_t N_eff_TX = tm_mode.N_eff_TX;

    uint32_t N_PDC_subc = 0;
    if(get_N_PDC_subc(N_PDC_subc, N_PACKET_symb, u, N_eff_TX, numerology.N_b_OCC)){
        ERROR("Configuration failed: u=%d b=%d PacketLengthType=%d PacketLength=%d tm_mode_index=%d mcs_index=%d Z=%d N_PACKET_symb=%d", u, b, PacketLengthType, PacketLength, tm_mode_index, mcs_index, Z, N_PACKET_symb);
        return DECT2020_ERROR;
    }

    mcs_t mcs = {};
    get_mcs(mcs, mcs_index);

    uint32_t N_SS = tm_mode.N_SS;

    uint32_t N_TB_bits = 0;
    if(get_N_TB_bits(N_TB_bits, N_SS, N_PDC_subc, mcs.N_bps, mcs.R_numerator, mcs.R_denominator, Z)){
        ERROR("Configuration failed: u=%d b=%d PacketLengthType=%d PacketLength=%d tm_mode_index=%d mcs_index=%d Z=%d N_PACKET_symb=%d", u, b, PacketLengthType, PacketLength, tm_mode_index, mcs_index, Z, N_PACKET_symb);
        return DECT2020_ERROR;
    }

    q.packet_sizes_required_input = qq;

    q.numerology       = numerology;
    q.mcs              = mcs;
    q.tm_mode          = tm_mode;

    q.N_PACKET_symb    = N_PACKET_symb;
    q.N_PDC_subc       = N_PDC_subc;
    q.N_PDC_subc_bits  = get_N_PDC_bits_raw(N_SS, N_PDC_subc, mcs.N_bps);
    q.N_PDC_bits       = get_N_PDC_bits(N_SS, N_PDC_subc, mcs.N_bps, mcs.R_numerator, mcs.R_denominator);
    q.N_TB_bits        = N_TB_bits;
    q.N_TB_byte        = ceil_divide_integer<uint32_t>(N_TB_bits,8);

    q.Z                = Z;

    // 6.1.3
    uint32_t B = q.N_TB_bits;
    uint32_t L = 24;
    uint32_t C = ceil_divide_integer<uint32_t>(B, Z - L);

    q.N_code_blocks     = C;

    // sanity check
    if(C == 0){
        ERROR("Configuration failed: u=%d b=%d PacketLengthType=%d PacketLength=%d tm_mode_index=%d mcs_index=%d Z=%d N_PACKET_symb=%d", u, b, PacketLengthType, PacketLength, tm_mode_index, mcs_index, Z, N_PACKET_symb);
        return DECT2020_ERROR;
    }

    return DECT2020_SUCCESS;
}

int get_maximum_packet_sizes_t(packet_sizes_t& q, std::string radio_device_class_string, uint32_t maxPacketLengthType, uint32_t maxPacketLength){

        // radio device and maximum tbs
    dect2020::radio_device_class_t radio_device_class;
    if(dect2020::get_radio_device_class(radio_device_class, radio_device_class_string)){
        ERROR("Unable to initialize Radio Device Class");
        return DECT2020_ERROR;
    }

    dect2020::packet_sizes_required_input_t packet_sizes_required_input_maximum_sizes;
    packet_sizes_required_input_maximum_sizes.u                 = radio_device_class.u_min;
    packet_sizes_required_input_maximum_sizes.b                 = radio_device_class.b_min;
    packet_sizes_required_input_maximum_sizes.PacketLengthType  = maxPacketLengthType;
    packet_sizes_required_input_maximum_sizes.PacketLength      = maxPacketLength;
    packet_sizes_required_input_maximum_sizes.tm_mode_index     = dect2020::get_max_tm_mode_index_depending_on_N_ant(radio_device_class.N_ant_min);
    packet_sizes_required_input_maximum_sizes.mcs_index         = radio_device_class.mcs_index_min;
    packet_sizes_required_input_maximum_sizes.Z                 = radio_device_class.Z_min;

    return get_packet_sizes_t(q, packet_sizes_required_input_maximum_sizes);
}

int get_maximum_packet_sizes_t(packet_sizes_t& q, std::string radio_device_class_string){

    // part 3, 5.1 Note
    // part 4, Table 6.2.1-1: 4 bit, value+1 (4 bit max value is 15, +1 makes it 16)
    return get_maximum_packet_sizes_t(q, radio_device_class_string, 1, 16);
}

void print_packet_sizes_t(packet_sizes_t& q){
    std::cout << "Packet Sizes:    " << std::endl;
    std::cout << "N_PACKET_symb:   " << q.N_PACKET_symb << std::endl;
    std::cout << "N_PDC_subc:      " << q.N_PDC_subc << std::endl;
    std::cout << "N_PDC_subc_bits: " << q.N_PDC_subc_bits  << std::endl;
    std::cout << "N_PDC_bits:      " << q.N_PDC_bits << std::endl;
    std::cout << "N_TB_bits:       " << q.N_TB_bits << std::endl;
    std::cout << "N_TB_byte:       " << q.N_TB_byte << std::endl;
    std::cout << "Z:               " << q.Z << std::endl;
    std::cout << "N_code_blocks:   " << q.N_code_blocks << std::endl;
    std::cout << std::endl;
}

}
