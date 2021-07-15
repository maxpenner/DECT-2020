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
#include <string.h>

#include "dect2020/sections_part4/physical_header_field.h"
#include "dect2020/utils/debug.h"

namespace dect2020 {

int get_plcf(plcf_10_t& plcf_10){

    plcf_10.plcf[0] = (plcf_10.plcf[0] & 0x1F) | (plcf_10.HeaderFormat << 5);
    plcf_10.plcf[0] = (plcf_10.plcf[0] & 0xEF) | (plcf_10.PacketLengthType << 4);
    plcf_10.plcf[0] = (plcf_10.plcf[0] & 0xF0) | plcf_10.PacketLength;

    plcf_10.plcf[1] = plcf_10.ShortNetworkID;

    plcf_10.plcf[2] = (uint8_t) (plcf_10.TransmitterIdentity >> 8);
    plcf_10.plcf[3] = (uint8_t) (plcf_10.TransmitterIdentity & 0xFF);

    plcf_10.plcf[4] = (plcf_10.plcf[4] & 0xF) | (plcf_10.TransmitPower << 4);
    plcf_10.plcf[4] = (plcf_10.plcf[4] & 0xF7) | (plcf_10.Reserved << 3);
    plcf_10.plcf[4] = (plcf_10.plcf[4] & 0xF8) | plcf_10.DFMCS;

    return DECT2020_SUCCESS;
}

int get_plcf(plcf_20_t& plcf_20){

    plcf_20.plcf[0] = (plcf_20.plcf[0] & 0x1F) | (plcf_20.HeaderFormat << 5);
    plcf_20.plcf[0] = (plcf_20.plcf[0] & 0xEF) | (plcf_20.PacketLengthType << 4);
    plcf_20.plcf[0] = (plcf_20.plcf[0] & 0xF0) | plcf_20.PacketLength;

    plcf_20.plcf[1] = plcf_20.ShortNetworkID;

    plcf_20.plcf[2] = (uint8_t) (plcf_20.TransmitterIdentity >> 8);
    plcf_20.plcf[3] = (uint8_t) (plcf_20.TransmitterIdentity & 0xFF);

    plcf_20.plcf[4] = (plcf_20.plcf[4] & 0xF) | (plcf_20.TransmitPower << 4);
    plcf_20.plcf[4] = (plcf_20.plcf[4] & 0xF0) | plcf_20.DFMCS;

    plcf_20.plcf[5] = (uint8_t) (plcf_20.ReceiverIdentity >> 8);
    plcf_20.plcf[6] = (uint8_t) (plcf_20.ReceiverIdentity & 0xFF);

    plcf_20.plcf[7] = (uint8_t) (plcf_20.plcf[7] & 0x3F) | (plcf_20.NumberOfSpatialStreams << 6);
    plcf_20.plcf[7] = (uint8_t) (plcf_20.plcf[7] & 0xCF) | (plcf_20.DFRedundancyVersion << 4);
    plcf_20.plcf[7] = (uint8_t) (plcf_20.plcf[7] & 0xF7) | (plcf_20.DFNewDataIndication << 3);
    plcf_20.plcf[7] = (uint8_t) (plcf_20.plcf[7] & 0xF8) | plcf_20.DFHARQProcessNumber;

    plcf_20.plcf[8] = (uint8_t) (plcf_20.plcf[8] & 0xF) | (plcf_20.FeedbackFormat << 4);
    plcf_20.plcf[8] = (uint8_t) (plcf_20.plcf[8] & 0xF0) | (plcf_20.FeedbackInfo >> 8);

    plcf_20.plcf[9] = (uint8_t) (plcf_20.FeedbackInfo & 0xFF);

    return DECT2020_SUCCESS;
}

int get_plcf(plcf_21_t& plcf_21){

    plcf_21.plcf[0] = (plcf_21.plcf[0] & 0x1F) | (plcf_21.HeaderFormat << 5);
    plcf_21.plcf[0] = (plcf_21.plcf[0] & 0xEF) | (plcf_21.PacketLengthType << 4);
    plcf_21.plcf[0] = (plcf_21.plcf[0] & 0xF0) | plcf_21.PacketLength;

    plcf_21.plcf[1] = plcf_21.ShortNetworkID;

    plcf_21.plcf[2] = (uint8_t) (plcf_21.TransmitterIdentity >> 8);
    plcf_21.plcf[3] = (uint8_t) (plcf_21.TransmitterIdentity & 0xFF);

    plcf_21.plcf[4] = (plcf_21.plcf[4] & 0xF) | (plcf_21.TransmitPower << 4);
    plcf_21.plcf[4] = (plcf_21.plcf[4] & 0xF0) | plcf_21.DFMCS;

    plcf_21.plcf[5] = (uint8_t) (plcf_21.ReceiverIdentity >> 8);
    plcf_21.plcf[6] = (uint8_t) (plcf_21.ReceiverIdentity & 0xFF);

    plcf_21.plcf[7] = (uint8_t) (plcf_21.plcf[7] & 0x3F) | (plcf_21.NumberOfSpatialStreams << 6);
    plcf_21.plcf[7] = (plcf_21.plcf[7] & 0xC0) | plcf_21.Reserved;

    plcf_21.plcf[8] = (uint8_t) (plcf_21.plcf[8] & 0xF) | (plcf_21.FeedbackFormat << 4);
    plcf_21.plcf[8] = (uint8_t) (plcf_21.plcf[8] & 0xF0) | (plcf_21.FeedbackInfo >> 8);

    plcf_21.plcf[9] = (uint8_t) (plcf_21.FeedbackInfo & 0xFF);

    return DECT2020_SUCCESS;
}

int get_plcf_rev(plcf_10_t& plcf_10){

    plcf_10.HeaderFormat        = plcf_10.plcf[0] >> 5;
    plcf_10.PacketLengthType    = (plcf_10.plcf[0] >> 4) & 1;
    plcf_10.PacketLength        = plcf_10.plcf[0] & 0xF;
    plcf_10.ShortNetworkID      = plcf_10.plcf[1];
    plcf_10.TransmitterIdentity = ((uint16_t) plcf_10.plcf[2]) << 8;
    plcf_10.TransmitterIdentity = (plcf_10.TransmitterIdentity & 0xFF00) | (uint16_t) plcf_10.plcf[3];
    plcf_10.TransmitPower       = plcf_10.plcf[4] >> 4;
    plcf_10.Reserved            = (plcf_10.plcf[4] >> 3) & 1;
    plcf_10.DFMCS               = plcf_10.plcf[4] & 7;

    return DECT2020_SUCCESS;
}

int get_plcf_rev(plcf_20_t& plcf_20){

    plcf_20.HeaderFormat            = plcf_20.plcf[0] >> 5;
    plcf_20.PacketLengthType        = (plcf_20.plcf[0] >> 4) & 1;
    plcf_20.PacketLength            = plcf_20.plcf[0] & 0xF;
    plcf_20.ShortNetworkID          = plcf_20.plcf[1];
    plcf_20.TransmitterIdentity     = ((uint16_t) plcf_20.plcf[2]) << 8;
    plcf_20.TransmitterIdentity     = (plcf_20.TransmitterIdentity & 0xFF00) | (uint16_t) plcf_20.plcf[3];
    plcf_20.TransmitPower           = plcf_20.plcf[4] >> 4;
    plcf_20.DFMCS                   = plcf_20.plcf[4] & 0xF;
    plcf_20.ReceiverIdentity        = ((uint16_t) plcf_20.plcf[5]) << 8;
    plcf_20.ReceiverIdentity        = (plcf_20.ReceiverIdentity & 0xFF00) | (uint16_t) plcf_20.plcf[6];
    plcf_20.NumberOfSpatialStreams  = plcf_20.plcf[7] >> 6;
    plcf_20.DFRedundancyVersion     = (plcf_20.plcf[7] >> 4) & 3;
    plcf_20.DFNewDataIndication     = (plcf_20.plcf[7] >> 3) & 1;
    plcf_20.DFHARQProcessNumber     = plcf_20.plcf[7] & 7;
    plcf_20.FeedbackFormat          = (plcf_20.plcf[8] >> 4) & 0xF;
    plcf_20.FeedbackInfo            = (uint16_t) (plcf_20.plcf[8] & 0xF) << 8;
    plcf_20.FeedbackInfo            = (plcf_20.FeedbackInfo & 0xFF00) | (uint16_t) plcf_20.plcf[9];

    return DECT2020_SUCCESS;
}

int get_plcf_rev(plcf_21_t& plcf_21){

    plcf_21.HeaderFormat            = plcf_21.plcf[0] >> 5;
    plcf_21.PacketLengthType        = (plcf_21.plcf[0] >> 4) & 1;
    plcf_21.PacketLength            = plcf_21.plcf[0] & 0xF;
    plcf_21.ShortNetworkID          = plcf_21.plcf[1];
    plcf_21.TransmitterIdentity     = ((uint16_t) plcf_21.plcf[2]) << 8;
    plcf_21.TransmitterIdentity     = (plcf_21.TransmitterIdentity & 0xFF00) | (uint16_t) plcf_21.plcf[3];
    plcf_21.TransmitPower           = plcf_21.plcf[4] >> 4;
    plcf_21.DFMCS                   = plcf_21.plcf[4] & 0xF;
    plcf_21.ReceiverIdentity        = ((uint16_t) plcf_21.plcf[5]) << 8;
    plcf_21.ReceiverIdentity        = (plcf_21.ReceiverIdentity & 0xFF00) | (uint16_t) plcf_21.plcf[6];
    plcf_21.NumberOfSpatialStreams  = plcf_21.plcf[7] >> 6;
    plcf_21.Reserved                = plcf_21.plcf[7] & 0x3F;
    plcf_21.FeedbackFormat          = (plcf_21.plcf[8] >> 4) & 0xF;
    plcf_21.FeedbackInfo            = (uint16_t) (plcf_21.plcf[8] & 0xF) << 8;
    plcf_21.FeedbackInfo            = (plcf_21.FeedbackInfo & 0xFF00) | (uint16_t) plcf_21.plcf[9];

    return DECT2020_SUCCESS;
}

uint8_t get_transmit_power(int32_t tx_power_dBm){

    uint32_t ret = 15;

    int32_t tx_power_table[16] = {-40, -30, -20, -10, -6, -3, 0, 3, 6, 10, 14, 19, 23, 26, 29, 32};

    // find closest value
    int32_t dist_min = 1000000;
    int32_t dist;
    for(uint32_t i=0; i<16; i++){
        dist = (tx_power_dBm - tx_power_table[i]);
        dist *= dist;

        if(dist <= dist_min){
            dist_min = dist;
            ret = (uint8_t) i;
        }
    }

    return ret;
}

uint8_t get_number_of_spatial_streams(int32_t N_SS){
    if(N_SS == 1)
        return 0;
    else if(N_SS == 2)
        return 1;
    else if(N_SS == 4)
        return 2;
    else if(N_SS == 8)
        return 3;

    return 0;
}

}