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

#include "dect2020/sections_part3/stf.h"
#include "dect2020/sections_part3/physical_resources.h"
#include "dect2020/utils/debug.h"

static const std::complex<float> y_b_1[14] = { {0,-1},{0,-1},{-1,0},{-1,0},{0,1},{0,-1},{0,1}, {0,-1},{0,-1},{0,1},{0,1},{-1,0},{0,-1},{-1,0}};

static const std::complex<float> y_b_2[28] = { {1,0},{-1,0},{1,0},{1,0},{1,0},{-1,0},{1,0},{1,0},{1,0},{1,0},{-1,0},{-1,0},{1,0},{-1,0}, {1,0},{1,0},{1,0},{-1,0},{1,0},{1,0},{-1,0},{1,0},{1,0},{1,0},{1,0},{-1,0},{1,0},{-1,0}};

static void fill_k_i(int* k_i, const int* k_b_OCC, int N_b_OCC){
    int idx = 0;
    for(int i=0; i<= N_b_OCC/8-1; i++)
        k_i[idx++] = k_b_OCC[i*4];
    for(int i=N_b_OCC/8; i<=N_b_OCC/4-1; i++)
        k_i[idx++] = k_b_OCC[N_b_OCC/2 + 3 + (i-N_b_OCC/8)*4];
}

static void fill_y_STF_i(std::complex<float>* y_STF_i_b_x, int N_b_OCC, int N_eff_TX){

    uint32_t log2_N_eff_TX;
    switch(N_eff_TX){
        case 1:
            log2_N_eff_TX = 0;
            break;
        case 2:
            log2_N_eff_TX = 1;
            break;
        case 4:
            log2_N_eff_TX = 2;
            break;
        case 8:
            log2_N_eff_TX = 3;
            break;
    }

    if(N_b_OCC == 56){
        for(int i=0; i<=N_b_OCC/4-1; i++)
            y_STF_i_b_x[i] = y_b_1[ (i+2*log2_N_eff_TX) % (N_b_OCC/4) ];
    }
    else{
        for(int i=0; i<=N_b_OCC/4-1; i++)
            y_STF_i_b_x[i] = y_b_2[ ( (i+2*log2_N_eff_TX) % (N_b_OCC/4) ) % 28 ];
    }
}

namespace dect2020 {

void get_stf(stf_t& stf){

    k_b_OCC_t k_b_OCC;
    get_k_b_OCC(k_b_OCC);

    // k_i
    fill_k_i(stf.k_i_b_1,  k_b_OCC.b_1,  k_b_OCC.N_b_OCC[0]);
    fill_k_i(stf.k_i_b_2,  k_b_OCC.b_2,  k_b_OCC.N_b_OCC[1]);
    fill_k_i(stf.k_i_b_4,  k_b_OCC.b_4,  k_b_OCC.N_b_OCC[2]);
    fill_k_i(stf.k_i_b_8,  k_b_OCC.b_8,  k_b_OCC.N_b_OCC[3]);
    fill_k_i(stf.k_i_b_12, k_b_OCC.b_12, k_b_OCC.N_b_OCC[4]);
    fill_k_i(stf.k_i_b_16, k_b_OCC.b_16, k_b_OCC.N_b_OCC[5]);

    // y_STF,(0)_i
    uint32_t idx = 0;
    for(uint32_t N_eff_TX=1; N_eff_TX<=8; N_eff_TX=N_eff_TX*2){

        fill_y_STF_i(&stf.y_STF_i_b_1[idx][0],  k_b_OCC.N_b_OCC[0], N_eff_TX);
        fill_y_STF_i(&stf.y_STF_i_b_2[idx][0],  k_b_OCC.N_b_OCC[1], N_eff_TX);
        fill_y_STF_i(&stf.y_STF_i_b_4[idx][0],  k_b_OCC.N_b_OCC[2], N_eff_TX);
        fill_y_STF_i(&stf.y_STF_i_b_8[idx][0],  k_b_OCC.N_b_OCC[3], N_eff_TX);
        fill_y_STF_i(&stf.y_STF_i_b_12[idx][0], k_b_OCC.N_b_OCC[4], N_eff_TX);
        fill_y_STF_i(&stf.y_STF_i_b_16[idx][0], k_b_OCC.N_b_OCC[5], N_eff_TX);

        idx++;
    }
}

void get_stf(stf_vec_t& stf_vec){

    stf_vec.k_i_b_x.clear();
    stf_vec.y_STF_i_b_x.clear();

    k_b_OCC_vec_t k_b_OCC_vec;
    get_k_b_OCC(k_b_OCC_vec);

    // six values of beta
    for(uint32_t b_idx=0; b_idx<6; b_idx++){

        int32_t b = k_b_OCC_vec.b_idx2b[b_idx];
        int32_t N_b_OCC = k_b_OCC_vec.N_b_OCC[b_idx];

        // k_i_b_x
        stf_vec.k_i_b_x.push_back(std::vector<int32_t>());

        // fill k_i_b_x
        for(int i=0; i<= N_b_OCC/8-1; i++)
            stf_vec.k_i_b_x[b_idx].push_back(  k_b_OCC_vec.b_x[b_idx][i*4]  );
        for(int i=N_b_OCC/8; i<=N_b_OCC/4-1; i++)
            stf_vec.k_i_b_x[b_idx].push_back(  k_b_OCC_vec.b_x[b_idx][N_b_OCC/2 + 3 + (i-N_b_OCC/8)*4]  );

        // y_STF_i_b_x
        stf_vec.y_STF_i_b_x.push_back(std::vector<std::vector<std::complex<float>>>());

        // we have 1, 2, 4, or 8 effective antennas
        uint32_t idx = 0;
        for(uint32_t N_eff_TX=1; N_eff_TX<=8; N_eff_TX=N_eff_TX*2){

            // log2(N_eff_TX)
            uint32_t log2_N_eff_TX;
            switch(N_eff_TX){
                case 1:
                    log2_N_eff_TX = 0;
                    break;
                case 2:
                    log2_N_eff_TX = 1;
                    break;
                case 4:
                    log2_N_eff_TX = 2;
                    break;
                case 8:
                    log2_N_eff_TX = 3;
                    break;
            }

            // push_back a vector
            stf_vec.y_STF_i_b_x[b_idx].push_back(std::vector<std::complex<float>>());

            // fill y_STF_i_b_x
            if(N_b_OCC == 56){
                for(int i=0; i<=N_b_OCC/4-1; i++)
                    stf_vec.y_STF_i_b_x[b_idx][idx].push_back(y_b_1[ (i+2*log2_N_eff_TX) % (N_b_OCC/4) ]);
            }
            else{
                for(int i=0; i<=N_b_OCC/4-1; i++)
                    stf_vec.y_STF_i_b_x[b_idx][idx].push_back(y_b_2[ ( (i+2*log2_N_eff_TX) % (N_b_OCC/4) ) % 28 ] );
            }

            idx++;
        }
    }
}

}
