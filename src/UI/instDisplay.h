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
using MyOledDisplay = OledDisplay<SSD130xI2c128x64Driver>;

class InstrumentDisplay : public buttonInterface {
public:

    InstrumentDisplay(){};
    ~InstrumentDisplay(){};

    void Init(std::vector<Instrument*>* instruments_, InstrumentHandler* handler_, cFont *MainFont);

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
    // MyOledDisplay* display;
    std::vector<Instrument*>* instruments;
    InstrumentHandler* handler; 
    std::vector<Instrument*>::iterator activeInst;
    int active;
    size_t currentSlice;
    char strbuff[256];
    bool sliceEdit;
    cFont* MainFont;

    void WriteString(cLayer* display, char* strbuff, int x, int y, DadGFX::sColor color);
};

#endif