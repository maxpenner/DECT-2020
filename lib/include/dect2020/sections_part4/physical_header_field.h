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

#ifndef DECT2020_PHYSICAL_HEADER_FIELD_H
#define DECT2020_PHYSICAL_HEADER_FIELD_H

#include <cstdint>

// ETSI TS 103 636-4 V1.1.1 (2020-07), 6.2

namespace dect2020 {

typedef struct{
    uint8_t HeaderFormat;
    uint8_t PacketLengthType;
    uint8_t PacketLength;
    uint8_t ShortNetworkID;
    uint16_t TransmitterIdentity;
    uint8_t TransmitPower;
    uint8_t Reserved;
    uint8_t DFMCS;

    uint8_t plcf[5];
} plcf_10_t;

typedef struct{
    uint8_t HeaderFormat;
    uint8_t PacketLengthType;
    uint8_t PacketLength;
    uint8_t ShortNetworkID;
    uint16_t TransmitterIdentity;
    uint8_t TransmitPower;
    uint8_t DFMCS;
    uint16_t ReceiverIdentity;
    uint8_t NumberOfSpatialStreams;
    uint8_t DFRedundancyVersion;
    uint8_t DFNewDataIndication;
    uint8_t DFHARQProcessNumber;
    uint8_t FeedbackFormat;
    uint16_t FeedbackInfo;

    uint8_t plcf[10];
} plcf_20_t;

typedef struct{
    uint8_t HeaderFormat;
    uint8_t PacketLengthType;
    uint8_t PacketLength;
    uint8_t ShortNetworkID;
    uint16_t TransmitterIdentity;
    uint8_t TransmitPower;
    uint8_t DFMCS;
    uint16_t ReceiverIdentity;
    uint8_t NumberOfSpatialStreams;
    uint8_t Reserved;
    uint8_t FeedbackFormat;
    uint16_t FeedbackInfo;

    uint8_t plcf[10];
} plcf_21_t;

int get_plcf(plcf_10_t& plcf_10);
int get_plcf(plcf_20_t& plcf_20);
int get_plcf(plcf_21_t& plcf_21);

int get_plcf_rev(plcf_10_t& plcf_10);
int get_plcf_rev(plcf_20_t& plcf_20);
int get_plcf_rev(plcf_21_t& plcf_21);

uint8_t get_transmit_power(int32_t tx_power_dBm);

uint8_t get_number_of_spatial_streams(int32_t N_SS);

}

#endif