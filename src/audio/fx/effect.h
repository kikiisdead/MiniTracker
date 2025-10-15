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

/**
 * The Effect class is an abstract framework for effects to be used in
 * It also has basic user interactivity built in
 * 
 * @author Kiyoko Iuchi-Fung
 * @version 0.1.0
 */
class Effect {
public:
    Effect(){}
    virtual ~Effect(){} // destructor is virtual to allow for potential specific implementation

    /**
     * The possible effects so far
     * There will hopefully be more
     */
    enum EFFECT_TYPE { NOFX, FILTER, DISTORTION, COMPRESSION, AUTOPAN, REDUX };

    bool        selected;   /**< selected toggle for display and editing */
    EFFECT_TYPE effectType; /**< The instance Effect Type */

    /**
     * Pure virtual initializer
     * @param samplerate samplerate
     * @param MainFont the main font for design consistency
     */
    virtual void Init(float samplerate, cFont* MainFont) = 0;

    /**
     * Pure virtual process function
     * Processes the samples and applies the effect. 
     * @param left the left channel input passed by reference
     * @param right the right channel input passed by reference
     */
    virtual void Process(float& left, float& right) = 0;

    /**
     * Pure virtual display function
     * Displays the effect to the screen
     * @param display display to be drawn to
     * @param x the x offset
     * @param y the y offset
     */
    virtual void Display(cLayer* display, int x, int y) = 0;

    /**
     * Pure virtual increment function
     * Increments selected parameter
     */
    virtual void Increment() = 0;

    /**
     * Pure virtual decrement function
     * Decrements selected parameter
     */
    virtual void Decrement() = 0;

    /**
     * Virtual next param
     * Increments to the next parameter
     */
    virtual void NextParam() {
        param += 1;
        if (param > paramNum - 1) {
            param = paramNum - 1;
        }
    }

    /**
     * Virtual prev param
     * Decrements to the next parameter
     */
    virtual void PrevParam() {
        param -= 1;
        if (param < 0) {
            param = 0;
        }
    }

    /**
     * Makes an array parameter values to be used when saving
     * @param buf a pointer to a char buf
     */
    virtual void GetSnapshot(char *buf) = 0;

    /**
     * Loads and initializes the effect based on an input array
     * @param buf the input char array with saved param values
     * @param samplerate the samplerate
     * @param MainFont main font to be used when displaying
     */
    virtual void Load(char *buf, float samplerate, cFont* MainFont) = 0;

protected:
    char    strbuff[20];    /**< Char buff used to write strings */
    int     param;          /**< Selected parameter */
    int     paramNum;       /**< Number of parameters */
    float   samplerate;     /**< Samplerate */
    cFont  *MainFont;       /**< MainFont for displaying text to display */

    /**
     * Writes a string to the display
     * @param display display to be written to
     * @param strbuff buffer to be used
     * @param x the x offset
     * @param y the y offset
     * @param color the color of the text
     */
    void WriteString(cLayer* display, char* strbuff, int x, int y, DadGFX::sColor color) {
        display->setCursor(x, y);
        display->setFont(MainFont);
        display->setTextFrontColor(color);
        display->drawText(strbuff);
    }
};

#endif