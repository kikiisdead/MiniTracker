#ifndef EFFECT_H
#define EFFECT_H

#include "daisy_seed.h"
#include "daisysp.h"
#include "dev/oled_ssd130x.h"

using namespace daisy;
using namespace daisysp;

#define FX_HEIGHT 53
#define FX_WIDTH 39
#define EFFECT_NUMBER 2;

using MyOledDisplay = OledDisplay<SSD130xI2c128x64Driver>;

class Effect {
public:
    enum EFFECT_TYPE { NOFX, FILTER };

    bool selected;
    EFFECT_TYPE effectType;

    virtual void Init(float samplerate);
    virtual void Process(float&, float&) = 0;
    virtual void Display(MyOledDisplay&, int, int) = 0;
    virtual void Increment() = 0;
    virtual void Decrement() = 0;
    virtual void NextParam() = 0;
    virtual void PrevParam() = 0;

protected:
    char strbuff[20];
    int param;
    

    /**
     * Common display functions
     * \param direction 0 = left, 1 = up, 2 = right, 3 = down
     */
    void DrawArrow(MyOledDisplay& display, int x, int y, int direction, bool on) {
        display.DrawPixel(x, y, on);
        if (direction == 0) {
            display.DrawPixel(x + 1, y - 1, on);
            display.DrawPixel(x + 1, y + 1, on);
        } else if (direction == 1) {
            display.DrawPixel(x - 1, y - 1, on);
            display.DrawPixel(x + 1, y - 1, on);
        } else if (direction == 2) {
            display.DrawPixel(x - 1, y - 1, on);
            display.DrawPixel(x - 1, y + 1, on);
        } else if (direction == 3) {
            display.DrawPixel(x - 1, y + 1, on);
            display.DrawPixel(x + 1, y + 1, on);
        }
    };

    void WriteString(MyOledDisplay& display, int x, int y, bool on) {
        display.SetCursor(x, y);
        display.WriteString(strbuff, Font_4x6, on);
    }
};

#endif