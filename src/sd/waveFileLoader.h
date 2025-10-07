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
     * @param node the node from which to open
     * @overload of function that turns node into search path
     * @return a new instrument object pointer
     */
    Instrument* CreateInstrument(Node<File>* node);

    /**
     * creates an instrument object from 
     * @param path the search path of where to find the object
     * @return a new instrument object pointer
     */
    Instrument* CreateInstrument(std::string path);

private:
    FIL*   fil;
    float samplerate;
    void* (*sample_buffer_allocate)(size_t size);
};


#endif