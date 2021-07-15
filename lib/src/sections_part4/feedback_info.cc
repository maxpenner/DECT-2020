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
#include <string.h>

#include "dect2020/sections_part4/feedback_info.h"
#include "dect2020/utils/debug.h"

namespace dect2020 {

uint16_t get_feedback_info(feedback_info_f1_t& feedback_info_f1){
    uint16_t res = 0;

    res = (res & 0xF1FF) | (((uint16_t) feedback_info_f1.HARQ_Process_number) << 9);
    res = (res & 0xFEFF) | (((uint16_t) feedback_info_f1.Transmission_feedback) << 8);
    res = (res & 0xFF0F) | (((uint16_t) feedback_info_f1.CQI) << 4);
    res = (res & 0xFFF0) | ((uint16_t) feedback_info_f1.Buffer_Status);

    feedback_info_f1.feedback_info = res;

    return res;
}

uint16_t get_feedback_info(feedback_info_f2_t& feedback_info_f2){
    uint16_t res = 0;

    res = (res & 0xF0FF) | (((uint16_t) feedback_info_f2.CQI) << 8);
    res = (res & 0xFF0F) | (((uint16_t) feedback_info_f2.Buffer_Status) << 4);
    res = (res & 0xFFF7) | (((uint16_t) feedback_info_f2.MIMO_feedback) << 3);
    res = (res & 0xFFF8) | ((uint16_t) feedback_info_f2.Codebook_index);

    feedback_info_f2.feedback_info = res;

    return res;
}

uint16_t get_feedback_info(feedback_info_f3_t& feedback_info_f3){
    uint16_t res = 0;

    res = (res & 0xF1FF) | (((uint16_t) feedback_info_f3.HARQ_Process_number_0) << 9);
    res = (res & 0xFEFF) | (((uint16_t) feedback_info_f3.Transmission_feedback_0) << 8);
    res = (res & 0xFF1F) | (((uint16_t) feedback_info_f3.HARQ_Process_number_1) << 5);
    res = (res & 0xFFEF) | (((uint16_t) feedback_info_f3.Transmission_feedback_1) << 4);
    res = (res & 0xFFF0) | ((uint16_t) feedback_info_f3.CQI);

    feedback_info_f3.feedback_info = res;

    return res;
}

uint16_t get_feedback_info(feedback_info_f4_t& feedback_info_f4){
    uint16_t res = 0;

    res = (res & 0xF00F) | (((uint16_t) feedback_info_f4.HARQ_feedback_bitmap) << 4);
    res = (res & 0xFFF0) | ((uint16_t) feedback_info_f4.CQI);

    feedback_info_f4.feedback_info = res;

    return res;
}

uint16_t get_feedback_info(feedback_info_f5_t& feedback_info_f5){
    uint16_t res = 0;

    res = (res & 0xF1FF) | (((uint16_t) feedback_info_f5.HARQ_Process_number) << 9);
    res = (res & 0xFEFF) | (((uint16_t) feedback_info_f5.Transmission_feedback) << 8);
    res = (res & 0xFF3F) | (((uint16_t) feedback_info_f5.MIMO_feedback) << 6);
    res = (res & 0xFFC0) | ((uint16_t) feedback_info_f5.Codebook_index);

    feedback_info_f5.feedback_info = res;

    return res;
}

void get_feedback_info_rev(feedback_info_f1_t& feedback_info_f1){
    uint16_t res = feedback_info_f1.feedback_info;

    feedback_info_f1.HARQ_Process_number    = (res & 0xE00) >> 9;
    feedback_info_f1.Transmission_feedback  = (res & 0x100) >> 8;
    feedback_info_f1.CQI                    = (res & 0xF0) >> 4;
    feedback_info_f1.Buffer_Status          = (res & 0xF);

    res = (res & 0xF1FF) | (((uint16_t) feedback_info_f1.HARQ_Process_number) << 9);
    res = (res & 0xFEFF) | (((uint16_t) feedback_info_f1.Transmission_feedback) << 8);
    res = (res & 0xFF0F) | (((uint16_t) feedback_info_f1.CQI) << 4);
    res = (res & 0xFFF0) | ((uint16_t) feedback_info_f1.Buffer_Status);
}

void get_feedback_info_rev(feedback_info_f2_t& feedback_info_f2){
    uint16_t res = feedback_info_f2.feedback_info;

    feedback_info_f2.CQI            = (res & 0xF00) >> 8;
    feedback_info_f2.Buffer_Status  = (res & 0xF0) >> 4;
    feedback_info_f2.MIMO_feedback  = (res & 0x8) >> 3;
    feedback_info_f2.Codebook_index = (res & 0x7);
}

void get_feedback_info_rev(feedback_info_f3_t& feedback_info_f3){
    uint16_t res = feedback_info_f3.feedback_info;

    feedback_info_f3.HARQ_Process_number_0      = (res & 0xE00) >> 9;
    feedback_info_f3.Transmission_feedback_0    = (res & 0x100) >> 8;
    feedback_info_f3.HARQ_Process_number_1      = (res & 0xE0) >> 5;
    feedback_info_f3.Transmission_feedback_1    = (res & 0x10) >> 4;
    feedback_info_f3.CQI                        = (res & 0xF);
}

void get_feedback_info_rev(feedback_info_f4_t& feedback_info_f4){
    uint16_t res = feedback_info_f4.feedback_info;

    feedback_info_f4.HARQ_feedback_bitmap   = (res & 0xFF0) >> 4;
    feedback_info_f4.CQI                    = (res & 0xF);
}

void get_feedback_info_rev(feedback_info_f5_t& feedback_info_f5){
    uint16_t res = feedback_info_f5.feedback_info;

    feedback_info_f5.HARQ_Process_number    = (res & 0xE00) >> 9;
    feedback_info_f5.Transmission_feedback  = (res & 0x100) >> 8;
    feedback_info_f5.MIMO_feedback          = (res & 0xC0) >> 6;
    feedback_info_f5.Codebook_index         = (res & 0x3F);
}

uint8_t get_cqi(uint32_t mcs_index){
    if(mcs_index <= 11)
        return (uint8_t) (mcs_index+1);

    // out of range
    return 0;
}

uint8_t get_buffer_status(uint32_t buffer_size){

    if(buffer_size == 0)                                    return 0;
    else if(0 < buffer_size && buffer_size <= 16)           return 1;
    else if(16 < buffer_size && buffer_size <= 32)          return 2;
    else if(32 < buffer_size && buffer_size <= 64)          return 3;
    else if(64 < buffer_size && buffer_size <= 128)         return 4;
    else if(128 < buffer_size && buffer_size <= 256)        return 5;
    else if(256 < buffer_size && buffer_size <= 512)        return 6;

    // from the standard
    //else if(512 < buffer_size && buffer_size <= 1024)       return 7;

    // replacement
    else if(512 < buffer_size && buffer_size <= 2048)       return 7;

    else if(2048 < buffer_size && buffer_size <= 4096)      return 8;
    else if(4096 < buffer_size && buffer_size <= 8192)      return 9;
    else if(8192 < buffer_size && buffer_size <= 16384)     return 10;
    else if(16384 < buffer_size && buffer_size <= 32768)    return 11;
    else if(32768 < buffer_size && buffer_size <= 65536)    return 12;
    else if(65536 < buffer_size && buffer_size <= 131072)   return 13;
    else if(131072 < buffer_size && buffer_size <= 262144)  return 14;
    else if(262144 < buffer_size)                           return 15;

    return 0;
}

}