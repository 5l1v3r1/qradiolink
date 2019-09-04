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

#include "gr_modem.h"

gr_modem::gr_modem(Settings *settings, QObject *parent) :
    QObject(parent)
{
    _modem_type_rx = gr_modem_types::ModemTypeBPSK2000;
    _modem_type_tx = gr_modem_types::ModemTypeBPSK2000;
    _repeater = false;
    _settings = settings;
    _transmitting = false;
    //_gr_mod_gmsk = new gr_mod_gmsk(0,24,48000,1600,1200,1);
    //_gr_mod_gmsk->start();
    //_gr_demod_gmsk = new gr_demod_gmsk(0,24,48000,1600,1200,1);
    //_gr_demod_gmsk->start();
    //_gr_mod_bpsk = new gr_mod_bpsk(0,24,48000,1700,1200,1);
    //_gr_mod_bpsk->start();
    //_gr_demod_bpsk = new gr_demod_bpsk(0,24,48000,1700,1200,1);
    //_gr_demod_bpsk->start();
    _rx_frame_length = 7;
    _tx_frame_length = 7;
    _bit_buf_len = 8 *8;
    _bit_buf = new unsigned char[_bit_buf_len];
    _bit_buf_index = 0;
    _sync_found = false;
    _frame_counter = 0;
    _shift_reg = 0;
    _last_frame_type = FrameTypeNone;
    _current_frame_type = FrameTypeNone;
    _frequency_found =0;
    _requested_frequency_hz = 433500000;
    _gr_mod_base = 0;
    _gr_demod_base = 0;


}

gr_modem::~gr_modem()
{
    //deinitRX();
    //deinitTX();
}

void gr_modem::initTX(int modem_type, std::string device_args, std::string device_antenna, int freq_corr)
{
    _modem_type_tx = modem_type;
    _gr_mod_base = new gr_mod_base(0, _requested_frequency_hz, 0.5, device_args, device_antenna, freq_corr);
    toggleTxMode(modem_type);

}

void gr_modem::initRX(int modem_type, std::string device_args, std::string device_antenna, int freq_corr)
{
    _modem_type_rx = modem_type;
    _gr_demod_base = new gr_demod_base(0, _requested_frequency_hz, 0.9, device_args, device_antenna, freq_corr);
    toggleRxMode(modem_type);

}

void gr_modem::toggleTxMode(int modem_type)
{
    _modem_type_tx = modem_type;
    if(_gr_mod_base)
    {
        _gr_mod_base->set_mode(modem_type);
        if(modem_type == gr_modem_types::ModemTypeBPSK2000)
        {
            _tx_frame_length = 7;
        }
        else if(modem_type == gr_modem_types::ModemTypeBPSK1000)
        {
            _tx_frame_length = 4;
        }
        else if(modem_type == gr_modem_types::ModemTypeQPSK20000)
        {
            _tx_frame_length = 47;
        }
        else if(modem_type == gr_modem_types::ModemTypeQPSK2000)
        {
            _tx_frame_length = 7;
        }
        else if(modem_type == gr_modem_types::ModemType4FSK20000)
        {
            _tx_frame_length = 47;
        }
        else if(modem_type == gr_modem_types::ModemType2FSK20000)
        {
            _tx_frame_length = 47;
        }
        else if(modem_type == gr_modem_types::ModemType4FSK2000)
        {
            _tx_frame_length = 7;
        }
        else if(modem_type == gr_modem_types::ModemTypeQPSKVideo)
        {
            _tx_frame_length = 3122;
        }
        else if(modem_type == gr_modem_types::ModemType2FSK2000)
        {
            _tx_frame_length = 7;
        }
        else if(modem_type == gr_modem_types::ModemTypeQPSK250000)
        {
            _tx_frame_length = 1516;
        }
    }

}

