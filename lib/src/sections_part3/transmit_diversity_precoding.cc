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

#include "dect2020/sections_part3/transmit_diversity_precoding.h"
#include "dect2020/utils/debug.h"

// matrices copied from matlab code
// matlab accepts 1 as complex double, here 1 is an integer
// converted to std::complex<float> in function get_y_i()

// complex marker
#define q 2

// two transmit streams
static std::vector<int32_t> Y0_2 = {1, 0, q, 0,
                                    0,-1, 0, q,
                                    0, 1, 0, q,
                                    1, 0,-q, 0};

// four transmit streams
static std::vector<int32_t> Y0_4 = {1, 0, q, 0,
                                    0,-1, 0, q,
                                    0, 0, 0, 0,
                                    0, 0, 0, 0,
                                    0, 1, 0, q,
                                    1, 0,-q, 0,
                                    0, 0, 0, 0,
                                    0, 0, 0, 0};

static std::vector<int32_t> Y1_4 = {0, 0, 0, 0,
                                    0, 0, 0, 0,
                                    1, 0, q, 0,
                                    0,-1, 0, q,
                                    0, 0, 0, 0,
                                    0, 0, 0, 0,
                                    0, 1, 0, q,
                                    1, 0,-q, 0};

static std::vector<int32_t> Y2_4 = {1, 0, q, 0,
                                    0, 0, 0, 0,
                                    0,-1, 0, q,
                                    0, 0, 0, 0,
                                    0, 1, 0, q,
                                    0, 0, 0, 0,
                                    1, 0,-q, 0,
                                    0, 0, 0, 0};

static std::vector<int32_t> Y3_4 = {0, 0, 0, 0,
                                    1, 0, q, 0,
                                    0, 0, 0, 0,
                                    0,-1, 0, q,
                                    0, 0, 0, 0,
                                    0, 1, 0, q,
                                    0, 0, 0, 0,
                                    1, 0,-q, 0};

static std::vector<int32_t> Y4_4 = {1, 0, q, 0,
                                    0, 0, 0, 0,
                                    0, 0, 0, 0,
                                    0,-1, 0, q,
                                    0, 1, 0, q,
                                    0, 0, 0, 0,
                                    0, 0, 0, 0,
                                    1, 0,-q, 0};

static std::vector<int32_t> Y5_4 = {0, 0, 0, 0,
                                    1, 0, q, 0,
                                    0,-1, 0, q,
                                    0, 0, 0, 0,
                                    0, 0, 0, 0,
                                    0, 1, 0, q,
                                    1, 0,-q, 0,
                                    0, 0, 0, 0};

// eight transmit streams
static std::vector<int32_t> Y0_8 = {    1, 0, q, 0,
                                        0,-1, 0, q,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 1, 0, q,
                                        1, 0,-q, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0};

static std::vector<int32_t> Y1_8 = {    0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        1, 0, q, 0,
                                        0,-1, 0, q,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 1, 0, q,
                                        1, 0,-q, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0};

static std::vector<int32_t> Y2_8 = {    0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        1, 0, q, 0,
                                        0,-1, 0, q,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 1, 0, q,
                                        1, 0,-q, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0};

static std::vector<int32_t> Y3_8 = {    0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        1, 0, q, 0,
                                        0,-1, 0, q,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 1, 0, q,
                                        1, 0,-q, 0};

static std::vector<int32_t> Y4_8 = {    1, 0, q, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0,-1, 0, q,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 1, 0, q,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        1, 0,-q, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0};

static std::vector<int32_t> Y5_8 = {    0, 0, 0, 0,
                                        1, 0, q, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0,-1, 0, q,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 1, 0, q,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        1, 0,-q, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0};


static std::vector<int32_t> Y6_8 = {    0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        1, 0, q, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0,-1, 0, q,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 1, 0, q,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        1, 0,-q, 0,
                                        0, 0, 0, 0};

static std::vector<int32_t> Y7_8 = {    0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        1, 0, q, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0,-1, 0, q,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 1, 0, q,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        1, 0, -q, 0};

static std::vector<int32_t> Y8_8 = {    1, 0, q, 0,
                                        0, 0, 0, 0,
                                        0,-1, 0, q,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 1, 0, q,
                                        0, 0, 0, 0,
                                        1, 0,-q, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0};

