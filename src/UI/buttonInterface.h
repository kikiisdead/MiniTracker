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
 * though all methods are pure virtual, interfaces are not required to fully implement
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
    virtual void AButton() = 0;
    virtual void BButton() = 0;
    virtual void UpButton() = 0;
    virtual void DownButton() = 0;
    virtual void LeftButton() = 0;
    virtual void RightButton() = 0;
    virtual void PlayButton() = 0;

    /**
     * Controls alternative button controls (when shift is held)
     */
    virtual void AltAButton() = 0;
    virtual void AltBButton() = 0;
    virtual void AltUpButton() = 0;
    virtual void AltDownButton() = 0;
    virtual void AltLeftButton() = 0;
    virtual void AltRightButton() = 0;
    virtual void AltPlayButton() = 0;

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
    void WriteString(cLayer* display, char* strbuff, int x, int y, DadGFX::sColor color) {
        display->setCursor(x, y);
        display->setFont(MainFont);
        display->setTextFrontColor(color);
        display->drawText(strbuff);
    }

protected:
    cFont*  MainFont;   /**< The main font used in screens */
};

#endif