void gr_modem::toggleRxMode(int modem_type)
{
    _modem_type_rx = modem_type;
    if(_gr_demod_base)
    {
        _gr_demod_base->set_mode(modem_type);
        if(modem_type == gr_modem_types::ModemTypeBPSK2000)
        {
            _bit_buf_len = 8 *8;
            _rx_frame_length = 7;
        }
        else if(modem_type == gr_modem_types::ModemTypeBPSK1000)
        {
            _bit_buf_len = 4 *8;
            _rx_frame_length = 4;
        }
        else if (modem_type == gr_modem_types::ModemTypeQPSK20000)
        {
            _bit_buf_len = 48 *8;
            _rx_frame_length = 47;
        }
        else if (modem_type == gr_modem_types::ModemTypeQPSK2000)
        {
            _bit_buf_len = 8 *8;
            _rx_frame_length = 7;
        }
        else if (modem_type == gr_modem_types::ModemType4FSK20000)
        {
            _bit_buf_len = 48 *8;
            _rx_frame_length = 47;
        }
        else if (modem_type == gr_modem_types::ModemType2FSK20000)
        {
            _bit_buf_len = 48 *8;
            _rx_frame_length = 47;
        }
        else if (modem_type == gr_modem_types::ModemType4FSK2000)
        {
            _bit_buf_len = 8 *8;
            _rx_frame_length = 7;
        }
        else if (modem_type == gr_modem_types::ModemTypeQPSKVideo)
        {
            _bit_buf_len = 3123 *8;
            _rx_frame_length = 3122;
        }
        else if(modem_type == gr_modem_types::ModemType2FSK2000)
        {
            _bit_buf_len = 8 *8;
            _rx_frame_length = 7;
        }
        else if (modem_type == gr_modem_types::ModemTypeQPSK250000)
        {
            _bit_buf_len = 1517 *8;
            _rx_frame_length = 1516;
        }
        delete[] _bit_buf;
        _bit_buf = new unsigned char[_bit_buf_len];
    }
}

void gr_modem::deinitTX(int modem_type)
{
    _modem_type_tx = modem_type;

    _gr_mod_base->stop();
    delete _gr_mod_base;
    _gr_mod_base =0;
}

void gr_modem::deinitRX(int modem_type)
{
    _modem_type_rx = modem_type;
    _gr_demod_base->stop();
    delete _gr_demod_base;
    _gr_demod_base =0;
}

void gr_modem::startRX()
{
    _gr_demod_base->start();
}

void gr_modem::stopRX()
{
    _gr_demod_base->stop();
}

void gr_modem::startTX()
{
    _gr_mod_base->start();
}

void gr_modem::stopTX()
{
    _gr_mod_base->stop();
}

double gr_modem::getFreqGUI()
{
    if(_gr_demod_base)
        return _gr_demod_base->get_freq();
    return 0;
}

void gr_modem::tune(long center_freq)
{
    if(_gr_demod_base)
        _gr_demod_base->tune(center_freq);
}

void gr_modem::set_carrier_offset(long center_freq)
{
    if(_gr_demod_base)
        _gr_demod_base->set_carrier_offset(center_freq);
}

void gr_modem::setFFTSize(int size)
{
    if(_gr_demod_base)
        _gr_demod_base->set_fft_size(size);
}

void gr_modem::tuneTx(long center_freq)
{

    if(_gr_mod_base)
        _gr_mod_base->tune(center_freq);
}

void gr_modem::setTxPower(float value)
{
    if(_gr_mod_base)
        _gr_mod_base->set_power(value);
}

void gr_modem::setBbGain(int value)
{
    if(_gr_mod_base)
        _gr_mod_base->set_bb_gain(value);
}

void gr_modem::setRxSensitivity(float value)
{
    if(_gr_demod_base)
        _gr_demod_base->set_rx_sensitivity(value);
}

void gr_modem::setSquelch(int value)
{
    if(_gr_demod_base)
        _gr_demod_base->set_squelch(value);
}

void gr_modem::setRxCTCSS(float value)
{
    if(_gr_demod_base)
        _gr_demod_base->set_ctcss(value);
}

void gr_modem::setTxCTCSS(float value)
{
    if(_gr_mod_base)
        _gr_mod_base->set_ctcss(value);
}

void gr_modem::enableGUIConst(bool value)
{
    if(_gr_demod_base)
        _gr_demod_base->enable_gui_const(value);
}

void gr_modem::enableGUIFFT(bool value)
{
    if(_gr_demod_base)
        _gr_demod_base->enable_gui_fft(value);
}

void gr_modem::setRepeater(bool value)
{
    _repeater = value;
}

