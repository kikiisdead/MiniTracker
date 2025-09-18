#include <cstring>
#include "sampleplayer.h"

// to add more flexibility to the thing I need to add support for 24 bit wave files as well as pithch changing

// maybe do all conversions before reading to SDRAM (use all floats in SDRAM?)

using namespace daisy;

void SamplePlayer::Init(WavFile* wave_, float samplerate_)
{
    // First check for all .wav files, and add them to the list until its full or there are no more.
    // Only checks '/'
    samplerate = samplerate_;
    wave = wave_;
    size_ = wave->format.SubCHunk2Size / wave->format.NbrChannels; // for interleaved
    playing_  = false;
    looping_  = false;
    SetPitch(0.0);

}

float SamplePlayer::Process() {

    pos_ = (pos_ + samplePerStep);
    if (pos_ > size_) {
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

    __disable_irq(); // prevent interrupts while accessing SDRAM (especially when using DMA)
    int32_t t  =  (pos_integral);
    float  xm1 = *(SAMP_POINTER + ((t - 1) % size_)); // casting into float (not as memory efficient with ram usage but that's okay)
    float  x0  = *(SAMP_POINTER + ((t    ) % size_));
    float  x1  = *(SAMP_POINTER + ((t + 1) % size_));
    float  x2  = *(SAMP_POINTER + ((t + 2) % size_));
    __enable_irq();

    const float c     = (x1 - xm1) * 0.5f;
    const float v     = x0 - x1;
    const float w     = c + v;
    const float a     = w + v + (x2 - x0) * 0.5f;
    const float b_neg = w + a;
    const float f     = pos_fractional;
    float interpolated = (((a * f) - b_neg) * f + c) * f + x0;


    return interpolated;

}

uint16_t* SamplePlayer::GetVisual(int size) {
    uint16_t* visual = new uint16_t[size];
    
    size_t stepSize = size_/size;

    for (int i = 0; i < size; i++) {

        float max = 0.0f;

        for (size_t j = 0; j < stepSize; j++) {
            float step = abs(*(SAMP_POINTER + ((i * stepSize) + j) * wave->format.NbrChannels));
            if (step > max) {
                max = step;
            }
        }

        visual[i] = f2s16(max);
    }

    return visual;
}

void SamplePlayer::Process(float& outL, float& outR) {

    pos_ = (pos_ + samplePerStep);
    if (pos_ > size_) {
        pos_ = 0.0; 
        if (!looping_) {
            Stop();
        }
    }
    if (!playing_) {
        outL = 0.0f; // if its not playing then do nothing
        outR = 0.0f;
        return;
    }

    int32_t pos_integral   = static_cast<int32_t>(pos_); // rounding float pos into int
    float   pos_fractional = pos_ - static_cast<float>(pos_integral); // getting difference between the two

    float interpolated[wave->format.NbrChannels];

    for (uint16_t i = 0; i < wave->format.NbrChannels; i++) {
        __disable_irq(); // prevent interrupts while accessing SDRAM (especially when using DMA)
        int32_t t  =  (pos_integral);
        float  xm1 = *(SAMP_POINTER + (((t - 1) % size_) * wave->format.NbrChannels + i)); // casting into float
        float  x0  = *(SAMP_POINTER + (((t    ) % size_) * wave->format.NbrChannels + i));
        float  x1  = *(SAMP_POINTER + (((t + 1) % size_) * wave->format.NbrChannels + i)); 
        float  x2  = *(SAMP_POINTER + (((t + 2) % size_) * wave->format.NbrChannels + i));
        __enable_irq();

        const float c     = (x1 - xm1) * 0.5f;
        const float v     = x0 - x1;
        const float w     = c + v;
        const float a     = w + v + (x2 - x0) * 0.5f;
        const float b_neg = w + a;
        const float f     = pos_fractional;

        interpolated[i] = (((a * f) - b_neg) * f + c) * f + x0;
    }

    outL = interpolated[0];
    outR = interpolated[wave->format.NbrChannels - 1];
    
}
