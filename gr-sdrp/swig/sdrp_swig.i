/* -*- c++ -*- */

#define DIGITAL_API
//#define TRELLIS_API
#define SDRP_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "sdrp_swig_doc.i"
%include "gnuradio/digital/constellation.h"
//%include "gnuradio/trellis/core_algorithms.h"

%{
#include "sdrp/dsn_pn_tx.h"
#include "sdrp/dsn_pn_rx.h"
#include "sdrp/dsn_sequential_tx.h"
#include "sdrp/dsn_sequential_rx.h"
#include "sdrp/square_sub_tracker_ff.h"
#include "sdrp/square_data_tracker_ff.h"
#include "sdrp/sweep_generator_cc.h"
#include "sdrp/ccsds_aos_framer.h"
#include "sdrp/ccsds_tm_framer.h"
#include "sdrp/ccsds_tm_conv_decoder.h"
#include "sdrp/ccsds_tm_derandomizer.h"
#include "sdrp/ccsds_tm_tx.h"
#include "sdrp/constellation_soft_receiver_cf.h"
#include "sdrp/correlate_long_access_code_bb.h"
#include "sdrp/correlate_soft_access_tag_ff.h"
#include "sdrp/sdrp_packet_interpreter.h"
#include "sdrp/pll_refout_freqout_ccf.h"
#include "sdrp/costas_refout_freqout_ccf.h"
#include "sdrp/pll_freq_acq_cc.h"
#include "sdrp/ws_sink_c.h"
%}

%include "sdrp/dsn_pn_tx.h"
GR_SWIG_BLOCK_MAGIC2(sdrp, dsn_pn_tx);

%include "sdrp/dsn_pn_rx.h"
GR_SWIG_BLOCK_MAGIC2(sdrp, dsn_pn_rx);

%include "sdrp/dsn_sequential_tx.h"
GR_SWIG_BLOCK_MAGIC2(sdrp, dsn_sequential_tx);

%include "sdrp/dsn_sequential_rx.h"
GR_SWIG_BLOCK_MAGIC2(sdrp, dsn_sequential_rx);

%include "sdrp/square_sub_tracker_ff.h"
GR_SWIG_BLOCK_MAGIC2(sdrp, square_sub_tracker_ff);

%include "sdrp/square_data_tracker_ff.h"
GR_SWIG_BLOCK_MAGIC2(sdrp, square_data_tracker_ff);

%include "sdrp/sweep_generator_cc.h"
GR_SWIG_BLOCK_MAGIC2(sdrp, sweep_generator_cc);

%include "sdrp/ccsds_aos_framer.h"
GR_SWIG_BLOCK_MAGIC2(sdrp, ccsds_aos_framer);

%include "sdrp/ccsds_tm_framer.h"
GR_SWIG_BLOCK_MAGIC2(sdrp, ccsds_tm_framer);

%include "sdrp/ccsds_tm_conv_decoder.h"
GR_SWIG_BLOCK_MAGIC2(sdrp, ccsds_tm_conv_decoder);

%include "sdrp/ccsds_tm_derandomizer.h"
GR_SWIG_BLOCK_MAGIC2(sdrp, ccsds_tm_derandomizer);

%include "sdrp/ccsds_tm_tx.h"
GR_SWIG_BLOCK_MAGIC2(sdrp, ccsds_tm_tx);

%include "sdrp/constellation_soft_receiver_cf.h"
GR_SWIG_BLOCK_MAGIC2(sdrp, constellation_soft_receiver_cf);

%include "sdrp/pll_refout_freqout_ccf.h"
GR_SWIG_BLOCK_MAGIC2(sdrp, pll_refout_freqout_ccf);

%include "sdrp/costas_refout_freqout_ccf.h"
GR_SWIG_BLOCK_MAGIC2(sdrp, costas_refout_freqout_ccf);

%include "sdrp/correlate_long_access_code_bb.h"
GR_SWIG_BLOCK_MAGIC2(sdrp, correlate_long_access_code_bb);

%include "sdrp/correlate_soft_access_tag_ff.h"
GR_SWIG_BLOCK_MAGIC2(sdrp, correlate_soft_access_tag_ff);

%include "sdrp/sdrp_packet_interpreter.h"
GR_SWIG_BLOCK_MAGIC2(sdrp, sdrp_packet_interpreter);

%include "sdrp/pll_freq_acq_cc.h"
GR_SWIG_BLOCK_MAGIC2(sdrp, pll_freq_acq_cc);

%include "sdrp/ws_sink_c.h"
GR_SWIG_BLOCK_MAGIC2(sdrp, ws_sink_c);

