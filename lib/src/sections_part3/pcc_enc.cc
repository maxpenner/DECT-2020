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

extern "C"{
#include "srsran/phy/utils/vector.h"
#include "srsran/phy/fec/turbo/rm_turbo.h"
#include "srsran/phy/scrambling/scrambling.h"
}

#ifdef LV_HAVE_SSE
#include <immintrin.h>
//#pragma message "DECT2020 PLCF using SSE"
#endif /* LV_HAVE_SSE */

#include "dect2020/config.h"
#include "dect2020/sections_part3/pcc_enc.h"
#include "dect2020/utils/debug.h"

#define SRSRAN_PDSCH_MAX_TDEC_ITERS 5

static const uint16_t mask_none         = 0x0000;
static const uint16_t mask_mimo_cl      = 0x5555;
static const uint16_t mask_bf           = 0xAAAA;
static const uint16_t mask_mimo_cl_bf   = 0xFFFF;

// we debug by writing data to files and importing into Matlab
//#define DEBUG_INTO_FILES

namespace dect2020 {

int pcc_enc_init(pcc_enc_t* q, const packet_sizes_t* packet_sizes_maximum){

    int ret = SRSRAN_ERROR;

    // up to 80 user bits + 16 bit crc = 96 bits total
    // packed
    q->c_systematic = srsran_vec_u8_malloc((80 + 16) / 8);
    if (!q->c_systematic) {
      goto clean;
    }
    // after turbo coding we have up to 3*96 + 3*4 = 300 bits, next full byte is 304
    // the other value is 3*(40+16) + 3*4 = 180
    // packed
    q->c_parity = srsran_vec_u8_malloc(304 / 8);
    if (!q->c_parity) {
      goto clean;
    }
    // after ratematching we always have 196 bits (QPSK for 98 PCC subcarriers)
    // however, input can be either int8_t or int16_t, therefore *2
    // data type determined by q->llr_bit_width
    // unpacked
    q->e_rx = (void*) srsran_vec_u8_malloc(196*2);
    if (!q->e_rx) {
      goto clean;
    }
    // up to 80 user bits + 16 bit crc = 96 bits total
    // packed
    q->c_rx = srsran_vec_u8_malloc((80 + 16) / 8);
    if (!q->c_rx) {
      goto clean;
    }

    // crc of plcf is not 24 bits as for LTE, but only 16 bits, see 7.5.2.1
    if (srsran_crc_init(&q->crc_tb, SRSRAN_LTE_CRC16, 16)) {
        ERROR("Error initiating CRC");
        goto clean;
    }
    // second argument is uint32_t max_long_cb, usually 6144
    // maximum here is 80+16=96
    if (srsran_tcod_init(&q->encoder, 80 + 16)) {
      ERROR("Error initiating Turbo Coder");
      goto clean;
    }
    // second argument is uint32_t max_long_cb, usually 6144
    // maximum here is 80+16=96
    if (srsran_tdec_init(&q->decoder, 80 + 16)) {
      ERROR("Error initiating Turbo Decoder");
      goto clean;
    }
    // second argument is length of sequence used for scrambling
    // length is up to 196 bits, next full byte is 200 (necessary if xor'ed with packed d bits)
    // value 0x44454354 from 7.5.4
    if (srsran_sequence_LTE_pr(&q->seq, 200, 0x44454354)) {
      ERROR("Error initiating Scrambling Sequence");
      goto clean;
    }
    //for(uint32_t i=0; i<200; i++)
//        q->seq.c_char[i] *= -1;

    q->max_iterations = SRSRAN_PDSCH_MAX_TDEC_ITERS;

    q->llr_bit_width = DECT2020_LLR_BIT_WIDTH;

    srsran_rm_turbo_gentables();

    ret = SRSRAN_SUCCESS;

clean:
    if (ret == SRSRAN_ERROR) {
        pcc_enc_free(q);
    }
    return ret;
}

int pcc_enc_free(pcc_enc_t* q){
    if (q->c_systematic) {
      free(q->c_systematic);
    }
    if (q->c_parity) {
      free(q->c_parity);
    }
    if (q->e_rx) {
      free(q->e_rx);
    }
    if (q->c_rx) {
      free(q->c_rx);
    }
    srsran_rm_turbo_free_tables();
    srsran_tcod_free(&q->encoder);
    srsran_tdec_free(&q->decoder);
    srsran_sequence_free(&q->seq);
    return SRSRAN_SUCCESS;
}

int pcc_enc_encode(pcc_enc_t* q,
                    const uint8_t* a,                           // input
                    uint8_t* d,                                 // output
                    srsran_softbuffer_tx_t* softbuffer_d,       // output, for later harq processingturbo decoder iterations
                    uint32_t N_PLCF_bits,
                    bool cl,
                    bool bf)
{
#ifdef DEBUG_INTO_FILES
    dect2020::file_binary_representation_msb_first("a.txt", a, N_PLCF_bits/8);
#endif

    uint32_t N_PLCF_byte = N_PLCF_bits/8;

    // save local copy of either 40 or 80 bits in q->c_systematic
    memcpy(q->c_systematic, a, N_PLCF_byte);

    // 7.5.2.4, attach crc
    srsran_crc_set_init(&q->crc_tb, 0);
    srsran_crc_attach_byte(&q->crc_tb, q->c_systematic, N_PLCF_bits);

    // 7.5.2.2 and 7.5.2.3, mask crc for closed loop MIMO or beamforming
    uint16_t mask;
    if(cl == false && bf == false)
        mask = mask_none;
    else if(cl == true && bf == false)
        mask = mask_mimo_cl;
    else if (cl == false && bf == true)
        mask = mask_bf;
    else
        mask = mask_mimo_cl_bf;

    uint16_t *tmp = (uint16_t*) &q->c_systematic[N_PLCF_byte];
    tmp[0] = tmp[0] ^ mask;

#ifdef DEBUG_INTO_FILES
        dect2020::file_binary_representation_msb_first("c.txt", q->c, (N_PLCF_bits + 16)/8);
#endif

    // if 40 bits + 16 crc = 56 bits it must be index 2
    // if 80 bits + 16 crc = 96 bits it must be index 7
    // source: https://github.com/srsran/srsRAN/blob/master/lib/src/phy/fec/cbsegm.c
    // see at 6.1.4.2.3
    uint32_t cblen_idx = (N_PLCF_bits==40) ? 2 : 7;

    int this_number_must_be_300_or_180 = srsran_tcod_encode_lut(&q->encoder,
                                                                &q->crc_tb,     // last argument is false, so crc_tb is ignored internally
                                                                NULL,           // no crc for codeblocks required
                                                                q->c_systematic,
                                                                q->c_parity,
                                                                cblen_idx,
                                                                false);          // we don't want srsran_tcod_encode_lut to attach the crc_tb, we already did, so we pass is as not last cb

#ifdef DEBUG_INTO_FILES
    dect2020::file_binary_representation_msb_first("c_parity.txt", q->c_parity, (this_number_must_be_300_or_180 + 4)/8);
#endif

    if (srsran_rm_turbo_tx_lut(softbuffer_d->buffer_b[0],
                                q->c_systematic,
                                q->c_parity,
                                d,
                                cblen_idx,
                                196,                // PCC is always 196 bits
                                0,                  // no write offset
                                0))                 // rv always 0, see 7.5.3
    {
        ERROR("Error in rate matching");
        return SRSRAN_ERROR;
    }

#ifdef DEBUG_INTO_FILES
    // is always 196, next full byte is 200
    dect2020::file_binary_representation_msb_first("e.txt", d, 200/8);
    dect2020::file_binary_representation_msb_first("seq.txt", q->seq.c_bytes, 200/8);
#endif

    // scrambling
    // we want to scramble 196 bits, next number dividable by 8 is 200
    srsran_scrambling_bytes(&q->seq, d, 200);

#ifdef DEBUG_INTO_FILES
    // is always 196, next full byte is 200
    dect2020::file_binary_representation_msb_first("d.txt", d, 200/8);
#endif

    return SRSRAN_SUCCESS;
}

int pcc_enc_decode(pcc_enc_t* q,
                    uint8_t* a_rx,                              // output
                    const void* d_rx,                           // input
                    srsran_softbuffer_rx_t* softbuffer_d_rx,    // output, for later harq processing
                    uint32_t &N_PLCF_bits,
                    bool &cl,
                    bool &bf)
{
    int ret = SRSRAN_ERROR;
    N_PLCF_bits = 0;
    cl = false;
    bf = false;

    // 7.5.1, blind decode both tb sizes
    for(uint32_t i=0; i<2; i++){

        // we abort when we have found a correct crc
        bool early_stop = false;

        // start with 80, more likely during operation
        uint32_t N_PLCF_bits_blind_test = (i==0) ? 80 : 40;
        uint32_t N_PLCF_byte_blind_test = N_PLCF_bits_blind_test/8;

        // if 40 bits + 16 crc = 56 bits it must be index 2
        // if 80 bits + 16 crc = 96 bits it must be index 7
        // source: https://github.com/srsran/srsRAN/blob/master/lib/src/phy/fec/cbsegm.c
        // see at 6.1.4.2.3
        uint32_t cblen_idx = (N_PLCF_bits_blind_test==40) ? 2 : 7;

        if (q->llr_bit_width == 8) {

#ifdef DEBUG_INTO_FILES
            dect2020::file_binary_representation_unpacked<int8_t>("d_rx.txt", (int8_t*) d_rx, 196, false);
#endif

            // descrambling alternative (does not work properly)
            memcpy(q->e_rx, d_rx, sizeof(int8_t)*196);
            srsran_scrambling_sb_offset(&q->seq, (int8_t*) q->e_rx, 0, 196);

#ifdef DEBUG_INTO_FILES
            dect2020::file_binary_representation_unpacked<int8_t>("seq_rx.txt", (int8_t*) q->seq.c_char, 196, false);
            dect2020::file_binary_representation_unpacked<int8_t>("e_rx.txt", (int8_t*) q->e_rx, 196, false);
#endif

            // rate matching
            srsran_softbuffer_rx_reset(softbuffer_d_rx);
            if (srsran_rm_turbo_rx_lut_8bit((int8_t*) q->e_rx, (int8_t*)softbuffer_d_rx->buffer_f[0], 196, cblen_idx, 0)) {
                ERROR("Error in rate matching");
                return SRSRAN_ERROR;
            }

#ifdef DEBUG_INTO_FILES
            dect2020::file_binary_representation_unpacked<int8_t>("c_parity_rx.txt", (int8_t*)softbuffer_d_rx->buffer_f[0], 196, true);
#endif

            // turbo decoding
            //srsran_tdec_force_not_sb(&q->decoder);
            srsran_tdec_new_cb(&q->decoder, N_PLCF_bits_blind_test + 16);
            // Run iterations and use CRC for early stopping
            uint32_t cb_noi = 0;
            do {

                srsran_tdec_iteration_8bit(&q->decoder, (int8_t*)softbuffer_d_rx->buffer_f[0], q->c_rx);

                cb_noi++;

                // get checksum of decoded data
                uint16_t* c_rx_u16 = (uint16_t*) &q->c_rx[N_PLCF_byte_blind_test];
                uint16_t checksum_rx = c_rx_u16[0];

                // recalculate crc on decoded data
                srsran_crc_set_init(&q->crc_tb, 0);
                srsran_crc_attach_byte(&q->crc_tb, q->c_rx, N_PLCF_bits_blind_test);
                uint16_t checksum_rx_recalc = c_rx_u16[0];

                // check with masks
                if (!(  (checksum_rx ^ mask_none) - checksum_rx_recalc)  ){
                    cl = false;
                    bf = false;
                    softbuffer_d_rx->cb_crc[0] = true;
                    early_stop = true;
                }else if (!(  (checksum_rx ^ mask_mimo_cl) - checksum_rx_recalc)  ){
                    cl = true;
                    bf = false;
                    softbuffer_d_rx->cb_crc[0] = true;
                    early_stop = true;
                }else if (!(  (checksum_rx ^ mask_bf) - checksum_rx_recalc)  ){
                    cl = false;
                    bf = true;
                    softbuffer_d_rx->cb_crc[0] = true;
                    early_stop = true;
                }else if (!(  (checksum_rx ^ mask_mimo_cl_bf) - checksum_rx_recalc)  ){
                    cl = true;
                    bf = true;
                    softbuffer_d_rx->cb_crc[0] = true;
                    early_stop = true;
                }

            } while (cb_noi < q->max_iterations && !early_stop);

        } else if (q->llr_bit_width == 16){

#ifdef DEBUG_INTO_FILES
            dect2020::file_binary_representation_unpacked<int16_t>("d_rx.txt", (int16_t*) d_rx, 196, true);
#endif

            // descrambling
            memcpy(q->e_rx, d_rx, sizeof(int16_t)*196);
            srsran_scrambling_s_offset(&q->seq, (int16_t*) q->e_rx, 0, 196);

#ifdef DEBUG_INTO_FILES
            dect2020::file_binary_representation_msb_first("seq_rx.txt", q->seq.c_bytes, 200/8);
            dect2020::file_binary_representation_unpacked<int16_t>("e_rx.txt", (int16_t*) q->e_rx, 196, true);
#endif

            // rate matching
            srsran_softbuffer_rx_reset(softbuffer_d_rx);
            if (srsran_rm_turbo_rx_lut((int16_t*) q->e_rx, (int16_t*)softbuffer_d_rx->buffer_f[0], 196, cblen_idx, 0)) {
                ERROR("Error in rate matching");
                return SRSRAN_ERROR;
            }

#ifdef DEBUG_INTO_FILES
            dect2020::file_binary_representation_unpacked<int16_t>("c_parity_rx.txt", (int16_t*)softbuffer_d_rx->buffer_f[0], 196, true);
#endif

            // turbo decoding
            //srsran_tdec_force_not_sb(&q->decoder);
            srsran_tdec_new_cb(&q->decoder, N_PLCF_bits_blind_test + 16);
            // Run iterations and use CRC for early stopping
            uint32_t cb_noi = 0;
            do {

                srsran_tdec_iteration(&q->decoder, (int16_t*)softbuffer_d_rx->buffer_f[0], q->c_rx);

                cb_noi++;

                // get checksum of decoded data
                uint16_t* c_rx_u16 = (uint16_t*) &q->c_rx[N_PLCF_byte_blind_test];
                uint16_t checksum_rx = c_rx_u16[0];

                // recalculate crc on decoded data
                srsran_crc_set_init(&q->crc_tb, 0);
                srsran_crc_attach_byte(&q->crc_tb, q->c_rx, N_PLCF_bits_blind_test);
                uint16_t checksum_rx_recalc = c_rx_u16[0];

                // check with masks
                if (!(  (checksum_rx ^ mask_none) - checksum_rx_recalc)  ){
                    cl = false;
                    bf = false;
                    softbuffer_d_rx->cb_crc[0] = true;
                    early_stop = true;
                }else if (!(  (checksum_rx ^ mask_mimo_cl) - checksum_rx_recalc)  ){
                    cl = true;
                    bf = false;
                    softbuffer_d_rx->cb_crc[0] = true;
                    early_stop = true;
                }else if (!(  (checksum_rx ^ mask_bf) - checksum_rx_recalc)  ){
                    cl = false;
                    bf = true;
                    softbuffer_d_rx->cb_crc[0] = true;
                    early_stop = true;
                }else if (!(  (checksum_rx ^ mask_mimo_cl_bf) - checksum_rx_recalc)  ){
                    cl = true;
                    bf = true;
                    softbuffer_d_rx->cb_crc[0] = true;
                    early_stop = true;
                }

            } while (cb_noi < q->max_iterations && !early_stop);
        } else {
            ERROR("Unknown value of llr_bit_width");
        }

        if (early_stop == true){
            N_PLCF_bits = N_PLCF_bits_blind_test;
            ret = SRSRAN_SUCCESS;
            break;
        }
    }

    // save local copy of either 40 or 80 bits in a_rx
    memcpy(a_rx, q->c_rx, N_PLCF_bits/8);

#ifdef DEBUG_INTO_FILES
    dect2020::file_binary_representation_msb_first("a_rx.txt", a_rx, N_PLCF_bits/8);
#endif

    return SRSRAN_SUCCESS;
}

}
