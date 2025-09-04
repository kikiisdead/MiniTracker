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
    // float* sampleData;
    Oscillator osc;
    // Adsr env;
    // float env_out;
    bool playing;

public:
    

    Instrument(){};

    void Init(float samplerate);

    float Process();

    void Trigger(int note);

    void Release();
};

#endif