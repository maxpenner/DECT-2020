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

#include <cmath>

#include "dect2020/sections_part3/beamforming_and_antenna_port_mapping.h"
#include "dect2020/utils/debug.h"

// matrices copied from matlab code
// matlab accepts 1 as complex double, here 1 is an integer
// converted to std::complex<float> in function get_w()
// prefactor can be calculate on the fly: prefactor = 1 / sqrt( sum( abs(W_x_x).^2 ) )

// complex marker
#define q 2

// Table 6.3.4-1
static std::vector<int> W_0_A = {1,0};
static std::vector<int> W_1_A = {0,1};
static std::vector<int> W_2_A = {1,1};
static std::vector<int> W_3_A = {1,-1};
static std::vector<int> W_4_A = {1,q};
static std::vector<int> W_5_A = {1,-q};

// Table 6.3.4-2
static std::vector<int> W_0_B = {1,0,0,0};
static std::vector<int> W_1_B = {0,1,0,0};
static std::vector<int> W_2_B = {0,0,1,0};
static std::vector<int> W_3_B = {0,0,0,1};
static std::vector<int> W_4_B = {1,0,1,0};
static std::vector<int> W_5_B = {1,0,-1,0};
static std::vector<int> W_6_B = {1,0,q,0};
static std::vector<int> W_7_B = {1,0,-q,0};
static std::vector<int> W_8_B = {0,1,0,1};
static std::vector<int> W_9_B = {0,1,0,-1};
static std::vector<int> W_10_B = {0,1,0,q};
static std::vector<int> W_11_B = {0,1,0,-q};
static std::vector<int> W_12_B = {1,1,1,1};
static std::vector<int> W_13_B = {1,1,q,q};
static std::vector<int> W_14_B = {1,1,-1,-1};
static std::vector<int> W_15_B = {1,1,-q,-q};
static std::vector<int> W_16_B = {1,q,1,q};
static std::vector<int> W_17_B = {1,q,q,-1};
static std::vector<int> W_18_B = {1,q,-1,-q};
static std::vector<int> W_19_B = {1,q,-q,1};
static std::vector<int> W_20_B = {1,-1,1,-1};
static std::vector<int> W_21_B = {1,-1,q,-q};
static std::vector<int> W_22_B = {1,-1,-1,1};
static std::vector<int> W_23_B = {1,-1,-q,q};
static std::vector<int> W_24_B = {1,-q,1,-q};
static std::vector<int> W_25_B = {1,-q,q,1};
static std::vector<int> W_26_B = {1,-q,-1,q};
static std::vector<int> W_27_B = {1,-q,-q,-1};

// Table 6.3.4-3
static std::vector<int> W_0_C = {1,0,0,1};
static std::vector<int> W_1_C = {1,1,1,-1};
static std::vector<int> W_2_C = {1,1,q,-q};

// Table 6.3.4-4
static std::vector<int> W_0_D = {   1, 0,
                                    0, 1,
                                    0, 0,
                                    0, 0};

static std::vector<int> W_1_D = {   1, 0,
                                    0, 0,
                                    0, 1,
                                    0, 0};

static std::vector<int> W_2_D = {   1, 0,
                                    0, 0,
                                    0, 0,
                                    0, 1};

static std::vector<int> W_3_D = {   0, 0,
                                    1, 0,
                                    0, 1,
                                    0, 0};

static std::vector<int> W_4_D = {   0, 0,
                                    1, 0,
                                    0, 0,
                                    0, 1};

static std::vector<int> W_5_D = {   0, 0,
                                    0, 0,
                                    1, 0,
                                    0, 1};

static std::vector<int> W_6_D = {   1, 0,
                                    0, 1,
                                    1, 0,
                                    0,-q};

static std::vector<int> W_7_D = {   1, 0,
                                    0, 1,
                                    1, 0,
                                    0, q};

static std::vector<int> W_8_D = {   1, 0,
                                    0, 1,
                                   -q, 0,
                                    0, 1};

