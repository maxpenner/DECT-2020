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

#ifndef DECT2020_TRANSMIT_DIVERSITY_PRECODING_H
#define DECT2020_TRANSMIT_DIVERSITY_PRECODING_H

#include <cstdint>
#include <vector>
#include <complex>

// ETSI TS 103 636-3 V1.1.1 (2020-07), 6.3.3.2

namespace dect2020 {

typedef struct{

    float prefactor;

    /*
     * vector y_i will have 3 rows, each containing one matrix
     *
     * row 0:   [1][16]     elements (one matrix for N_TS=1, 16 values)
     * row 1:   [6][32]     elements (six matrices for N_TS=4, 24 values)
     * row 2:   [12][64]    elements (twelve matrices for N_S=8, 64 elements)
     */
    std::vector< std::vector< std::vector < std::complex<float> > > > y_i;
} y_i_t;

void get_y_i(y_i_t& y_i, float prefactor);

}

#endif