static std::vector<int32_t> Y9_8 = {    0, 0, 0, 0,
                                        1, 0, q, 0,
                                        0, 0, 0, 0,
                                        0,-1, 0, q,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 1, 0, q,
                                        0, 0, 0, 0,
                                        1, 0,-q, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0};

static std::vector<int32_t> Y10_8 = {   0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        1, 0, q, 0,
                                        0, 0, 0, 0,
                                        0,-1, 0, q,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 1, 0, q,
                                        0, 0, 0, 0,
                                        1, 0,-q, 0,
                                        0, 0, 0, 0};

static std::vector<int32_t> Y11_8 = {   0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        1, 0, q, 0,
                                        0, 0, 0, 0,
                                        0,-1, 0, q,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 0, 0, 0,
                                        0, 1, 0, q,
                                        0, 0, 0, 0,
                                        1, 0,-q, 0};

static std::vector<int32_t> mat_ptr(uint32_t N_TS, uint32_t index_i_mod_x){

    std::vector<int32_t> ret;

    if(N_TS == 2){
        ret = Y0_2;
    }
    else if(N_TS == 4){
        if(index_i_mod_x == 0) ret = Y0_4;
        else if(index_i_mod_x == 1) ret = Y1_4;
        else if(index_i_mod_x == 2) ret = Y2_4;
        else if(index_i_mod_x == 3) ret = Y3_4;
        else if(index_i_mod_x == 4) ret = Y4_4;
        else if(index_i_mod_x == 5) ret = Y5_4;
        else ERROR("Unknown index_i_mod_x=%d", index_i_mod_x);
    }
    else if(N_TS == 8){
        if(index_i_mod_x == 0) ret = Y0_8;
        else if(index_i_mod_x == 1) ret = Y1_8;
        else if(index_i_mod_x == 2) ret = Y2_8;
        else if(index_i_mod_x == 3) ret = Y3_8;
        else if(index_i_mod_x == 4) ret = Y4_8;
        else if(index_i_mod_x == 5) ret = Y5_8;
        else if(index_i_mod_x == 6) ret = Y6_8;
        else if(index_i_mod_x == 7) ret = Y7_8;
        else if(index_i_mod_x == 8) ret = Y8_8;
        else if(index_i_mod_x == 9) ret = Y9_8;
        else if(index_i_mod_x == 10) ret = Y10_8;
        else if(index_i_mod_x == 11) ret = Y11_8;
        else ERROR("Unknown index_i_mod_x=%d", index_i_mod_x);
    }

    return ret;
}

static std::vector<std::complex<float>> convert(const std::vector<int32_t> in, float prefactor){

    std::vector<std::complex<float>> ret;

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

namespace dect2020{


void get_y_i(y_i_t& y_i, float prefactor){

    y_i.prefactor = prefactor;

    y_i.y_i.push_back(std::vector<std::vector<std::complex<float>>>());
    y_i.y_i.push_back(std::vector<std::vector<std::complex<float>>>());
    y_i.y_i.push_back(std::vector<std::vector<std::complex<float>>>());

    // N_TS = 2
    y_i.y_i[0].push_back( convert(mat_ptr(2, 0), prefactor));

    // N_TS = 4
    for(uint32_t i=0; i<6; i++)
        y_i.y_i[1].push_back( convert(mat_ptr(4, i), prefactor));

    // N_TS = 8
    for(uint32_t i=0; i<12; i++)
        y_i.y_i[2].push_back( convert(mat_ptr(8, i), prefactor));

    // sanity checks
    if(y_i.y_i.size() != 3) ERROR("Incorrect size of y_i.y_i");

    if(y_i.y_i[0].size() != 1)  ERROR("Incorrect size of y_i.y_i");
    if(y_i.y_i[1].size() != 6)  ERROR("Incorrect size of y_i.y_i");
    if(y_i.y_i[2].size() != 12) ERROR("Incorrect size of y_i.y_i");

    // N_TS = 2
    if(y_i.y_i[0][0].size() != 16)     ERROR("Incorrect size of y_i.y_i");

    // N_TS = 4
    for(uint32_t i=0; i<6; i++){
        if(y_i.y_i[1][i].size() != 32) ERROR("Incorrect size of y_i.y_i");
    }

    // N_TS = 8
    for(uint32_t i=0; i<12; i++){
        if(y_i.y_i[2][i].size() != 64) ERROR("Incorrect size of y_i.y_i");
    }
}

}
