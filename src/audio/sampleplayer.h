#pragma once
#ifndef SAMPLE_PLAYER
#define SAMPLE_PLAYER /**< Macro */
#include "daisy_core.h"
#include "daisy_seed.h"
#include "util/wav_format.h"
#include "ff.h"

#define SAMP_POINTER static_cast<float*>(wave->start)

#define WAV_FILENAME_MAX \
    256 /**< Maximum LFN (set to same in FatFs (ffconf.h) */

namespace daisy
{

/** Struct containing details of Wav File. */

struct WavFile
{
    WAV_FormatTypeDef           format;                 /**< Raw wav data */
    char                        name[WAV_FILENAME_MAX]; /**< Wav filename */
    void*                       start;                  /**< start index in SDRAM buffer */
};

/* 
TODO:
- Make template-y to reduce memory usage.
*/

class SamplePlayer
{
public:
    SamplePlayer() {}
    ~SamplePlayer() {}

    /** Initializes the SamplePlayer, loading up to max_files of wav files from an SD Card. 
     * @param wave_ the wavfile object to be played from (needs pointer to sample in SDRAM)
     * @param samplerate_ the samplerate of the project to determine interpolation and step size
     */
    void Init(WavFile* wave_, float samplerate_);

    /**
     * Processes audio by getting the next step to be taken
     * @param outL reference to a float whose value will be the interpolated sample of the left channel
     * @param outR reference to a float whose value will be the interpolated sample of the right channel
     */
    void Process(float& outL, float& outR);


    /**
     * Plays sample from the start
     */
    void Play() { pos_ = 0; playing_ = true; }

    /**
     * Plays sample from a specified position
     */
    void Play(double pos) { pos_ = pos; playing_ = true; }

    /** 
     * stops playback 
     */
    void Stop() { playing_ = false; }

    /** 
     * Returns the current position of playback
     * @return position of the play head 
     */
    double GetPos() { return pos_; }

    uint32_t GetSize() { return size_; }

    /** 
     * Sets whether or not the current file will repeat after completing playback. 
     * @param loop To loop or not to loop.
     */
    inline void SetLooping(bool loop) { looping_ = loop; }

    /** \return Whether the SamplePlayer is looping or not. */
    inline bool GetLooping() const { return looping_; }

    /** 
     * Sets the pitch at which it plays and updates the sample speed
     * @param pitch_ the pitch
     */
    inline void SetPitch(float pitch_) { 
      pitch = pitch_; 
      samplePerStep = (((double) wave->format.SampleRate) / ((double) samplerate)) * pow(2.0f, (pitch / 12.0f)); 
    } 

    /** 
     * Gets the current pitch
     * @return the pitch it plays at 
     */
    inline float GetPitch() const { return pitch; }

    /**
     * Gets playing value
     * @return value of playing
     */
    inline bool IsPlaying() { return playing_; }

    uint16_t* GetVisual (int length);

    char* GetName() { return wave->name; }

    int GetNumChannels() { return wave->format.NbrChannels; }

    int GetSampleRate() { return wave->format.SubChunk2ID; }
    
    int GetBitsPerSample() { return wave->format.SubCHunk2Size; }

    int GetRawSize() { return wave->format.AudioFormat; }

private:

    WavFile                 *wave;
    int                     numChannels;
    uint32_t                size_;
    bool                    looping_, playing_;
    float                   samplerate;
    double                  pos_;
    float                   samplePerStep;
    float                   pitch;
};

} 

#endif
