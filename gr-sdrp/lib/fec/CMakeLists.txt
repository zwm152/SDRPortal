# Copyright 2011,2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

set(fec_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/viterbi27_port.c
    ${CMAKE_CURRENT_SOURCE_DIR}/decode_rs_ccsds.c
    ${CMAKE_CURRENT_SOURCE_DIR}/encode_rs_ccsds.c
    ${CMAKE_CURRENT_SOURCE_DIR}/fec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/metrics.c
    ${CMAKE_CURRENT_SOURCE_DIR}/viterbi.c
)

list(APPEND sdrp_sources ${fec_sources})

