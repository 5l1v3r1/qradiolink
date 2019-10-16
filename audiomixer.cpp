// Written by Adrian Musceac YO8RZZ , started October 2013.
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

#include "audiomixer.h"

AudioMixer::AudioMixer(QObject *parent) : QObject(parent)
{

}


void AudioMixer::addSamples(short *pcm, int samples, int sid)
{
    _mutex.lock();
    if(_sample_buffers.contains(sid))
    {
        QVector<short> *samples_for_sid = _sample_buffers[sid];
        for(int i = 0;i< samples;i++)
        {
            samples_for_sid->push_back(pcm[i]);
        }
    }
    else
    {
        QVector<short> *samples_for_sid = new QVector<short>;
        for(int i = 0;i< samples;i++)
        {
            samples_for_sid->push_back(pcm[i]);
        }
        _sample_buffers[sid] = samples_for_sid;
    }
    _mutex.unlock();
}


short* AudioMixer::mix_samples()
{
    // need semaphore
    const int frame_size = 320; // to radio is always 40 msec
    short *pcm;
    int max_samples = 0;
    bool samples_ready = false;

    /// get buffers with available samples
    QMap<int, int> sizes_map;
    QMap<int, QVector<short>*>::const_iterator it = _sample_buffers.constBegin();
    while (it != _sample_buffers.constEnd())
    {
        int sid = it.key();
        QVector<short> *samples_for_sid = it.value();
        if(samples_for_sid->size() > max_samples)
        {
            max_samples = samples_for_sid->size();
        }
        if(samples_for_sid->size() > 0)
            sizes_map[sid] = samples_for_sid->size();
        ++it;
    }
    if(max_samples >= frame_size)
    {
        samples_ready = true;
        int num_channels = sizes_map.size();
        pcm = new short[frame_size];
        for(int i = 0;i<frame_size;i++)
        {
            QMap<int, int>::const_iterator it = sizes_map.constBegin();
            while (it != sizes_map.constEnd())
            {
                QVector<short> *samples_for_sid = _sample_buffers[it.key()];
                if(i < it.value())
                    pcm[i] += (short)((float)samples_for_sid->at(i) / (float)num_channels);
                ++it;
            }
        }
        /// remove mixed samples from buffers
        QMap<int, int>::const_iterator it = sizes_map.constBegin();
        while (it != sizes_map.constEnd())
        {
            QVector<short> *samples_for_sid = _sample_buffers[it.key()];
            if(samples_for_sid->size() < frame_size)
            {
                samples_for_sid->remove(0, samples_for_sid->size());
            }
            else
            {
                samples_for_sid->remove(0, frame_size);
            }
            /// cleanup empty buffers
            if(samples_for_sid->size() < 1)
            {
                delete samples_for_sid;
                _sample_buffers.remove(it.key());
            }
            ++it;
        }
    }
    if(samples_ready)
        return pcm;
    else
        return nullptr;
}
