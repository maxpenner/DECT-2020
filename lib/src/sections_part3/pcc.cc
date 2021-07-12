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

#include <algorithm>

#include "dect2020/sections_part3/drs.h"
#include "dect2020/sections_part3/pcc.h"
#include "dect2020/sections_part3/physical_resources.h"
#include "dect2020/utils/debug.h"

#define N_PACKET_symb_MINIMUM   15 // 5.1: "For N_eff_TX>=4 transmission length should be at least three subslos (15 OFDM symbols) to accomodate [...]"

// marks subcarriers as occupied, must be negative
#define OCCUPIED_DC             -1
#define OCCUPIED_GUARDS         -2
#define OCCUPIED_DRS            -3

namespace dect2020 {

static std::vector<uint32_t> fill_k_i_linear(const k_b_OCC_vec_t& k_b_OCC_vec, uint32_t b, uint32_t N_TS){

    // get packet dimensions
    uint32_t b_idx          = k_b_OCC_vec.b2b_idx[b];
    uint32_t N_b_DFT        = k_b_OCC_vec.N_b_DFT[b_idx];
    uint32_t guards_top     = k_b_OCC_vec.guards_top[b_idx];
    uint32_t guards_bottom  = k_b_OCC_vec.guards_bottom[b_idx];

    // we will closely follow the algorithm in 5.2.4, for this we create a virtual frame
    int32_t virtual_frame[N_PACKET_symb_MINIMUM][N_b_DFT];

    // fill each subcarrier with its linear index
    for(uint32_t l=0; l<N_PACKET_symb_MINIMUM; l++){
        uint32_t linear_offset = l*N_b_DFT;
        for(uint32_t i=0; i<N_b_DFT; i++)
            virtual_frame[l][i] = linear_offset + i;
    }

    // mark DC subcarrier in virtual frame
    for(uint32_t l=0; l<N_PACKET_symb_MINIMUM; l++)
        virtual_frame[l][N_b_DFT/2] = OCCUPIED_DC;

    // mark guard subcarriers in virtual frame
    for(uint32_t l=0; l<N_PACKET_symb_MINIMUM; l++){
        for(uint32_t i=0; i<guards_bottom; i++)
            virtual_frame[l][i] = OCCUPIED_GUARDS;
        for(uint32_t i=N_b_DFT-guards_top; i<N_b_DFT; i++)
            virtual_frame[l][i] = OCCUPIED_GUARDS;
    }

    // to exclude DRS subcarriers we need DRS symbol positions, for this fill a temporary DRS structure
    uint32_t N_eff_TX = N_TS;   // Table 7.2-1
    dect2020::drs_t drs;
    dect2020::get_drs(drs, k_b_OCC_vec, b, N_PACKET_symb_MINIMUM, N_TS, N_eff_TX);

    // mark DRS subcarriers in virtual_frame
    int32_t* virtual_frame_ptr = &virtual_frame[0][0];
    // for each transmit stream
    for(uint32_t t : drs.t){
        // for each subcarrier
        for (uint32_t i_linear : drs.k_i_linear[t])
            virtual_frame_ptr[i_linear] = OCCUPIED_DRS;
    }

    // here we start the actual algorithm according to 5.2.4

    // final set of PCC subcarriers
    std::vector<uint32_t> k_PCC_l;

    // step 1)
    uint32_t l = 1;
    uint32_t N_unalloc_subc = 98;

    while(1==1){

        // step 2)
        std::vector<uint32_t> k_0_U_l;
        for(uint32_t i=0; i<N_b_DFT; i++){

            // not DC or guard or DRS?
            if(virtual_frame[l][i] >= 0){

                // add linear index
                k_0_U_l.push_back(virtual_frame[l][i]);
            }
        }

        // step 3)
        uint32_t U = k_0_U_l.size();
        if(U<N_unalloc_subc){

            // step 4 a)
            for(uint32_t u=0; u<U; u++)
                k_PCC_l.push_back(k_0_U_l[u]);

            // step 4 b)
            l = l+1;
            N_unalloc_subc = N_unalloc_subc - U;

            // step 4 c)
            continue;
        }
        else{

            // step 5)
            const uint32_t R_PCC = 7;

            // step 6)
            uint32_t C_PCC = U/R_PCC;

            // sanity check
            if(U%R_PCC != 0)
                ERROR("Incorrect number of subcarriers U");

            // step 7) create and fill matrix
            uint32_t mat[R_PCC][C_PCC];
            uint32_t* mat_ptr = &mat[0][0];
            for(uint32_t u=0; u<U; u++){
                mat_ptr[u] = k_0_U_l[u];
            }

            // step 8)
            for(uint32_t c=0; c<C_PCC; c++){
                for(uint32_t r=0; r<R_PCC; r++){

                    k_PCC_l.push_back(mat[r][c]);

                    N_unalloc_subc--;

                    // do we have 98 subcarriers?
                    if(N_unalloc_subc == 0)
                        break;
                }

                // abort prematurely
                if(N_unalloc_subc == 0)
                    break;
            }

            // sanity check
            if(k_PCC_l.size() != 98)
                ERROR("Incorrect number of PCC subcarriers");

            // NOTE
            std::sort(k_PCC_l.begin(), k_PCC_l.end());

            // leave while loop
            break;
        }
    }

    return k_PCC_l;
}

void get_pcc(pcc_t& pcc){

    pcc.k_i_linear.clear();

    k_b_OCC_vec_t k_b_OCC_vec;
    get_k_b_OCC(k_b_OCC_vec);

    // for each value of beta
    for(uint32_t b_idx=0; b_idx<6; b_idx++){

        uint32_t b = k_b_OCC_vec.b_idx2b[b_idx];

        // add two dimensional matrix for this beta
        pcc.k_i_linear.push_back(std::vector<std::vector< uint32_t >>());

        // we have 1, 2, 4 or 8 transmit streams occupying DRS
        for(uint32_t N_TS=1; N_TS<=8; N_TS=N_TS*2){

            // calculate 98 PCC subcarriers for this beta and N_TS
            pcc.k_i_linear[b_idx].push_back( fill_k_i_linear(k_b_OCC_vec, b, N_TS) );
        }
    }
}

}