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

#include "srsran/srsran.h"
#include "dect2020/config.h"
#include "dect2020/phy/transport_channels/trch.h"
#include "dect2020/sections_part3/radio_device_class.h"
#include "dect2020/sections_part3/tm_mode.h"
#include "dect2020/utils/debug.h"

#define N_REPETITIONS   10000

int main(int argc, char** argv)
{
    // define our radio device
    std::string radio_device_class_string("4.2.2.C");
    dect2020::radio_device_class_t radio_device_class;
    if(dect2020::get_radio_device_class(radio_device_class, radio_device_class_string)){
        return -1;
    }
    dect2020::print_radio_device_class(radio_device_class);

    // required for PDC scrambling
    uint32_t network_id = 123456789;

    // to allocate memory we need maximum packet sizes
    dect2020::packet_sizes_t packet_sizes_maximum;
    if(dect2020::get_maximum_packet_sizes_t(packet_sizes_maximum, radio_device_class_string, DECT2020_PACKETLENGTHTYPE_MAX, DECT2020_PACKETLENGTH_MAX)){
        return -1;
    }
    dect2020::print_packet_sizes_t(packet_sizes_maximum);

    // allocate TX buffer
    uint8_t* a = srsran_vec_u8_malloc(80/8);        // 80 bits max, packed
    uint8_t* d = srsran_vec_u8_malloc((196+4)/8);   // is always 196 bis, 200 is next number dividable by 8, packed
    // TODO: the size DECT2020_SOFTBUFFER_SIZE is too large for PLCF
    srsran_softbuffer_tx_t softbuffer_d;
    if(srsran_softbuffer_tx_init_guru(&softbuffer_d, 1, DECT2020_SOFTBUFFER_SIZE) != SRSRAN_SUCCESS){
        ERROR("Softbuffer not initialized.");
    }

    // allocate RX buffer
    uint8_t* a_rx = srsran_vec_u8_malloc(80/8);     // 80 bits max, packed
    void* d_rx = srsran_vec_u8_malloc(196*2);       // *2 as it can be int8_t and int16_t, unpacked
    // TODO: the size DECT2020_SOFTBUFFER_SIZE is too large for PLCF
    srsran_softbuffer_rx_t softbuffer_d_rx;
    if(srsran_softbuffer_rx_init_guru(&softbuffer_d_rx, 1, DECT2020_SOFTBUFFER_SIZE) != SRSRAN_SUCCESS){
        ERROR("Softbuffer not initialized.");
    }

    // init transport layer
    dect2020::trch_t trch;
    dect2020::trch_init(trch, packet_sizes_maximum, network_id);

    // set pointers
    trch.a = a;
    trch.d = d;
    trch.softbuffer_d = &softbuffer_d;
    trch.a_rx = a_rx;
    trch.d_rx = d_rx;
    trch.softbuffer_d_rx = &softbuffer_d_rx;

    // test all combinations of N_PLCF_bits/cl/bf
    std::cout << "Test A Start" << std::endl;

    // local test variables
    bool any_error = false;
    uint8_t* a_unpacked = srsran_vec_u8_malloc(80);
    uint8_t* a_rx_unpacked = srsran_vec_u8_malloc(80);
    uint8_t* d_unpacked = srsran_vec_u8_malloc(196);

    for(int iter=0; iter<8; iter++){

        // write random data
        for(uint32_t i=0; i<80/8; i++)
            a[i] = rand() % 127;

        // reset softbuffer
        srsran_softbuffer_tx_reset(&softbuffer_d);
        srsran_softbuffer_rx_reset(&softbuffer_d_rx);

        // set TX parameters for this transmission
        if(iter==0){
            trch.tx_meta.N_PLCF_bits = 40;
            trch.tx_meta.closed_loop = false;
            trch.tx_meta.beamforming = false;
        }
        else if (iter==1){
            trch.tx_meta.N_PLCF_bits = 40;
            trch.tx_meta.closed_loop = true;
            trch.tx_meta.beamforming = false;
        }
        else if (iter==2){
            trch.tx_meta.N_PLCF_bits = 40;
            trch.tx_meta.closed_loop = false;
            trch.tx_meta.beamforming = true;
        }
        else if (iter==3){
            trch.tx_meta.N_PLCF_bits = 40;
            trch.tx_meta.closed_loop = true;
            trch.tx_meta.beamforming = true;
        }
        else if (iter==4){
            trch.tx_meta.N_PLCF_bits = 80;
            trch.tx_meta.closed_loop = true;
            trch.tx_meta.beamforming = false;
        }
        else if (iter==5){
            trch.tx_meta.N_PLCF_bits = 80;
            trch.tx_meta.closed_loop = true;
            trch.tx_meta.beamforming = false;
        }
        else if (iter==6){
            trch.tx_meta.N_PLCF_bits = 80;
            trch.tx_meta.closed_loop = false;
            trch.tx_meta.beamforming = true;
        }
        else if (iter==7){
            trch.tx_meta.N_PLCF_bits = 80;
            trch.tx_meta.closed_loop = true;
            trch.tx_meta.beamforming = true;
        }

        // encode data
        dect2020::trch_encode_plcf(trch);

        // transfer data from d to d_rx
        srsran_bit_unpack_vector(d, d_unpacked, 196);
        DECT2020_D_RX_DATA_TYPE *tmp = (DECT2020_D_RX_DATA_TYPE*) d_rx;
        for(uint32_t i=0; i<196; i++)
            tmp[i] = (d_unpacked[i] > 0) ? 127 : -127;

        // decode data
        dect2020::trch_decode_plcf(trch);

        // compare data
        srsran_bit_unpack_vector(a, a_unpacked, trch.tx_meta.N_PLCF_bits);
        srsran_bit_unpack_vector(a_rx, a_rx_unpacked, trch.tx_meta.N_PLCF_bits);
        for(uint32_t a_idx=0; a_idx<trch.tx_meta.N_PLCF_bits; a_idx++){
            if(a_unpacked[a_idx] != a_rx_unpacked[a_idx]){
                any_error = true;
                break;
            }
        }

        // compare meta data
        if( trch.tx_meta.N_PLCF_bits != trch.rx_meta.N_PLCF_bits ||
            trch.tx_meta.closed_loop != trch.rx_meta.closed_loop ||
            trch.tx_meta.beamforming != trch.rx_meta.beamforming){
                any_error = true;
        }
    }

    std::cout << (any_error == true ? "PHY PLCF Coding Test failed" : "PHY PLCF Coding Test passed") << std::endl;
    std::cout << "Test A End" << std::endl;

    // TEST B: speed
    std::cout << std::endl << "Test B Start" << std::endl;

    any_error = false;

    struct timeval  tdata[3];
    gettimeofday(&tdata[1], NULL);

    for(uint32_t iter=0; iter<N_REPETITIONS; iter++){

        // write random data
        for(uint32_t i=0; i<80/8; i++){
            a[i] = (i+iter)%127;
        }

        // reset softbuffer
        srsran_softbuffer_tx_reset(&softbuffer_d);
        srsran_softbuffer_rx_reset(&softbuffer_d_rx);

        // set TX parameters for this transmission
        trch.tx_meta.N_PLCF_bits = 80;
        trch.tx_meta.closed_loop = false;
        trch.tx_meta.beamforming = false;
        trch.a = a;
        trch.d = d;
        trch.softbuffer_d = &softbuffer_d;
        trch.a_rx = a_rx;
        trch.d_rx = d_rx;
        trch.softbuffer_d_rx = &softbuffer_d_rx;

        // encode data
        dect2020::trch_encode_plcf(trch);

        // transfer from d into d_rx
        srsran_bit_unpack_vector(d, d_unpacked, 196);
        DECT2020_D_RX_DATA_TYPE *tmp = (DECT2020_D_RX_DATA_TYPE*) d_rx;
        for(int i=0; i<196; i++)
            tmp[i] = (d_unpacked[i] > 0) ? 127 : -127;

        // decode data
        dect2020::trch_decode_plcf(trch);

        // compare data
        srsran_bit_unpack_vector(a, a_unpacked, trch.tx_meta.N_PLCF_bits);
        srsran_bit_unpack_vector(a_rx, a_rx_unpacked, trch.tx_meta.N_PLCF_bits);
        for(uint32_t a_idx=0; a_idx<trch.tx_meta.N_PLCF_bits; a_idx++){
            if(a_unpacked[a_idx] != a_rx_unpacked[a_idx]){
                any_error = true;
                break;
            }
        }

        // compare meta data
        if( trch.tx_meta.N_PLCF_bits != trch.rx_meta.N_PLCF_bits ||
            trch.tx_meta.closed_loop != trch.rx_meta.closed_loop ||
            trch.tx_meta.beamforming != trch.rx_meta.beamforming){
                any_error = true;
        }
    }

    // measure time
    gettimeofday(&tdata[2], NULL);
    get_time_interval(tdata);
    float total_time, mean_usec;
    total_time = (tdata[0].tv_sec * 1e6 + tdata[0].tv_usec);
    mean_usec = total_time / N_REPETITIONS;
    std::cout << "PLCF encoding + decoding:" << std::endl;
    std::cout << "Total time is " << total_time/1e6 << " s for " << N_REPETITIONS << " repetitions"<< std::endl;
    std::cout << "It took on average " << mean_usec << " us" << std::endl;
    std::cout << "any_error " << (any_error == true ? "true" : "false") << std::endl;
    std::cout << "Test B End" << std::endl;

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
