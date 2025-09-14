#ifndef BUTTON_INTERFACE_H
#define BUTTON_INTERFACE_H

#include "daisy_seed.h"
#include "dev/oled_ssd130x.h"
#include "../../DaisySeedGFX2/cDisplay.h"
#include "util/colors.h"



using namespace daisy;
using namespace DadGFX;

using MyOledDisplay = OledDisplay<SSD130xI2c128x64Driver>;

/*
buttonInterface is an abstract class that all the displays will implement. Creates common HID functions between everything.
*/

class buttonInterface {
public:
    virtual void AButton() = 0;
    virtual void BButton() = 0;
    virtual void UpButton() = 0;
    virtual void DownButton() = 0;
    virtual void LeftButton() = 0;
    virtual void RightButton() = 0;
    virtual void PlayButton() = 0;

    virtual void AltAButton() = 0;
    virtual void AltBButton() = 0;
    virtual void AltUpButton() = 0;
    virtual void AltDownButton() = 0;
    virtual void AltLeftButton() = 0;
    virtual void AltRightButton() = 0;
    virtual void AltPlayButton() = 0;

    virtual void UpdateDisplay(cLayer* tft) = 0;

protected:
    
};

#endif