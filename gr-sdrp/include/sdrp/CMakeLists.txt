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

########################################################################
# Install public header files
########################################################################
install(FILES
    api.h
    dsn_pn_tx.h
    dsn_pn_rx.h
    dsn_sequential_tx.h
    dsn_sequential_rx.h
    square_sub_tracker_ff.h
    square_data_tracker_ff.h
    sweep_generator_cc.h
    correlate_long_access_code_bb.h
    correlate_soft_access_tag_ff.h
    constellation_soft_receiver_cf.h
    ccsds_tm_framer.h
    ccsds_tm_tx.h
    pll_refout_freqout_ccf.h
    costas_refout_freqout_ccf.h
    sdrp_packet_interpreter.h
    pll_freq_acq_cc.h
    ccsds_aos_framer.h DESTINATION include/sdrp
)
