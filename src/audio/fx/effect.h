#ifndef EFFECT_H
#define EFFECT_H

#include "daisy_seed.h"
#include "daisysp.h"
#include "../../../DaisySeedGFX2/cDisplay.h"
#include "../../UI/util/colors.h"

using namespace daisy;
using namespace daisysp;
using namespace DadGFX;

#define FX_HEIGHT 210
#define FX_WIDTH 100
#define FX_BUFFER 5
#define EFFECT_NUMBER 2;

class Effect {
public:
    enum EFFECT_TYPE { NOFX, FILTER, DISTORTION, COMPRESSION };

    bool selected;
    EFFECT_TYPE effectType;

    virtual void Init(float samplerate, cFont* MainFont) = 0;
    virtual void Process(float&, float&) = 0;
    virtual void Display(cLayer*, int, int) = 0;
    virtual void Increment() = 0;
    virtual void Decrement() = 0;
    virtual void NextParam() = 0;
    virtual void PrevParam() = 0;

protected:
    char strbuff[20];
    int param;
    cFont* MainFont;

    void WriteString(cLayer* display, char* strbuff, int x, int y, DadGFX::sColor color) {
        display->setCursor(x, y);
        display->setFont(MainFont);
        display->setTextFrontColor(color);
        display->drawText(strbuff);
    }
};

#endif