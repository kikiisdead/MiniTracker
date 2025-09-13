#ifndef FX_DISPLAY_H
#define FX_DISPLAY_H

#include "buttonInterface.h"
#include "../audio/instrumentHandler.h"
#include "daisy_seed.h"
#include "dev/oled_ssd130x.h"
#include <vector>

#define CURRENT_EFFECT handler.at(currentLane)->effects.at(currentEffect) // a little macro cozi write this too much

using namespace daisy;

using MyOledDisplay = OledDisplay<SSD130xI2c128x64Driver>;

class FXDisplay : public buttonInterface {
public:
    FXDisplay(){}
    ~FXDisplay(){}

    void Init(float samplerate_, std::vector<InstrumentHandler*> handler_, cFont* MainFont) {
        handler = handler_;
        samplerate = samplerate_;
        currentLane = 0;
        currentEffect = 0;
        changeEffect = false;
        EffectNormal();
        this->MainFont = MainFont;
    }

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
    std::vector<InstrumentHandler*> handler;
    int param;
    bool changeEffect;
    int type;
    cFont* MainFont;

    void WriteString(MyOledDisplay &oled, uint16_t x, uint16_t y, bool on);
    void WriteString(cLayer* display, char* strbuff, int x, int y, DadGFX::sColor color) {
        display->setCursor(x, y);
        display->setFont(MainFont);
        display->setTextFrontColor(color);
        display->drawText(strbuff);
    }
    
    void EffectNormal() {
        CURRENT_EFFECT->selected = true;
        type = (int) CURRENT_EFFECT->effectType;
    }
};

#endif