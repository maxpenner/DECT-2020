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

#ifndef DECT2020_CONFIG_H
#define DECT2020_CONFIG_H

// standard compliant maximum length is 16 slots, we can further limit this value
#define DECT2020_PACKETLENGTHTYPE_MAX   1
#define DECT2020_PACKETLENGTH_MAX       2

// https://github.com/srsran/srsRAN/blob/master/lib/include/srsran/phy/fec/softbuffer.h
#define DECT2020_SOFTBUFFER_SIZE        18600

// resolution of bits passed to receiver
#define DECT2020_LLR_BIT_WIDTH_08       8
#define DECT2020_LLR_BIT_WIDTH_16       16
#define DECT2020_LLR_BIT_WIDTH          DECT2020_LLR_BIT_WIDTH_16
#if DECT2020_LLR_BIT_WIDTH == DECT2020_LLR_BIT_WIDTH_08
    #define DECT2020_D_RX_DATA_TYPE     int8_t
#elif DECT2020_LLR_BIT_WIDTH == DECT2020_LLR_BIT_WIDTH_16
    #define DECT2020_D_RX_DATA_TYPE     int16_t
#endif

#endif