static std::vector<int> W_9_D = {   1, 0,
                                    0, 1,
                                   -q, 0,
                                    0,-1};

static std::vector<int> W_10_D = {  1, 0,
                                    0, 1,
                                   -1, 0,
                                    0,-q};

static std::vector<int> W_11_D = {  1, 0,
                                    0, 1,
                                   -1, 0,
                                    0, q};

static std::vector<int> W_12_D = {  1, 0,
                                    0, 1,
                                    q, 0,
                                    0, 1};

static std::vector<int> W_13_D = {  1, 0,
                                    0, 1,
                                    q, 0,
                                    0,-1};

static std::vector<int> W_14_D = {  1, 1,
                                    1, 1,
                                    1,-1,
                                    1,-1};

static std::vector<int> W_15_D = {  1, 1,
                                    1, 1,
                                    q,-q,
                                    q,-q};

static std::vector<int> W_16_D = {  1, 1,
                                    q, q,
                                    1,-1,
                                    q,-q};

static std::vector<int> W_17_D = {  1, 1,
                                    q, q,
                                    q,-q,
                                   -1, 1};

static std::vector<int> W_18_D = {  1, 1,
                                    -1,-1,
                                     1,-1,
                                    -1, 1};

static std::vector<int> W_19_D = {  1, 1,
                                    -1,-1,
                                     q,-q,
                                    -q, q};

static std::vector<int> W_20_D = {  1, 1,
                                    -q,-q,
                                     1,-1,
                                    -q, q};

static std::vector<int> W_21_D = {  1, 1,
                                    -q,-q,
                                     q,-q,
                                     1,-1};

// Table 6.3.4-5
static std::vector<int> W_0_E = {   1, 0, 0, 0,
                                    0, 1, 0, 0,
                                    0, 0, 1, 0,
                                    0, 0, 0, 1};

static std::vector<int> W_1_E = {   1, 1, 0, 0,
                                    0, 0, 1, 1,
                                    1,-1, 0, 0,
                                    0, 0, 1,-1};

static std::vector<int> W_2_E = {   1, 1, 0, 0,
                                    0, 0, 1, 1,
                                    q,-q, 0, 0,
                                    0, 0, q,-q};

static std::vector<int> W_3_E = {   1, 1, 1, 1,
                                    1,-1, 1,-1,
                                    1, 1,-1,-1,
                                    1,-1,-1, 1};

static std::vector<int> W_4_E = {   1, 1, 1, 1,
                                    1,-1, 1,-1,
                                    q, q,-q,-q,
                                    q,-q,-q, q};

// Table 6.3.4-6
static std::vector<int> W_0_F = {   1, 0, 0, 0, 0, 0, 0, 0,
                                    0, 1, 0, 0, 0, 0, 0, 0,
                                    0, 0, 1, 0, 0, 0, 0, 0,
                                    0, 0, 0, 1, 0, 0, 0, 0,
                                    0, 0, 0, 0, 1, 0, 0, 0,
                                    0, 0, 0, 0, 0, 1, 0, 0,
                                    0, 0, 0, 0, 0, 0, 1, 0,
                                    0, 0, 0, 0, 0, 0, 0, 1};

