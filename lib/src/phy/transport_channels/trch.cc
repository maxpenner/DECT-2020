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

#include "dect2020/phy/transport_channels/trch.h"
#include "dect2020/utils/debug.h"

namespace dect2020 {

int trch_init(trch_t& q, const packet_sizes_t& packet_sizes_maximum, uint32_t network_id){

    int ret = DECT2020_ERROR;

    if (pcc_enc_init(&q.pcc_enc, &packet_sizes_maximum)) {
        goto clean;
    }

    if (pdc_enc_init(&q.pdc_enc, &packet_sizes_maximum, network_id)) {
        goto clean;
    }

    ret = DECT2020_SUCCESS;

clean:
    if (ret == DECT2020_ERROR) {
        trch_free(q);
    }
    return ret;
}

int trch_free(trch_t& q){
    pcc_enc_free(&q.pcc_enc);
    pdc_enc_free(&q.pdc_enc);

    return DECT2020_SUCCESS;
}

int trch_encode(trch_t& q){

    trch_encode_plcf(q);
    trch_encode_tb(q);

    return DECT2020_SUCCESS;
}

int trch_encode_plcf(trch_t& q){

    pcc_enc_encode(&q.pcc_enc,
                    q.a,
                    q.d,
                    q.softbuffer_d,
                    q.tx_meta.N_PLCF_bits,
                    q.tx_meta.closed_loop,
                    q.tx_meta.beamforming);

    return DECT2020_SUCCESS;
}

int trch_encode_tb(trch_t& q){

    pdc_enc_encode(&q.pdc_enc,
                    q.a_tb,
                    q.d_tb,
                    q.softbuffer_d_tb,
                    q.tx_meta.N_TB_bits,
                    q.tx_meta.N_bps,
                    q.tx_meta.rv,
                    q.tx_meta.G,
                    q.tx_meta.PLCF_type);

    return DECT2020_SUCCESS;
}

int trch_decode(trch_t& q){

    trch_decode_plcf(q);
    trch_decode_tb(q);

    return DECT2020_SUCCESS;
}

int trch_decode_plcf(trch_t& q){

    pcc_enc_decode(&q.pcc_enc,
                    q.a_rx,
                    q.d_rx,
                    q.softbuffer_d_rx,
                    q.rx_meta.N_PLCF_bits,
                    q.rx_meta.closed_loop,
                    q.rx_meta.beamforming);

    return DECT2020_SUCCESS;
}

int trch_decode_tb(trch_t& q){

    pdc_enc_decode(&q.pdc_enc,
                    q.a_tb_rx,
                    q.d_tb_rx,
                    q.softbuffer_d_tb_rx,
                    q.rx_meta.N_TB_bits,
                    q.rx_meta.N_bps,
                    q.rx_meta.rv,
                    q.rx_meta.G,
                    q.rx_meta.PLCF_type);

    return DECT2020_SUCCESS;
}

}
