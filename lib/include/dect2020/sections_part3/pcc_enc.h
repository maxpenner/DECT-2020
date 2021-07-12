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

#ifndef DECT2020_PCC_ENC_H
#define DECT2020_PCC_ENC_H

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

    // decoding intermediary steps
    void* e_rx;
    uint8_t* c_rx;

    // sub elements
    srsran_crc_t crc_tb;        // required as input for internal function, but not used
    srsran_tcod_t encoder;
    srsran_tdec_t decoder;
    srsran_sequence_t seq = {}; // scrambling

    // configuration
    uint32_t max_iterations;    // turbo decoder
    uint32_t llr_bit_width;     // width of "void* e_rx"
} pcc_enc_t;

int pcc_enc_init(pcc_enc_t* q, const packet_sizes_t* packet_sizes_maximum);

int pcc_enc_free(pcc_enc_t* q);

int pcc_enc_encode(pcc_enc_t* q,
                    const uint8_t* a,                           // input
                    uint8_t* d,                                 // output
                    srsran_softbuffer_tx_t* softbuffer_d,       // output
                    uint32_t N_PLCF_bits,
                    bool cl,
                    bool bf);

int pcc_enc_decode(pcc_enc_t* q,
                    uint8_t* a_rx,                              // output
                    const void* d_rx,                           // input
                    srsran_softbuffer_rx_t* softbuffer_d_rx,    // output
                    uint32_t &N_PLCF_bits,
                    bool &cl,
                    bool &bf);
}

#endif
