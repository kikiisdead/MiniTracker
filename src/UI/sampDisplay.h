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

    /**
     * Initialize with all functionality to add and remove
     * Updated to pass directory root node separate from waveFileLoader for future implementation of project saving and loading
     * @param fileLoader pointer to wave file loader object to load files into SDRAM
     * @param instruments pointer to vector holding all instruments (for display and appending)
     * @param MainFont pointer to main font for UI desgin consistency
     * @param bufferIndex pointer to the bufferIndex of sample_buffer in SDRAM (for figuring out how much space is left)
     * @param rootNode root node of tree representing samples in SDCard
     */
    void Init(WaveFileLoader* fileLoader, std::vector<Instrument*>* instruments, cFont *MainFont, size_t* bufferIndex, Node<File>* rootNode);

    /**
     * INHERITED FROM BUTTON INTERFACE
     */
    void AButton();
    void BButton();
    void UpButton();
    void DownButton();
    void LeftButton();
    void RightButton();
    void PlayButton();

    void AltAButton();
    void AltBButton();
    void AltUpButton();
    void AltDownButton();
    void AltLeftButton();
    void AltRightButton();
    void AltPlayButton();

    void UpdateDisplay(cLayer* tft);

private:
    WaveFileLoader             *fileLoader;     /**< Pointer to the wave file loader object */
    std::vector<Instrument*>   *instruments;    /**< Pointer to the instrument vector */
    Node<File>                 *currentNode;    /**< The current node in the file directory being observed */
    char                        strbuff[256];   /**< A string buffer to help display strings to the screen */
    size_t                     *bufferIndex;    /**< Pointer to the buffer index to display memory usage */
    int                         row;            /**< Selected row  */
    int                         col;            /**< Selected column */
    int                         lev;            /**< Selected column relative to screen */
    int                         scrRow;         /**< Selected row relative to screen */

};

#endif