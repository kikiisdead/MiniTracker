#pragma once

#ifndef INSTRUMENT_H
#define INSTRUMENT_H

#include "daisy_seed.h"
#include "daisysp.h"
#include "sampleplayer.h"
#include "../sd/dirLoader.h"
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

    struct Config {
        float Attack;
        float Decay;
        float Sustain;
        float Release;
        float Pitch;
        float Gain;
        std::vector<double> slices;

        void Defaults() {
            Attack = 0.01f;
            Decay = 0.6f;
            Sustain = 0.0f;
            Release = 0.01f;
            Pitch = 0.0f;
            Gain = 0.0f;
            slices.push_back(0.0f);
        }

    };

    enum param { p, g, a, d, s, r };

    Instrument(){};

    /**
     * Initializes the instrument with sample rate and sample. Each instrument will only have one sample
     * @param samplerate samplerate of project, used to initialize daisysp obejcts
     * @param sample the wavefile object that contains pointers to the file in SDRAM
     * @param path the path to the file in the sdcard
     * @overload Init, passes config defaults as config
     */
    void Init(float samplerate, WavFile* sample, std::string path);

    /**
     * Initializes the instrument with sample rate and sample. Each instrument will only have one sample
     * @param samplerate samplerate of project, used to initialize daisysp obejcts
     * @param sample the wavefile object that contains pointers to the file in SDRAM
     * @param path the path to the file in the sdcard
     * @param cfg configuration to help with loading from project file
     */
    void Init(float samplerate, WavFile* sample, std::string path, Config cfg);

    /**
     * Loads configurations from project save file
     * @param cfg the configuration struct
     */
    void Load(Config cfg);

    /**
     * Called in audioCallback to get next sample
     * @param left float of left channel passed by reference so that the value changes
     * @param right float of right channel passed by reference so that the value changes
     */
    void Process(float& left, float& right);

    /**
     * Trigger the next sample slice or trigger at different pitch (depends on mode)
     * @param note the note to play
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

    std::string GetPath() { return path; }

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

    /**
     * the slices / subsections of the sample to be played
     */
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

    std::string path;

    float Scaling(float num) { return pow(num, ((num / 5.0f) + 1.0f) / 2.0f) / 10.0f; }

};

#endif