#ifndef NO_EFFECT_H
#define NO_EFFECT_H

#include "effect.h"

class NoEffect : public Effect {
public:
    NoEffect(){}
    ~NoEffect(){}

    void Init(float samplerate) {
        selected = false;
        effectType = NOFX;
    }

    void Process(float& left, float& right) {

    }

    void Display(MyOledDisplay& display, int x, int y){
        // drawing box
        display.DrawRect(x, y, x + FX_WIDTH, y + FX_HEIGHT, true, selected);

        // drawing plus
        display.DrawLine(x + (FX_WIDTH / 2) - 2, y + FX_HEIGHT / 2, x + FX_WIDTH / 2 + 2, y + FX_HEIGHT / 2, !selected);
        display.DrawLine(x + (FX_WIDTH / 2), y + FX_HEIGHT / 2 + 2, x + FX_WIDTH / 2, y + FX_HEIGHT / 2 - 2, !selected);
    }

    void Increment(){}
    void Decrement(){}
    void NextParam(){}
    void PrevParam(){}
};

#endif