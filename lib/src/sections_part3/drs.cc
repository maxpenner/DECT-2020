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

#include "dect2020/sections_part3/drs.h"
#include "dect2020/utils/debug.h"

static const std::complex<float> y_b_1[56] = { {1,0},{1,0},{1,0},{1,0}, {-1,0},{1,0},{1,0},{-1,0}, {-1,0},{1,0},{1,0},{1,0}, {1,0},{-1,0},{1,0},{-1,0}, {1,0},{1,0},{-1,0},{1,0}, {-1,0},{1,0},{-1,0},{1,0}, {1,0},{1,0},{1,0},{1,0} ,{-1,0},{1,0},
                                               {-1,0},{-1,0},{1,0},{1,0}, {-1,0},{-1,0},{-1,0},{-1,0}, {1,0},{-1,0},{-1,0},{-1,0}, {-1,0},{-1,0},{1,0},{1,0}, {1,0},{-1,0},{1,0},{1,0}, {-1,0},{-1,0},{1,0},{-1,0} ,{-1,0},{-1,0}};

namespace dect2020 {

int get_drs(drs_t& drs, const k_b_OCC_vec_t& k_b_OCC_vec, uint32_t b, uint32_t N_PACKET_symb, uint32_t N_TS, uint32_t N_eff_TX){

    // sanity check, 5.1
    if(N_eff_TX >= 4 && N_PACKET_symb < 15){
        ERROR("Not enough OFDM symbols for N_eff_TX>=4, we need at least 15");
        return DECT2020_ERROR;
    }

    // same configuration?
    if(drs.b==b && drs.N_PACKET_symb==N_PACKET_symb && drs.N_TS==N_TS && drs.N_eff_TX==N_eff_TX){
        return DECT2020_SUCCESS;
    }

    drs.b = b;
    drs.N_PACKET_symb = N_PACKET_symb;
    drs.N_TS = N_TS;
    drs.N_eff_TX = N_eff_TX;

    drs.t.clear();
    drs.k_i.clear();
    drs.k_i_linear.clear();
    drs.l.clear();
    drs.y_DRS_i.clear();

    uint32_t b_idx      = k_b_OCC_vec.b2b_idx[b];
    uint32_t N_b_DFT    = k_b_OCC_vec.N_b_DFT[b_idx];
    uint32_t N_b_OCC    = k_b_OCC_vec.N_b_OCC[b_idx];

    uint32_t N_step = (N_eff_TX <= 2) ? 5 : 10;


    uint32_t n_subc = N_b_OCC/4;
    uint32_t n_symb = N_PACKET_symb/N_step;

    // sanity check
    if(n_symb == 0){
        ERROR("Number of OFDM symbols for DRS n_symb is 0");
        return DECT2020_ERROR;
    }

    // go over each transmit stream
    for(uint32_t t=0; t<N_TS; t++){

        // t
        drs.t.push_back(t);

        // k_i, k_i_linear and l
        drs.k_i.push_back(std::vector<int32_t>(n_subc*n_symb));
        drs.k_i_linear.push_back(std::vector<uint32_t>(n_subc*n_symb));
        drs.l.push_back(std::vector<uint32_t>(n_symb));
        int idx = 0;
        for(uint32_t n=0; n<= n_symb - 1; n++){

            uint32_t l = 1 + t/4 + n*N_step;

            drs.l[t][n] = l;

            int linear_offset = N_b_DFT/2 + N_b_DFT * l;

            for(uint32_t i=0; i<=n_subc-1; i++){
                drs.k_i[t][idx]         = k_b_OCC_vec.b_x[b_idx][i*4 + (t + (n%2) * 2 ) % 4];
                drs.k_i_linear[t][idx]  = drs.k_i[t][idx] + linear_offset;
                idx++;
            }

        }

        // y_DRS_i
        drs.y_DRS_i.push_back(std::vector<std::complex<float>>(n_subc));
        idx = 0;
        if(t<=4){

            for(uint32_t i=0; i<= n_subc - 1; i++)
                drs.y_DRS_i[t][idx++] = y_b_1[(4*i + t % 4) % 56];

        }
        else if(t>4){

            for(uint32_t i=0; i<= n_subc - 1; i++)
                drs.y_DRS_i[t][idx++] = -y_b_1[(4*i + t % 4) % 56];

        }
    }

    return DECT2020_SUCCESS;
}

}