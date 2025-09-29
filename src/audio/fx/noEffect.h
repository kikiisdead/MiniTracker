#ifndef NO_EFFECT_H
#define NO_EFFECT_H

#include "effect.h"

class NoEffect : public Effect {
public:
    NoEffect(){}
    ~NoEffect(){}

    void Init(float samplerate, cFont* MainFont) {
        param = 0;
        paramNum = 0;
        selected = false;
        effectType = NOFX;
        this->MainFont = MainFont;
    }

    void Process(float& left, float& right) {

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

    void GetSnapshot(char *buf) {

    }

    void Load(char *buf, float samplerate, cFont* MainFont) {
        param = 0;
        paramNum = 0;
        selected = false;
        effectType = NOFX;
        this->MainFont = MainFont;
    }

};

#endif