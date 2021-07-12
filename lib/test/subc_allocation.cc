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
#include "dect2020/phy/packet_sizes.h"
#include "dect2020/sections_part3/drs.h"
#include "dect2020/sections_part3/pcc.h"
#include "dect2020/sections_part3/pdc.h"
#include "dect2020/sections_part3/physical_resources.h"
#include "dect2020/sections_part3/radio_device_class.h"
#include "dect2020/sections_part3/stf.h"
#include "dect2020/utils/debug.h"

int main(int argc, char** argv)
{
    // define radio class
    std::string radio_device_class_string("4.2.2.C");
    dect2020::radio_device_class_t radio_device_class;
    if(dect2020::get_radio_device_class(radio_device_class, radio_device_class_string)){
        return -1;
    }
    dect2020::print_radio_device_class(radio_device_class);

    // required for PDC scrambling
    uint32_t network_id = 123456789;

    // precalculate for all cases
    dect2020::k_b_OCC_t k_b_OCC;
    dect2020::get_k_b_OCC(k_b_OCC);
    dect2020::k_b_OCC_vec_t k_b_OCC_vec;
    dect2020::get_k_b_OCC(k_b_OCC_vec);

    // precalculate for all cases
    dect2020::stf_t stf;
    dect2020::get_stf(stf);
    dect2020::stf_vec_t stf_vec;
    dect2020::get_stf(stf_vec);

    // precalculate for all cases
    dect2020::pcc_t pcc;
    dect2020::get_pcc(pcc);

    // some stats
    uint32_t MIN_N_PACKET_symb = 1000;
    uint32_t MAX_N_PACKET_symb = 0;
    uint32_t MIN_N_TS = 1000;
    uint32_t MAX_N_TS = 0;
    uint32_t MIN_N_eff_TX = 1000;
    uint32_t MAX_N_eff_TX = 0;
    uint64_t configuration_possible = 0;
    uint64_t configuration_valid = 0;

    std::cout << "Test A Start" << std::endl;

    for(uint32_t u=1; u<=8; u=u*2){

        std::cout << "u=" << u << std::endl;

        for(uint32_t b_idx=0; b_idx<6; b_idx++){

            uint32_t b = k_b_OCC_vec.b_idx2b[b_idx];

            for(uint32_t t=0; t<=11; t++){
                for(uint32_t p_type=0; p_type<=1; p_type++){

                    uint32_t p_start = 1;

                    // with only 5 symbols many there's a large number of failing configurations
                    if(p_type == 0){
                        p_start = 2;
                    }

                    // if N_eff_TX >= 4 we need at least 15 OFDM symbols or three subslots
                    if(t==5 || t==6 || t==9 || t==10 || t==11){
                        if(p_type == 0){
                            p_start = 3;
                        }

                        // we need at least 15 symbols, for u==1 one slot provides only 10
                        if(u == 1 && p_type == 1){
                            p_start = 2;
                        }
                    }

                    uint32_t p_step = 1;

                    for(uint32_t p=p_start; p<=16; p+=p_step){

                        uint32_t mcs_step = 1;

                        for(uint32_t mcs=0; mcs<=11; mcs+=mcs_step){

                            configuration_possible++;

                            // << std::endldefine a specific transmission
                            dect2020::packet_sizes_required_input_t packet_sizes_required_input;
                            packet_sizes_required_input.u = u;
                            packet_sizes_required_input.b = b;
                            packet_sizes_required_input.PacketLengthType = p_type;
                            packet_sizes_required_input.PacketLength = p;
                            packet_sizes_required_input.tm_mode_index = t;
                            packet_sizes_required_input.mcs_index = mcs;
                            packet_sizes_required_input.Z = 6144;

                            // calculate sizes of this transmission
                            dect2020::packet_sizes_t packet_sizes;
                            if(dect2020::get_packet_sizes_t(packet_sizes, packet_sizes_required_input)){
                                continue;
                            }

                            configuration_valid++;

                            // get relevant parameters
                            uint32_t N_PACKET_symb = packet_sizes.N_PACKET_symb;
                            uint32_t N_TS = packet_sizes.tm_mode.N_TS;
                            uint32_t N_eff_TX = packet_sizes.tm_mode.N_eff_TX;

                            MIN_N_PACKET_symb   = (N_PACKET_symb < MIN_N_PACKET_symb) ? N_PACKET_symb : MIN_N_PACKET_symb;
                            MAX_N_PACKET_symb   = (N_PACKET_symb > MAX_N_PACKET_symb) ? N_PACKET_symb : MAX_N_PACKET_symb;
                            MIN_N_TS            = (N_TS < MIN_N_TS) ? N_TS : MIN_N_TS;
                            MAX_N_TS            = (N_TS > MAX_N_TS) ? N_TS : MAX_N_TS;
                            MIN_N_eff_TX        = (N_eff_TX < MIN_N_eff_TX) ? N_eff_TX : MIN_N_eff_TX;
                            MAX_N_eff_TX        = (N_eff_TX > MAX_N_eff_TX) ? N_eff_TX : MAX_N_eff_TX;

                            dect2020::drs_t drs;
                            if(dect2020::get_drs(drs, k_b_OCC_vec, b, N_PACKET_symb, N_TS, N_eff_TX)){
                                continue;
                            }

                            uint32_t u = packet_sizes_required_input.u;

                            dect2020::pdc_t pdc;
                            dect2020::get_pdc(pdc, k_b_OCC_vec, pcc, drs, u, b, N_PACKET_symb, N_TS);
                        }//mcs
                    }//p
                }//p_type
            }//t
        }//b
    }//u

    std::cout << "MIN_N_PACKET_symb= " << MIN_N_PACKET_symb << std::endl;
    std::cout << "MAX_N_PACKET_symb= " << MAX_N_PACKET_symb << std::endl;
    std::cout << "MIN_N_TS=          " << MIN_N_TS << std::endl;
    std::cout << "MAX_N_TS=          " << MAX_N_TS << std::endl;
    std::cout << "MIN_N_eff_TX=      " << MIN_N_eff_TX << std::endl;
    std::cout << "MAX_N_eff_TX=      " << MAX_N_eff_TX << std::endl;
    std::cout << "configuration_possible=  " << configuration_possible << std::endl;
    std::cout << "configuration_valid=     " << configuration_valid << std::endl;
    std::cout << "Test A End" << std::endl;

    std::cout << "Done!" << std::endl;
    return 0;
}
