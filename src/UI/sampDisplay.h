#pragma once

#ifndef SAMP_DISPLAY_H
#define SAMP_DISPLAY_H

#include "buttonInterface.h"
#include "../sd/waveFileLoader.h"

#define MAX_BUFFER_SIZE (48 * 1024 * 1024)

/**
 * SampDisplay
 * The UI screen that displays the file directory and allow for file saving and loading
 * 
 * @author Kiyoko Iuchi-Fung
 * @version 0.1.0
 */
class SampDisplay : public buttonInterface {
public:
    SampDisplay(){}
    ~SampDisplay(){}

    struct Config {
        WaveFileLoader* fileLoader;
        std::vector<Instrument*>* instruments;
        cFont *MainFont;
        size_t* bufferIndex;
        Node<File>* rootNode;
        void (*sample_buffer_deallocate)(void* start, size_t size);
    };

    /**
     * Initialize with all functionality to add and remove
     * Updated to pass directory root node separate from waveFileLoader for future implementation of project saving and loading
     * @param fileLoader pointer to wave file loader object to load files into SDRAM
     * @param instruments pointer to vector holding all instruments (for display and appending)
     * @param MainFont pointer to main font for UI desgin consistency
     * @param bufferIndex pointer to the bufferIndex of sample_buffer in SDRAM (for figuring out how much space is left)
     * @param rootNode root node of tree representing samples in SDCard
     */
    void Init(WaveFileLoader* fileLoader, std::vector<Instrument*>* instruments, cFont *MainFont, size_t* bufferIndex, Node<File>* rootNode, void (*sample_buffer_deallocate)(void* start, size_t size));

    void Init(Config config);

    /**
     * INHERITED FROM BUTTON INTERFACE
     */

    /**
     * Opens selected file object
     *  - if that object is a DIR, open it and display contents
     *  - if that object is a wave file, load it as instrument
     *  - otherwise ignore it
     */
    void AButton() override;

    /**
     * Closes the selected DIR and moves user back one level
     * If the node has no parent, then don't do anything
     */
    void BButton() override;

    /**
     * Moves selector up
     */
    void UpButton();    

    /**
     * Moves selector down
     */
    void DownButton(); 

    /**
     * Deletes the selected isntrument calling DeleteInstrument
     */
    void AltBButton() override;

    /**
     * Increments the instrument selector
     */
    void AltUpButton() override;

    /**
     * Decrements the instrument selector
     */
    void AltDownButton() override;

    /**
     * Updates display
     * Shows open dirs on the left and loaded instruments on the write
     * @param display the display to be written to
     */
    void UpdateDisplay(cLayer* tft);

private:
    WaveFileLoader             *fileLoader;     /**< Pointer to the wave file loader object */
    std::vector<Instrument*>   *instruments;    /**< Pointer to the instrument vector */
    Node<File>                 *currentNode;    /**< The current node in the file directory being observed */
    size_t                     *bufferIndex;    /**< Pointer to the buffer index to display memory usage */
    
    int     row;            /**< Selected row  */
    int     col;            /**< Selected column */
    int     lev;            /**< Selected column relative to screen */
    int     scrRow;         /**< Selected row relative to screen */
    int     instRow;        /**< Selected row of inst relative to screen */
    size_t  selectedInst;   /**< Index of Selected Instrument */

    /**
     * Deallocates space inside SDRAM
     * moves everything to prevent fragmentation
     * @param start the starting memory position
     * @param size in bytes to deallocate
     */
    void (*sample_buffer_deallocate)(void* start, size_t size);

    /**
     * Deletes an instrument from the instrument array
     * @param index position of instrument in the array
     */
    void DeleteInstrument(int index);

};

#endif