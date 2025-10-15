#ifndef REDUX_H
#define REDUX_H

#include "effect.h"
#include "../../../../DaisyExamples/DaisySP/DaisySP-LGPL/Source/Effects/bitcrush.h"

#define REDUXSCALE samplerate * pow(1.0f - crush, 1.5f) + 20.0f
#define OSCDISPRATE FX_WIDTH - 16.0f

/**
 * Bitcrush and downsampling effect
 * Wraps DaisySP Decimator in the Effect abstract class
 * 
 * @author Kiyoko Iuchi-Fung
 * @version 0.1.0
 */
class Redux : public Effect {
public:
    Redux(){}
    ~Redux(){}

    /**
     * Initializes the redux effect
     * Overrides init in abstract effect
     * @param samplerate the samplerate
     * @param MainFont pointer to the main font
     */
    void Init(float samplerate, cFont* MainFont) override {
        param = 0;
        paramNum = 2;
        selected = false;
        effectType = REDUX;
        this->MainFont = MainFont;

        crush = 0.0f;
        bitDepth = 16;

        this->samplerate = samplerate;
        bitcrushL.Init();
        bitcrushR.Init();
        osc.Init(OSCDISPRATE);

        bitcrushL.SetBitsToCrush(16 - bitDepth);
        bitcrushR.SetBitsToCrush(16 - bitDepth);

        bitcrushL.SetDownsampleFactor(crush);
        bitcrushR.SetDownsampleFactor(crush);
    }

    /**
     * Processes incoming audio through the decimator
     * Overrides Process in abstract effect
     * @param left left audio channel passed by reference to change the value
     * @param right right audio channel passed by reference to change the value
     */
    void Process(float& left, float& right) override {
        left  = bitcrushL.Process(left);
        right = bitcrushR.Process(right);
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

        osc.SetFreq(1);
        osc.SetAmp(1);


        float x0 = x + FX_BUFFER + 8;
        float y0 = y + FX_BUFFER + 28;

        int xDiff = ((float) FX_WIDTH - 16.0f) / (((float)FX_WIDTH - 16.0f - 4.0f) * pow(1.0f - crush, 2.0f) + 4.0f);

        for (int i = 0; i < OSCDISPRATE; i++) {
            float y1 = ((osc.Process() + 1.0f) / 2.0f) * 255;
            int yQuant = static_cast<uint8_t>(y1) >> ((16 - bitDepth) / 2); // chopping off bits 
            yQuant = yQuant << ((16 - bitDepth) / 2); // keeping most significant ones
            y1 = y + FX_BUFFER + 28 + ((static_cast<float>(yQuant) / 127.0f) - 1) * 20;
            float x1 = x + FX_BUFFER + 8 + i;
            if (i % xDiff == 0) {
                display->drawLine(x0, y1, x1, y1, color);
                y0 = y1;
            } else {
                display->drawLine(x0, y0, x1, y0, color);
            }
            x0 = x1;
        }

        sprintf(strbuff, "CRUSH");
        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 3 * (CHAR_HEIGHT + 4), color);

        if (REDUXSCALE < 100.0f) sprintf(strbuff, "%.2fhz", REDUXSCALE);
        else if (REDUXSCALE < 1000.0f) sprintf(strbuff, "%.1fhz", REDUXSCALE);
        else sprintf(strbuff, "%.2fkhz", ((float) REDUXSCALE) / 1000.0f);
        if (param == 0 && selected) WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 4 * (CHAR_HEIGHT + 4), ACCENT1);
        else                        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 4 * (CHAR_HEIGHT + 4), MAIN);


        sprintf(strbuff, "BITDEPTH");
        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 5 * (CHAR_HEIGHT + 4), color);

        sprintf(strbuff, "%dbit", bitDepth);
        if (param == 1 && selected) WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 6 * (CHAR_HEIGHT + 4), ACCENT1);
        else                        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 6 * (CHAR_HEIGHT + 4), MAIN);



    }

    /**
     * Increments the selected param
     * Overrides Increment in abstract effect
     */
    void Increment() override {
        if (param == 0) {
            crush -= 0.01f;
            if (crush < 0.0f) {
                crush = 0.0f;
            }
            bitcrushL.SetDownsampleFactor(crush);
            bitcrushR.SetDownsampleFactor(crush);
        } else if (param == 1) {
            bitDepth += 1;
            if (bitDepth > 16) {
                bitDepth = 16;
            }
            bitcrushL.SetBitsToCrush(16 - bitDepth);
            bitcrushR.SetBitsToCrush(16 - bitDepth);
        }
    }

    /**
     * Decrements the selected param
     * Overrides Decrement in abstract effect
     */
    void Decrement() override {
        if (param == 0) {
            crush += 0.01f;
            if (crush > 1.0f) {
                crush = 1.0f;
            }
            bitcrushL.SetDownsampleFactor(crush);
            bitcrushR.SetDownsampleFactor(crush);
        } else if (param == 1) {
            bitDepth -= 1;
            if (bitDepth < 1) {
                bitDepth = 1;
            }
            bitcrushL.SetBitsToCrush(16 - bitDepth);
            bitcrushR.SetBitsToCrush(16 - bitDepth);
        }
    }

    /**
     * Creates a config buffer
     * Overrides GetSnapshot in abstract effect
     * @param buf the char buf to write to 
     * NOTE only 32 byte buffer but no type safety
     */
    void GetSnapshot(char *buf) {
        
        void* ptr = &buf[0];

        *(static_cast<float*>(ptr)) = crush;
        ptr = static_cast<float*>(ptr) + 1;

        *(static_cast<int*>(ptr)) = bitDepth;
        ptr = static_cast<int*>(ptr) + 1;
    }

    /**
     * Loads from a config buffer
     * Overrides Load in abstract effect
     * @param buf the char buf to load from
     * @param samplerate the samplerate
     * @param MainFont pointer to the main font
     */
    void Load(char* buf, float samplerate, cFont* MainFont) {
        param = 0;
        paramNum = 2;
        selected = false;
        effectType = REDUX;

        void* ptr = &buf[0];

        this->samplerate = samplerate;

        this->MainFont = MainFont;

        crush = *(static_cast<float*>(ptr));
        ptr = static_cast<float*>(ptr) + 1;

        bitDepth = *(static_cast<int*>(ptr));
        ptr = static_cast<int*>(ptr) + 1;

        bitcrushL.Init();
        bitcrushR.Init();
        osc.Init(OSCDISPRATE);

        bitcrushL.SetBitsToCrush(16 - bitDepth);
        bitcrushR.SetBitsToCrush(16 - bitDepth);

        bitcrushL.SetDownsampleFactor(crush);
        bitcrushR.SetDownsampleFactor(crush);
    }

private:
    Decimator bitcrushL;
    Decimator bitcrushR;
    Oscillator osc;

    float crush;
    int bitDepth;

};

#endif