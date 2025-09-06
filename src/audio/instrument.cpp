#include "instrument.h"

void Instrument::Init(float samplerate) {
    osc.Init(samplerate);
    env.Init(samplerate);
    osc.SetWaveform(osc.WAVE_TRI);
    osc.SetAmp(0.0f);
    osc.SetFreq(220.0f);
    env.SetTime(ADSR_SEG_ATTACK, 0.1f);
    env.SetTime(ADSR_SEG_DECAY, 0.6f);
    env.SetSustainLevel(0.5f);
    env.SetTime(ADSR_SEG_RELEASE, 0.1f);
}

float Instrument::Process() {
    env_out = env.Process(playing);
    osc.SetAmp(env_out);
    return osc.Process();
}

void Instrument::Trigger(int note) {
    osc.SetFreq(mtof(note));
    playing = true;
}

void Instrument::Release() {
    playing = false;
    osc.SetAmp(0.0f);
}
