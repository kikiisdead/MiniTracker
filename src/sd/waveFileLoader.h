#pragma once

#ifndef WAVE_FILE_LOADER_H
#define WAVE_FILE_LOADER_H

#include "daisy.h"
#include "fatfs.h"
#include "../audio/sampleplayer.h"
#include "../audio/instrument.h"
#include "../audio/decode.h"
#include "node.h"

using namespace daisy;


struct Bit24x4 {
    int32_t val1;
    int32_t val2;
    int32_t val3;
};


/**
 * WAVEFILELOADER
 * loads wave files from SD Card into SDRAM
 * will return an instrument object with WavFile struct (file info + file name + void pointer to start index in SDRAM)
 * 
 * will have ability to select dirs and append things to them.
 */

struct File {
    char name[256];
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
     * Get Root Node of tree
     * \return the root node
     */
    Node<File>* GetRootNode() { return &rootNode; }

    /**
     * creates an instrument object from 
     * \param index selects the index of what file to read, if index is a dir, will call open dir instead
     * \return an instrument that the sampleDisplay UI thing will append it to the Instruments vector
     * Need to create a case where it opens a dir and returns a nullptr
     */
    Instrument* CreateInstrument(Node<File>* node);

    

private:
    FIL            fil;
    Node<File>     rootNode;
    bool           safe = false;
    char           searchpath[256]; // need method to append and subtract from searchpath so I can effectively open directories
    float          samplerate;
    void* (*sample_buffer_allocate)(size_t size);

    /**
     * Recursively searches through the SDCard to extract all file info and put into TREE
     * \param path the path to the directory being opened
     * \param node the current node being modified and added to
     */
    void OpenDir(const char* path, Node<File>* node);
};


#endif