void gr_modem::sendCallsign(QString callsign)
{
    std::vector<unsigned char> *send_callsign = new std::vector<unsigned char>;
    QVector<std::vector<unsigned char>*> callsign_frames;

    send_callsign->push_back(0x8C);
    send_callsign->push_back(0xC8);
    send_callsign->push_back(0xDD);
    for(int i = 0;i<callsign.size();i++)
    {
        send_callsign->push_back(callsign.toStdString().c_str()[i]);
    }

    for(int i = 0;i<_tx_frame_length-callsign.size();i++)
    {
        send_callsign->push_back(0x00);
    }


    callsign_frames.append(send_callsign);
    transmit(callsign_frames);
}

void gr_modem::startTransmission(QString callsign)
{
    _transmitting = true;
    std::vector<unsigned char> *tx_start = new std::vector<unsigned char>;
    for(int i = 0;i<_tx_frame_length*2;i++)
    {

        tx_start->push_back(0x8C);
    }
    QVector<std::vector<unsigned char>*> frames;
    frames.append(tx_start);
    transmit(frames);
    sendCallsign(callsign);
}

void gr_modem::endTransmission(QString callsign)
{
    _frame_counter = 0;
    _transmitting = false;
    sendCallsign(callsign);
    std::vector<unsigned char> *tx_end = new std::vector<unsigned char>;
    tx_end->push_back(0x4C);
    tx_end->push_back(0x8A);
    tx_end->push_back(0x2B);
    for(int i = 0;i<_tx_frame_length;i++)
    {
        tx_end->push_back(0x01);
    }
    QVector<std::vector<unsigned char>*> frames;
    frames.append(tx_end);
    transmit(frames);
}

void gr_modem::processAudioData(unsigned char *data, int size)
{
    std::vector<unsigned char> *one_frame = frame(data, size, FrameTypeVoice);
    QVector<std::vector<unsigned char>*> frames;
    frames.append(one_frame);
    transmit(frames);
    delete[] data;
}

void gr_modem::textData(QString text, int frame_type)
{
    QStringList list;
    QVector<std::vector<unsigned char>*> frames;
    for( int k=0;k<text.length();k+=_tx_frame_length)
    {
        list.append(text.mid(k,_tx_frame_length));
    }

    for(int o = 0;o < list.length();o++)
    {
        QString chunk=list.at(o);
        unsigned char *data = new unsigned char[_tx_frame_length];
        memset(data, 0, _tx_frame_length);
        memcpy(data,chunk.toStdString().c_str(),chunk.length());
        std::vector<unsigned char> *one_frame = frame(data,_tx_frame_length, frame_type);

        frames.append(one_frame);

        delete[] data;
    }
    transmit(frames);
}

void gr_modem::binData(QByteArray bin_data, int frame_type)
{
    QVector<std::vector<unsigned char>*> frames;
    for( int k=0;k<bin_data.length();k+=_tx_frame_length)
    {
        QByteArray d = bin_data.mid(k,_tx_frame_length);
        int copy = _tx_frame_length;
        if(d.size() < _tx_frame_length)
            copy = d.size();
        char * c = d.data();
        unsigned char *data = new unsigned char[_tx_frame_length];
        memset(data, 0, _tx_frame_length);
        memcpy(data, c, copy);
        std::vector<unsigned char> *one_frame = frame(data,_tx_frame_length, frame_type);

        frames.append(one_frame);
    }
    transmit(frames);
}

void gr_modem::processPCMAudio(std::vector<float> *audio_data)
{

    if((_modem_type_tx == gr_modem_types::ModemTypeNBFM2500)
            || (_modem_type_tx == gr_modem_types::ModemTypeNBFM5000)
            || (_modem_type_tx == gr_modem_types::ModemTypeUSB2500)
            || (_modem_type_tx == gr_modem_types::ModemTypeLSB2500)
            || (_modem_type_tx == gr_modem_types::ModemTypeAM5000))
    {
        int ret = 1;
        while(ret)
        {
            struct timespec time_to_sleep = {0, 1000L };
            nanosleep(&time_to_sleep, NULL);
            ret = _gr_mod_base->setAudio(audio_data);
        }
    }
}

void gr_modem::processVideoData(unsigned char *data, int size)
{
    std::vector<unsigned char> *one_frame = frame(data, size, FrameTypeVideo);
    QVector<std::vector<unsigned char>*> frames;
    frames.append(one_frame);
    transmit(frames);
    delete[] data;
}

void gr_modem::processNetData(unsigned char *data, int size)
{
    QVector<std::vector<unsigned char>*> frames;
    std::vector<unsigned char> *one_frame = frame(data, size, FrameTypeData);
    frames.append(one_frame);
    transmit(frames);
    delete[] data;
}

