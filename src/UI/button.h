#pragma once
#ifndef BUTTON_H
#define BUTTON_H

#include "daisy_seed.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

/**
 * A simple button class with call back to allow for user interaction
 * Implementation in main function pushes commands into an eventQueue
 * 
 * @author Kiyoko Iuchi-Fung
 * @version 0.1.0
 */

class Button {
private:
    Switch      button;             /**< Switch the read GPIO values */
    uint32_t    timeAtPress;        /**< Time at press */
    int         delay;              /**< Delay between press and repetition */
    bool        pressed;            /**< Whether the switch is pressed or not */
    bool        repeatable;         /**< If it repeats when held */

    /**
     * Function call back that is the action taken on button press
     */
    void (*buttonCallback)();  
public:
    Button(){}
    ~Button(){}

    /**
     * Initializes the Button object
     * @param pin the GPIO pin to read from
     * @param samplerate the sample rate
     * @param repeatable whether or not the button will repeat when held
     */
    void Init(dsy_gpio_pin pin, float samplerate, bool repeatable) {
        button.Init(pin, samplerate);
        pressed = false;
        this->repeatable = repeatable;
    }

    /**
     * Sets the function callback to be activated on button press
     * @param func the function to be called back
     */
    void CallbackHandler(void (*func)()) {
        buttonCallback = func;
    }

    /**
     * Debounces and checks the button. If it is pressed do call
     * the call back function
     */
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