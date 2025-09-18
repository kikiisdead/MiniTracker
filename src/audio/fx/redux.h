#ifndef REDUX_H
#define REDUX_H

#include "effect.h"
#include "../../../../DaisyExamples/DaisySP/DaisySP-LGPL/Source/Effects/bitcrush.h"

#define REDUXSCALE samplerate * pow(1.0f - crush, 1.5f) + 20.0f
#define OSCDISPRATE FX_WIDTH - 16.0f

class Redux : public Effect {
public:
    Redux(){}
    ~Redux(){}

    void Init(float samplerate, cFont* MainFont) {
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

        bitcrushL.SetBitsToCrush(bitDepth);
        bitcrushR.SetBitsToCrush(bitDepth);

        bitcrushL.SetDownsampleFactor(crush);
        bitcrushR.SetDownsampleFactor(crush);
    }

    void Process(float& left, float& right) {
        left  = bitcrushL.Process(left);
        right = bitcrushR.Process(right);
    }

    void Display(cLayer *display, int x, int y){

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

    void Increment(){
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

    void Decrement(){
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

private:
    Decimator bitcrushL;
    Decimator bitcrushR;
    Oscillator osc;

    float crush, samplerate;
    int bitDepth;

};

#endif