void gr_modem::transmit(QVector<std::vector<unsigned char>*> frames)
{
    std::vector<unsigned char> *all_frames = new std::vector<unsigned char>;
    for (int i=0; i<frames.size();i++)
    {
        all_frames->insert( all_frames->end(), frames.at(i)->begin(), frames.at(i)->end() );
        frames.at(i)->clear();
        delete frames.at(i);
    }
    int ret = 1;
    while(ret)
    {
        struct timespec time_to_sleep = {0, 1000L };
        ret = _gr_mod_base->setData(all_frames);
    }

}

std::vector<unsigned char>* gr_modem::frame(unsigned char *encoded_audio, int data_size, int frame_type)
{
    std::vector<unsigned char> *data = new std::vector<unsigned char>;
    if(frame_type == FrameTypeVoice)
    {
        if(_modem_type_tx == gr_modem_types::ModemTypeBPSK1000)
        {
            data->push_back(0xB5);
        }
        else
        {
            data->push_back(0xED);
            data->push_back(0x89);
        }
    }
    else if(frame_type == FrameTypeText)
    {
        data->push_back(0x89);
        data->push_back(0xED);
    }
    else if(frame_type == FrameTypeVideo)
    {
        data->push_back(0x98);
        data->push_back(0xDE);
    }
    else if(frame_type == FrameTypeData)
    {
        data->push_back(0xDE);
        data->push_back(0x98);
    }
    else if(frame_type == FrameTypeRepeaterInfo)
    {
        data->push_back(0xED);
        data->push_back(0x77);
    }

    if(_modem_type_tx != gr_modem_types::ModemTypeBPSK1000)
        data->push_back(0xAA); // frame start
    for(int i=0;i< data_size;i++)
    {
        data->push_back(encoded_audio[i]);
    }

    return data;

}

static void packBytes(unsigned char *pktbuf, const unsigned char *bitbuf, int bitcount)
{
    for(int i = 0; i < bitcount; i += 8)
    {
        int t = bitbuf[i+0] & 0x1;
        t = (t << 1) | (bitbuf[i+1] & 0x1);
        t = (t << 1) | (bitbuf[i+2] & 0x1);
        t = (t << 1) | (bitbuf[i+3] & 0x1);
        t = (t << 1) | (bitbuf[i+4] & 0x1);
        t = (t << 1) | (bitbuf[i+5] & 0x1);
        t = (t << 1) | (bitbuf[i+6] & 0x1);
        t = (t << 1) | (bitbuf[i+7] & 0x1);
        *pktbuf++ = t;
    }
}

static void unpackBytes(unsigned char *bitbuf, const unsigned char *bytebuf, int bytecount)
{
    for(int i=0; i<bytecount; i++)
    {
        for(int j=0; j<8; j++)
        {
            bitbuf[i*8+j] = (bytebuf[i] & (128 >> j)) != 0;
        }
    }
}

void gr_modem::get_fft_data(std::complex<float>* data, unsigned int &size)
{
    _gr_demod_base->getFFTData(data, size);
}

bool gr_modem::demodulateAnalog()
{
    std::vector<float> *audio_data;
    if((_modem_type_rx == gr_modem_types::ModemTypeNBFM2500)
            || (_modem_type_rx == gr_modem_types::ModemTypeNBFM5000)
            || (_modem_type_rx == gr_modem_types::ModemTypeUSB2500)
            || (_modem_type_rx == gr_modem_types::ModemTypeLSB2500)
            || (_modem_type_rx == gr_modem_types::ModemTypeAM5000)
            || (_modem_type_rx == gr_modem_types::ModemTypeWBFM))
    {
        audio_data = _gr_demod_base->getAudio();
    }
    if(audio_data->size() > 0)
    {
        if(_repeater)
        {
            std::vector<float> *repeated_audio = new std::vector<float>(audio_data->begin(),audio_data->end());
            processPCMAudio(repeated_audio);
        }
        emit pcmAudio(audio_data);
        return true;
    }
    else
    {
        delete audio_data;
        return false;
    }

}

