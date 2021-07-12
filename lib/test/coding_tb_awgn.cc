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

#include "srsran/srsran.h"

#include "dect2020/config.h"
#include "dect2020/phy/packet_sizes.h"
#include "dect2020/phy/transport_channels/trch.h"
#include "dect2020/sections_part3/radio_device_class.h"
#include "dect2020/utils/debug.h"
#include "dect2020/utils/math_functions.h"

#define MCS_MIN         0
#define MCS_MAX         10

#define SNR_DB_MIN      -5.0
#define SNR_DB_MAX      40.0
#define SNR_DB_STEP     0.5

#define N_HARQ_RETX     0

#define N_REPETITIONS   1000

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
    uint8_t* d = srsran_vec_u8_malloc(dect2020::ceil_divide_integer<uint32_t>(packet_sizes_maximum.N_PDC_subc_bits,8));
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

    cf_t* symbols = srsran_vec_cf_malloc(packet_sizes_maximum.N_PDC_subc);
    cf_t* symbols_plus_noise = srsran_vec_cf_malloc(packet_sizes_maximum.N_PDC_subc);

    // channel
    srsran_channel_awgn_t srsran_channel_awgn;
    srsran_channel_awgn_init(&srsran_channel_awgn, 123);

    // time measurement
    struct timeval tdata[3];
    gettimeofday(&tdata[1], NULL);

    // stats
    uint32_t N_TB_bits_min = 1000000;
    uint32_t N_TB_bits_max = 0;

    bool any_error = false;

    // read into matlab for plotting
    std::ofstream f("output.txt", std::ios::trunc);
    f << "MCS,SNR_dB,SNR_dB_measured,BER_uncoded,packet_error,PER,N_TB_bits,N_PDC_subc,G" << std::endl;

    for(uint32_t mcs=MCS_MIN; mcs<=std::min(radio_device_class.mcs_index_min, (uint32_t) MCS_MAX); mcs++){

        // define a transmission
        dect2020::packet_sizes_required_input_t packet_sizes_required_input;
        packet_sizes_required_input.u = 1;
        packet_sizes_required_input.b = 1;
        packet_sizes_required_input.PacketLengthType = 1;
        packet_sizes_required_input.PacketLength = 1;
        packet_sizes_required_input.tm_mode_index = 0;
        packet_sizes_required_input.mcs_index = mcs;
        packet_sizes_required_input.Z = 6144;

        // calculate sizes of this transmission
        dect2020::packet_sizes_t packet_sizes;
        if(dect2020::get_packet_sizes_t(packet_sizes, packet_sizes_required_input)){
            ERROR("Simulation failed");
            return -1;
        }

        uint32_t N_TB_bits = packet_sizes.N_TB_bits;
        uint32_t N_TB_byte = packet_sizes.N_TB_byte;
        uint32_t N_PDC_subc = packet_sizes.N_PDC_subc;
        uint32_t N_bps = packet_sizes.mcs.N_bps;
        uint32_t G = packet_sizes.N_PDC_subc_bits;

        N_TB_bits_min = (N_TB_bits < N_TB_bits_min) ? N_TB_bits : N_TB_bits_min;
        N_TB_bits_max = (N_TB_bits > N_TB_bits_min) ? N_TB_bits : N_TB_bits_min;

        // set TX and RX meta parameters for this transmission
        trch.tx_meta.N_TB_bits = N_TB_bits;
        trch.tx_meta.N_bps = N_bps;
        trch.tx_meta.G = G;

        // assume same data is known at receiver, usually extracted from PLCF
        trch.rx_meta.N_TB_bits = trch.tx_meta.N_TB_bits;
        trch.rx_meta.N_bps = trch.tx_meta.N_bps;
        trch.rx_meta.G = trch.tx_meta.G;

        // convert mcs to srsRAN terminology
        srsran_mod_t srsran_mod;
        if(mcs==0){
            srsran_mod = SRSRAN_MOD_BPSK;
        }
        else if(mcs==1 || mcs==2){
            srsran_mod = SRSRAN_MOD_QPSK;
        }
        else if(mcs==3 || mcs==4){
            srsran_mod = SRSRAN_MOD_16QAM;
        }
        else if(mcs==5 || mcs==6 || mcs==7){
            srsran_mod = SRSRAN_MOD_64QAM;
        }
        else if(mcs==8 || mcs==9){
            srsran_mod = SRSRAN_MOD_256QAM;
        }
        else{
            return -1;
        }

        // init modulation table
        srsran_modem_table_t srsran_modem_table;
        srsran_modem_table_lte(&srsran_modem_table, srsran_mod);
        srsran_modem_table_bytes(&srsran_modem_table);

        float SNR_mcs_min = SNR_DB_MIN + 3.0 * (float) mcs;

        for(float SNR_dB=SNR_mcs_min; SNR_dB<=SNR_DB_MAX; SNR_dB=SNR_dB+SNR_DB_STEP){

            uint64_t uncoded_bit_error = 0;
            uint64_t packet_transmissions = 0;
            uint64_t packet_error = 0;

            double power_signal = 0.0;
            double power_signal_plus_noise = 0.0;

            for(uint32_t iter=0; iter<N_REPETITIONS; iter++){

                // write random data
                for(uint32_t i=0; i<N_TB_byte; i++)
                    a[i] = rand() % 127;
                srsran_bit_unpack_vector(a, a_unpacked, trch.tx_meta.N_TB_bits);

                // must be done for each new transmission
                srsran_softbuffer_tx_reset(&softbuffer_d);
                srsran_softbuffer_rx_reset(&softbuffer_d_rx);

                bool packet_correct = false;

                // send retransmission until packet correct or N_HARQ_RETX reached
                for(uint32_t transmission=0; transmission<(1+N_HARQ_RETX); transmission++){

                    packet_transmissions++;

                    switch(transmission){
                        case 0:
                            trch.tx_meta.rv = 0;
                            break;
                        case 1:
                            trch.tx_meta.rv = 2;
                            break;
                        case 2:
                            trch.tx_meta.rv = 3;
                            break;
                        case 3:
                            trch.tx_meta.rv = 1;
                            break;
                    }
                    trch.rx_meta.rv = trch.tx_meta.rv;

                    // encode data
                    dect2020::trch_encode_tb(trch);

                    // modulate
                    srsran_bit_unpack_vector(d, d_unpacked, trch.tx_meta.G);
                    srsran_mod_modulate(&srsran_modem_table, d_unpacked, symbols, trch.tx_meta.G);

                    // add noise
                    float n0_dBfs = (0 - SNR_dB)*1.0;
                    srsran_channel_awgn_set_n0(&srsran_channel_awgn, n0_dBfs);
                    srsran_channel_awgn_run_c(&srsran_channel_awgn, symbols, symbols_plus_noise, N_PDC_subc);

                    // measure SNR
                    float* ri_symbols = (float*) symbols;
                    float* ri_symbols_plus_noise = (float*) symbols_plus_noise;
                    for(uint32_t s_idx=0; s_idx<N_PDC_subc; s_idx++){
                        power_signal            += pow(ri_symbols[2*s_idx],2.0)            + pow(ri_symbols[2*s_idx+1],2.0);
                        power_signal_plus_noise += pow(ri_symbols_plus_noise[2*s_idx],2.0) + pow(ri_symbols_plus_noise[2*s_idx+1],2.0);
                    }

                    // demodulate
#if DECT2020_LLR_BIT_WIDTH == DECT2020_LLR_BIT_WIDTH_08
                    srsran_demod_soft_demodulate(srsran_mod_t modulation, const cf_t* symbols, float* llr, int nsymbols);
#elif DECT2020_LLR_BIT_WIDTH == DECT2020_LLR_BIT_WIDTH_16
                    srsran_demod_soft_demodulate_s(srsran_mod, symbols_plus_noise, (short*) d_rx, N_PDC_subc);
#endif

                    // count uncoded bit errors
                    DECT2020_D_RX_DATA_TYPE *tmp = (DECT2020_D_RX_DATA_TYPE*) d_rx;
                    for(uint32_t i=0; i<G; i++){
                        int signA = (d_unpacked[i] > 0) ? 1 : -1;
                        int signB = (tmp[i] > 0) ? 1 : -1;
                        if(signA != signB)
                            uncoded_bit_error++;
                    }

                    // decode data
                    dect2020::trch_decode_tb(trch);

                    // compare data
                    uint64_t coded_bit_error = 0;
                    srsran_bit_unpack_vector(a_rx, a_rx_unpacked, trch.tx_meta.N_TB_bits);
                    for(uint32_t a_idx=0; a_idx<N_TB_bits; a_idx++){
                        if(a_unpacked[a_idx] != a_rx_unpacked[a_idx]){
                            any_error = true;
                            coded_bit_error++;
                        }
                    }

                    // abort harq transmission prematurely
                    if(coded_bit_error == 0){
                        packet_correct = true;
                        break;
                    }

                }// harq

                if(packet_correct == false)
                    packet_error++;

            }//iter

            double SNR_dB_measured  = 10.0*log10(power_signal / (power_signal_plus_noise - power_signal));
            double BER_uncoded      = (double) uncoded_bit_error / (double) (G*packet_transmissions);
            double PER              = (double) packet_error      / (double) (N_REPETITIONS);

            std::cout << std::fixed << std::setprecision(8) << std::setfill(' ');
            std::cout << "MCS="                << std::setw(2) << mcs;
            std::cout << " SNR_dB="             << SNR_dB;
            std::cout << " SNR_dB_measured="    << SNR_dB_measured;
            std::cout << " BER_uncoded="        << BER_uncoded;
            std::cout << " packet_error="       << std::setw(7) << packet_error;
            std::cout << " PER="                << PER;
            std::cout << " N_TB_bits="          << N_TB_bits;
            std::cout << " N_PDC_subc="         << N_PDC_subc;
            std::cout << " G="                  << G;
            std::cout << std::endl;

            f << std::fixed << std::setprecision(8);
            f << mcs             << ",";
            f << SNR_dB          << ",";
            f << SNR_dB_measured << ",";
            f << BER_uncoded     << ",";
            f << packet_error    << ",";
            f << PER             << ",";
            f << N_TB_bits       << ",";
            f << N_PDC_subc      << ",";
            f << G               << std::endl;

            // abort prematurely
            if(packet_error == 0)
                    break;
        }//SNR

        srsran_modem_table_free(&srsran_modem_table);
        std::cout << std::endl;
    }//mcs

    f.close();

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

    srsran_channel_awgn_free(&srsran_channel_awgn);

    free(a);
    free(d);
    srsran_softbuffer_tx_free(&softbuffer_d);

    free(a_rx);
    free(d_rx);
    srsran_softbuffer_rx_free(&softbuffer_d_rx);

    free(symbols);
    free(symbols_plus_noise);

    free(a_unpacked);
    free(a_rx_unpacked);
    free(d_unpacked);

    std::cout << "Done!" << std::endl;
    return 0;
}
