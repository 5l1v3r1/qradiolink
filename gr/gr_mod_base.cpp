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

#include "gr_mod_base.h"

gr_mod_base::gr_mod_base(QObject *parent, float device_frequency, float rf_gain,
                           std::string device_args, std::string device_antenna, int freq_corr) :
    QObject(parent)
{
    _device_frequency = device_frequency;
    _top_block = gr::make_top_block("modulator");
    _mode = 9999;
    _vector_source = make_gr_vector_source();
    _audio_source = make_gr_audio_source();

    _carrier_offset = 0;

    _rotator = gr::blocks::rotator_cc::make(2*M_PI*_carrier_offset/1000000);
    _osmosdr_sink = osmosdr::sink::make(device_args);
    _osmosdr_sink->set_sample_rate(1000000);
    _osmosdr_sink->set_antenna(device_antenna);
    _osmosdr_sink->set_center_freq(_device_frequency - _carrier_offset);
    _osmosdr_sink->set_freq_corr(freq_corr);
    _gain_range = _osmosdr_sink->get_gain_range();
    if (!_gain_range.empty())
    {
        double gain =  _gain_range.start() + rf_gain*(_gain_range.stop()-_gain_range.start());
        _osmosdr_sink->set_gain(gain);
    }

    _2fsk = make_gr_mod_2fsk_sdr(25, 1000000, 1700, 4000); // 4000 for non FM demod, 2700 for FM demod
    _2fsk_10k = make_gr_mod_2fsk_sdr(5, 1000000, 1700, 13500, true);
    _4fsk_2k = make_gr_mod_4fsk_sdr(500, 1000000, 1700, 4000);
    _4fsk_10k = make_gr_mod_4fsk_sdr(100, 1000000, 1700, 20000);
    _am = make_gr_mod_am_sdr(0,1000000, 1700, 4000);
    _bpsk_1k = make_gr_mod_bpsk_sdr(50, 1000000, 1700, 1200);
    _bpsk_2k = make_gr_mod_bpsk_sdr(25, 1000000, 1700, 2400);
    _fm_2500 = make_gr_mod_nbfm_sdr(0, 1000000, 1700, 2500);
    _fm_5000 = make_gr_mod_nbfm_sdr(0, 1000000, 1700, 4000);
    _qpsk_2k = make_gr_mod_qpsk_sdr(500, 1000000, 1700, 1300);
    _qpsk_10k = make_gr_mod_qpsk_sdr(100, 1000000, 1700, 6500);
    _qpsk_250k = make_gr_mod_qpsk_sdr(4, 1000000, 1700, 160000);
    _qpsk_video = make_gr_mod_qpsk_sdr(4, 1000000, 1700, 160000);
    _usb = make_gr_mod_ssb_sdr(0, 1000000, 1700, 2500);
    _lsb = make_gr_mod_ssb_sdr(1, 1000000, 1700, 2500);

}

