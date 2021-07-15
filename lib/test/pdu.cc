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
#include <stdlib.h>

#include "dect2020/config.h"
#include "dect2020/sections_part4/feedback_info.h"
#include "dect2020/sections_part4/physical_header_field.h"
#include "dect2020/utils/debug.h"

int main(int argc, char** argv)
{
    // ###############################
    //
    //  Type 1, Header 000
    //
    // ###############################

    std::cout << "Test A Type 1, Header Format: 000 Start" << std::endl;

    bool any_error = false;

    uint32_t header_format_step = 2;
    uint32_t short_network_id_step = 50;
    uint32_t transmitter_identity_step = 500;

    for(uint32_t header_format=0; header_format<8; header_format+=header_format_step){

        for(uint32_t packet_length_type=0; packet_length_type<2; packet_length_type++){

            for(uint32_t packet_length=0; packet_length<16; packet_length++){

                for(uint32_t short_network_id=0; short_network_id<256; short_network_id+=short_network_id_step){

                    for(uint32_t transmitter_identity=0; transmitter_identity<65536; transmitter_identity+=transmitter_identity_step){

                        for(uint32_t transmit_power=0; transmit_power<16; transmit_power++){

                            for(uint32_t reserved=0; reserved<2; reserved++){

                                for(uint32_t DFMCS=0; DFMCS<8; DFMCS++){

                                    dect2020::plcf_10_t plcf_10, plcf_10_rev;

                                    // randomize arrays
                                    for(int i=0; i<5; i++){
                                        plcf_10.plcf[i] = (uint8_t) (rand() % 256);
                                        plcf_10_rev.plcf[i] = (uint8_t) (rand() % 256);
                                    }

                                    // pack
                                    plcf_10.HeaderFormat        = header_format;
                                    plcf_10.PacketLengthType    = packet_length_type;
                                    plcf_10.PacketLength        = packet_length;
                                    plcf_10.ShortNetworkID      = short_network_id;
                                    plcf_10.TransmitterIdentity = transmitter_identity;
                                    plcf_10.TransmitPower       = transmit_power;
                                    plcf_10.Reserved            = reserved;
                                    plcf_10.DFMCS               = DFMCS;
                                    dect2020::get_plcf(plcf_10);

                                    // copy
                                    memcpy(plcf_10_rev.plcf, plcf_10.plcf, 5);

                                    // unpack
                                    dect2020::get_plcf_rev(plcf_10_rev);

                                    // compare
                                    if(plcf_10.HeaderFormat != plcf_10_rev.HeaderFormat)
                                        any_error = true;
                                    if(plcf_10_rev.HeaderFormat != header_format)
                                        any_error = true;

                                    if(plcf_10.PacketLengthType != plcf_10_rev.PacketLengthType)
                                        any_error = true;
                                    if(plcf_10_rev.PacketLengthType != packet_length_type)
                                        any_error = true;

                                    if(plcf_10.PacketLength != plcf_10_rev.PacketLength)
                                        any_error = true;
                                    if(plcf_10_rev.PacketLength != packet_length)
                                        any_error = true;

                                    if(plcf_10.ShortNetworkID != plcf_10_rev.ShortNetworkID)
                                        any_error = true;
                                    if(plcf_10_rev.ShortNetworkID != short_network_id)
                                        any_error = true;

                                    if(plcf_10.TransmitterIdentity != plcf_10_rev.TransmitterIdentity)
                                        any_error = true;
                                    if(plcf_10_rev.TransmitterIdentity != transmitter_identity)
                                        any_error = true;

                                    if(plcf_10.TransmitPower != plcf_10_rev.TransmitPower)
                                        any_error = true;
                                    if(plcf_10_rev.TransmitPower != transmit_power)
                                        any_error = true;

                                    if(plcf_10.Reserved != plcf_10_rev.Reserved)
                                        any_error = true;
                                    if(plcf_10_rev.Reserved != reserved)
                                        any_error = true;

                                    if(plcf_10.DFMCS != plcf_10_rev.DFMCS)
                                        any_error = true;
                                    if(plcf_10_rev.DFMCS != DFMCS)
                                        any_error = true;

                                }//DFMCS
                            }//reserved
                        }//transmit_power
                    }//transmitter_identity
                }//short_network_id
            }//packet_length
        }//packet_length_type
    }//header format

    std::cout << "any_error " << (any_error == true ? "true" : "false") << std::endl;
    std::cout << "Test A End" << std::endl;

    any_error = false;

    // ###############################
    //
    //  Type 2, Header 000
    //
    // ###############################

    std::cout << std::endl << "Test B Type 2, Header Format: 000 Start" << std::endl;

    any_error = false;

    header_format_step = 2;
    short_network_id_step = 50;
    transmitter_identity_step = 500;

    uint32_t N_random_try = 10;

    for(uint32_t header_format=0; header_format<8; header_format+=header_format_step){

        for(uint32_t packet_length_type=0; packet_length_type<2; packet_length_type++){

            for(uint32_t packet_length=0; packet_length<16; packet_length++){

                for(uint32_t short_network_id=0; short_network_id<256; short_network_id+=short_network_id_step){

                    for(uint32_t transmitter_identity=0; transmitter_identity<65536; transmitter_identity+=transmitter_identity_step){

                        for(uint32_t transmit_power=0; transmit_power<16; transmit_power++){

                            for(uint32_t DFMCS=0; DFMCS<16; DFMCS++){

                                for(uint32_t random_try=0; random_try<N_random_try; random_try++){

                                    dect2020::plcf_20_t plcf_20, plcf_20_rev;

                                    // randomize arrays
                                    for(int i=0; i<10; i++){
                                        plcf_20.plcf[i] = (uint8_t) (rand() % 256);
                                        plcf_20_rev.plcf[i] = (uint8_t) (rand() % 256);
                                    }

                                    // pack
                                    plcf_20.HeaderFormat        = header_format;
                                    plcf_20.PacketLengthType    = packet_length_type;
                                    plcf_20.PacketLength        = packet_length;
                                    plcf_20.ShortNetworkID      = short_network_id;
                                    plcf_20.TransmitterIdentity = transmitter_identity;
                                    plcf_20.TransmitPower       = transmit_power;
                                    plcf_20.DFMCS               = DFMCS;

                                    // random values within valid range
                                    uint16_t ReceiverIdentity       = (uint16_t) (rand() % 65636);
                                    uint8_t NumberOfSpatialStreams  = (uint8_t) (rand() % 4);
                                    uint8_t DFRedundancyVersion     = (uint8_t) (rand() % 4);
                                    uint8_t DFNewDataIndication     = (uint8_t) (rand() % 2);
                                    uint8_t DFHARQProcessNumber     = (uint8_t) (rand() % 8);
                                    uint8_t FeedbackFormat          = (uint8_t) (rand() % 16);
                                    uint16_t FeedbackInfo           = (uint16_t) (rand() % 4096);

                                    plcf_20.ReceiverIdentity        = ReceiverIdentity;
                                    plcf_20.NumberOfSpatialStreams  = NumberOfSpatialStreams;
                                    plcf_20.DFRedundancyVersion     = DFRedundancyVersion;
                                    plcf_20.DFNewDataIndication     = DFNewDataIndication;
                                    plcf_20.DFHARQProcessNumber     = DFHARQProcessNumber;
                                    plcf_20.FeedbackFormat          = FeedbackFormat;
                                    plcf_20.FeedbackInfo            = FeedbackInfo;

                                    dect2020::get_plcf(plcf_20);

                                    // copy
                                    memcpy(plcf_20_rev.plcf, plcf_20.plcf, 10);

                                    // unpack
                                    dect2020::get_plcf_rev(plcf_20_rev);

                                    // compare
                                    if(plcf_20.HeaderFormat != plcf_20_rev.HeaderFormat)
                                        any_error = true;
                                    if(plcf_20_rev.HeaderFormat != header_format)
                                        any_error = true;

                                    if(plcf_20.PacketLengthType != plcf_20_rev.PacketLengthType)
                                        any_error = true;
                                    if(plcf_20_rev.PacketLengthType != packet_length_type)
                                        any_error = true;

                                    if(plcf_20.PacketLength != plcf_20_rev.PacketLength)
                                        any_error = true;
                                    if(plcf_20_rev.PacketLength != packet_length)
                                        any_error = true;

                                    if(plcf_20.ShortNetworkID != plcf_20_rev.ShortNetworkID)
                                        any_error = true;
                                    if(plcf_20_rev.ShortNetworkID != short_network_id)
                                        any_error = true;

                                    if(plcf_20.TransmitterIdentity != plcf_20_rev.TransmitterIdentity)
                                        any_error = true;
                                    if(plcf_20_rev.TransmitterIdentity != transmitter_identity)
                                        any_error = true;

                                    if(plcf_20.TransmitPower != plcf_20_rev.TransmitPower)
                                        any_error = true;
                                    if(plcf_20_rev.TransmitPower != transmit_power)
                                        any_error = true;

                                    if(plcf_20.DFMCS != plcf_20_rev.DFMCS)
                                        any_error = true;
                                    if(plcf_20_rev.DFMCS != DFMCS)
                                        any_error = true;

                                    if(plcf_20.ReceiverIdentity != plcf_20_rev.ReceiverIdentity)
                                        any_error = true;
                                    if(plcf_20_rev.ReceiverIdentity != ReceiverIdentity)
                                        any_error = true;

                                    if(plcf_20.NumberOfSpatialStreams != plcf_20_rev.NumberOfSpatialStreams)
                                        any_error = true;
                                    if(plcf_20_rev.NumberOfSpatialStreams != NumberOfSpatialStreams)
                                        any_error = true;

                                    if(plcf_20.DFRedundancyVersion != plcf_20_rev.DFRedundancyVersion)
                                        any_error = true;
                                    if(plcf_20_rev.DFRedundancyVersion != DFRedundancyVersion)
                                        any_error = true;

                                    if(plcf_20.DFNewDataIndication != plcf_20_rev.DFNewDataIndication)
                                        any_error = true;
                                    if(plcf_20_rev.DFNewDataIndication != DFNewDataIndication)
                                        any_error = true;

                                    if(plcf_20.DFHARQProcessNumber != plcf_20_rev.DFHARQProcessNumber)
                                        any_error = true;
                                    if(plcf_20_rev.DFHARQProcessNumber != DFHARQProcessNumber)
                                        any_error = true;

                                    if(plcf_20.FeedbackFormat != plcf_20_rev.FeedbackFormat)
                                        any_error = true;
                                    if(plcf_20_rev.FeedbackFormat != FeedbackFormat)
                                        any_error = true;

                                    if(plcf_20.FeedbackInfo != plcf_20_rev.FeedbackInfo)
                                        any_error = true;
                                    if(plcf_20_rev.FeedbackInfo != FeedbackInfo)
                                        any_error = true;

                                }//random try
                            }//DFMCS
                        }//transmit_power
                    }//transmitter_identity
                }//short_network_id
            }//packet_length
        }//packet_length_type
    }//header format

    std::cout << "any_error " << (any_error == true ? "true" : "false") << std::endl;
    std::cout << "Test B End" << std::endl;

    any_error = false;

    // ###############################
    //
    //  Type 2, Header 000
    //
    // ###############################

    std::cout << std::endl << "Test C Type 2, Header Format: 001 Start" << std::endl;

    any_error = false;

    header_format_step = 2;
    short_network_id_step = 50;
    transmitter_identity_step = 500;
    N_random_try = 10;

    for(uint32_t header_format=0; header_format<8; header_format+=header_format_step){

        for(uint32_t packet_length_type=0; packet_length_type<2; packet_length_type++){

            for(uint32_t packet_length=0; packet_length<16; packet_length++){

                for(uint32_t short_network_id=0; short_network_id<256; short_network_id+=short_network_id_step){

                    for(uint32_t transmitter_identity=0; transmitter_identity<65536; transmitter_identity+=transmitter_identity_step){

                        for(uint32_t transmit_power=0; transmit_power<16; transmit_power++){

                            for(uint32_t DFMCS=0; DFMCS<16; DFMCS++){

                                for(uint32_t random_try=0; random_try<N_random_try; random_try++){

                                    dect2020::plcf_21_t plcf_21, plcf_21_rev;

                                    // randomize arrays
                                    for(int i=0; i<10; i++){
                                        plcf_21.plcf[i] = (uint8_t) (rand() % 256);
                                        plcf_21_rev.plcf[i] = (uint8_t) (rand() % 256);
                                    }

                                    // pack
                                    plcf_21.HeaderFormat        = header_format;
                                    plcf_21.PacketLengthType    = packet_length_type;
                                    plcf_21.PacketLength        = packet_length;
                                    plcf_21.ShortNetworkID      = short_network_id;
                                    plcf_21.TransmitterIdentity = transmitter_identity;
                                    plcf_21.TransmitPower       = transmit_power;
                                    plcf_21.DFMCS               = DFMCS;

                                    // random values within valid range
                                    uint16_t ReceiverIdentity       = (uint16_t) (rand() % 65636);
                                    uint8_t NumberOfSpatialStreams  = (uint8_t) (rand() % 4);
                                    uint8_t Reserved                = (uint8_t) (rand() % 64);
                                    uint8_t FeedbackFormat          = (uint8_t) (rand() % 16);
                                    uint16_t FeedbackInfo           = (uint16_t) (rand() % 4096);

                                    plcf_21.ReceiverIdentity        = ReceiverIdentity;
                                    plcf_21.NumberOfSpatialStreams  = NumberOfSpatialStreams;
                                    plcf_21.Reserved                = Reserved;
                                    plcf_21.FeedbackFormat          = FeedbackFormat;
                                    plcf_21.FeedbackInfo            = FeedbackInfo;

                                    dect2020::get_plcf(plcf_21);

                                    // copy
                                    memcpy(plcf_21_rev.plcf, plcf_21.plcf, 10);

                                    // unpack
                                    dect2020::get_plcf_rev(plcf_21_rev);

                                    // compare
                                    if(plcf_21.HeaderFormat != plcf_21_rev.HeaderFormat)
                                        any_error = true;
                                    if(plcf_21_rev.HeaderFormat != header_format)
                                        any_error = true;

                                    if(plcf_21.PacketLengthType != plcf_21_rev.PacketLengthType)
                                        any_error = true;
                                    if(plcf_21_rev.PacketLengthType != packet_length_type)
                                        any_error = true;

                                    if(plcf_21.PacketLength != plcf_21_rev.PacketLength)
                                        any_error = true;
                                    if(plcf_21_rev.PacketLength != packet_length)
                                        any_error = true;

                                    if(plcf_21.ShortNetworkID != plcf_21_rev.ShortNetworkID)
                                        any_error = true;
                                    if(plcf_21_rev.ShortNetworkID != short_network_id)
                                        any_error = true;

                                    if(plcf_21.TransmitterIdentity != plcf_21_rev.TransmitterIdentity)
                                        any_error = true;
                                    if(plcf_21_rev.TransmitterIdentity != transmitter_identity)
                                        any_error = true;

                                    if(plcf_21.TransmitPower != plcf_21_rev.TransmitPower)
                                        any_error = true;
                                    if(plcf_21_rev.TransmitPower != transmit_power)
                                        any_error = true;

                                    if(plcf_21.DFMCS != plcf_21_rev.DFMCS)
                                        any_error = true;
                                    if(plcf_21_rev.DFMCS != DFMCS)
                                        any_error = true;

                                    if(plcf_21.ReceiverIdentity != plcf_21_rev.ReceiverIdentity)
                                        any_error = true;
                                    if(plcf_21_rev.ReceiverIdentity != ReceiverIdentity)
                                        any_error = true;

                                    if(plcf_21.NumberOfSpatialStreams != plcf_21_rev.NumberOfSpatialStreams)
                                        any_error = true;
                                    if(plcf_21_rev.NumberOfSpatialStreams != NumberOfSpatialStreams)
                                        any_error = true;

                                    if(plcf_21.Reserved != plcf_21_rev.Reserved)
                                        any_error = true;
                                    if(plcf_21_rev.Reserved != Reserved)
                                        any_error = true;

                                    if(plcf_21.FeedbackFormat != plcf_21_rev.FeedbackFormat)
                                        any_error = true;
                                    if(plcf_21_rev.FeedbackFormat != FeedbackFormat)
                                        any_error = true;

                                    if(plcf_21.FeedbackInfo != plcf_21_rev.FeedbackInfo)
                                        any_error = true;
                                    if(plcf_21_rev.FeedbackInfo != FeedbackInfo)
                                        any_error = true;

                                }//random try
                            }//DFMCS
                        }//transmit_power
                    }//transmitter_identity
                }//short_network_id
            }//packet_length
        }//packet_length_type
    }//header format

    std::cout << "any_error " << (any_error == true ? "true" : "false") << std::endl;
    std::cout << "Test C End" << std::endl;

    // ###############################
    //
    //  feedback info
    //
    // ###############################

    std::cout << std::endl << "Test D miscellanea" << std::endl;

    std::cout << "get_transmit_power(-100) = " << (int) dect2020::get_transmit_power(-100) << std::endl;
    std::cout << "get_transmit_power(-50) = " << (int) dect2020::get_transmit_power(-50) << std::endl;
    std::cout << "get_transmit_power(-40) = " << (int) dect2020::get_transmit_power(-40) << std::endl;
    std::cout << "get_transmit_power(-35) = " << (int) dect2020::get_transmit_power(-35) << std::endl;
    std::cout << "get_transmit_power(-34) = " << (int) dect2020::get_transmit_power(-34) << std::endl;
    std::cout << "get_transmit_power(-30) = " << (int) dect2020::get_transmit_power(-30) << std::endl;
    std::cout << "get_transmit_power(-20) = " << (int) dect2020::get_transmit_power(-20) << std::endl;
    std::cout << "get_transmit_power(-10) = " << (int) dect2020::get_transmit_power(-10) << std::endl;
    std::cout << "get_transmit_power(-6) = " << (int) dect2020::get_transmit_power(-6) << std::endl;
    std::cout << "get_transmit_power(-3) = " << (int) dect2020::get_transmit_power(-3) << std::endl;
    std::cout << "get_transmit_power(0) = " << (int) dect2020::get_transmit_power(0) << std::endl;
    std::cout << "get_transmit_power(3) = " << (int) dect2020::get_transmit_power(3) << std::endl;
    std::cout << "get_transmit_power(5) = " << (int) dect2020::get_transmit_power(5) << std::endl;
    std::cout << "get_transmit_power(6) = " << (int) dect2020::get_transmit_power(6) << std::endl;
    std::cout << "get_transmit_power(10) = " << (int) dect2020::get_transmit_power(10) << std::endl;
    std::cout << "get_transmit_power(14) = " << (int) dect2020::get_transmit_power(14) << std::endl;
    std::cout << "get_transmit_power(19) = " << (int) dect2020::get_transmit_power(19) << std::endl;
    std::cout << "get_transmit_power(23) = " << (int) dect2020::get_transmit_power(23) << std::endl;
    std::cout << "get_transmit_power(26) = " << (int) dect2020::get_transmit_power(26) << std::endl;
    std::cout << "get_transmit_power(29) = " << (int) dect2020::get_transmit_power(29) << std::endl;
    std::cout << "get_transmit_power(30) = " << (int) dect2020::get_transmit_power(30) << std::endl;
    std::cout << "get_transmit_power(32) = " << (int) dect2020::get_transmit_power(32) << std::endl;
    std::cout << "get_transmit_power(40) = " << (int) dect2020::get_transmit_power(40) << std::endl << std::endl;

    std::cout << "get_number_of_spatial_streams(1) = " << (int) dect2020::get_number_of_spatial_streams(1) << std::endl;
    std::cout << "get_number_of_spatial_streams(2) = " << (int) dect2020::get_number_of_spatial_streams(2) << std::endl;
    std::cout << "get_number_of_spatial_streams(4) = " << (int) dect2020::get_number_of_spatial_streams(4) << std::endl;
    std::cout << "get_number_of_spatial_streams(8) = " << (int) dect2020::get_number_of_spatial_streams(8) << std::endl << std::endl;

    std::cout << "get_cqi(0) = " << (int) dect2020::get_cqi(0) << std::endl;
    std::cout << "get_cqi(1) = " << (int) dect2020::get_cqi(1) << std::endl;
    std::cout << "get_cqi(10) = " << (int) dect2020::get_cqi(10) << std::endl;
    std::cout << "get_cqi(11) = " << (int) dect2020::get_cqi(11) << std::endl;
    std::cout << "get_cqi(12) = " << (int) dect2020::get_cqi(12) << std::endl << std::endl;

    std::cout << "get_buffer_status(0) = " << (int) dect2020::get_buffer_status(0) << std::endl;
    std::cout << "get_buffer_status(10) = " << (int) dect2020::get_buffer_status(10) << std::endl;
    std::cout << "get_buffer_status(50) = " << (int) dect2020::get_buffer_status(50) << std::endl;
    std::cout << "get_buffer_status(43244) = " << (int) dect2020::get_buffer_status(43244) << std::endl;
    std::cout << "get_buffer_status(262143) = " << (int) dect2020::get_buffer_status(262143) << std::endl;
    std::cout << "get_buffer_status(262144) = " << (int) dect2020::get_buffer_status(262144) << std::endl;
    std::cout << "get_buffer_status(262145) = " << (int) dect2020::get_buffer_status(262145) << std::endl;
    std::cout << "get_buffer_status(1000000) = " << (int) dect2020::get_buffer_status(1000000) << std::endl << std::endl;


    std::cout << "Test D End" << std::endl;

    // ###############################
    //
    //  feedback info
    //
    // ###############################

    std::cout << std::endl << "Test E feedback info" << std::endl;

    any_error = false;

    // FORMAT 1

    for(uint32_t harq_process_number=0; harq_process_number<8; harq_process_number++){
        for(uint32_t transmission_feedback=0; transmission_feedback<2; transmission_feedback++){
            for(uint32_t cqi=0; cqi<16; cqi++){
                for(uint32_t buffer_status=0; buffer_status<16; buffer_status++){

                    dect2020::feedback_info_f1_t feedback_info_f1, feedback_info_f1_rev;

                    // randomize data
                    feedback_info_f1.feedback_info = rand() % 65536;

                    feedback_info_f1.HARQ_Process_number    = harq_process_number;
                    feedback_info_f1.Transmission_feedback  = transmission_feedback;
                    feedback_info_f1.CQI                    = cqi;
                    feedback_info_f1.Buffer_Status          = buffer_status;

                    dect2020::get_feedback_info(feedback_info_f1);

                    // copy
                    feedback_info_f1_rev.feedback_info = feedback_info_f1.feedback_info;

                    dect2020::get_feedback_info_rev(feedback_info_f1_rev);

                    // compare
                    if(feedback_info_f1.HARQ_Process_number != feedback_info_f1_rev.HARQ_Process_number)
                        any_error = true;
                    if(feedback_info_f1.HARQ_Process_number != harq_process_number)
                        any_error = true;

                    if(feedback_info_f1.Transmission_feedback != feedback_info_f1_rev.Transmission_feedback)
                        any_error = true;
                    if(feedback_info_f1.Transmission_feedback != transmission_feedback)
                        any_error = true;

                    if(feedback_info_f1.CQI != feedback_info_f1_rev.CQI)
                        any_error = true;
                    if(feedback_info_f1.CQI != cqi)
                        any_error = true;

                    if(feedback_info_f1.Buffer_Status != feedback_info_f1_rev.Buffer_Status)
                        any_error = true;
                    if(feedback_info_f1.Buffer_Status != buffer_status)
                        any_error = true;

                }//buffer_status
            }//cqi
        }//transmission_feedback
    }//harq_process_number

    // FORMAT 2

    for(uint32_t cqi=0; cqi<16; cqi++){
        for(uint32_t buffer_status=0; buffer_status<16; buffer_status++){
            for(uint32_t mimo_feedback=0; mimo_feedback<2; mimo_feedback++){
                for(uint32_t codebook_index=0; codebook_index<8; codebook_index++){

                    dect2020::feedback_info_f2_t feedback_info_f2, feedback_info_f2_rev;

                    // randomize data
                    feedback_info_f2.feedback_info = rand() % 65536;

                    feedback_info_f2.CQI                = cqi;
                    feedback_info_f2.Buffer_Status      = buffer_status;
                    feedback_info_f2.MIMO_feedback      = mimo_feedback;
                    feedback_info_f2.Codebook_index     = codebook_index;

                    dect2020::get_feedback_info(feedback_info_f2);

                    // copy
                    feedback_info_f2_rev.feedback_info = feedback_info_f2.feedback_info;

                    dect2020::get_feedback_info_rev(feedback_info_f2_rev);

                    // compare
                    if(feedback_info_f2.CQI != feedback_info_f2_rev.CQI)
                        any_error = true;
                    if(feedback_info_f2.CQI != cqi)
                        any_error = true;

                    if(feedback_info_f2.Buffer_Status != feedback_info_f2_rev.Buffer_Status)
                        any_error = true;
                    if(feedback_info_f2.Buffer_Status != buffer_status)
                        any_error = true;

                    if(feedback_info_f2.MIMO_feedback != feedback_info_f2_rev.MIMO_feedback)
                        any_error = true;
                    if(feedback_info_f2.MIMO_feedback != mimo_feedback)
                        any_error = true;

                    if(feedback_info_f2.Codebook_index != feedback_info_f2_rev.Codebook_index)
                        any_error = true;
                    if(feedback_info_f2.Codebook_index != codebook_index)
                        any_error = true;

                }
            }
        }
    }

    // FORMAT 3

    for(uint32_t harq_process_number0=0; harq_process_number0<8; harq_process_number0++){
        for(uint32_t transmission_feedback0=0; transmission_feedback0<2; transmission_feedback0++){
            for(uint32_t harq_process_number1=0; harq_process_number1<8; harq_process_number1++){
                for(uint32_t transmission_feedback1=0; transmission_feedback1<2; transmission_feedback1++){
                    for(uint32_t cqi=0; cqi<16; cqi++){

                        dect2020::feedback_info_f3_t feedback_info_f3, feedback_info_f3_rev;

                        // randomize data
                        feedback_info_f3.feedback_info = rand() % 65536;

                        feedback_info_f3.HARQ_Process_number_0      = harq_process_number0;
                        feedback_info_f3.Transmission_feedback_0    = transmission_feedback0;
                        feedback_info_f3.HARQ_Process_number_1      = harq_process_number1;
                        feedback_info_f3.Transmission_feedback_1    = transmission_feedback1;
                        feedback_info_f3.CQI                        = cqi;

                        dect2020::get_feedback_info(feedback_info_f3);

                        // copy
                        feedback_info_f3_rev.feedback_info = feedback_info_f3.feedback_info;

                        dect2020::get_feedback_info_rev(feedback_info_f3_rev);

                        // compare
                        if(feedback_info_f3.HARQ_Process_number_0 != feedback_info_f3_rev.HARQ_Process_number_0)
                            any_error = true;
                        if(feedback_info_f3.HARQ_Process_number_0 != harq_process_number0)
                            any_error = true;

                        if(feedback_info_f3.Transmission_feedback_0 != feedback_info_f3_rev.Transmission_feedback_0)
                            any_error = true;
                        if(feedback_info_f3.Transmission_feedback_0 != transmission_feedback0)
                            any_error = true;

                        if(feedback_info_f3.HARQ_Process_number_1 != feedback_info_f3_rev.HARQ_Process_number_1)
                            any_error = true;
                        if(feedback_info_f3.HARQ_Process_number_1 != harq_process_number1)
                            any_error = true;

                        if(feedback_info_f3.Transmission_feedback_1 != feedback_info_f3_rev.Transmission_feedback_1)
                            any_error = true;
                        if(feedback_info_f3.Transmission_feedback_1 != transmission_feedback1)
                            any_error = true;

                        if(feedback_info_f3.CQI != feedback_info_f3_rev.CQI)
                            any_error = true;
                        if(feedback_info_f3.CQI != cqi)
                            any_error = true;
                    }
                }
            }
        }
    }

    // FORMAT 4

    for(uint32_t harq_feedback_bitmap=0; harq_feedback_bitmap<256; harq_feedback_bitmap++){
        for(uint32_t cqi=0; cqi<16; cqi++){

            dect2020::feedback_info_f4_t feedback_info_f4, feedback_info_f4_rev;

            // randomize data
            feedback_info_f4.feedback_info = rand() % 65536;

            feedback_info_f4.HARQ_feedback_bitmap   = harq_feedback_bitmap;
            feedback_info_f4.CQI                    = cqi;

            dect2020::get_feedback_info(feedback_info_f4);

            // copy
            feedback_info_f4_rev.feedback_info = feedback_info_f4.feedback_info;

            dect2020::get_feedback_info_rev(feedback_info_f4_rev);

            // compare
            if(feedback_info_f4.HARQ_feedback_bitmap != feedback_info_f4_rev.HARQ_feedback_bitmap)
                any_error = true;
            if(feedback_info_f4.HARQ_feedback_bitmap != harq_feedback_bitmap)
                any_error = true;

            if(feedback_info_f4.CQI != feedback_info_f4_rev.CQI)
                any_error = true;
            if(feedback_info_f4.CQI != cqi)
                any_error = true;
        }
    }

    // FORMAT 5

    for(uint32_t harq_process_number=0; harq_process_number<8; harq_process_number++){
        for(uint32_t transmission_feedback=0; transmission_feedback<2; transmission_feedback++){
            for(uint32_t mimo_feedback=0; mimo_feedback<4; mimo_feedback++){
                for(uint32_t codebook_index=0; codebook_index<64; codebook_index++){

                    dect2020::feedback_info_f5_t feedback_info_f5, feedback_info_f5_rev;

                    // randomize data
                    feedback_info_f5.feedback_info = rand() % 65536;

                    feedback_info_f5.HARQ_Process_number    = harq_process_number;
                    feedback_info_f5.Transmission_feedback  = transmission_feedback;
                    feedback_info_f5.MIMO_feedback          = mimo_feedback;
                    feedback_info_f5.Codebook_index         = codebook_index;

                    dect2020::get_feedback_info(feedback_info_f5);

                    // copy
                    feedback_info_f5_rev.feedback_info = feedback_info_f5.feedback_info;

                    dect2020::get_feedback_info_rev(feedback_info_f5_rev);

                    // compare
                    if(feedback_info_f5.HARQ_Process_number != feedback_info_f5_rev.HARQ_Process_number)
                        any_error = true;
                    if(feedback_info_f5.HARQ_Process_number != harq_process_number)
                        any_error = true;

                    if(feedback_info_f5.Transmission_feedback != feedback_info_f5_rev.Transmission_feedback)
                        any_error = true;
                    if(feedback_info_f5.Transmission_feedback != transmission_feedback)
                        any_error = true;

                    if(feedback_info_f5.MIMO_feedback != feedback_info_f5_rev.MIMO_feedback)
                        any_error = true;
                    if(feedback_info_f5.MIMO_feedback != mimo_feedback)
                        any_error = true;

                    if(feedback_info_f5.Codebook_index != feedback_info_f5_rev.Codebook_index)
                        any_error = true;
                    if(feedback_info_f5.Codebook_index != codebook_index)
                        any_error = true;
                }
            }
        }
    }

    std::cout << "any_error " << (any_error == true ? "true" : "false") << std::endl;
    std::cout << "Test E End" << std::endl;

    std::cout << "Done!" << std::endl;
    return 0;
}
