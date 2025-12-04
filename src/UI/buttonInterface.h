#ifndef BUTTON_INTERFACE_H
#define BUTTON_INTERFACE_H

#include "daisy_seed.h"
#include "dev/oled_ssd130x.h"
#include "../../DaisySeedGFX2/cDisplay.h"
#include "util/colors.h"



using namespace daisy;
using namespace DadGFX;

using MyOledDisplay = OledDisplay<SSD130xI2c128x64Driver>;


/**
 * buttonInterface is an abstract class that controls HID and UI
 * inherited by all screens to allow for polymorphic control of various interfaces
 * also includes some generic common utilities for UI like writestring 
 * all methods if button has no use for that specific interface
 * 
 * @author Kiyoko Iuchi-Fung
 * @version 0.1.0
 */
class buttonInterface {
public:
    /**
     * Controls main button controls
     */

    virtual void AButton(){};
    virtual void BButton(){};
    virtual void UpButton(){};
    virtual void DownButton(){};
    virtual void LeftButton(){};
    virtual void RightButton(){};
    virtual void PlayButton(){};

    /**
     * Controls alternative button controls (when shift is held)
     */
    
    virtual void AltAButton(){};
    virtual void AltBButton(){};
    virtual void AltUpButton(){};
    virtual void AltDownButton(){};
    virtual void AltLeftButton(){};
    virtual void AltRightButton(){};
    virtual void AltPlayButton(){};

    /**
     * Updates display
     * @param display display / layer to be drawn to
     */
    virtual void UpdateDisplay(cLayer* display) = 0;

    /**
     * Writes a string to a display layer
     * @param display display / layer to be written
     * @param strbuff the char array to be written
     * @param x pos to write to
     * @param y pos to write to
     * @param color the color of the text
     */
    void WriteString(cLayer* display, int x, int y, DadGFX::sColor color) {
        display->setCursor(x, y);
        display->setFont(MainFont);
        display->setTextFrontColor(color);
        display->drawText(strbuff);
    }

    /**
     * Writes a string to a display layer
     * @param display display / layer to be written
     * @param x pos to write to
     * @param y pos to write to
     * @param color the color of the text
     */
    void WriteString(cLayer* display, int x, int y, DadGFX::sColor color, const char* fmt, ...) {
        va_list args;
        va_start(args, fmt);
        vsnprintf(strbuff, 256, fmt, args);
        va_end(args);
        display->setCursor(x, y);
        display->setFont(MainFont);
        display->setTextFrontColor(color);
        display->drawText(strbuff);
    }

    /**
     * Gets the next button interface
     * @return pointer to said interface
     */
    buttonInterface* GetNext() {
        return next;
    }

    /**
     * Updates the pointer to the next buttonInterface object
     * @param next interface object
     */
    void SetNext(buttonInterface* next) {
        this->next = next;
    }

    /**
     * Gets the prev button interface
     * @return pointer to said interface
     */
    buttonInterface* GetPrev() {
        return prev;
    }

    /**
     * Updates the pointer to the prev buttonInterface object
     * @param prev interface object
     */
    void SetPrev(buttonInterface* prev) {
        this->prev = prev;
    }

protected:
    cFont*  MainFont;   /**< The main font used in screens */
    buttonInterface* next;  /**< Pointer to next ui screen */
    buttonInterface* prev;  /**< Pointer to prev ui screen */
    
    char strbuff[256]; /**< char buffer to write strings to the screen */
};

#endif