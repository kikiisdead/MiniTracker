#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include "daisy_seed.h"
#include "daisysp.h"
#include "sampleplayer.h"
#include <vector>

using namespace daisy;
using namespace daisysp;

#define WAVEWIDTH 240
#define WAVEHEIGHT 168

/* 
This class will have all the audio objects for each individual object (FX, slicepoints, pitch, etc. )
Will have a public process function which will get called if its playing. pointer to the object will be passed through each lane in sequencer
*/

class Instrument {


public:
    enum param { p, g, a, d, s, r };

    Instrument(){};

    /**
     * Initializes the instrument with sample rate and sample. Each instrument will only have one sample
     */
    void Init(float samplerate, WavFile* sample);

    /**
     * Called in audioCallback to get next sample
     */
    void Process(float& left, float& right);

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

    /**
     * Getters and Setters
     */

    std::vector<double> GetSlices() { return slices; }

    uint32_t GetSize() { return samplePlayer.GetSize(); }

    float GetPitch() { return pitch; }

    float GetAttack() { return att; }

    float GetDecay() { return dec; }

    float GetSustain() {return sus; }

    float GetRelease() {return rel; }

    float GetGain() { return gain; }

    char* GetName() { return samplePlayer.GetName(); }

    param GetEdit() { return edit; }

    void NextParam();
    void PrevParam();
    void Increment();
    void Decrement();


    /**
     * array of averages from the sample for display
     */
    uint16_t* visual;

    std::vector<double> slices;

private:
    SamplePlayer samplePlayer;
    Adsr env;
    float env_out;
    double volume;
    size_t currentSlice;
    bool playing;
    float att, dec, sus, rel, pitch, gain;
    param edit;

    float Scaling(float num) { return pow(num, ((num / 5.0f) + 1.0f) / 2.0f) / 10.0f; }

};

#endif