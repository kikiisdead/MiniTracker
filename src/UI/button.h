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
    enum Pull {
        NO_PULL = 0,
        PULL_UP,
        PULL_DOWN
    };

    enum Edge {
        FALLING = 0,
        RISING
    };

    Switch      button;             /**< Switch the read GPIO values */
    uint32_t    timeAtPress;        /**< Time at press */
    int         delay;              /**< Delay between press and repetition */
    bool        pressed;            /**< Whether the switch is pressed or not */
    bool        repeatable;         /**< If it repeats when held */
    Edge        edge;

    /**
     * Function call back that is the action taken on button press
     */
    void (*buttonCallback)();  

    bool Activate() {
        if (edge == FALLING) {
            return button.FallingEdge();
        } else {
            return button.RisingEdge();
        }
    }

    bool Deactivate() {
        if (edge == FALLING) {
            return button.RisingEdge();
        } else {
            return button.FallingEdge();
        }
    }
public:
    Button(){}
    ~Button(){}

    enum BUTTON_VALUE {
        off = 0,
        press,
        hold,
        release
    };

    /**
     * Initializes the Button object 
     * @param pin the GPIO pin to read from
     * @param samplerate the sample rate
     * @param repeatable whether or not the button will repeat when held
     */
    void Init(dsy_gpio_pin pin, float samplerate, bool repeatable) {
        Init(pin, samplerate, repeatable, Switch::PULL_UP);
    }

    /**
     * @brief Initializes the Button object
     * 
     * @param pin the pin of the button
     * @param samplerate update rate of the button
     * @param repeatable whether it is repeatble or not
     * @param pull the internal resistor pull (PULL_NONE, PULL_UP, PULL_DOWN)
     */
    void Init(dsy_gpio_pin pin, float samplerate, bool repeatable, Switch::Pull pull) {
        Switch::Polarity pol = (pull == Switch::PULL_UP) ? Switch::POLARITY_INVERTED : Switch::POLARITY_NORMAL;
        button.Init(pin, samplerate, Switch::TYPE_MOMENTARY, pol, pull);
        pressed = false;
        this->repeatable = repeatable;
        if (pull == Switch::PULL_NONE) {
            edge = FALLING;
        } else {
            edge = RISING;
        }
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
        if (Activate()) {
            buttonCallback();
            timeAtPress = System::GetNow();
            delay = 250;
            pressed = true;
        } else if (pressed && (System::GetNow() > (timeAtPress + delay)) && repeatable) {
            buttonCallback();
            timeAtPress = System::GetNow();
            delay = 50;
        } else if (Deactivate()) {
            pressed = false;
        }
    }
};

/**
 * @brief ButtonHandler Class to make UX expandable and flexible
 * 
 * Will be able to parse combinations of button presses to push new commands onto the event Queue
 * 
 */
class ButtonHandler {
public:

   

    ButtonHandler(){}


};

#endif