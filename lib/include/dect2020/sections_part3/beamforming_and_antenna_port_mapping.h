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

#ifndef DECT2020_BEAMFORMING_AND_ANTENNA_PORT_MAPPING_H
#define DECT2020_BEAMFORMING_AND_ANTENNA_PORT_MAPPING_H

#include <cstdint>
#include <vector>
#include <complex>

// ETSI TS 103 636-3 V1.1.1 (2020-07), 6.3.4

namespace dect2020 {

typedef struct{

    float prefactor;

    /*
     * vector y_STF_i_b_x will have 6 rows, each containing one matrix
     *
     * row 0:   [ 6][ 2]
     * row 1:   [28][ 4]
     * row 2:   [ 3][ 4]
     * row 3:   [22][ 8]
     * row 4:   [ 5][16]
     * row 5:   [ 1][64]
     */
    std::vector< std::vector< std::vector < std::complex<float> > > > w;
} w_t;

void get_w(w_t& w, float prefactor);

}

#endif
