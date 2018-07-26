// Written by Adrian Musceac YO8RZZ , started March 2016.
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "gr_mod_2fsk_sdr.h"

gr_mod_2fsk_sdr_sptr make_gr_mod_2fsk_sdr(int sps, int samp_rate, int carrier_freq,
                                          int filter_width)
{
    return gnuradio::get_initial_sptr(new gr_mod_2fsk_sdr(sps, samp_rate, carrier_freq,
                                                      filter_width));
}

gr_mod_2fsk_sdr::gr_mod_2fsk_sdr(int sps, int samp_rate, int carrier_freq,
                                 int filter_width) :
    gr::hier_block2 ("gr_mod_2fsk_sdr",
                      gr::io_signature::make (1, 1, sizeof (char)),
                      gr::io_signature::make (1, 1, sizeof (gr_complex)))
{
    std::vector<float> constellation;
    constellation.push_back(-1);
    constellation.push_back(1);
    std::vector<int> polys;
    polys.push_back(109);
    polys.push_back(79);

    _samples_per_symbol = sps;
    _samp_rate =samp_rate;
    _carrier_freq = carrier_freq;
    _filter_width = filter_width;

    _packed_to_unpacked = gr::blocks::packed_to_unpacked_bb::make(1,gr::GR_MSB_FIRST);
    _scrambler = gr::digital::scrambler_bb::make(0x8A, 0x7F ,7);

     gr::fec::code::cc_encoder::sptr encoder = gr::fec::code::cc_encoder::make(80, 7, 2, polys);
    _encode_ccsds = gr::fec::encoder::make(encoder, 1, 1);
    _chunks_to_symbols = gr::digital::chunks_to_symbols_bf::make(constellation);
    _freq_modulator = gr::analog::frequency_modulator_fc::make((2*M_PI/2)/(_samples_per_symbol));
    _repeat = gr::blocks::repeat::make(4, _samples_per_symbol);
    _amplify = gr::blocks::multiply_const_cc::make(0.2,1);
    _bb_gain = gr::blocks::multiply_const_cc::make(1,1);
    _filter = gr::filter::fft_filter_ccf::make(
                1,gr::filter::firdes::low_pass(
                    1, _samp_rate, _filter_width, 600,gr::filter::firdes::WIN_BLACKMAN_HARRIS));

    connect(self(),0,_packed_to_unpacked,0);
    connect(_packed_to_unpacked,0,_scrambler,0);
    connect(_scrambler,0,_encode_ccsds,0);
    connect(_encode_ccsds,0,_chunks_to_symbols,0);
    connect(_chunks_to_symbols,0,_repeat,0);

    connect(_repeat,0,_freq_modulator,0);
    connect(_freq_modulator,0,_amplify,0);
    connect(_amplify,0,_bb_gain,0);
    connect(_bb_gain,0,_filter,0);
    connect(_filter,0,self(),0);
}

void gr_mod_2fsk_sdr::set_bb_gain(int value)
{
    _bb_gain->set_k(value);
}




