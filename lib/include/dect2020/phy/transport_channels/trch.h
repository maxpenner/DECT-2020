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

#ifndef DECT2020_TRCH_H
#define DECT2020_TRCH_H

#include <cstdint>

extern "C"{
#include "srsran/phy/fec/softbuffer.h"
}

#include "dect2020/phy/packet_sizes.h"
#include "dect2020/sections_part3/pcc_enc.h"
#include "dect2020/sections_part3/pdc_enc.h"

namespace dect2020 {

typedef struct{
    uint32_t N_PLCF_bits;
    uint32_t PLCF_type;
    uint32_t N_TB_bits;
    uint32_t N_SS;
    bool closed_loop;
    bool beamforming;
    uint32_t N_bps;
    uint32_t rv;
    uint32_t G;             // G = N_SS * N_PDC_subc * N_bps, see 7.6.6
} trch_meta_t;

typedef struct{

    // TX PLCF
    uint8_t* a;
    uint8_t* d;
    srsran_softbuffer_tx_t* softbuffer_d;

    // TX transport block
    uint8_t* a_tb;
    uint8_t* d_tb;
    srsran_softbuffer_tx_t* softbuffer_d_tb;

    trch_meta_t tx_meta;

    // RX PLCF
    uint8_t* a_rx;
    void* d_rx;
    srsran_softbuffer_rx_t* softbuffer_d_rx;

    // X transport block
    uint8_t* a_tb_rx;
    void* d_tb_rx;
    srsran_softbuffer_rx_t* softbuffer_d_tb_rx;

    trch_meta_t rx_meta;

    // common for TX and RX
    dect2020::pcc_enc_t pcc_enc;
    dect2020::pdc_enc_t pdc_enc;
} trch_t;

int trch_init(trch_t& q, const packet_sizes_t& packet_sizes_maximum, uint32_t network_id);
int trch_free(trch_t& q);

int trch_encode(trch_t& q);
int trch_encode_plcf(trch_t& q);
int trch_encode_tb(trch_t& q);

int trch_decode(trch_t& q);
int trch_decode_plcf(trch_t& q);
int trch_decode_tb(trch_t& q);

}

#endif
