#include <cstring>
#include "sampleplayer.h"

// to add more f=lexibility to the thing I need to add support for 24 bit wave files as well as pithch changing

using namespace daisy;

void SamplePlayer::Init(const unsigned int* sample, float samplerate_)
{
    // First check for all .wav files, and add them to the list until its full or there are no more.
    // Only checks '/'
    samplerate = samplerate_;
    playing_  = false;
    looping_  = false;
}

void SamplePlayer::Deinterleave() {

}

void SamplePlayer::Process(float& out) {

    if (!playing_) {
        return; // if its not playing then do nothing
    }

    int32_t pos_integral   = static_cast<int32_t>(pos_);
    float   pos_fractional = pos_ - static_cast<float>(pos_integral);

    int32_t     t     = (pos_integral + size_);
    int32_t     xm1   = wave->sample[0][(t - 1) % size_];
    int32_t     x0    = wave->sample[0][(t) % size_];
    int32_t     x1    = wave->sample[0][(t + 1) % size_];
    int32_t     x2    = wave->sample[0][(t + 2) % size_];
    const float c     = (x1 - xm1) * 0.5f;
    const float v     = x0 - x1;
    const float w     = c + v;
    const float a     = w + v + (x2 - x0) * 0.5f;
    const float b_neg = w + a;
    const float f     = pos_fractional;

    int32_t interpolated = (((a * f) - b_neg) * f + c) * f + x0;

    if (wave->format.BitPerSample == 16) {
        out = s162f(static_cast<int16_t>(interpolated));
    } else {
        out = s242f(interpolated);
    }

    pos_ = (pos_ + samplePerStep);
    if (pos_ > size_) pos_ = 0.0;
}

void SamplePlayer::Process(float& outL, float& outR) {

    int32_t *interp = new int32_t[2];

    if (!playing_) {
        return; // if its not playing then do nothing
    }
    
    int32_t pos_integral   = static_cast<int32_t>(pos_);
    float   pos_fractional = pos_ - static_cast<float>(pos_integral);

    for (int i = 0; i < 2; i++) {
        int32_t     t     = (pos_integral + size_);
        int32_t     xm1   = wave->sample[i][(t - 1) % size_];
        int32_t     x0    = wave->sample[i][(t) % size_];
        int32_t     x1    = wave->sample[i][(t + 1) % size_];
        int32_t     x2    = wave->sample[i][(t + 2) % size_];
        const float c     = (x1 - xm1) * 0.5f;
        const float v     = x0 - x1;
        const float w     = c + v;
        const float a     = w + v + (x2 - x0) * 0.5f;
        const float b_neg = w + a;
        const float f     = pos_fractional;
        interp[i] = (((a * f) - b_neg) * f + c) * f + x0;
    }

    if (wave->format.BitPerSample == 16) {
        outL = s162f(static_cast<int16_t>(interp[0]));
        outR = s162f(static_cast<int16_t>(interp[1]));
    } else {
        outL = s242f(interp[0]);
        outR = s242f(interp[1]);
    }

    delete[] interp;

    pos_ = (pos_ + samplePerStep);
    if (pos_ > size_) pos_ = 0.0;
}
