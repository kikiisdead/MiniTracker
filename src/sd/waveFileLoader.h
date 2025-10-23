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

/**
 * A struct that reads 3 unsigned chars for reading 24 bit wave files
 */
struct Bit3x8 {
    uint8_t b1;
    uint8_t b2;
    uint8_t b3;
};

/**
 * The WaveFileoader loads wave files from an SD card and into SDRAM
 * 
 * @author Kiyoko Iuchi-Fung
 * @version 0.1.0
 */
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
    FIL    *fil;        /**< A pointer to the FIL object (needs to be global in scope) */
    float   samplerate; /**< The samplerate passed at Init */

    /**
     * A call back that allocates space inside SDRAM
     * @param size the size in bytes to be allocated inside SDRAM
     * @return a void pointer to the start of allocated memory
     */
    void* (*sample_buffer_allocate)(size_t size);

};


#endif