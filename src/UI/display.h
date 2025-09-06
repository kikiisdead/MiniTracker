#ifndef display_h
#define display_h

#include "sequencer.h"
#include "daisy_seed.h"
#include "step.h"
#include "dev/oled_ssd130x.h"
#include <vector>

using namespace daisy;
using MyOledDisplay = OledDisplay<SSD130xI2c128x64Driver>;

class Display {
private:
    MyOledDisplay* display_;
    Sequencer* sequencer;
    char strbuff[20];

    // Sequencer display methods
    void DrawStep(int x, int y, Step* step);
    void DrawSquare(int x, int y, bool fill);
    // 1 = left, 2 = right, 3 = up, 4 = down
    void DrawArrow(int x, int y, int direction); 
    void GetNoteString(char* strbuff, int note);
    void GetFxString(char* strbuff, int fx, int fxAmount);
    void InitStep(Step* step, int index);

public:
    Display(){}
    ~Display(){}

    void Init(MyOledDisplay* display, Sequencer* sequencer_);

    void UpdateDisplay();

};

#endif