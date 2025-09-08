#include "instrument.h"

void Instrument::Init(float samplerate, WavFile* sample) {
    samplePlayer.Init(sample, samplerate);
    env.Init(samplerate);
    att = 0.01;
    dec = 0.6;
    sus = 0.0;
    rel = 0.01;
    pitch = 0;
    env.SetAttackTime(att);
    env.SetDecayTime(dec);
    env.SetSustainLevel(sus);
    env.SetReleaseTime(rel);
    slices.push_back(0.0f);
    edit = p;
    visual = samplePlayer.GetVisual(WAVEWIDTH);
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
    return samplePlayer.Process() * env_out;
}

void Instrument::Trigger(int note) {
    currentSlice = note - 48;
    env.Retrigger(false);
    if (currentSlice >= 0 && currentSlice < slices.size()) {
        samplePlayer.SetPitch(pitch);
        samplePlayer.Play(slices.at(currentSlice) * samplePlayer.GetSize());
    } else {
        samplePlayer.SetPitch(((float) note) - 48 + pitch);
        samplePlayer.Play();
    }
    playing = true;
}

void Instrument::Release() {
    playing = false;
}

void Instrument::NextParam() {
    if ((int) edit < 4) edit = (param) ((int) edit + 1);
    else edit = (param) 4;
}

void Instrument::PrevParam() {
    if ((int) edit > 0) edit = (param) ((int) edit - 1);
    else edit = (param) 0;
}

void Instrument::Increment() {
    if (edit == p) {
        pitch += 1;
        samplePlayer.SetPitch(pitch);
    } else if (edit == a) {
        att += Scaling(att);
        if (att > 5) att = 5;
        env.SetAttackTime(att);
    } else if (edit == d) {
        dec += Scaling(dec);
        if (dec > 5) dec = 5;
        env.SetDecayTime(dec);
    } else if (edit == s) {
        sus += 0.01;
        if (sus > 1.0f) sus = 1.0f;
        env.SetSustainLevel(sus);
    } else if (edit == r) {
        rel += Scaling(rel);
        if (rel > 5) rel = 5;
        env.SetReleaseTime(rel);
    }
}

void Instrument::Decrement() {
    if (edit == p) {
        pitch -= 1;
        samplePlayer.SetPitch(pitch);
    } else if (edit == a) {
        att -= Scaling(att);
        if (att < 0) att = 0.001;
        env.SetAttackTime(att);
    } else if (edit == d) {
        dec -= Scaling(dec);
        if (dec < 0) dec = 0.001;
        env.SetDecayTime(dec);
    } else if (edit == s) {
        sus -= 0.01;
        if (sus < 0) sus = 0;
        env.SetSustainLevel(sus);
    } else if (edit == r) {
        rel -= Scaling(rel);
        if (rel < 0) rel = 0.001;
        env.SetReleaseTime(rel);
    }
}