#pragma once

#ifndef PROJ_SAVER_LOADER_H 
#define PROJ_SAVER_LOADER_H

#include "../UI/sequencer.h"
#include "waveFileLoader.h"
#include "dirLoader.h"
#include <vector>
#include "../globals.h"

#include <string.h>

#define WRITE_BUFFER_SIZE 128

/**
 * The ProjSaverLoader loads and saves projects to the SDCard
 * Overwrites any of the current changes at load time
 * 
 * Saved file format is as follows (with byte sizes of items)
 *
 * - SEQUENCER HEADER (4 bytes)
 *     - bpm (4 bytes)
 *     - SONG ORDER HEADER(4 bytes)
 *         - Song order length (8 bytes)
 *         - pattern index (x * 4 bytes)
 *     - num patterns (8 bytes)
 *     - PATTERN HEADER (4 bytes)
 *         - num lanes (4 bytes)
 *         - LANE HEADER (4 bytes)
 *             - length (4 bytes)
 *             - STEP HEADER  (4 bytes)
 *                 - instrument (1 byte signed) 
 *                 - note (1 byte unsigned)
 *                 - fx (1 byte unsigned)
 *                 - fx amout (1 byte unsigned)
 * - EFFECTS CONTAINER HEADER (4 bytes)
 * - Num Lanes (4 bytes)
 *     - FX HEADER (4 bytes)
 *         - FX type (4 bytes)
 *         - efect specific params (32 bytes)
 * - INSTRUMENT CONTAINER HEADER (4 bytes)
 *     - num instruments (4 bytes)
 *     - INSTRUMENT HEADER (4 bytes)
 *         - attack (4bytes)
 *         - decay (4bytes)
 *         - sustain (4 bytes)
 *         - release (4 bytes)
 *         - gain (4 bytes)
 *         - pitch (4 bytes)
 *         - num slices (4 bytes)
 *         - slices (4 byets * num slices)
 *         - searchpath length (4 bytes unsigned)
 *         - searchpath (searchpath length bytes)
 * 
 * @author Kiyoko Iuchi-Fung
 * @version 0.1.0
 */
class ProjSaverLoader {
public:
    ProjSaverLoader(){}
    ~ProjSaverLoader(){}

    /**
     * Initializes the object with all containers of data that will be
     * used to save a file
     * @param savePath the overall save path that will be used to save projects
     * @param fil the FIL object used to save (for some reason needs to be global to write files)
     * @param sequencer the sequencer object that allows to access patterns 
     * @param instVector the container holding all Instrument objects
     * @param instHandler instrument handlers that house the effect containers
     * @param CLEAR a function callback that clears the patterns, frees up the memory, and marks those containers as unsafe
     * @param SAFE makrs containers as safe for access again
     * @param waveFileLoader to load wave files on project load
     * 
     */
    void Init(const char* savePath, FIL* fil, FIL *writeFIL, Sequencer* sequencer, std::vector<Instrument*>* instVector, std::vector<InstrumentHandler*>* instHandler, void (*CLEAR)(), void (*SAFE)(), WaveFileLoader* waveFileLoader);

    /**
     * Saves project
     * takes all data from patterns instruments and effects and writes them
     * to the file in a format that is easy to decode later on
     * @param name the name of the file to be saved MUST CONTAIN ".mtrk"
     */
    void SaveProject(const char* name);

    /**
     * Loads a project from a specified file, 
     * clears al patterns and instruments and recreates them based on 
     * the data from the save file
     * @param name name of the project MUST CONTAIN ".mtrk"
     */
    void LoadProject(const char* name);

private:
    std::string     savePath;               /**< The save path in file directory */
    bool            safe = false;           /**< Whether it is safe to save or not */
    char            buf[WRITE_BUFFER_SIZE]; /**< Buffer to write in larger chunks */
    size_t          buffer_index;           /**< Position of write head in buffer */

    FIL            *fil;            /**< Pointer to read FIL object */
    FIL            *writeFIL;       /**< Pointer to write FIL object */
    UINT            bytesWritten;   /**< Number of bytes written */

    Sequencer                          *sequencer;      /**< Pointer to the sequencer object */
    std::vector<Instrument::Config*>    instCfg;        /**< Vector of instrument config pointers for loading */
    std::vector<std::string*>           searchPaths;    /**< Vector of sample search paths for loading */
    std::vector<Instrument*>           *instVector;     /**< Pointer to the instrument vector */
    std::vector<InstrumentHandler*>    *instHandler;    /**< instrument Handler vector */
    WaveFileLoader                     *waveFileLoader; /**< Pointer to wave file loader object */

    /**
     * Function call back that clears all memory and marks project as unsafe
     */
    void (*CLEAR)();

    /**
     * Function that reinstates defaults and marks project as safe
     */
    void (*SAFE)();

    /**
     * Overload of other Write function
     * @param val value to write
     * passes sizeof(val) as size parameter to main Write function
     */
    template <typename T>
    void Write(T val);

    /**
     * Writes to my saving buffer to write data in WRITE_BUFFER_SIZE chunks
     * Templated and using void pointers to achieve type erasure to write 
     * many different data types into the same char buffer
     * @param val the value to write to the buffer
     * @param size the amount of bytes that val consists of
     */
    template <typename T>
    void Write(T val, size_t size);

    
    /**
     * Simple overload for the other Write function which passes sizeof
     * @param val as size parameter
     */
    void ResetAndPush();

    /**
     * Getting stack overflow so I needed to break up the functions
     */

    /**
     * Reads Pattern section of the saved file
     */
    void ReadPattern();

    /**
     * Reads FX section of the saved file
     */
    void ReadFX();

    /**
     * Reads Instrument section of the saved file
     */
    void ReadInstruments();

};

#endif