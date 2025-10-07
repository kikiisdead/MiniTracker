#pragma once
#ifndef INST_DISPLAY_H
#define INST_DISPLAY_H

#include "buttonInterface.h"
#include "../audio/instrumentHandler.h"
#include "daisy_seed.h"
#include "dev/oled_ssd130x.h"
#include <vector>

#define ACTIVEINST instruments->at(active)

using namespace daisy;

/**
 * Instrument Display
 * interface that display all isntruments and allows simple editing like
 * ADSR, sample slicing, gain, and pitch shifting
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
    std::vector<Instrument*>* instruments;
    InstrumentHandler* handler; 
    std::vector<Instrument*>::iterator activeInst;
    int active;
    size_t currentSlice;
    char strbuff[256];
    bool sliceEdit;

};

#endif