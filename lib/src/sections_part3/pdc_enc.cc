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
//#pragma message "DECT2020 TB using SSE"
#endif /* LV_HAVE_SSE */

#include "dect2020/config.h"
#include "dect2020/sections_part3/pdc_enc.h"
#include "dect2020/utils/debug.h"

#define SRSRAN_PDSCH_MAX_TDEC_ITERS 5

// we debug by writing data to files and importing into Matlab
//#define DEBUG_INTO_FILES

namespace dect2020 {

int pdc_enc_init(pdc_enc_t* q, const packet_sizes_t* packet_sizes_maximum, uint32_t network_id){

    int ret = SRSRAN_ERROR;

    // we have up to Z = 2048 or 6144 bits per cb + 4 bit tail
    // packed
    q->c_systematic = srsran_vec_u8_malloc((packet_sizes_maximum->Z + 8)/ 8);
    if (!q->c_systematic) {
      goto clean;
    }
    // after turbo coding we have up to 3*6144 + 3*4 = 18444 bits
    // packed
    q->c_parity = srsran_vec_u8_malloc((3 * packet_sizes_maximum->Z + 16) / 8);
    if (!q->c_parity) {
      goto clean;
    }

    // 6.1.2 and 7.6.2
    if (srsran_crc_init(&q->crc_cb, SRSRAN_LTE_CRC24B, 24)) {
        ERROR("Error initiating CRC");
        goto clean;
    }
    // 6.1.2 and 7.6.2
    if (srsran_crc_init(&q->crc_tb, SRSRAN_LTE_CRC24A, 24)) {
        ERROR("Error initiating CRC");
        goto clean;
    }
    // second argument is uint32_t max_long_cb, usually 6144 for LTE
    if (srsran_tcod_init(&q->encoder, packet_sizes_maximum->Z)) {
      ERROR("Error initiating Turbo Coder");
      goto clean;
    }
    // second argument is uint32_t max_long_cb, usually 6144 for LTE
    if (srsran_tdec_init(&q->decoder, packet_sizes_maximum->Z)) {
      ERROR("Error initiating Turbo Decoder");
      goto clean;
    }

    q->network_id = network_id;
    q->g_init_type1 = network_id & 0x000000FF;
    q->g_init_type2 = (network_id >> 8) & 0x00FFFFFF;
    if (srsran_sequence_LTE_pr(&q->seq_type1, packet_sizes_maximum->N_PDC_subc_bits, q->g_init_type1)) {
      ERROR("Error initiating Scrambling Sequence");
      goto clean;
    }
    if (srsran_sequence_LTE_pr(&q->seq_type2, packet_sizes_maximum->N_PDC_subc_bits, q->g_init_type2)) {
      ERROR("Error initiating Scrambling Sequence");
      goto clean;
    }

    q->max_iterations = SRSRAN_PDSCH_MAX_TDEC_ITERS;

    q->llr_bit_width = DECT2020_LLR_BIT_WIDTH;

    srsran_rm_turbo_gentables();

    srsran_tdec_force_not_sb(&q->decoder);

    ret = SRSRAN_SUCCESS;

clean:
    if (ret == SRSRAN_ERROR) {
        pdc_enc_free(q);
    }
    return ret;
}

int pdc_enc_free(pdc_enc_t* q){
    if (q->c_systematic) {
      free(q->c_systematic);
    }
    if (q->c_parity) {
      free(q->c_parity);
    }
    srsran_rm_turbo_free_tables();
    srsran_tcod_free(&q->encoder);
    srsran_tdec_free(&q->decoder);
    srsran_sequence_free(&q->seq_type1);
    srsran_sequence_free(&q->seq_type2);
    return SRSRAN_SUCCESS;
}

// following code with adjustments from
// source: https://github.com/srsran/srsRAN/blob/master/lib/src/phy/phch/sch.c
static int encode_tb_off(pdc_enc_t*                   q,
                         srsran_softbuffer_tx_t* softbuffer,
                         srsran_cbsegm_t*        cb_segm,
                         uint32_t                Qm,
                         uint32_t                rv,
                         uint32_t                nof_e_bits,
                         uint8_t*                data,
                         uint8_t*                e_bits,
                         uint32_t                w_offset)
{
  uint32_t i;
  uint32_t cb_len = 0, rp = 0, wp = 0, rlen = 0, n_e = 0;
  int      ret = SRSRAN_ERROR_INVALID_INPUTS;

  if (q != NULL && e_bits != NULL && cb_segm != NULL && softbuffer != NULL) {
    if (cb_segm->F) {
      ERROR("Error filler bits are not supported. Use standard TBS");
      return SRSRAN_ERROR;
    }

    if (cb_segm->C > softbuffer->max_cb) {
      ERROR("Error number of CB to encode (%d) exceeds soft buffer size (%d CBs)", cb_segm->C, softbuffer->max_cb);
      return SRSRAN_ERROR;
    }

    if (Qm == 0) {
      ERROR("Invalid Qm");
      return SRSRAN_ERROR;
    }

    uint32_t Gp = nof_e_bits / Qm;

    uint32_t gamma = Gp;
    if (cb_segm->C > 0) {
      gamma = Gp % cb_segm->C;
    }

    /* Reset TB CRC */
    srsran_crc_set_init(&q->crc_tb, 0);

    wp = 0;
    rp = 0;
    for (i = 0; i < cb_segm->C; i++) {
      uint32_t cblen_idx;
      /* Get read lengths */
      if (i < cb_segm->C2) {
        cb_len    = cb_segm->K2;
        cblen_idx = cb_segm->K2_idx;
      } else {
        cb_len    = cb_segm->K1;
        cblen_idx = cb_segm->K1_idx;
      }
      if (cb_segm->C > 1) {
        rlen = cb_len - 24;
      } else {
        rlen = cb_len;
      }
      if (i <= cb_segm->C - gamma - 1) {
        n_e = Qm * (Gp / cb_segm->C);
      } else {
        n_e = Qm * ((uint32_t)ceilf((float)Gp / cb_segm->C));
      }

      INFO("CB#%d: cb_len: %d, rlen: %d, wp: %d, rp: %d, E: %d", i, cb_len, rlen, wp, rp, n_e);

      if (data) {
        bool last_cb = false;

        /* Copy data to another buffer, making space for the Codeblock CRC */
        if (i < cb_segm->C - 1) {
          // Copy data
          memcpy(q->c_systematic, &data[rp / 8], rlen * sizeof(uint8_t) / 8);
        } else {
          INFO("Last CB, appending parity: %d from %d and 24 to %d", rlen - 24, rp, rlen - 24);

          /* Append Transport Block parity bits to the last CB */
          memcpy(q->c_systematic, &data[rp / 8], (rlen - 24) * sizeof(uint8_t) / 8);
          last_cb = true;
        }

        /* Turbo Encoding
         * If Codeblock CRC is required it is given the CRC instance pointer, otherwise CRC pointer shall be NULL
         */
        uint32_t n_sys_par = srsran_tcod_encode_lut(&q->encoder,
                                                    &q->crc_tb,
                                                    (cb_segm->C > 1) ? &q->crc_cb : NULL,
                                                    q->c_systematic,                //q->cb_in,
                                                    q->c_parity, //q->parity_bits,
                                                    cblen_idx,
                                                    last_cb);

#ifdef DEBUG_INTO_FILES
        uint32_t cb_idx_debug = 0;
        if (i == cb_idx_debug){
            uint8_t c_systematic_parity[n_sys_par];
            srsran_bit_unpack_vector(q->c_systematic, c_systematic_parity, n_sys_par/3);
            uint8_t tmp[n_sys_par/3*2];
            srsran_bit_unpack_vector(q->c_parity, tmp, n_sys_par/3*2);
            memcpy(&c_systematic_parity[n_sys_par/3], tmp, n_sys_par/3*2);
            dect2020::file_binary_representation_unpacked<uint8_t>("c_systematic_parity.txt", c_systematic_parity, n_sys_par, true);
        }
#endif
      }
      else{
        ERROR("Pointer data not valid");
      }

      /* Rate matching */
      if (srsran_rm_turbo_tx_lut(softbuffer->buffer_b[i],
                                 q->c_systematic,
                                 q->c_parity,
                                 &e_bits[(wp + w_offset) / 8],
                                 cblen_idx,
                                 n_e,
                                 (wp + w_offset) % 8,
                                 rv)) {
        ERROR("Error in rate matching");
        return SRSRAN_ERROR;
      }

      /* Set read/write pointers */
      rp += rlen;
      wp += n_e;
    }

    INFO("END CB#%d: wp: %d, rp: %d", i, wp, rp);
    ret = SRSRAN_SUCCESS;
  } else {
    ERROR("Invalid parameters: e_bits=%d, cb_segm=%d, softbuffer=%d", e_bits != 0, cb_segm != 0, softbuffer != 0);
  }

  return ret;
}

int pdc_enc_encode(pdc_enc_t* q,
                    uint8_t* a,                                 // input
                    uint8_t* d,                                 // output
                    srsran_softbuffer_tx_t* softbuffer_d,       // output, for later harq processing
                    uint32_t N_TB_bits,
                    uint32_t N_pbs,
                    uint32_t rv,
                    uint32_t G,                                 // G = N_SS * N_PDC_subc * N_bps, see 7.6.6
                    uint32_t plcf_type)
{
    // codeblock segmentation
    srsran_cbsegm_t cb_segm;
    if (srsran_cbsegm(&cb_segm, N_TB_bits)) {
        ERROR("Error computing Codeword Segmentation for TBS=%d", N_TB_bits);
        return SRSRAN_ERROR;
    }

    int ret = encode_tb_off(q,
                            softbuffer_d,
                            &cb_segm,
                            N_pbs,
                            rv,
                            G,
                            a,
                            d,  // these are the e_bits, but after scrambling they become the d bits
                            0);

    // scrambling
    if(plcf_type == 0)
        srsran_scrambling_bytes(&q->seq_type1, d, G);
    else if(plcf_type == 1)
        srsran_scrambling_bytes(&q->seq_type2, d, G);
    else
        ERROR("Unknown PLCF type");

    return ret;
}

// following code with adjustments from
// source: https://github.com/srsran/srsRAN/blob/master/lib/src/phy/phch/sch.c
static bool decode_tb_cb(pdc_enc_t*            q,
                          srsran_softbuffer_rx_t* softbuffer,
                          srsran_cbsegm_t*        cb_segm,
                          uint32_t                Qm,
                          uint32_t                rv,
                          uint32_t                nof_e_bits,
                          void*                   e_bits,
                          uint8_t*                data)
{
  int8_t*  e_bits_b = (int8_t*) e_bits;
  int16_t* e_bits_s = (int16_t*) e_bits;

  uint32_t write_offset = 0;

  if (cb_segm->C > SRSRAN_MAX_CODEBLOCKS) {
    ERROR("Error SRSRAN_MAX_CODEBLOCKS=%d", SRSRAN_MAX_CODEBLOCKS);
    return false;
  }

  //q->avg_iterations = 0;

  for (uint32_t cb_idx = 0; cb_idx < cb_segm->C; cb_idx++) {
    /* Do not process blocks with CRC Ok */
    if (softbuffer->cb_crc[cb_idx] == false) {

      // unsure if these lines are correct
      uint32_t cb_len     = 0;//cb_idx < cb_segm->C1 ? cb_segm->K1 : cb_segm->K2;
      uint32_t cb_len_idx = 0;//cb_idx < cb_segm->C1 ? cb_segm->K1_idx : cb_segm->K2_idx;

      // replacement for above lines
      if (cb_idx < cb_segm->C2) {
        cb_len    = cb_segm->K2;
        cb_len_idx = cb_segm->K2_idx;
      } else {
        cb_len    = cb_segm->K1;
        cb_len_idx = cb_segm->K1_idx;
      }

      uint32_t rlen  = cb_segm->C == 1 ? cb_len : (cb_len - 24);
      uint32_t Gp    = nof_e_bits / Qm;
      uint32_t gamma = cb_segm->C > 0 ? Gp % cb_segm->C : Gp;
      uint32_t n_e   = Qm * (Gp / cb_segm->C);

      uint32_t rp   = cb_idx * n_e;
      uint32_t n_e2 = n_e;

      if (cb_idx > cb_segm->C - gamma) {
        n_e2 = n_e + Qm;
        rp   = (cb_segm->C - gamma) * n_e + (cb_idx - (cb_segm->C - gamma)) * n_e2;
      }

      if (q->llr_bit_width == 8) {
        if (srsran_rm_turbo_rx_lut_8bit(&e_bits_b[rp], (int8_t*)softbuffer->buffer_f[cb_idx], n_e2, cb_len_idx, rv)) {
          ERROR("Error in rate matching");
          return SRSRAN_ERROR;
        }

#ifdef DEBUG_INTO_FILES
        uint32_t cb_idx_debug = 0;
        if (cb_idx == cb_idx_debug){
            uint32_t long_cb_enc = 3*cb_len+12;
            int8_t c_systematic_parity_rx[3 * 6144 + 12];
            int8_t* tmp_ptr = (int8_t*) softbuffer->buffer_f[cb_idx];
            for(uint32_t ii = 0; ii<6144 / 3; ii++){
                c_systematic_parity_rx[ii] = tmp_ptr[ii*3];
                c_systematic_parity_rx[ii+6144/3] = tmp_ptr[ii*3+1];
                c_systematic_parity_rx[ii+6144/3*2] = tmp_ptr[ii*3+2];
            }
            dect2020::file_binary_representation_unpacked<int8_t>("c_systematic_parity_rx.txt", c_systematic_parity_rx, n_e2, false);
        }
#endif
    } else if (q->llr_bit_width == 16){
        //if (srsran_rm_turbo_rx_lut(&e_bits_s[rp], softbuffer->buffer_f[cb_idx], n_e2, cb_len_idx, rv)) {
        if (srsran_rm_turbo_rx_lut_(&e_bits_s[rp], softbuffer->buffer_f[cb_idx], n_e2, cb_len_idx, rv, false)) {
          ERROR("Error in rate matching");
          return SRSRAN_ERROR;
        }

#ifdef DEBUG_INTO_FILES
        uint32_t cb_idx_debug = 0;
        if (cb_idx == cb_idx_debug){
            uint32_t long_cb_enc = 3*cb_len+12;
            int16_t c_systematic_parity_rx[3 * 6144 + 12];
            int16_t* tmp_ptr = (int16_t*) softbuffer->buffer_f[cb_idx];
            for(uint32_t ii = 0; ii<long_cb_enc / 3; ii++){
                c_systematic_parity_rx[ii] = tmp_ptr[ii*3];
                c_systematic_parity_rx[ii+long_cb_enc/3] = tmp_ptr[ii*3+1];
                c_systematic_parity_rx[ii+long_cb_enc/3*2] = tmp_ptr[ii*3+2];
            }
            dect2020::file_binary_representation_unpacked<int16_t>("c_systematic_parity_rx.txt", c_systematic_parity_rx, cb_len*3+12, false);
        }
#endif
      }

      srsran_tdec_new_cb(&q->decoder, cb_len);

      // Run iterations and use CRC for early stopping
      bool     early_stop = false;
      uint32_t cb_noi     = 0;

      do {
        if (q->llr_bit_width == 8) {
          //srsran_tdec_iteration_8bit(&q->decoder, (int8_t*)softbuffer->buffer_f[cb_idx], &data[cb_idx * rlen / 8]);
          srsran_tdec_iteration_8bit(&q->decoder, (int8_t*)softbuffer->buffer_f[cb_idx], &data[write_offset/8]);
        } else if (q->llr_bit_width == 16){
          //srsran_tdec_iteration(&q->decoder, softbuffer->buffer_f[cb_idx], &data[cb_idx * rlen / 8]);
          srsran_tdec_iteration(&q->decoder, softbuffer->buffer_f[cb_idx], &data[write_offset/8]);
        }

        //q->avg_iterations++;
        cb_noi++;

        uint32_t      len_crc;
        srsran_crc_t* crc_ptr;

        if (cb_segm->C > 1) {
          len_crc = cb_len;
          crc_ptr = &q->crc_cb;
        } else {
          len_crc = cb_segm->tbs + 24;
          crc_ptr = &q->crc_tb;
        }

        // CRC is OK
        if (!srsran_crc_checksum_byte(crc_ptr, &data[cb_idx * rlen / 8], len_crc)) {
          softbuffer->cb_crc[cb_idx] = true;
          early_stop                 = true;

          // CRC is error and exceeded maximum iterations for this CB.
          // Early stop the whole transport block.
        }

      } while (cb_noi < q->max_iterations && !early_stop);

      // new line added
      write_offset += rlen;

      INFO("CB %d: rp=%d, n_e=%d, cb_len=%d, CRC=%s, rlen=%d, iterations=%d/%d",
           cb_idx,
           rp,
           n_e2,
           cb_len,
           early_stop ? "OK" : "KO",
           rlen,
           cb_noi,
           q->max_iterations);

    } else {
      // Copy decoded data from previous transmissions
      uint32_t cb_len = cb_idx < cb_segm->C1 ? cb_segm->K1 : cb_segm->K2;
      uint32_t rlen   = cb_segm->C == 1 ? cb_len : (cb_len - 24);
      memcpy(&data[cb_idx * rlen / 8], softbuffer->data[cb_idx], rlen / 8 * sizeof(uint8_t));
    }
  }

  softbuffer->tb_crc = true;
  for (uint32_t i = 0; i < cb_segm->C && softbuffer->tb_crc; i++) {
    /* If one CB failed return false */
    softbuffer->tb_crc = softbuffer->cb_crc[i];
  }
  // If TB CRC failed, save correct CB for next retransmission
  if (!softbuffer->tb_crc) {
    for (uint32_t i = 0; i < cb_segm->C; i++) {
      if (softbuffer->cb_crc[i]) {
        uint32_t cb_len = i < cb_segm->C1 ? cb_segm->K1 : cb_segm->K2;
        uint32_t rlen   = cb_segm->C == 1 ? cb_len : (cb_len - 24);
        memcpy(softbuffer->data[i], &data[i * rlen / 8], rlen / 8 * sizeof(uint8_t));
      }
    }
  }

  //q->avg_iterations /= (float)cb_segm->C;
  return softbuffer->tb_crc;
}

/**
 * Decode a transport block according to 36.212 5.3.2
 *
 * @param[in] q
 * @param[inout] softbuffer Initialized softbuffer
 * @param[in] cb_segm Code block segmentation parameters
 * @param[in] e_bits Input transport block
 * @param[in] Qm Modulation type
 * @param[in] rv Redundancy Version. Indicates which part of FEC bits is in input buffer
 * @param[out] softbuffer Initialized output softbuffer
 * @param[out] data Decoded transport block
 * @return negative if error in parameters or CRC error in decoding
 */
static int decode_tb(pdc_enc_t*                   q,
                     srsran_softbuffer_rx_t* softbuffer,
                     srsran_cbsegm_t*        cb_segm,
                     uint32_t                Qm,
                     uint32_t                rv,
                     uint32_t                nof_e_bits,
                     int16_t*                e_bits,
                     uint8_t*                data)
{
  if (q != NULL && data != NULL && softbuffer != NULL && e_bits != NULL && cb_segm != NULL && Qm != 0) {
    if (cb_segm->tbs == 0 || cb_segm->C == 0) {
      return SRSRAN_SUCCESS;
    }

    if (cb_segm->F) {
      fprintf(stderr, "Error filler bits are not supported. Use standard TBS\n");
      return SRSRAN_ERROR_INVALID_INPUTS;
    }

    if (cb_segm->C > softbuffer->max_cb) {
      fprintf(stderr,
              "Error number of CB to decode (%d) exceeds soft buffer size (%d CBs)\n",
              cb_segm->C,
              softbuffer->max_cb);
      return SRSRAN_ERROR_INVALID_INPUTS;
    }

    bool crc_ok = true;

    data[cb_segm->tbs / 8 + 0] = 0;
    data[cb_segm->tbs / 8 + 1] = 0;
    data[cb_segm->tbs / 8 + 2] = 0;

    // Process Codeblocks
    crc_ok = decode_tb_cb(q, softbuffer, cb_segm, Qm, rv, nof_e_bits, e_bits, data);

    if (crc_ok) {
      uint32_t par_rx = 0, par_tx = 0;

      // Compute transport block CRC
      par_rx = srsran_crc_checksum_byte(&q->crc_tb, data, cb_segm->tbs);

      // check parity bits
      par_tx = ((uint32_t)data[cb_segm->tbs / 8 + 0]) << 16 | ((uint32_t)data[cb_segm->tbs / 8 + 1]) << 8 |
               ((uint32_t)data[cb_segm->tbs / 8 + 2]);

      if (par_rx == par_tx && par_rx) {
        INFO("TB decoded OK");
        return SRSRAN_SUCCESS;
      } else {
        INFO("Error in TB parity: par_tx=0x%x, par_rx=0x%x", par_tx, par_rx);
        return SRSRAN_ERROR;
      }
    } else {
      return SRSRAN_ERROR;
    }
  } else {
    ERROR("Missing inputs: data=%d, softbuffer=%d, e_bits=%d, cb_segm=%d Qm=%d",
          data != 0,
          softbuffer != 0,
          e_bits != 0,
          cb_segm != 0,
          Qm);
    return SRSRAN_ERROR_INVALID_INPUTS;
  }
}

int pdc_enc_decode(pdc_enc_t* q,
                    uint8_t* a,                                 // output
                    void* d,                                    // input
                    srsran_softbuffer_rx_t* softbuffer_d,       // output, for later harq processing
                    uint32_t N_TB_bits,
                    uint32_t N_pbs,
                    uint32_t rv,
                    uint32_t G,                                 // G = N_SS * N_PDC_subc * N_bps, see 7.6.6
                    uint32_t plcf_type)
{

    srsran_cbsegm_t cb_segm;
    if (srsran_cbsegm(&cb_segm, N_TB_bits)) {
        ERROR("Error computing Codeword Segmentation for TBS=%d", N_TB_bits);
        return SRSRAN_ERROR;
    }

#ifdef DEBUG_INTO_FILES
    dect2020::file_binary_representation_unpacked<DECT2020_D_RX_DATA_TYPE>("d_rx_internal.txt", (DECT2020_D_RX_DATA_TYPE*) d, G, false);
#endif

    // descrambling
    if(plcf_type == 0){
        if (q->llr_bit_width == 8)
            srsran_scrambling_sb_offset(&q->seq_type1, (int8_t*) d, 0, G);
        else if (q->llr_bit_width == 16)
            srsran_scrambling_s_offset(&q->seq_type1, (int16_t*) d, 0, G);
    }
    else if(plcf_type == 1){
        if (q->llr_bit_width == 8)
            srsran_scrambling_sb_offset(&q->seq_type2, (int8_t*) d, 0, G);
        else if (q->llr_bit_width == 16)
            srsran_scrambling_s_offset(&q->seq_type2, (int16_t*) d, 0, G);
    }
    else{
        ERROR("Unknown PLCF type");
    }

    uint32_t ret = decode_tb(q,
                                softbuffer_d,
                                &cb_segm,
                                N_pbs,
                                rv,
                                G,
                                (int16_t*) d,
                                a);

    return SRSRAN_SUCCESS;
}

}
