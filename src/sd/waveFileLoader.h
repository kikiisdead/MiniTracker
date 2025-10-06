#pragma once

#ifndef WAVE_FILE_LOADER_H
#define WAVE_FILE_LOADER_H

#include "daisy.h"
#include "fatfs.h"
#include "../audio/sampleplayer.h"
#include "../audio/instrument.h"
#include "decode.h"
#include "node.h"
#include "dirLoader.h"

using namespace daisy;

#define FILE_PTR false


struct Bit24x4 {
    int32_t val1;
    int32_t val2;
    int32_t val3;
};




class WaveFileLoader {
public:
    WaveFileLoader(){}
    ~WaveFileLoader(){}

    /**
     * Initializes the wave file object
     */
    void Init(float samplerate, void* (*sample_buffer_allocate)(size_t size), FIL* fil);

    /**
     * creates an instrument object from 
     * @param index selects the index of what file to read, if index is a dir, will call open dir instead
     * @return an instrument that the sampleDisplay UI thing will append it to the Instruments vector
     * Need to create a case where it opens a dir and returns a nullptr
     */
    Instrument* CreateInstrument(Node<File>* node);

    /**
     * creates an instrument object from 
     * \param index selects the index of what file to read, if index is a dir, will call open dir instead
     * \return an instrument that the sampleDisplay UI thing will append it to the Instruments vector
     * Need to create a case where it opens a dir and returns a nullptr
     */
    Instrument* CreateInstrument(std::string path);

private:
    FIL*   fil;
    float samplerate;
    void* (*sample_buffer_allocate)(size_t size);
};


#endif