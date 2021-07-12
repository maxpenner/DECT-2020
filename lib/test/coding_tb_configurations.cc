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
#include <algorithm>
#include <sys/time.h>

#include "srsran/srsran.h"

#include "dect2020/config.h"
#include "dect2020/phy/packet_sizes.h"
#include "dect2020/phy/transport_channels/trch.h"
#include "dect2020/sections_part3/physical_resources.h"
#include "dect2020/sections_part3/radio_device_class.h"
#include "dect2020/sections_part3/tm_mode.h"
#include "dect2020/utils/debug.h"
#include "dect2020/utils/math_functions.h"

#define N_REPETITIONS   10

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

    // to allocate memory we need to know maximum packet sizes in advance
    dect2020::packet_sizes_t packet_sizes_maximum;
    if(dect2020::get_maximum_packet_sizes_t(packet_sizes_maximum, radio_device_class_string, DECT2020_PACKETLENGTHTYPE_MAX, DECT2020_PACKETLENGTH_MAX)){
        return -1;
    }
    dect2020::print_packet_sizes_t(packet_sizes_maximum);

    // allocate largest conceivable TX buffer
    uint8_t* a = srsran_vec_u8_malloc(packet_sizes_maximum.N_TB_byte);
    uint8_t* d = srsran_vec_u8_malloc(dect2020::ceil_divide_integer<uint32_t>( packet_sizes_maximum.N_PDC_subc_bits,8));
    // provided by MAC
    srsran_softbuffer_tx_t softbuffer_d;
    if(srsran_softbuffer_tx_init_guru(&softbuffer_d, packet_sizes_maximum.N_code_blocks, DECT2020_SOFTBUFFER_SIZE) != SRSRAN_SUCCESS){
        ERROR("Softbuffer not initialized.");
    }

    // allocate largest conceivable RX buffer
    uint8_t* a_rx = srsran_vec_u8_malloc(packet_sizes_maximum.N_TB_byte);
    void* d_rx = srsran_vec_u8_malloc(packet_sizes_maximum.N_PDC_subc_bits*2);   // *2 as it can be int8_t and int16_t
    // provided by MAC
    srsran_softbuffer_rx_t softbuffer_d_rx;
    if(srsran_softbuffer_rx_init_guru(&softbuffer_d_rx, packet_sizes_maximum.N_code_blocks, DECT2020_SOFTBUFFER_SIZE) != SRSRAN_SUCCESS){
        ERROR("Softbuffer not initialized.");
    }

    // init transport layer
    dect2020::trch_t trch;
    if(dect2020::trch_init(trch, packet_sizes_maximum, network_id)){
        return -1;
    }

    // set pointers of transport channel
    trch.a_tb = a;
    trch.d_tb = d;
    trch.softbuffer_d_tb = &softbuffer_d;
    trch.a_tb_rx = a_rx;
    trch.d_tb_rx = d_rx;
    trch.softbuffer_d_tb_rx = &softbuffer_d_rx;

    std::cout << "Test A Start" << std::endl << std::endl;

    // required for testing
    uint8_t* a_unpacked = srsran_vec_u8_malloc(packet_sizes_maximum.N_TB_bits);
    uint8_t* a_rx_unpacked = srsran_vec_u8_malloc(packet_sizes_maximum.N_TB_bits);
    uint8_t* d_unpacked = srsran_vec_u8_malloc(packet_sizes_maximum.N_PDC_subc_bits);

    dect2020::k_b_OCC_vec_t k_b_OCC_vec;
    dect2020::get_k_b_OCC(k_b_OCC_vec);

    // time measurement
    struct timeval tdata[3];
    gettimeofday(&tdata[1], NULL);

    // stats
    uint32_t N_TB_bits_min = 1000000;
    uint32_t N_TB_bits_max = 0;

    bool any_error = false;

    for(uint32_t u=1; u<=radio_device_class.u_min; u=u*2){

        uint32_t b_idx_max = k_b_OCC_vec.b2b_idx[radio_device_class.b_min];

        for(uint32_t b_idx=0; b_idx<b_idx_max; b_idx++){

            uint32_t b = k_b_OCC_vec.b_idx2b[b_idx];

            uint32_t t_max = dect2020::get_max_tm_mode_index_depending_on_N_ant(radio_device_class.N_ant_min);

            for(uint32_t t=0; t<=t_max; t++){

                for(uint32_t p_type=0; p_type<=DECT2020_PACKETLENGTHTYPE_MAX; p_type++){

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

                    // we can't have more than 16 slots or subslots
                    uint32_t p_end = std::min(16, DECT2020_PACKETLENGTH_MAX);

                    for(uint32_t p=p_start; p<=p_end; p+=p_step){
                        for(uint32_t mcs=0; mcs<=radio_device_class.mcs_index_min; mcs++){
                            for(uint32_t iter=0; iter<N_REPETITIONS; iter++){

                                // define a transmission
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
                                if(dect2020::get_packet_sizes_t(packet_sizes, packet_sizes_required_input))
                                    continue;

                                uint32_t N_TB_bits = packet_sizes.N_TB_bits;
                                uint32_t N_TB_byte = packet_sizes.N_TB_byte;
                                uint32_t N_bps = packet_sizes.mcs.N_bps;
                                uint32_t G = packet_sizes.N_PDC_subc_bits;

                                N_TB_bits_min = (N_TB_bits < N_TB_bits_min) ? N_TB_bits : N_TB_bits_min;
                                N_TB_bits_max = (N_TB_bits > N_TB_bits_min) ? N_TB_bits : N_TB_bits_min;

                                // write random data
                                for(uint32_t i=0; i<N_TB_byte; i++)
                                    a[i] = rand() % 127;

                                // must be done for each new transmission
                                srsran_softbuffer_tx_reset(&softbuffer_d);
                                srsran_softbuffer_rx_reset(&softbuffer_d_rx);

                                // set TX and RX meta parameters for this transmission
                                trch.tx_meta.N_TB_bits = N_TB_bits;
                                trch.tx_meta.N_bps = N_bps;
                                trch.tx_meta.rv = 0;
                                trch.tx_meta.G = G;

                                // assume same data is known at receiver, usually extracted from PLCF
                                trch.rx_meta.N_TB_bits = trch.tx_meta.N_TB_bits;
                                trch.rx_meta.N_bps = trch.tx_meta.N_bps;
                                trch.rx_meta.rv = trch.tx_meta.rv;
                                trch.rx_meta.G = trch.tx_meta.G;

                                // encode data
                                dect2020::trch_encode_tb(trch);

                                // transfer data from d to d_rx
                                srsran_bit_unpack_vector(d, d_unpacked, trch.tx_meta.G);
                                DECT2020_D_RX_DATA_TYPE *tmp = (DECT2020_D_RX_DATA_TYPE*) d_rx;
                                for(uint32_t i=0; i<trch.tx_meta.G; i++)
                                    tmp[i] = (d_unpacked[i] > 0) ? 10 : -10;

                                // decode data
                                dect2020::trch_decode_tb(trch);

                                // compare data
                                srsran_bit_unpack_vector(a, a_unpacked, trch.tx_meta.N_TB_bits);
                                srsran_bit_unpack_vector(a_rx, a_rx_unpacked, trch.tx_meta.N_TB_bits);
                                for(uint32_t a_idx=0; a_idx<N_TB_bits; a_idx++){
                                    if(a_unpacked[a_idx] != a_rx_unpacked[a_idx]){
                                        any_error = true;
                                        break;
                                    }
                                }

                            }//iter
                        }//mcs
                    }//p
                }//p_type
            }//t
        }//b
    }//u

    // measure time
    gettimeofday(&tdata[2], NULL);
    get_time_interval(tdata);
    float total_time, mean_usec;
    total_time = (tdata[0].tv_sec * 1e6 + tdata[0].tv_usec);
    mean_usec = total_time / N_REPETITIONS;
    std::cout << "PLCF encoding + copying + decoding (1 iteration) + comparison results:" << std::endl;
    std::cout << "Total time is " << total_time/1e6 << " s for " << N_REPETITIONS << " repetitions"<< std::endl;
    std::cout << "N_TB_bits_min " << N_TB_bits_min << std::endl;
    std::cout << "N_TB_bits_max " << N_TB_bits_max << std::endl;
    std::cout << "any_error " << (any_error == true ? "true" : "false") << std::endl;
    std::cout << "Test A End" << std::endl;

    // clean up
    
    dect2020::trch_free(trch);

    free(a);
    free(d);
    srsran_softbuffer_tx_free(&softbuffer_d);

    free(a_rx);
    free(d_rx);
    srsran_softbuffer_rx_free(&softbuffer_d_rx);

    free(a_unpacked);
    free(a_rx_unpacked);
    free(d_unpacked);

    std::cout << "Done!" << std::endl;
    return 0;
}