namespace dect2020{

static std::vector<int32_t> mat_ptr(uint32_t N_TS, uint32_t N_ant_port, uint32_t codebook_index){

    std::vector<int32_t> ret;

    if(N_TS == 1 && N_ant_port == 2){
        if(codebook_index == 0) ret = W_0_A;
        else if(codebook_index == 1) ret = W_1_A;
        else if(codebook_index == 2) ret = W_2_A;
        else if(codebook_index == 3) ret = W_3_A;
        else if(codebook_index == 4) ret = W_4_A;
        else if(codebook_index == 5) ret = W_5_A;
        else ERROR("Unknown codebook_index");
    }
    else if(N_TS == 1 && N_ant_port == 4){
        if(codebook_index == 0) ret = W_0_B;
        else if(codebook_index == 1) ret = W_1_B;
        else if(codebook_index == 2) ret = W_2_B;
        else if(codebook_index == 3) ret = W_3_B;
        else if(codebook_index == 4) ret = W_4_B;
        else if(codebook_index == 5) ret = W_5_B;
        else if(codebook_index == 6) ret = W_6_B;
        else if(codebook_index == 7) ret = W_7_B;
        else if(codebook_index == 8) ret = W_8_B;
        else if(codebook_index == 9) ret = W_9_B;
        else if(codebook_index == 10) ret = W_10_B;
        else if(codebook_index == 11) ret = W_11_B;
        else if(codebook_index == 12) ret = W_12_B;
        else if(codebook_index == 13) ret = W_13_B;
        else if(codebook_index == 14) ret = W_14_B;
        else if(codebook_index == 15) ret = W_15_B;
        else if(codebook_index == 16) ret = W_16_B;
        else if(codebook_index == 17) ret = W_17_B;
        else if(codebook_index == 18) ret = W_18_B;
        else if(codebook_index == 19) ret = W_19_B;
        else if(codebook_index == 20) ret = W_20_B;
        else if(codebook_index == 21) ret = W_21_B;
        else if(codebook_index == 22) ret = W_22_B;
        else if(codebook_index == 23) ret = W_23_B;
        else if(codebook_index == 24) ret = W_24_B;
        else if(codebook_index == 25) ret = W_25_B;
        else if(codebook_index == 26) ret = W_26_B;
        else if(codebook_index == 27) ret = W_27_B;
        else ERROR("Unknown codebook_index");
    }
    else if(N_TS == 2 && N_ant_port == 2){
        if(codebook_index == 0) ret = W_0_C;
        else if(codebook_index == 1) ret = W_1_C;
        else if(codebook_index == 2) ret = W_2_C;
        else ERROR("Unknown codebook_index");
    }
    else if(N_TS == 2 && N_ant_port == 4){
        if(codebook_index == 0) ret = W_0_D;
        else if(codebook_index == 1) ret = W_1_D;
        else if(codebook_index == 2) ret = W_2_D;
        else if(codebook_index == 3) ret = W_3_D;
        else if(codebook_index == 4) ret = W_4_D;
        else if(codebook_index == 5) ret = W_5_D;
        else if(codebook_index == 6) ret = W_6_D;
        else if(codebook_index == 7) ret = W_7_D;
        else if(codebook_index == 8) ret = W_8_D;
        else if(codebook_index == 9) ret = W_9_D;
        else if(codebook_index == 10) ret = W_10_D;
        else if(codebook_index == 11) ret = W_11_D;
        else if(codebook_index == 12) ret = W_12_D;
        else if(codebook_index == 13) ret = W_13_D;
        else if(codebook_index == 14) ret = W_14_D;
        else if(codebook_index == 15) ret = W_15_D;
        else if(codebook_index == 16) ret = W_16_D;
        else if(codebook_index == 17) ret = W_17_D;
        else if(codebook_index == 18) ret = W_18_D;
        else if(codebook_index == 19) ret = W_19_D;
        else if(codebook_index == 20) ret = W_20_D;
        else if(codebook_index == 21) ret = W_21_D;
        else ERROR("Unknown codebook_index");
    }
    else if(N_TS == 4 && N_ant_port == 4){
        if(codebook_index == 0) ret = W_0_E;
        else if(codebook_index == 1) ret = W_1_E;
        else if(codebook_index == 2) ret = W_2_E;
        else if(codebook_index == 3) ret = W_3_E;
        else if(codebook_index == 4) ret = W_4_E;
        else ERROR("Unknown codebook_index");
    }
    else if(N_TS == 8 && N_ant_port == 8){
        if(codebook_index == 0) ret = W_0_F;
        else ERROR("Unknown codebook_index");
    }
    else{
        ERROR("Unknown N_TS=%d and N_ant_port=%d configuration", N_TS, N_ant_port);
    }

    return ret;
}

static std::vector<std::complex<float>> convert(const std::vector<int32_t> in, float prefactor){

    std::vector<std::complex<float>> ret;

    // determine prefactor
    float prefactor_default = 0.0f;
    for(uint32_t i=0; i<in.size(); i++){
        if(in[i] != 0)
            prefactor_default += 1.0f;
    }
    prefactor_default = 1.0f/sqrt(prefactor_default);
    prefactor = prefactor_default*prefactor;

    // convert from int to float
    for(uint32_t i=0; i<in.size(); i++){
        if(in[i] == 1)          ret.push_back(std::complex<float>(prefactor,0.0f));
        else if(in[i] == -1)    ret.push_back(std::complex<float>(-prefactor,0.0f));
        else if(in[i] == q)     ret.push_back(std::complex<float>(0.0f,prefactor));
        else if(in[i] == -q)    ret.push_back(std::complex<float>(0.0f,-prefactor));
        else if(in[i] == 0)     ret.push_back(std::complex<float>(0.0f,0.0f));
        else                    ERROR("Unknown matrix value in[i]=%d", in[i]);
    }

    return ret;
}

void get_w(w_t& w, float prefactor){

    w.prefactor = prefactor;

    // create 6 2d matrices
    for(uint32_t i=0; i<6; i++)
        w.w.push_back(std::vector<std::vector<std::complex<float>>>());

    // fill matrices

    // A: 1 ts 2 ant
    for(uint32_t i=0; i<6; i++)
        w.w[0].push_back( convert(mat_ptr(1, 2, i), prefactor));

    // B: 1 ts 4 ant
    for(uint32_t i=0; i<28; i++)
        w.w[1].push_back( convert(mat_ptr(1, 4, i), prefactor));

    // C: 2 ts 2 ant
    for(uint32_t i=0; i<3; i++)
        w.w[2].push_back( convert(mat_ptr(2, 2, i), prefactor));

    // D: 2 ts 4 ant
    for(uint32_t i=0; i<22; i++)
        w.w[3].push_back( convert(mat_ptr(2, 4, i), prefactor));

    // E: 4 ts 4 ant
    for(uint32_t i=0; i<5; i++)
        w.w[4].push_back( convert(mat_ptr(4, 4, i), prefactor));

    // F: 8 ts 8 ant
    for(uint32_t i=0; i<1; i++)
        w.w[5].push_back( convert(mat_ptr(8, 8, i), prefactor));

    // sanity checks

    if(w.w.size() != 6) ERROR("Incorrect size of w.w");

    if(w.w[0].size() != 6)  ERROR("Incorrect size of w.w");
    if(w.w[1].size() != 28) ERROR("Incorrect size of w.w");
    if(w.w[2].size() != 3)  ERROR("Incorrect size of w.w");
    if(w.w[3].size() != 22) ERROR("Incorrect size of w.w");
    if(w.w[4].size() != 5)  ERROR("Incorrect size of w.w");
    if(w.w[5].size() != 1)  ERROR("Incorrect size of w.w");

    for(uint32_t i=0; i<6; i++){
        if(w.w[0][i].size() != 2)   ERROR("Incorrect size of w.w");
    }
    for(uint32_t i=0; i<28; i++){
        if(w.w[1][i].size() != 4)   ERROR("Incorrect size of w.w");
    }
    for(uint32_t i=0; i<3; i++){
        if(w.w[2][i].size() != 4)   ERROR("Incorrect size of w.w");
    }
    for(uint32_t i=0; i<22; i++){
        if(w.w[3][i].size() != 8)   ERROR("Incorrect size of w.w");
    }
    for(uint32_t i=0; i<5; i++){
        if(w.w[4][i].size() != 16)  ERROR("Incorrect size of w.w");
    }
    for(uint32_t i=0; i<1; i++){
        if(w.w[5][i].size() != 64)  ERROR("Incorrect size of w.w");
    }
}

}
