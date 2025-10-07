#pragma once

#ifndef SAMP_DISPLAY_H
#define SAMP_DISPLAY_H

#include "buttonInterface.h"
#include "../sd/waveFileLoader.h"

#define MAX_BUFFER_SIZE (48 * 1024 * 1024)

/**
 * SampDisplay
 * an interface that displays samples to be loaded
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
    WaveFileLoader* fileLoader;
    std::vector<Instrument*>* instruments;
    Node<File> *currentNode;
    char strbuff[256];
    size_t* bufferIndex;
    int row, col, lev, scrRow;

};

#endif