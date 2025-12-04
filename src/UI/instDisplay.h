#pragma once
#ifndef INST_DISPLAY_H
#define INST_DISPLAY_H

#include "buttonInterface.h"
#include "../audio/instrumentHandler.h"
#include "daisy_seed.h"
#include "dev/oled_ssd130x.h"
#include <vector>

#define ACTIVEINST instruments->at(active)

#define INST_ROW_MAX 11

using namespace daisy;

/**
 * Instrument Display
 * A UI object that display all isntruments and allows simple editing like
 * ADSR, sample slicing, gain, and pitch shifting
 * 
 * @author Kiyoko Iuchi-Fung
 * @version 0.1.0
 */
class InstrumentDisplay : public buttonInterface {
public:

    InstrumentDisplay(){};
    ~InstrumentDisplay(){};

    /**
     * Initializes the the Instrument Display
     * @param instruments_ a pointer to a vector containing all the instruments (pointer because issues with passing container and mutability)
     * @param handler_ one of the instrument handlers to allow for preview playback
     * @param MainFont pointer to the main font used throughout for display consistency
     */
    void Init(std::vector<Instrument*>* instruments_, InstrumentHandler* handler_, cFont *MainFont);

    /**
     * INHERITED FROM BUTTONINTERFACE (ALLOWS FOR HID CONTROL)
     */

    /**
     * If in SLICE, adds a slice between the current slice and next slice
     */
    void AButton() override;

    /**
     * If in SLICE, deletes the current slice
     */
    void BButton() override;

    /**
     * Increments selected param
     * 
     * @note If in SLICE, increments position of current slice
     * @note If in PARAM, increments the selected parameter
     */
    void UpButton() override;

    /**
     * Decrements selected param
     * 
     * @note If in SLICE, decrements position of current slice
     * @note If in PARAM, decrements the selected parameter
     */
    void DownButton() override;

    /**
     * Moves selection left
     * 
     * @note If in SLICE, selects previous slice
     * @note If in PARAM, moves parameter select left
     */
    void LeftButton() override;

    /**
     * Moves selection right
     * 
     * @note If in SLICE, selects next slice
     * @note If in PARAM, moves parameter select right
     */
    void RightButton() override;

    /**
     * Previews the current slice without any lane FX
     */
    void PlayButton() override;

    /**
     * Changes instrument being viewed to instrument above current
     */
    void AltUpButton() override;

    /**
     * Changes instrument being viewed to instrument below current
     */
    void AltDownButton() override;

    /**
     * Toggles between SLICE mode and PARAM mode
     */
    void AltPlayButton() override;

    /**
     * Updates Display
     * 
     * @note Displays waveform in top left
     * @note Displays parameters along bottom
     * @note Displays instruments in top right
     * 
     * @param display the display to be written to
     */
    void UpdateDisplay(cLayer* tft);

private:
    std::vector<Instrument*>           *instruments;    /**< Pointer to the instrument vector (prevents concurrent modification) */
    InstrumentHandler                  *handler;        /**< Pointer to the instrument handler object */
    std::vector<Instrument*>::iterator  activeInst;     /**< An iterator belonging to the vector to display the active instrument */
    
    int     active;         /**< The position of the active instruent being edited */
    size_t  currentSlice;   /**< The current slice being edited */
    bool    sliceEdit;      /**< Whether we are editing slices or not */
    int     scrRow;         /**< The row of selected instrument on screen */                  

};

#endif