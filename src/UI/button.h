#ifndef button_h
#define button_h

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
public:
    Button(){}
    ~Button(){}

    void Init(dsy_gpio_pin pin, float samplerate) {
        button.Init(pin, samplerate);
        pressed = false;
    }

    void CallbackHandler(void (*func)()) {
        buttonCallback = func;
    }

    void Update() {
        button.Debounce();
        if (button.RisingEdge()) {
            buttonCallback();
            timeAtPress = System::GetNow();
            delay = 150;
            pressed = true;
        } else if (pressed && (System::GetNow() > (timeAtPress + delay))) {
            buttonCallback();
            timeAtPress = System::GetNow();
            delay = 15;
        } else if (button.FallingEdge()) {
            pressed = false;
        }
    }
};

#endif