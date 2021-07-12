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

#include "dect2020/sections_part3/pdc.h"
#include "dect2020/utils/debug.h"

// marks subcarriers as occupied, must be negative
#define OCCUPIED_DC     -1
#define OCCUPIED_GUARDS -2
#define OCCUPIED_DRS    -3
#define OCCUPIED_PCC    -4

static int get_N_DF_symb(uint32_t& N_DF_symb, uint32_t N_PACKET_symb, uint32_t u){

    uint32_t N_GI_STF_symb = 0;

    if(u==1)
        N_GI_STF_symb = 2;
    else if (u==2 || u==4)
        N_GI_STF_symb = 3;
    else if (u==8)
        N_GI_STF_symb = 4;
    else{
        ERROR("Unknown u=%d", u);
        return DECT2020_ERROR;
    }

    // sanity check
    if(N_PACKET_symb <= N_GI_STF_symb){
        ERROR("N_PACKET_symb <= N_GI_STF_symb with N_PACKET_symb=%d and N_GI_STF_symb=%d", N_PACKET_symb, N_GI_STF_symb);
        return DECT2020_ERROR;
    }

    N_DF_symb = N_PACKET_symb - N_GI_STF_symb;

    return DECT2020_SUCCESS;
}

static int get_N_DRS_subc(uint32_t& N_DRS_subc, uint32_t N_PACKET_symb, uint32_t N_eff_TX, uint32_t N_b_OCC){

    uint32_t N_step = 0;

    if (N_eff_TX <= 2)
        N_step = 5;
    else if (N_eff_TX >= 4)
        N_step = 10;

    // sanity check
    if(N_PACKET_symb / N_step == 0){
        ERROR("Value of floor(N_PACKET_symb / N_step) is zero, therefore N_DRS_subc is zero as well. N_PACKET_symb=%d and N_step=%d", N_PACKET_symb, N_step);
        return DECT2020_ERROR;
    }

    N_DRS_subc = N_eff_TX * N_b_OCC / 4 * (N_PACKET_symb / N_step);

    return DECT2020_SUCCESS;
}

namespace dect2020 {

int get_pdc(pdc_t& pdc, const k_b_OCC_vec_t& k_b_OCC_vec, const pcc_t& pcc, const drs_t& drs, uint32_t u, uint32_t b, uint32_t N_PACKET_symb, uint32_t N_TS){

    pdc.k_i_linear.clear();

    // get packet dimensions
    uint32_t b_idx          = k_b_OCC_vec.b2b_idx[b];
    uint32_t N_TX_idx       = k_b_OCC_vec.b2b_idx[N_TS];
    uint32_t N_b_DFT        = k_b_OCC_vec.N_b_DFT[b_idx];
    uint32_t N_b_OCC        = k_b_OCC_vec.N_b_OCC[b_idx];
    uint32_t guards_top     = k_b_OCC_vec.guards_top[b_idx];
    uint32_t guards_bottom  = k_b_OCC_vec.guards_bottom[b_idx];

    uint32_t N_DF_symb;
    if(get_N_DF_symb(N_DF_symb, N_PACKET_symb, u)){
        return DECT2020_ERROR;
    }

    // how many subcarriers will there be for PDC?
    uint32_t N_eff_TX = N_TS;   // Table 7.2-1
    uint32_t N_PDC_subc;
    if(get_N_PDC_subc(N_PDC_subc, N_PACKET_symb, u, N_eff_TX, N_b_OCC)){
        return DECT2020_ERROR;
    }

    // create a virtual frame
    int32_t virtual_frame[N_PACKET_symb][N_b_DFT];

    // fill each subcarrier with its linear index
    for(uint32_t l=0; l<N_PACKET_symb; l++){
        for(uint32_t i=0; i<N_b_DFT; i++){
            virtual_frame[l][i] = l*N_b_DFT + i;
        }
    }

    // mark DC subcarriers in virtual frame
    for(uint32_t l=0; l<N_PACKET_symb; l++){
        virtual_frame[l][N_b_DFT/2] = OCCUPIED_DC;
    }

    // mark guard subcarriers in virtual frame
    for(uint32_t l=0; l<N_PACKET_symb; l++){
        for(uint32_t i=0; i<guards_bottom; i++){
            virtual_frame[l][i] = OCCUPIED_GUARDS;
        }
        for(uint32_t i=N_b_DFT-guards_top; i<N_b_DFT; i++){
            virtual_frame[l][i] = OCCUPIED_GUARDS;
        }
    }

    // mark DRS subcarriers in virtual_frame
    int32_t* virtual_frame_ptr = &virtual_frame[0][0];
    // for each transmit stream
    for(uint32_t t : drs.t){
        // for each subcarrier
        for (uint32_t i_linear : drs.k_i_linear[t]){
            virtual_frame_ptr[i_linear] = OCCUPIED_DRS;
        }
    }

    // mark PCC subcarriers in virtual_frame
    for (uint32_t i_linear : pcc.k_i_linear[b_idx][N_TX_idx]){
        virtual_frame_ptr[i_linear] = OCCUPIED_PCC;
    }

    // allocate memory
    pdc.k_i_linear.resize(N_PDC_subc);

    // go through entire frame and check if subcarrier is available for PDC
    uint32_t N_PDC_subc_check = 0;
    for(uint32_t l=1; l< 1 + N_DF_symb; l++){
        for(uint32_t i=0; i<N_b_DFT; i++){
            // occupied markers are all negative
            if(virtual_frame[l][i] >= 0){
                pdc.k_i_linear[N_PDC_subc_check++] = virtual_frame[l][i];
            }
        }
    }

    // sanity check
    if(N_PDC_subc != N_PDC_subc_check){
        ERROR("N_PDC_subc=%d is not N_PDC_subc_check=%d", N_PDC_subc, N_PDC_subc_check);
        return DECT2020_ERROR;
    }

    return DECT2020_SUCCESS;
}

int get_N_PDC_subc(uint32_t& N_PDC_subc, uint32_t N_PACKET_symb, uint32_t u, uint32_t N_eff_TX, uint32_t N_b_OCC){

    uint32_t N_PCC_subc = 98;

    uint32_t N_DF_symb;
    if(get_N_DF_symb(N_DF_symb, N_PACKET_symb, u)){
        return DECT2020_ERROR;
    }

    uint32_t N_DRS_subc;
    if(get_N_DRS_subc(N_DRS_subc, N_PACKET_symb, N_eff_TX, N_b_OCC)){
        return DECT2020_ERROR;
    }

    // sanity check
    if(N_DF_symb * N_b_OCC <= (N_DRS_subc + N_PCC_subc)){
        ERROR("N_PDC_subc <= 0");
        return DECT2020_ERROR;
    }

    N_PDC_subc = N_DF_symb * N_b_OCC - N_DRS_subc - N_PCC_subc;

    return DECT2020_SUCCESS;
}

}
