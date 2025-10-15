#ifndef NO_EFFECT_H
#define NO_EFFECT_H

#include "effect.h"

/**
 * NoEffect is a subclass of Effect but without any audio processing
 * basically like an empty effect
 */
class NoEffect : public Effect {
public:
    NoEffect(){}
    ~NoEffect(){}

    /**
     * Initializes the NoEffect 
     * Overrides Init in abstract effect
     * @param samplerate the samplerate
     * @param MainFont pointer to the main font
     */
    void Init(float samplerate, cFont* MainFont) override {
        param = 0;
        paramNum = 0;
        selected = false;
        effectType = NOFX;
        this->MainFont = MainFont;
    }

    /**
     * Does nothing to the audio.
     * Overrides Process in abstract effect
     * @param left left audio channel passed by reference to change the value
     * @param right right audio channel passed by reference to change the value
     */
    void Process(float& left, float& right) override {

    }

    /**
     * Displays the effect to the screen
     * Overrides Display in abstract effect
     * @param display pointer to display to write to
     * @param x the x offset
     * @param y the y offset
     */
    void Display(cLayer *display, int x, int y) override {

        sColor color = (selected) ? ACCENT2 : ACCENT1;

        display->drawRect(x + FX_BUFFER, y + FX_BUFFER, FX_WIDTH, FX_HEIGHT, 1, color);

        display->drawLine(x + FX_BUFFER + (FX_WIDTH / 2) - 5, y + FX_BUFFER + (FX_HEIGHT / 2), x + FX_BUFFER + (FX_WIDTH / 2) + 6, y + FX_BUFFER + (FX_HEIGHT / 2), color);
        display->drawLine(x + FX_BUFFER + (FX_WIDTH / 2), y + FX_BUFFER + (FX_HEIGHT / 2) - 5, x + FX_BUFFER + (FX_WIDTH / 2), y + FX_BUFFER + (FX_HEIGHT / 2) + 6, color);
    }

    /**
     * Increments the selected param
     * Overrides Increment in abstract effect
     */
    void Increment(){}
    
    /**
     * Decrements the selected param
     * Overrides Decrement in abstract effect
     */
    void Decrement(){}
    
    void NextParam(){}
    
    void PrevParam(){}

    /**
     * Creates a config buffer
     * Overrides GetSnapshot in abstract effect
     * @param buf the char buf to write to 
     * NOTE only 32 byte buffer but no type safety
     */
    void GetSnapshot(char *buf) override {

    }

    /**
     * Loads from a config buffer
     * Overrides Load in abstract effect
     * @param buf the char buf to load from
     * @param samplerate the samplerate
     * @param MainFont pointer to the main font
     */
    void Load(char *buf, float samplerate, cFont* MainFont) override {
        param = 0;
        paramNum = 0;
        selected = false;
        effectType = NOFX;
        this->MainFont = MainFont;
    }

};

#endif