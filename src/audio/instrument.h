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

/**
 * The Instrument class wraps the sample player object adding volume, 
 * ADSR, and slice control. It essentially functions as a boilerplate
 * for the instrument handler to interface with
 * 
 * @author Kiyoko Iuchi-Fung
 * @version 0.1.0
 */
class Instrument {
public:

    /**
     * The config struct serves as an easy way to instantiate the Instrument 
     * object without having a constructor / init function with an overwhelming
     * amount of parameters
     */
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

    /**
     * Enumeration containing the possible parameters
     */
    enum param { p, g, a, d, s, r };

    /**
     * Constructor not used as hardware needs to be initialized
     */
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
    void Init(float samplerate, WavFile* sample, std::string  path, Config cfg);

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

    /**
     * Releases the ADSR
     */
    void Release();

    /**
     * Checks if the instrument is playing or not
     * @return a boolean: true if playing, false if not
     */
    bool IsPlaying() { return playing; }

    /**
     * Gets the slice vector of the instrument
     * @return a vector of doubles corresponding to the slices
     */
    std::vector<double> GetSlices() { return slices; }

    /**
     * Gets the size of the instrument sample array
     * @return size of the instrument sample array
     */
    uint32_t GetSize() { return samplePlayer.GetSize(); }

    /**
     * Gets the pitch in semitones
     * @return the pitch of the isntrument
     */
    float GetPitch() { return pitch; }

    /**
     * Gets the attack time in ms
     * @return the attack time
     */
    float GetAttack() { return att; }

    /**
     * Gets the decay time in ms
     * @return the decay time
     */
    float GetDecay() { return dec; }

    /**
     * Gets the sustain level in %
     * @return the sustain level
     */
    float GetSustain() {return sus; }

    /**
     * Gets the release time in ms
     * @return the release time
     */
    float GetRelease() {return rel; }

    /**
     * Gets the gain in dB
     * @return the gain
     */
    float GetGain() { return gain; }

    /**
     * Gets the path of the sample in file directory
     * @return the path as a string
     */
    std::string GetPath() { return path; }

    /**
     * Gets the name of the sample as a char array 
     * @return the name of the sample as a char array
     */
    char* GetName() { return samplePlayer.GetName(); }

    /**
     * Gets the selected param to edit
     * @return the param being edited
     */
    param GetEdit() { return edit; }

    /**
     * Gets pointer to the start of sample in SDRAM
     * @return void pointer to start of sample
     */
    void* GetStart() { return samplePlayer.GetStart(); }

    /**
     * Sets pointer to start of sample in SDRAM
     * @param start new start position
     */
    void SetStart(void* start) { samplePlayer.SetStart(start); }

    /**
     * @brief Set the Pitch Offset of the instrument handler
     * made for step fx
     * 
     * @param pitchOffset 
     */
    void SetPitchOffset(float pitchOffset) {
        this->pitchOffset = pitchOffset;
    }

    /**
     * Increments the param to the next param
     */
    void NextParam();

    /**
     * Decrements the param to the next param
     */
    void PrevParam();

    /**
     * Increments the selected parameter
     */
    void Increment();

    /**
     * Decrements the selected parameter
     */
    void Decrement();

    uint16_t*           visual; /**< array of averages from the sample for display */
    std::vector<double> slices; /**< the slices / subsections of the sample to be played */

private:
    SamplePlayer    samplePlayer;   /**< The sample player object playing the sample */
    Adsr            env;            /**< The adsr object */
    float           env_out;        /**< A float to hold the output of the envelope */
    double          volume;         /**< The volume applied after the ADSR */
    size_t          currentSlice;   /**< The index of the current slice */
    bool            playing;        /**< Playing toggle */
    float           att;            /**< Attack time in ms */
    float           dec;            /**< Decay time in ms */
    float           sus;            /**< Sustain level in % */
    float           rel;            /**< Release time in ms */
    float           pitch;          /**< Pitch in semitones */
    float           pitchOffset;    /**< Pitch offset */
    float           gain;           /**< Gain in dB */
    param           edit;           /**< The param to be edited */
    std::string     path;           /**< Path to the sample in file direction */

    /**
     * Scales the parameter being edited to be logarithmic
     * Used for scaling the attack, decay, and release times
     * @param num the input to be scaled
     * @return the logarithmically scaled number
     */
    float Scaling(float num) { return pow(num, ((num / 5.0f) + 1.0f) / 2.0f) / 10.0f; }

};

#endif