#include "instrument.h"

void Instrument::Init(float samplerate) {
    osc.Init(samplerate);
    // env.Init(samplerate);
    osc.SetWaveform(osc.WAVE_TRI);
    osc.SetAmp(0.0f);
    osc.SetFreq(220.0f);
    // env.SetTime(ADSR_SEG_ATTACK, 0.1f);
    // env.SetTime(ADSR_SEG_DECAY, 0.6f);
    // env.SetSustainLevel(0.5f);
    // env.SetTime(ADSR_SEG_RELEASE, 0.1f);
}

float Instrument::Process() {
    return osc.Process();
}

void Instrument::Trigger(int note) {
    osc.SetFreq(440.0f * pow(2.0f, (((float) note) - 69.0f)));
    playing = true;
    osc.SetAmp(2.0f);
}

void Instrument::Release() {
    playing = false;
    osc.SetAmp(0.0f);
}
