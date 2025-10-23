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
 * A UI object that displays the FX being used
 * FX are 3 per Lane and can be ay of the following
 *  - Filter
 *  - Distortion
 *  - Compression
 *  - Autopan
 *  - Redux
 * More FX will be added as development continues
 * 
 * @author Kiyoko Iuchi-Fung
 * @version 0.1.0
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

    /**
     * Opens change effect Menu
     * 
     * @note If change effect menu is open, will load selected effect
     */
    void AButton() override;

    /**
     * Closes change effect menu
     */
    void BButton() override;

    /**
     * Moves selected param to previous param
     */
    void UpButton() override;

    /**
     * Moves selected param to next param
     */
    void DownButton() override;

    /**
     * Increments selected param
     */
    void LeftButton() override;

    /**
     * Decrements selected param
     */
    void RightButton() override;
    
    /**
     * Changes current lane to previous lane
     */
    void AltUpButton() override;

    /**
     * Changes current lane to next lane
     */
    void AltDownButton() override;

    /**
     * Changes current effect to previous effect
     */
    void AltLeftButton() override;

    /**
     * Changes current effect to next effect
     */
    void AltRightButton() override;

    /**
     * Updates display
     * 
     * @note Displays lanes along the top
     * @note Displays 3 Effects along bottom
     * @note each effect has its own display function which gets called
     */
    void UpdateDisplay(cLayer* display);

private:
    float                               samplerate;     /**< Samplerate */
    size_t                              currentLane;    /**< Index of the current Lane */
    size_t                              currentEffect;  /**< Index of current Effect */
    char                                strbuff[20];    /**< string buffer for writing to display */
    InstrumentHandler                  *Lane;           /**< Pointer to active lane */
    Effect                             *effect;         /**< Pointer to active effect */
    std::vector<InstrumentHandler*>    *handler;        /**< Pointer to active instrument handler */
    int                                 param;          /**< Param to be edited */
    bool                                changeEffect;   /**< Change effect toggle */
    int                                 type;           /**< Type of effect to displayed */
    
    /**
     * Normalizes the selected effects
     */
    void EffectNormal() {
        CURRENT_EFFECT->selected = true;
        type = (int) CURRENT_EFFECT->effectType;
    }
};

#endif