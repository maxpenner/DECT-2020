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

#include "dect2020/sections_part3/physical_resources.h"

static void fill_subc(int* out, int N_b_OCC){
    uint32_t idx = 0;
    for(int i=-N_b_OCC/2; i<=-1; i++)
        out[idx++] = i;
    for(int i=1; i<=N_b_OCC/2; i++)
        out[idx++] = i;
}

namespace dect2020 {

void get_k_b_OCC(k_b_OCC_t& k_b_OCC){
    fill_subc(k_b_OCC.b_1,  k_b_OCC.N_b_OCC[0]);
    fill_subc(k_b_OCC.b_2,  k_b_OCC.N_b_OCC[1]);
    fill_subc(k_b_OCC.b_4,  k_b_OCC.N_b_OCC[2]);
    fill_subc(k_b_OCC.b_8,  k_b_OCC.N_b_OCC[3]);
    fill_subc(k_b_OCC.b_12, k_b_OCC.N_b_OCC[4]);
    fill_subc(k_b_OCC.b_16, k_b_OCC.N_b_OCC[5]);
}

void get_k_b_OCC(k_b_OCC_vec_t& k_b_OCC_vec){

    k_b_OCC_vec.b_x.clear();

    for(uint32_t b_idx=0; b_idx<6; b_idx++){

        int N_b_OCC = k_b_OCC_vec.N_b_OCC[b_idx];

        k_b_OCC_vec.b_x.push_back(std::vector<int>());

        for(int i=-N_b_OCC/2; i<=-1; i++)
            k_b_OCC_vec.b_x[b_idx].push_back(i);
        for(int i=1; i<=N_b_OCC/2; i++)
            k_b_OCC_vec.b_x[b_idx].push_back(i);
    }
}

}
