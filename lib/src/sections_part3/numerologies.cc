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

#include <iostream>

#include "dect2020/sections_part3/numerologies.h"

namespace dect2020 {

void get_numerologies(numerologies_t& q, uint32_t u, uint32_t b){
    q.u = u;
    q.b = b;

    q.delta_u_f = u*27000;
    q.T_u_symb = (64.0+8.0)/64.0 / ((double) q.delta_u_f);
    q.N_SLOT_u_symb = u*10;
    q.N_SLOT_u_subslot = u*2;

    switch(u){
        case 1:
            q.GI_u = 4.0/9.0 * (double) q.T_u_symb;     // Figure 5.1-1
            break;
        case 2:
            q.GI_u = (double) q.T_u_symb;               // Figure 5.1-2
            break;
        case 4:
            q.GI_u = (double) q.T_u_symb;               // Figure 5.1-2
            break;
        case 8:
            q.GI_u = 2.0 * (double) q.T_u_symb;         // Figure 5.1-3
            break;
    }

    q.B_u_b_DFT    = b*64*q.delta_u_f;
    q.T_u_b_s      = 1.0 / (double) q.B_u_b_DFT;
    q.N_b_DFT      = b*64;
    q.N_b_CP       = b*8;
    q.N_b_OCC      = b*56;
    q.B_u_b_TX     = q.N_b_OCC*q.delta_u_f;

    q.n_guards_top = (q.N_b_DFT - q.N_b_OCC)/2 - 1;
    q.n_guards_bottom = q.n_guards_top + 1;
}

void print_numerologies(numerologies_t& q){
    std::cout << "Numerologies:     " << std::endl;
    std::cout << "u:                " << q.u << std::endl;
    std::cout << "b:                " << q.b << std::endl;

    std::cout << "delta_u_f:        " << q.delta_u_f << std::endl;
    std::cout << "T_u_symb:         " << q.T_u_symb << std::endl;
    std::cout << "N_SLOT_u_symb:    " << q.N_SLOT_u_symb << std::endl;
    std::cout << "N_SLOT_u_subslot: " << q.N_SLOT_u_subslot << std::endl;
    std::cout << "GI_u:             " << q.GI_u << std::endl;

    std::cout << "B_u_b_DFT:        " << q.B_u_b_DFT << std::endl;
    std::cout << "T_u_b_s:          " << q.T_u_b_s << std::endl;
    std::cout << "N_b_DFT:          " << q.N_b_DFT << std::endl;
    std::cout << "N_b_CP:           " << q.N_b_CP << std::endl;
    std::cout << "N_b_OCC:          " << q.N_b_OCC << std::endl;
    std::cout << "B_u_b_TX:         " << q.B_u_b_TX << std::endl;

    // also calculate the guards
    std::cout << "n_guards_top:     " << q.n_guards_top << std::endl;
    std::cout << "n_guards_bottom:  " << q.n_guards_bottom << std::endl;
    std::cout << std::endl;
}

}