bool gr_modem::demodulate()
{
    std::vector<unsigned char> *demod_data;
    std::vector<unsigned char> *demod_data2;
    std::vector<unsigned char> *data;

    if((_modem_type_rx == gr_modem_types::ModemTypeBPSK2000)
            || (_modem_type_rx == gr_modem_types::ModemType2FSK2000)
            || (_modem_type_rx == gr_modem_types::ModemType2FSK20000)
            || (_modem_type_rx == gr_modem_types::ModemTypeBPSK1000))
    {
        demod_data = _gr_demod_base->getData(1);
        demod_data2 = _gr_demod_base->getData(2);
    }     
    else
        demod_data = _gr_demod_base->getData();

    int v_size;
    if((_modem_type_rx == gr_modem_types::ModemTypeBPSK2000)
            || (_modem_type_rx == gr_modem_types::ModemTypeBPSK1000)
            || (_modem_type_rx == gr_modem_types::ModemType2FSK20000)
            || (_modem_type_rx == gr_modem_types::ModemType2FSK2000))
    {
        if(demod_data->size() >= demod_data2->size())
        {
            v_size = demod_data->size();
            data = demod_data;
        }
        else
        {
            v_size = demod_data2->size();
            data = demod_data2;
        }
    }
    else
    {
        v_size = demod_data->size();
        data = demod_data;
    }

    bool data_to_process = synchronize(v_size, data);
    demod_data->clear();
    delete demod_data;
    if((_modem_type_rx == gr_modem_types::ModemTypeBPSK2000)
            || (_modem_type_rx == gr_modem_types::ModemTypeBPSK1000)
            || (_modem_type_rx == gr_modem_types::ModemType2FSK20000)
            || (_modem_type_rx == gr_modem_types::ModemType2FSK2000))
    {
        demod_data2->clear();
        delete demod_data2;
    }
    return data_to_process;

}

bool gr_modem::synchronize(int v_size, std::vector<unsigned char> *data)
{
    bool data_to_process = false;
    for(int i=0;i < v_size;i++)
    {
        if(!_sync_found)
        {
            _current_frame_type = findSync(data->at(i));
            if(_sync_found)
            {
                _bit_buf_index = 0;
                continue;
            }
            if(_frequency_found > 0)
                _frequency_found--; // substract one bit
        }
        if(_sync_found)
        {
            data_to_process = true;
            if(_frequency_found < 255)
                _frequency_found += 1; // 80 bits + counter
            _bit_buf[_bit_buf_index] =  (data->at(i)) & 0x1;
            _bit_buf_index++;
            int frame_length = _rx_frame_length;
            int bit_buf_len = _bit_buf_len;
            if((_modem_type_rx != gr_modem_types::ModemTypeBPSK1000)
                    && (_current_frame_type == FrameTypeVoice))
            {
                frame_length++; // reserved data
            }
            else if((_modem_type_rx != gr_modem_types::ModemTypeBPSK1000)
                    && (_current_frame_type != FrameTypeVoice))
            {
                bit_buf_len = _bit_buf_len - 8;
            }
            if(_bit_buf_index >= bit_buf_len)
            {

                unsigned char *frame_data = new unsigned char[frame_length];
                packBytes(frame_data,_bit_buf,_bit_buf_index);
                processReceivedData(frame_data, _current_frame_type);
                _sync_found = false;
                _shift_reg = 0;
                _bit_buf_index = 0;
            }
        }
    }
    return data_to_process;
}

int gr_modem::findSync(unsigned char bit)
{
    _shift_reg = (_shift_reg << 1) | (bit & 0x1);
    u_int32_t temp;
    if(_modem_type_rx == gr_modem_types::ModemTypeBPSK1000)
    {
        temp = _shift_reg & 0xFF;
        if (temp == 0xB5)
        {
            _sync_found = true;
            return FrameTypeVoice;
        }
    }
    if(_modem_type_rx != gr_modem_types::ModemTypeQPSK250000 &&
            _modem_type_rx != gr_modem_types::ModemTypeQPSKVideo)
    {
        temp = _shift_reg & 0xFFFF;
        if((temp == 0xED89))
        {
            _sync_found = true;
            return FrameTypeVoice;
        }
        temp = _shift_reg & 0xFFFFFF;
        if((temp == 0x89EDAA))
        {
            _sync_found = true;
            return FrameTypeText;
        }
        if((temp == 0xED77AA))
        {
            _sync_found = true;
            return FrameTypeRepeaterInfo;
        }

        if((temp == 0x98DEAA))
        {
            _sync_found = true;
            return FrameTypeVideo;
        }
        if((temp == 0x8CC8DD))
        {
            _sync_found = true;
            return FrameTypeCallsign;
        }
    }
    temp = _shift_reg & 0xFFFFFF;
    if((temp == 0xDE98AA))
    {
        _sync_found = true;
        return FrameTypeData;
    }
    if((temp == 0x98DEAA))
    {
        _sync_found = true;
        return FrameTypeVideo;
    }
    if((temp == 0x4C8A2B))
    {
        _sync_found = true;
        qDebug() << "tx end";
        return FrameTypeEnd;
    }
    return FrameTypeNone;
}


