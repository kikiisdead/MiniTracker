#pragma once
#ifndef FX_DISPLAY_H
#define FX_DISPLAY_H

#include "buttonInterface.h"
#include "../audio/instrumentHandler.h"
#include "daisy_seed.h"
#include <vector>

#define CURRENT_EFFECT handler->at(currentLane)->effects.at(currentEffect) // a little macro cozi write this too much

using namespace daisy;

/**
 * FXDisplay 
 * controls the effect interfaces belonging to all the instrument handlers
 */
class FXDisplay : public buttonInterface {
public:
    FXDisplay(){}
    ~FXDisplay(){}

    /**
     * Initializes the FXDisplay (needs to have none important constructor due to waiting for hardware init)
     * @param samplerate_ the samplerate of the program
     * @param handler_ pointer to a vector holding all instrumenthandlers which have the effects
     * @param MainFont pointer to main font for UI design consistency
     */
    void Init(float samplerate_, std::vector<InstrumentHandler*>* handler_, cFont* MainFont) {
        handler = handler_;
        samplerate = samplerate_;
        currentLane = 0;
        currentEffect = 0;
        changeEffect = false;
        this->MainFont = MainFont;
    }

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
    float samplerate;
    size_t currentLane;
    size_t currentEffect;
    char strbuff[20];
    InstrumentHandler* Lane;
    Effect* effect;
    std::vector<InstrumentHandler*>* handler;
    int param;
    bool changeEffect;
    int type;
    
    void EffectNormal() {
        CURRENT_EFFECT->selected = true;
        type = (int) CURRENT_EFFECT->effectType;
    }
};

#endif