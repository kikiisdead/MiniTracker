#ifndef instrument_h
#define instrument_h

#include "daisy_seed.h"
#include "daisysp.h"
#include "sampleplayer.h"
#include <vector>

using namespace daisy;
using namespace daisysp;

#define WAVEDISPLAY 96

/* 
This class will have all the audio objects for each individual object (FX, slicepoints, pitch, etc. )
Will have a public process function which will get called if its playing. pointer to the object will be passed through each lane in sequencer
*/

class Instrument {
private:
    // float* sampleData;
    SamplePlayer samplePlayer;
    Adsr env;
    std::vector<double> slices;
    float env_out;
    double volume;
    size_t currentSlice;
    bool playing;

public:
    
    Instrument(){};

    /**
     * Initializes the instrument with sample rate and sample. Each instrument will only have one sample
     */
    void Init(float samplerate, WavFile* sample);

    /**
     * Called in audioCallback to get next sample
     */
    float Process();

    /**
     * Trigger the next sample slice or trigger at different pitch (depends on mode)
     */
    void Trigger(int note);

    void Release();

    bool IsPlaying() { return playing; }

    void SetVolume(double vol) { volume = pow(2.0f, vol / 6.0f); } // volume in dB to constant

    double GetVolume() { return log2(volume) * 6.0f; } //volume in constant to dB

    /**
     * array of averages from the sample for display
     */
    uint16_t* visual;
};

#endif