void gr_modem::processReceivedData(unsigned char *received_data, int current_frame_type)
{
    if (current_frame_type == FrameTypeEnd)
    {
        handleStreamEnd();
    }
    if (current_frame_type == FrameTypeText)
    {
        emit dataFrameReceived();
        _last_frame_type = FrameTypeText;
        char *text_data = new char[_rx_frame_length];
        memcpy(text_data, received_data, _rx_frame_length);
        quint8 string_length = _rx_frame_length;

        for(int ii=_rx_frame_length-1;ii>=0;ii--)
        {
            QChar x(text_data[ii]);
            if(x.unicode()==0)
            {
                string_length--;
            }
            else
            {
                break;
            }
        }
        QString text = QString::fromLocal8Bit(text_data,string_length);
        if(_repeater)
        {
            textData(text);
        }
        emit textReceived(text);
        delete[] text_data;
    }
    if (current_frame_type == FrameTypeRepeaterInfo)
    {
        emit dataFrameReceived();
        _last_frame_type = FrameTypeRepeaterInfo;
        QByteArray data((const char*)received_data, _rx_frame_length);
        emit repeaterInfoReceived(data);
    }
    else if (current_frame_type == FrameTypeCallsign)
    {
        emit dataFrameReceived();
        _last_frame_type = FrameTypeCallsign;
        char *text_data = new char[_rx_frame_length];
        memcpy(text_data, received_data, _rx_frame_length);
        quint8 string_length = _rx_frame_length;

        for(int ii=_rx_frame_length-1;ii>=0;ii--)
        {
            QChar x(text_data[ii]);
            if(x.unicode()==0)
            {
                string_length--;
            }
        }
        QString callsign = QString::fromLocal8Bit(text_data,string_length);
        if(_repeater)
        {
            sendCallsign(callsign);
        }
        emit callsignReceived(callsign);
        delete[] text_data;
    }
    else if (current_frame_type == FrameTypeVoice )
    {
        emit audioFrameReceived();
        _last_frame_type = FrameTypeVoice;
        unsigned char *codec2_data = new unsigned char[_rx_frame_length];
        memset(codec2_data,0,_rx_frame_length);
        if(_modem_type_rx == gr_modem_types::ModemTypeBPSK1000)
            memcpy(codec2_data, received_data, _rx_frame_length);
        else
            memcpy(codec2_data, received_data+1, _rx_frame_length);
        if(_repeater)
        {
            unsigned char *repeated_frame = new unsigned char[_rx_frame_length];
            memcpy(repeated_frame, codec2_data, _rx_frame_length);
            processAudioData(repeated_frame, _rx_frame_length);
        }
        emit digitalAudio(codec2_data,_rx_frame_length);
    }
    else if (current_frame_type == FrameTypeVideo )
    {
        emit dataFrameReceived();
        _last_frame_type = FrameTypeVideo;
        unsigned char *video_data = new unsigned char[_rx_frame_length];
        memcpy(video_data, received_data, _rx_frame_length);
        emit videoData(video_data,_rx_frame_length);
    }
    else if (current_frame_type == FrameTypeData )
    {
        _last_frame_type = FrameTypeData;
        unsigned char *net_data = new unsigned char[_rx_frame_length];
        memcpy(net_data, received_data, _rx_frame_length);
        emit netData(net_data,_rx_frame_length);
        // poke repeater here
    }
    delete[] received_data;
}

void gr_modem::handleStreamEnd()
{
    if(_last_frame_type == FrameTypeText)
    {
        emit textReceived( QString("\n"));
    }
    else if(_last_frame_type == FrameTypeVoice)
    {
        emit endAudioTransmission();
    }
    else if(_last_frame_type == FrameTypeCallsign)
    {
        emit endAudioTransmission();
    }
    else if(_last_frame_type == FrameTypeVideo)
    {
        emit endAudioTransmission();
    }
    emit receiveEnd();
}
