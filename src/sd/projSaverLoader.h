#pragma once

#ifndef PROJ_SAVER_LOADER_H 
#define PROJ_SAVER_LOADER_H

#include "../UI/sequencer.h"
#include "waveFileLoader.h"
#include "dirLoader.h"
#include <vector>

#include <string.h>

#define WRITE_BUFFER_SIZE 128

class ProjSaverLoader {
public:
    ProjSaverLoader(){}
    ~ProjSaverLoader(){}

    void Init(const char* savePath, FIL *fil, Sequencer* sequencer, std::vector<Instrument*>* instVector, std::vector<InstrumentHandler*>* instHandler, void (*CLEAR)(), void (*SAFE)(), WaveFileLoader* waveFileLoader);

    void SaveProject(const char* name);

    void LoadProject(const char* name);

private:
    std::string savePath;
    bool safe = false;
    char buf[WRITE_BUFFER_SIZE]; // can't do multisector writes 
    size_t buffer_index;

    FIL *fil;
    UINT bytesWritten;
    Sequencer* sequencer; 
    std::vector<Instrument*>* instVector;
    std::vector<InstrumentHandler*>* instHandler;
    WaveFileLoader* waveFileLoader;
    void (*CLEAR)();
    void (*SAFE)();

    /**
     * Write data into buffer 
     * If the data being added exceeds the buffer limit, it will write that chunk to the file
     * template method to accomodate for various types of data being sent to Write
     */
    template <typename T>
    void Write(T val);

    /**
     * Write data into buffer 
     * If the data being added exceeds the buffer limit, it will write that chunk to the file
     * template method to accomodate for various types of data being sent to Write
     */
    template <typename T>
    void Write(T val, size_t size);

    /**
     * Forces buffer into file and resets counters
     */
    void ResetAndPush();

    void* writePtr; 

};

#endif