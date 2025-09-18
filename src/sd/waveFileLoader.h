#pragma once

#ifndef WAVE_FILE_LOADER_H
#define WAVE_FILE_LOADER_H

#include "daisy.h"
#include "fatfs.h"
#include "../audio/sampleplayer.h"
#include "../audio/instrument.h"
#include "../audio/decode.h"

using namespace daisy;

/**
 * WAVEFILELOADER
 * loads wave files from SD Card into SDRAM
 * will return an instrument object with WavFile struct (file info + file name + void pointer to start index in SDRAM)
 * 
 * will have ability to select dirs and append things to them.
 */

struct file {
    char name[256];
    char searchpath[256];
    BYTE attrib;
};

class WaveFileLoader {
public:
    WaveFileLoader(){}
    ~WaveFileLoader(){}

    /**
     * Initializes the wave file object
     */
    void Init(float samplerate, void* (*sample_buffer_allocate)(size_t size));

    /**
     * Get names of all files in open dir
     * \return 2d char array 
     */
    std::vector<file> GetFileNames();

    /**
     * creates an instrument object from 
     * \param index selects the index of what file to read, if index is a dir, will call open dir instead
     * \return an instrument that the sampleDisplay UI thing will append it to the Instruments vector
     * Need to create a case where it opens a dir and returns a nullptr
     */
    Instrument* CreateInstrument(int index);

    /**
     * closes current and opens  next directory by appending to searchpath
     * \param appendPath a char array that is appended to the waveFileLoaders searchpath
     */
    void OpenDir(const char* appendPath);

    /**
     * closes current and opens  next directory by deleting from searchpath
     */
    void CloseDir(){};

private:
    SdmmcHandler   sd;
    FatFSInterface fsi;
    DIR            dir;
    FILINFO        fno;
    FIL            fil;
    bool           safe;
    char           searchpath[256]; // need method to append and subtract from searchpath so I can effectively open directories
    float          samplerate;
    std::vector<file> fileNames;
    void* (*sample_buffer_allocate)(size_t size);
};


#endif