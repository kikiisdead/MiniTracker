#include <cstring>
#include "sampleplayer.h"

// to add more flexibility to the thing I need to add support for 24 bit wave files as well as pithch changing

using namespace daisy;

void SamplePlayer::Init(WavFile* wave_, float samplerate_)
{
    // First check for all .wav files, and add them to the list until its full or there are no more.
    // Only checks '/'
    samplerate = samplerate_;
    wave = wave_;
    size_ = wave->size;
    playing_  = false;
    looping_  = false;
    samplePerStep = (float) wave->format.SampleRate / samplerate;

}

void SamplePlayer::Deinterleave() {

}

float SamplePlayer::Process() {

    pos_ = (pos_ + samplePerStep);
    if (pos_ > wave->size) {
        pos_ = 0.0; 
        if (!looping_) {
            Stop();
        }
    }
    if (!playing_) {
        return 0.0f; // if its not playing then do nothing
    }

    int32_t pos_integral   = static_cast<int32_t>(pos_); // rounding float pos into int
    float   pos_fractional = pos_ - static_cast<float>(pos_integral); // getting difference between the two

    int32_t t  =  (pos_integral);
    int16_t xm1 = *(static_cast<int16_t*>(wave->start) + ((t - 1) % wave->size)); // casting into short int
    int16_t x0  = *(static_cast<int16_t*>(wave->start) + ((t    ) % wave->size));
    int16_t x1  = *(static_cast<int16_t*>(wave->start) + ((t + 1) % wave->size)); // casting into short int
    int16_t x2  = *(static_cast<int16_t*>(wave->start) + ((t + 2) % wave->size));

    const float c     = (x1 - xm1) * 0.5f;
    const float v     = x0 - x1;
    const float w     = c + v;
    const float a     = w + v + (x2 - x0) * 0.5f;
    const float b_neg = w + a;
    const float f     = pos_fractional;
    int16_t interpolated = (((a * f) - b_neg) * f + c) * f + x0;


    if (wave->format.BitPerSample == 16) {
        return s162f(interpolated);
    } else {
        return s242f(interpolated);
    }

}

void SamplePlayer::Process(float& outL, float& outR) {

    // if (!playing_) {
    //     outL = 0.0f;
    //     outR = 0.0f;
    //     return; // if its not playing then do nothing
    // }
    
    // int32_t pos_integral   = static_cast<int32_t>(pos_);
    // float   pos_fractional = pos_ - static_cast<float>(pos_integral);

    // int32_t     t     = (pos_integral + size_);
    // int32_t     xm1   = (wave->start + ((t - 1) % wave->size));
    // int32_t     x0    = (wave->start + ((t    ) % wave->size));
    // int32_t     x1    = (wave->start + ((t + 1) % wave->size));
    // int32_t     x2    = (wave->start + ((t + 2) % wave->size));
    // const float c     = (x1 - xm1) * 0.5f;
    // const float v     = x0 - x1;
    // const float w     = c + v;
    // const float a     = w + v + (x2 - x0) * 0.5f;
    // const float b_neg = w + a;
    // const float f     = pos_fractional;
    // int32_t interpolated = (((a * f) - b_neg) * f + c) * f + x0;

    // if (wave->format.BitPerSample == 16) {
    //     outL = s162f(static_cast<int16_t>(interpolated));
    //     outR = s162f(static_cast<int16_t>(interpolated));
    // } else {
    //     outL = s242f(interpolated);
    //     outR = s242f(interpolated);
    // }

    // pos_ = (pos_ + samplePerStep);
    // if (pos_ > wave->size) pos_ = 0.0;
}
