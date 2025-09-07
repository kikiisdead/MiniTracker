#ifndef instDisplay_h
#define instDosplay_h

#include "buttonInterface.h"

#include "daisy_seed.h"
#include "../audio/instrument.h"
#include "dev/oled_ssd130x.h"
#include <vector>

using namespace daisy;
using MyOledDisplay = OledDisplay<SSD130xI2c128x64Driver>;

class InstrumentDisplay : public buttonInterface {
public:

    InstrumentDisplay(){};
    ~InstrumentDisplay(){};

    void Init(MyOledDisplay* display_, std::vector<Instrument*> instruments_);

    void AButton(){};
    void BButton(){};
    void UpButton(){};
    void DownButton(){};
    void LeftButton(){};
    void RightButton(){};
    void PlayButton(){};

    void AltAButton(){};
    void AltBButton(){};
    void AltUpButton(){};
    void AltDownButton(){};
    void AltLeftButton(){};
    void AltRightButton(){};
    void AltPlayButton(){};

    void UpdateDisplay();

private:
    MyOledDisplay* display;
    std::vector<Instrument*> instruments;
    std::vector<Instrument*>::iterator activeInst;
};

#endif