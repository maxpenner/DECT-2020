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

#ifndef DECT2020_PDC_ENC_H
#define DECT2020_PDC_ENC_H

#include <cstdint>

extern "C"{
#include "srsran/phy/common/sequence.h"
#include "srsran/phy/fec/crc.h"
#include "srsran/phy/fec/softbuffer.h"
#include "srsran/phy/fec/turbo/turbocoder.h"
#include "srsran/phy/fec/turbo/turbodecoder.h"
}

#include "dect2020/phy/packet_sizes.h"

namespace dect2020 {

// ETSI TS 103 636-3 V1.1.1 (2020-07), 7.5.1
typedef struct{

    // encoding intermediary steps
    uint8_t* c_systematic;
    uint8_t* c_parity;

    // sub elements
    srsran_crc_t crc_cb;
    srsran_crc_t crc_tb;
    srsran_tcod_t encoder;
    srsran_tdec_t decoder;

    // scrambling
    uint32_t network_id;
    uint32_t g_init_type1;
    uint32_t g_init_type2;
    srsran_sequence_t seq_type1 = {};
    srsran_sequence_t seq_type2 = {};

    // configuration
    uint32_t max_iterations;    // turbo decoder
    uint32_t llr_bit_width;     // width of input
} pdc_enc_t;

int pdc_enc_init(pdc_enc_t* q, const packet_sizes_t* packet_sizes_maximum, uint32_t network_id);

int pdc_enc_free(pdc_enc_t* q);

int pdc_enc_encode(pdc_enc_t* q,
                    uint8_t* a,                                 // input
                    uint8_t* d,                                 // output
                    srsran_softbuffer_tx_t* softbuffer_d,       // output, for later harq processing
                    uint32_t N_TB_bits,
                    uint32_t N_pbs,
                    uint32_t rv,
                    uint32_t G,                                 // G = N_SS * N_PDC_subc * N_bps, see 7.6.6
                    uint32_t plcf_type);

int pdc_enc_decode(pdc_enc_t* q,
                    uint8_t* a,                                 // output
                    void* d,                                    // input, can contain int8_t or int16_t
                    srsran_softbuffer_rx_t* softbuffer_d,       // output, for later harq processing
                    uint32_t N_TB_bits,
                    uint32_t N_pbs,
                    uint32_t rv,
                    uint32_t G,                                 // G = N_SS * N_PDC_subc * N_bps, see 7.6.6
                    uint32_t plcf_type);
}

#endif
