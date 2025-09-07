#include "instrument.h"

void Instrument::Init(float samplerate, WavFile* sample) {
    samplePlayer.Init(sample, samplerate);
    env.Init(samplerate);
    env.SetTime(ADSR_SEG_ATTACK, 0.01f);
    env.SetTime(ADSR_SEG_DECAY, 0.6f);
    env.SetSustainLevel(0.0f);
    env.SetTime(ADSR_SEG_RELEASE, 0.01f);
    slices.push_back(0.0f);
    slices.push_back(0.25f);
    slices.push_back(0.5f);
    slices.push_back(0.625f);
    slices.push_back(0.75f);
    SetVolume(0.0f);

    visual = samplePlayer.GetVisual(WAVEDISPLAY);
}

float Instrument::Process() {
    env_out = env.Process(playing);

    if (!env.IsRunning() && samplePlayer.IsPlaying()) {
        samplePlayer.Stop(); // if the envelope has stopped and the sample is still playing: stop
    } else if (currentSlice + 1 < slices.size()) {
        if (samplePlayer.GetPos() >= slices.at(currentSlice + 1) * samplePlayer.GetSize()) {
            samplePlayer.Stop();
        }
    } 
    return samplePlayer.Process() * env_out * volume;
}

void Instrument::Trigger(int note) {
    currentSlice = note - 48;
    env.Retrigger(false);
    if (currentSlice >= 0 && currentSlice < slices.size()) {
        samplePlayer.Play(slices.at(currentSlice) * samplePlayer.GetSize());
    } else {
        samplePlayer.SetPitch(((float) note) - 48);
        samplePlayer.Play();
    }
    playing = true;
}

void Instrument::Release() {
    playing = false;
}
