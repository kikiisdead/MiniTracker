#pragma once
#ifndef BUTTON_H
#define BUTTON_H

#include "daisy_seed.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

class Button {
private:
    Switch button;
    uint32_t timeAtPress;
    void (*buttonCallback)();
    int delay;
    bool pressed;
    bool repeatable;
public:
    Button(){}
    ~Button(){}

    void Init(dsy_gpio_pin pin, float samplerate, bool repeatable) {
        button.Init(pin, samplerate);
        pressed = false;
        this->repeatable = repeatable;
    }

    void CallbackHandler(void (*func)()) {
        buttonCallback = func;
    }

    void Update() {
        button.Debounce();
        if (button.RisingEdge()) {
            buttonCallback();
            timeAtPress = System::GetNow();
            delay = 250;
            pressed = true;
        } else if (pressed && (System::GetNow() > (timeAtPress + delay)) && repeatable) {
            buttonCallback();
            timeAtPress = System::GetNow();
            delay = 50;
        } else if (button.FallingEdge()) {
            pressed = false;
        }
    }
};

#endif