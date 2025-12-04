#include "instrument.h"

void Instrument::Init(float samplerate, WavFile* sample, std::string  path) {
    Config cfg;
    cfg.Defaults();
    Init(samplerate, sample, path, cfg);
}

void Instrument::Init(float samplerate, WavFile* sample, std::string  path, Config cfg) {
    samplePlayer.Init(sample, samplerate);
    env.Init(samplerate);
    this->path = path;
    Load(cfg);
    edit = p;
    visual = samplePlayer.GetVisual(WAVEWIDTH);
}

void Instrument::Load(Config cfg) {

    att = cfg.Attack;
    dec = cfg.Decay;
    sus = cfg.Sustain;
    rel = cfg.Release;
    pitch = cfg.Pitch;
    gain = cfg.Gain;
    env.SetAttackTime(att);
    env.SetDecayTime(dec);
    env.SetSustainLevel(sus);
    env.SetReleaseTime(rel);
    slices.clear();
    for (double slice : cfg.slices) {
        slices.push_back(slice);
    }
}

void Instrument::Process(float& left, float& right) {
    env_out = env.Process(playing);

    float _left = 0;
    float _right = 0;

    if (!env.IsRunning() && samplePlayer.IsPlaying()) {
        samplePlayer.Stop(); // if the envelope has stopped and the sample is still playing: stop
        playing = false;
    } else if (currentSlice + 1 < slices.size()) {
        if (samplePlayer.GetPos() >= slices.at(currentSlice + 1) * samplePlayer.GetSize()) {
            samplePlayer.Stop();
            playing = false;
        }
    } else if (!samplePlayer.IsPlaying()) {
        playing = false;
    }
    samplePlayer.Process(_left, _right);

    left = _left * env_out * pow(10.0f, gain / 10.0f);
    right = _right * env_out * pow(10.0f, gain / 10.0f);
}

void Instrument::Trigger(int note) {
    currentSlice = note - 48;
    env.Retrigger(false);
    if (currentSlice >= 0 && currentSlice < slices.size()) {
        samplePlayer.SetPitch(pitch + pitchOffset);
        samplePlayer.Play(slices.at(currentSlice) * samplePlayer.GetSize());
    } else {
        samplePlayer.SetPitch(((float) note) - 48 + pitch + pitchOffset);
        samplePlayer.Play();
    }
    playing = true;
}

void Instrument::Release() {
    playing = false;
}

void Instrument::NextParam() {
    if ((int) edit < 5) edit = (param) ((int) edit + 1);
    else edit = (param) 5;
}

void Instrument::PrevParam() {
    if ((int) edit > 0) edit = (param) ((int) edit - 1);
    else edit = (param) 0;
}

void Instrument::Increment() {
    if (edit == p) {
        pitch += 1;
        samplePlayer.SetPitch(pitch);
    } else if (edit == g) {
        gain += 0.25;
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
    } else if (edit == g) {
        gain -= 0.25;
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