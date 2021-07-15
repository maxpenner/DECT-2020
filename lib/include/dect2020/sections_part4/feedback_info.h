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

#ifndef DECT2020_FEEDBACK_INFO_H
#define DECT2020_FEEDBACK_INFO_H

#include <cstdint>

// ETSI TS 103 636-4 V1.1.1 (2020-07), 6.2.2

namespace dect2020 {

typedef struct{
    uint8_t HARQ_Process_number;
    uint8_t Transmission_feedback;
    uint8_t CQI;
    uint8_t Buffer_Status;

    uint16_t feedback_info;
} feedback_info_f1_t;

typedef struct{
    uint8_t CQI;
    uint8_t Buffer_Status;
    uint8_t MIMO_feedback;
    uint8_t Codebook_index;

    uint16_t feedback_info;
} feedback_info_f2_t;

typedef struct{
    uint8_t HARQ_Process_number_0;
    uint8_t Transmission_feedback_0;
    uint8_t HARQ_Process_number_1;
    uint8_t Transmission_feedback_1;
    uint8_t CQI;

    uint16_t feedback_info;
} feedback_info_f3_t;

typedef struct{
    uint8_t HARQ_feedback_bitmap;
    uint8_t CQI;

    uint16_t feedback_info;
} feedback_info_f4_t;

typedef struct{
    uint8_t HARQ_Process_number;
    uint8_t Transmission_feedback;
    uint8_t MIMO_feedback;
    uint8_t Codebook_index;

    uint16_t feedback_info;
} feedback_info_f5_t;

uint16_t get_feedback_info(feedback_info_f1_t& feedback_info_f1);
uint16_t get_feedback_info(feedback_info_f2_t& feedback_info_f2);
uint16_t get_feedback_info(feedback_info_f3_t& feedback_info_f3);
uint16_t get_feedback_info(feedback_info_f4_t& feedback_info_f4);
uint16_t get_feedback_info(feedback_info_f5_t& feedback_info_f5);

void get_feedback_info_rev(feedback_info_f1_t& feedback_info_f1);
void get_feedback_info_rev(feedback_info_f2_t& feedback_info_f2);
void get_feedback_info_rev(feedback_info_f3_t& feedback_info_f3);
void get_feedback_info_rev(feedback_info_f4_t& feedback_info_f4);
void get_feedback_info_rev(feedback_info_f5_t& feedback_info_f5);

uint8_t get_cqi(uint32_t mcs_index);

uint8_t get_buffer_status(uint32_t buffer_size);

}

#endif