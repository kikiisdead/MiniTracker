/* Current Limitations:
- can only playback mono files right now (if passed a stereo file, will interpolate between channels)
- need to create separate buffers for left and right
*/
#pragma once
#ifndef SAMPLE_PLAYER
#define SAMPLE_PLAYER /**< Macro */
#include "daisy_core.h"
#include "daisy_seed.h"
#include "util/wav_format.h"
#include "ff.h"

#define WAV_FILENAME_MAX \
    256 /**< Maximum LFN (set to same in FatFs (ffconf.h) */

namespace daisy
{
// TODO: add bitrate, samplerate, length, etc.
/** Struct containing details of Wav File. */

struct WavFile
{
    WAV_FormatTypeDef format;               /**< Raw wav data */
    char              name[WAV_FILENAME_MAX]; /**< Wav filename */
    int32_t           **sample; // use a 2D array instead to deal with any num channels
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

    /** Initializes the SamplePlayer, loading up to max_files of wav files from an SD Card. */
    void Init(const unsigned int* sample, float samplerate_);

    /** \return the next sample played interpolated 
     * does logic based on number of channels
    */
    void Process(float& out);

    /** stereo out same as above except with 2 channels */
    void Process(float& outL, float& outR);


    /** plays the file from the beginning */
    void Play() { pos_ = 0; playing_ = true; }

    /** plays from specified position */
    void Play(double pos) { pos_ = pos; playing_ = true; }

    /** stops playback */
    void Stop() { playing_ = false; }

    /** \return position of the play head */
    double GetPos() { return pos_; }

    /** Sets whether or not the current file will repeat after completing playback. 
    \param loop To loop or not to loop.
    */
    inline void SetLooping(bool loop) { looping_ = loop; }

    /** \return Whether the SamplePlayer is looping or not. */
    inline bool GetLooping() const { return looping_; }

    /** Sets the pitch at which it plays and updates the sample speed
     * \param pitch_ the pitch
    */
    inline void SetPitch(float pitch_) { 
      pitch = pitch_; 
      samplePerStep = (((double) wave->format.SampleRate) / ((double) samplerate)) / pow(2.0, (pitch / 2.0)); 
    } 

    /** \return the pitch it plays at */
    inline float GetPitch() const { return pitch; }

  private:

    /** splits interleaved buff if interleaved */
    void Deinterleave();
    WavFile                 *wave;
    int32_t                 **buff_; // use a 2D array instead to deal with any num channels
    char                    *byte_buff_; // same as byte
    int                     numChannels;
    uint32_t                size_;
    bool                    looping_, playing_;
    float                   samplerate;
    double                  pos_;
    float                   samplePerStep;
    float                   pitch;
};

} // namespace daisy

#endif