void gr_mod_base::set_mode(int mode)
{
    _top_block->lock();
    _audio_source->flush();
    _vector_source->flush();

    switch(_mode)
    {
    case gr_modem_types::ModemType2FSK2000:
        _top_block->disconnect(_vector_source,0,_2fsk,0);
        _top_block->disconnect(_2fsk,0,_rotator,0);
        _top_block->disconnect(_rotator,0,_osmosdr_sink,0);
        break;
    case gr_modem_types::ModemType2FSK20000:
        _top_block->disconnect(_vector_source,0,_2fsk_10k,0);
        _top_block->disconnect(_2fsk_10k,0,_rotator,0);
        _top_block->disconnect(_rotator,0,_osmosdr_sink,0);
        break;
    case gr_modem_types::ModemType4FSK2000:
        _top_block->disconnect(_vector_source,0,_4fsk_2k,0);
        _top_block->disconnect(_4fsk_2k,0,_rotator,0);
        _top_block->disconnect(_rotator,0,_osmosdr_sink,0);
        break;
    case gr_modem_types::ModemType4FSK20000:
        _top_block->disconnect(_vector_source,0,_4fsk_10k,0);
        _top_block->disconnect(_4fsk_10k,0,_rotator,0);
        _top_block->disconnect(_rotator,0,_osmosdr_sink,0);
        break;
    case gr_modem_types::ModemTypeAM5000:
        _top_block->disconnect(_audio_source,0,_am,0);
        _top_block->disconnect(_am,0,_rotator,0);
        _top_block->disconnect(_rotator,0,_osmosdr_sink,0);
        break;
    case gr_modem_types::ModemTypeBPSK1000:
        _top_block->disconnect(_vector_source,0,_bpsk_1k,0);
        _top_block->disconnect(_bpsk_1k,0,_rotator,0);
        _top_block->disconnect(_rotator,0,_osmosdr_sink,0);
        break;
    case gr_modem_types::ModemTypeBPSK2000:
        _top_block->disconnect(_vector_source,0,_bpsk_2k,0);
        _top_block->disconnect(_bpsk_2k,0,_rotator,0);
        _top_block->disconnect(_rotator,0,_osmosdr_sink,0);
        break;
    case gr_modem_types::ModemTypeNBFM2500:
        _top_block->disconnect(_audio_source,0,_fm_2500,0);
        _top_block->disconnect(_fm_2500,0,_rotator,0);
        _top_block->disconnect(_rotator,0,_osmosdr_sink,0);
        break;
    case gr_modem_types::ModemTypeNBFM5000:
        _top_block->disconnect(_audio_source,0,_fm_5000,0);
        _top_block->disconnect(_fm_5000,0,_rotator,0);
        _top_block->disconnect(_rotator,0,_osmosdr_sink,0);
        break;
    case gr_modem_types::ModemTypeQPSK2000:
        _top_block->disconnect(_vector_source,0,_qpsk_2k,0);
        _top_block->disconnect(_qpsk_2k,0,_rotator,0);
        _top_block->disconnect(_rotator,0,_osmosdr_sink,0);
        break;
    case gr_modem_types::ModemTypeQPSK20000:
        _top_block->disconnect(_vector_source,0,_qpsk_10k,0);
        _top_block->disconnect(_qpsk_10k,0,_rotator,0);
        _top_block->disconnect(_rotator,0,_osmosdr_sink,0);
        break;
    case gr_modem_types::ModemTypeQPSK250000:
        _carrier_offset = 0;
        _osmosdr_sink->set_center_freq(_device_frequency - _carrier_offset);
        _top_block->disconnect(_vector_source,0,_qpsk_250k,0);
        _top_block->disconnect(_qpsk_250k,0,_rotator,0);
        _top_block->disconnect(_rotator,0,_osmosdr_sink,0);
        break;
    case gr_modem_types::ModemTypeQPSKVideo:
        _top_block->disconnect(_vector_source,0,_qpsk_video,0);
        _top_block->disconnect(_qpsk_video,0,_rotator,0);
        _top_block->disconnect(_rotator,0,_osmosdr_sink,0);
        break;
    case gr_modem_types::ModemTypeUSB2500:
        _top_block->disconnect(_audio_source,0,_usb,0);
        _top_block->disconnect(_usb,0,_rotator,0);
        _top_block->disconnect(_rotator,0,_osmosdr_sink,0);
        break;
    case gr_modem_types::ModemTypeLSB2500:
        _top_block->disconnect(_audio_source,0,_lsb,0);
        _top_block->disconnect(_lsb,0,_rotator,0);
        _top_block->disconnect(_rotator,0,_osmosdr_sink,0);
        break;
    default:
        break;
    }

    switch(mode)
    {
    case gr_modem_types::ModemType2FSK2000:
        _carrier_offset = 25000;
        _rotator->set_phase_inc(2*M_PI*_carrier_offset/1000000);
        _osmosdr_sink->set_center_freq(_device_frequency - _carrier_offset);
        _osmosdr_sink->set_sample_rate(1000000);
        _top_block->connect(_vector_source,0,_2fsk,0);
        _top_block->connect(_2fsk,0,_rotator,0);
        _top_block->connect(_rotator,0,_osmosdr_sink,0);
        break;
    case gr_modem_types::ModemType2FSK20000:
        _carrier_offset = 25000;
        _rotator->set_phase_inc(2*M_PI*_carrier_offset/1000000);
        _osmosdr_sink->set_center_freq(_device_frequency - _carrier_offset);
        _osmosdr_sink->set_sample_rate(1000000);
        _top_block->connect(_vector_source,0,_2fsk_10k,0);
        _top_block->connect(_2fsk_10k,0,_rotator,0);
        _top_block->connect(_rotator,0,_osmosdr_sink,0);
        break;
    case gr_modem_types::ModemType4FSK2000:
        _carrier_offset = 25000;
        _rotator->set_phase_inc(2*M_PI*_carrier_offset/1000000);
        _osmosdr_sink->set_center_freq(_device_frequency - _carrier_offset);
        _osmosdr_sink->set_sample_rate(1000000);
        _top_block->connect(_vector_source,0,_4fsk_2k,0);
        _top_block->connect(_4fsk_2k,0,_rotator,0);
        _top_block->connect(_rotator,0,_osmosdr_sink,0);
        break;
    case gr_modem_types::ModemType4FSK20000:
        _carrier_offset = 50000;
        _rotator->set_phase_inc(2*M_PI*_carrier_offset/1000000);
        _osmosdr_sink->set_center_freq(_device_frequency - _carrier_offset);
        _osmosdr_sink->set_sample_rate(1000000);
        _top_block->connect(_vector_source,0,_4fsk_10k,0);
        _top_block->connect(_4fsk_10k,0,_rotator,0);
        _top_block->connect(_rotator,0,_osmosdr_sink,0);
        break;
    case gr_modem_types::ModemTypeAM5000:
        _carrier_offset = 25000;
        _rotator->set_phase_inc(2*M_PI*_carrier_offset/1000000);
        _osmosdr_sink->set_center_freq(_device_frequency - _carrier_offset);
        _osmosdr_sink->set_sample_rate(1000000);
        _top_block->connect(_audio_source,0,_am,0);
        _top_block->connect(_am,0,_rotator,0);
        _top_block->connect(_rotator,0,_osmosdr_sink,0);
        break;
    case gr_modem_types::ModemTypeBPSK1000:
        _carrier_offset = 25000;
        _rotator->set_phase_inc(2*M_PI*_carrier_offset/1000000);
        _osmosdr_sink->set_center_freq(_device_frequency - _carrier_offset);
        _osmosdr_sink->set_sample_rate(1000000);
        _top_block->connect(_vector_source,0,_bpsk_1k,0);
        _top_block->connect(_bpsk_1k,0,_rotator,0);
        _top_block->connect(_rotator,0,_osmosdr_sink,0);
        break;
    case gr_modem_types::ModemTypeBPSK2000:
        _carrier_offset = 25000;
        _rotator->set_phase_inc(2*M_PI*_carrier_offset/1000000);
        _osmosdr_sink->set_center_freq(_device_frequency - _carrier_offset);
        _osmosdr_sink->set_sample_rate(1000000);
        _top_block->connect(_vector_source,0,_bpsk_2k,0);
        _top_block->connect(_bpsk_2k,0,_rotator,0);
        _top_block->connect(_rotator,0,_osmosdr_sink,0);
        break;
    case gr_modem_types::ModemTypeNBFM2500:
        _carrier_offset = 25000;
        _rotator->set_phase_inc(2*M_PI*_carrier_offset/1000000);
        _osmosdr_sink->set_center_freq(_device_frequency - _carrier_offset);
        _osmosdr_sink->set_sample_rate(1000000);
        _top_block->connect(_audio_source,0,_fm_2500,0);
        _top_block->connect(_fm_2500,0,_rotator,0);
        _top_block->connect(_rotator,0,_osmosdr_sink,0);
        break;
    case gr_modem_types::ModemTypeNBFM5000:
        _carrier_offset = 25000;
        _rotator->set_phase_inc(2*M_PI*_carrier_offset/1000000);
        _osmosdr_sink->set_center_freq(_device_frequency - _carrier_offset);
        _osmosdr_sink->set_sample_rate(1000000);
        _top_block->connect(_audio_source,0,_fm_5000,0);
        _top_block->connect(_fm_5000,0,_rotator,0);
        _top_block->connect(_rotator,0,_osmosdr_sink,0);
        break;
    case gr_modem_types::ModemTypeQPSK2000:
        _carrier_offset = 25000;
        _rotator->set_phase_inc(2*M_PI*_carrier_offset/1000000);
        _osmosdr_sink->set_center_freq(_device_frequency - _carrier_offset);
        _osmosdr_sink->set_sample_rate(1000000);
        _top_block->connect(_vector_source,0,_qpsk_2k,0);
        _top_block->connect(_qpsk_2k,0,_rotator,0);
        _top_block->connect(_rotator,0,_osmosdr_sink,0);
        break;
    case gr_modem_types::ModemTypeQPSK20000:
        _carrier_offset = 25000;
        _rotator->set_phase_inc(2*M_PI*_carrier_offset/1000000);
        _osmosdr_sink->set_center_freq(_device_frequency - _carrier_offset);
        _osmosdr_sink->set_sample_rate(1000000);
        _top_block->connect(_vector_source,0,_qpsk_10k,0);
        _top_block->connect(_qpsk_10k,0,_rotator,0);
        _top_block->connect(_rotator,0,_osmosdr_sink,0);
        break;
    case gr_modem_types::ModemTypeQPSK250000:
        _carrier_offset = 250000;
        _rotator->set_phase_inc(2*M_PI*_carrier_offset/1000000);
        _osmosdr_sink->set_center_freq(_device_frequency - _carrier_offset);
        _osmosdr_sink->set_sample_rate(1000000);
        _top_block->connect(_vector_source,0,_qpsk_250k,0);
        _top_block->connect(_qpsk_250k,0,_rotator,0);
        _top_block->connect(_rotator,0,_osmosdr_sink,0);
        break;
    case gr_modem_types::ModemTypeQPSKVideo:
        _carrier_offset = 250000;
        _rotator->set_phase_inc(2*M_PI*_carrier_offset/1000000);
        _osmosdr_sink->set_center_freq(_device_frequency - _carrier_offset);
        _osmosdr_sink->set_sample_rate(1000000);
        _top_block->connect(_vector_source,0,_qpsk_video,0);
        _top_block->connect(_qpsk_video,0,_rotator,0);
        _top_block->connect(_rotator,0,_osmosdr_sink,0);
        break;
    case gr_modem_types::ModemTypeUSB2500:
        _carrier_offset = 25000;
        _rotator->set_phase_inc(2*M_PI*_carrier_offset/1000000);
        _osmosdr_sink->set_center_freq(_device_frequency - _carrier_offset);
        _osmosdr_sink->set_sample_rate(1000000);
        _top_block->connect(_audio_source,0,_usb,0);
        _top_block->connect(_usb,0,_rotator,0);
        _top_block->connect(_rotator,0,_osmosdr_sink,0);
        break;
    case gr_modem_types::ModemTypeLSB2500:
        _carrier_offset = 25000;
        _rotator->set_phase_inc(2*M_PI*_carrier_offset/1000000);
        _osmosdr_sink->set_center_freq(_device_frequency - _carrier_offset);
        _osmosdr_sink->set_sample_rate(1000000);
        _top_block->connect(_audio_source,0,_lsb,0);
        _top_block->connect(_lsb,0,_rotator,0);
        _top_block->connect(_rotator,0,_osmosdr_sink,0);
        break;
    default:
        break;
    }

    _mode = mode;

    _top_block->unlock();
}

