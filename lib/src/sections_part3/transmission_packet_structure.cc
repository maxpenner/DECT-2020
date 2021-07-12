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

#include "dect2020/sections_part3/transmission_packet_structure.h"
#include "dect2020/utils/debug.h"

namespace dect2020 {

uint32_t get_N_PACKET_symb(uint32_t PacketLengthType, uint32_t PacketLength, uint32_t N_SLOT_u_symb, uint32_t N_SLOT_u_subslot){
    return (PacketLengthType == 0) ? PacketLength*N_SLOT_u_symb/N_SLOT_u_subslot : PacketLength*N_SLOT_u_symb;
}

uint32_t get_N_PACKET_symb(uint32_t PacketLengthType, uint32_t PacketLength, const numerologies_t& q){
    return get_N_PACKET_symb(PacketLengthType, PacketLength, q.N_SLOT_u_symb, q.N_SLOT_u_subslot);
}

}
