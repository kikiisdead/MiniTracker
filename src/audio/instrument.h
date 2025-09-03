#ifndef instrument_h
#define instrument_h

#include "daisy_seed.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

/* 
This class will have all the audio objects for each individual object (FX, slicepoints, pitch, etc. )
Will have a public process function which will get called if its playing. pointer to the object will be passed through each lane in sequencer
*/

class Instrument {
private:
    float* sampleData;
    Oscillator osc;
    Adsr env;
    float env_out;
    bool playing;

public:
    

    Instrument(){};

    void Init(float samplerate) {
        osc.Init(samplerate);
        env.Init(samplerate);
        osc.SetWaveform(osc.WAVE_TRI);
        env.SetTime(ADSR_SEG_ATTACK, 0.1);
        env.SetTime(ADSR_SEG_DECAY, 0.6);
        env.SetSustainLevel(0.5);
        env.SetTime(ADSR_SEG_RELEASE, 0.1);
    }

    float Process() {
        env_out = env.Process(playing);
        osc.SetAmp(env_out);
        return osc.Process();
    }

    void Trigger(int note) {
        osc.SetFreq(440.0f * pow(2.0f, ((float) note - 69.0f)));
        playing = true;
    }

    void Release() {
        playing = false;
    }
};

#endif