void gr_mod_base::start()
{
    _audio_source->flush();
    _vector_source->flush();
    _top_block->start();
}

void gr_mod_base::stop()
{
    _top_block->stop();
    _audio_source->flush();
    _vector_source->flush();
    _top_block->wait();
}

int gr_mod_base::setData(std::vector<u_int8_t> *data)
{
    return _vector_source->set_data(data);
}

int gr_mod_base::setAudio(std::vector<float> *data)
{
    return _audio_source->set_data(data);

}

void gr_mod_base::tune(long center_freq)
{
    _device_frequency = center_freq;
    _top_block->lock();
    _osmosdr_sink->set_center_freq(_device_frequency - _carrier_offset);
    _top_block->unlock();
}

void gr_mod_base::set_power(float dbm)
{
    if (!_gain_range.empty())
    {
        double gain =  _gain_range.start() + dbm*(_gain_range.stop()-_gain_range.start());
        _top_block->lock();
        _osmosdr_sink->set_gain(gain);
        _top_block->unlock();
    }
}


void gr_mod_base::set_ctcss(float value)
{
    _top_block->lock();
    _fm_2500->set_ctcss(value);
    _fm_5000->set_ctcss(value);
    _top_block->unlock();
}

void gr_mod_base::set_bb_gain(int value)
{
    _top_block->lock();
    _2fsk->set_bb_gain(value);
    _2fsk_10k->set_bb_gain(value);
    _4fsk_2k->set_bb_gain(value);
    _4fsk_10k->set_bb_gain(value);
    _am->set_bb_gain(value);
    _bpsk_1k->set_bb_gain(value);
    _bpsk_2k->set_bb_gain(value);
    _fm_2500->set_bb_gain(value);
    _fm_5000->set_bb_gain(value);
    _qpsk_2k->set_bb_gain(value);
    _qpsk_10k->set_bb_gain(value);
    _qpsk_250k->set_bb_gain(value);
    _qpsk_video->set_bb_gain(value);
    _usb->set_bb_gain(value);
    _lsb->set_bb_gain(value);
    _top_block->unlock();
}

void gr_mod_base::set_carrier_offset(long carrier_offset)
{
    _carrier_offset = carrier_offset;
    _rotator->set_phase_inc(2*M_PI*-_carrier_offset/_samp_rate);

}


