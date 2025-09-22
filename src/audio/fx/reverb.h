#ifndef REVERB_H
#define REVERB_H

#include "effect.h"
#include "../../../../DaisyExamples/DaisySP/DaisySP-LGPL/Source/Effects/reverbsc.h"

class Reverb : public Effect {
public:
    Reverb(){}
    ~Reverb(){}

    void Init(float samplerate, cFont* MainFont) {
        param = 0;
        paramNum = 0;
        selected = false;
        effectType = NOFX;
        this->MainFont = MainFont;
    }

    void Init2(ReverbSc* reverb, float* lpFreq, float* feedback) {
        this->reverb = reverb;
        this->lpFreq = lpFreq;
        this->feedback = feedback;
    }

    void Process(float& left, float& right) {
        float inLeft = left * pow(10.0f, inputVol / 10.0f);
        float rightLeft = right * pow(10.0f, inputVol / 10.0f);
    }

    void Display(cLayer *display, int x, int y){

        sColor color = (selected) ? ACCENT2 : ACCENT1;

        display->drawRect(x + FX_BUFFER, y + FX_BUFFER, FX_WIDTH, FX_HEIGHT, 1, color);

        display->drawLine(x + FX_BUFFER + (FX_WIDTH / 2) - 5, y + FX_BUFFER + (FX_HEIGHT / 2), x + FX_BUFFER + (FX_WIDTH / 2) + 6, y + FX_BUFFER + (FX_HEIGHT / 2), color);
        display->drawLine(x + FX_BUFFER + (FX_WIDTH / 2), y + FX_BUFFER + (FX_HEIGHT / 2) - 5, x + FX_BUFFER + (FX_WIDTH / 2), y + FX_BUFFER + (FX_HEIGHT / 2) + 6, color);
    }

    void Increment(){}
    void Decrement(){}
    void NextParam(){}
    void PrevParam(){}

private:
    ReverbSc* reverb; // pointer because it takes up so much fucking SRAM

    float inputVol;
    float* lpFreq; 
    float* feedback;
};

#endif