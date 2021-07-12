#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Author: Maxim Penner <maxim.penner@ikt.uni-hannover.de>
#

#set(DECT2020_MAC_SOURCES
#    ${CMAKE_SOURCE_DIR}/lib/src/max/abc.cc
#    ${CMAKE_SOURCE_DIR}/lib/src/max/abc.cc)
#add_library(dect2020_mac OBJECT ${DECT2020_MAC_SOURCES})

set(DECT2020_PHY_SOURCES
    ${CMAKE_SOURCE_DIR}/lib/src/phy/packet_sizes.cc
    ${CMAKE_SOURCE_DIR}/lib/src/phy/transport_channels/trch.cc)
add_library(dect2020_phy OBJECT ${DECT2020_PHY_SOURCES})

set(DECT2020_SOURCES
    ${CMAKE_SOURCE_DIR}/lib/src/sections_part3/beamforming_and_antenna_port_mapping.cc
    ${CMAKE_SOURCE_DIR}/lib/src/sections_part3/drs.cc
    ${CMAKE_SOURCE_DIR}/lib/src/sections_part3/mcs.cc
    ${CMAKE_SOURCE_DIR}/lib/src/sections_part3/numerologies.cc
    ${CMAKE_SOURCE_DIR}/lib/src/sections_part3/pcc.cc
    ${CMAKE_SOURCE_DIR}/lib/src/sections_part3/pcc_enc.cc
    ${CMAKE_SOURCE_DIR}/lib/src/sections_part3/pdc.cc
    ${CMAKE_SOURCE_DIR}/lib/src/sections_part3/pdc_enc.cc
    ${CMAKE_SOURCE_DIR}/lib/src/sections_part3/physical_resources.cc
    ${CMAKE_SOURCE_DIR}/lib/src/sections_part3/radio_device_class.cc
    ${CMAKE_SOURCE_DIR}/lib/src/sections_part3/stf.cc
    ${CMAKE_SOURCE_DIR}/lib/src/sections_part3/tm_mode.cc
    ${CMAKE_SOURCE_DIR}/lib/src/sections_part3/transmission_packet_structure.cc
    ${CMAKE_SOURCE_DIR}/lib/src/sections_part3/transmit_diversity_precoding.cc
    ${CMAKE_SOURCE_DIR}/lib/src/sections_part3/transport_block_size.cc
    ${CMAKE_SOURCE_DIR}/lib/src/sections_part4/physical_header_field.cc
    ${CMAKE_SOURCE_DIR}/lib/src/utils/debug.cc
    ${CMAKE_SOURCE_DIR}/lib/src/utils/math_functions.cc)
add_library(dect2020 OBJECT ${DECT2020_SOURCES})

########################################################################
# Boost is required
########################################################################
if(NOT Boost_FOUND)
    message(FATAL_ERROR "Boost required to compile srsUE and ")
endif()

########################################################################
# Setup the include and linker paths
########################################################################
include_directories(
    ${Boost_INCLUDE_DIRS}
    ${SEC_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}
)

link_directories(
    ${Boost_LIBRARY_DIRS}
    ${SEC_LIBRARY_DIRS}
)

########################################################################
# Add subdirectories
########################################################################

# Link libstdc++ and libgcc
if(BUILD_STATIC)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
endif(BUILD_STATIC)

if (RPATH)
  set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif (RPATH)

add_executable(coding_plcf coding_plcf.cc)
target_link_libraries(coding_plcf
                        srsran_phy
                        dect2020
                        #dect2020_mac
                        dect2020_phy
                        ${CMAKE_THREAD_LIBS_INIT}
                        ${Boost_LIBRARIES})

add_executable(coding_tb_awgn coding_tb_awgn.cc)
target_link_libraries(coding_tb_awgn
                        srsran_phy
                        dect2020
                        #dect2020_mac
                        dect2020_phy
                        ${CMAKE_THREAD_LIBS_INIT}
                        ${Boost_LIBRARIES})

add_executable(coding_tb_configurations coding_tb_configurations.cc)
target_link_libraries(coding_tb_configurations
                        srsran_phy
                        dect2020
                        #dect2020_mac
                        dect2020_phy
                        ${CMAKE_THREAD_LIBS_INIT}
                        ${Boost_LIBRARIES})

add_executable(modulation modulation.cc)
target_link_libraries(modulation
                        srsran_phy
                        dect2020
                        #dect2020_mac
                        dect2020_phy
                        ${CMAKE_THREAD_LIBS_INIT}
                        ${Boost_LIBRARIES})

add_executable(subc_allocation subc_allocation.cc)
target_link_libraries(subc_allocation
                        srsran_phy
                        dect2020
                        #dect2020_mac
                        dect2020_phy
                        ${CMAKE_THREAD_LIBS_INIT}
                        ${Boost_LIBRARIES})
