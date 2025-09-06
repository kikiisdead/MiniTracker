#ifndef display_h
#define display_h

#include "daisy_seed.h"
#include "step.h"
#include "dev/oled_ssd130x.h"
#include "buttonInterface.h"
#include <vector>

using namespace daisy;
using MyOledDisplay = OledDisplay<SSD130xI2c128x64Driver>;

class Display {
public:
    Display(){}
    ~Display(){}

    void Init(buttonInterface* interface, MyOledDisplay* display) {
        interface_ = interface;
        display_ = display;
    }

    void Update();

private:
    buttonInterface* interface_;
    MyOledDisplay* display_;
    char* strbuff;



};

#endif