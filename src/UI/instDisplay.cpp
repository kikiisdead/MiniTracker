#include "instDisplay.h"

void InstrumentDisplay::Init(MyOledDisplay* display_, std::vector<Instrument*> instruments_) {
    display = display_;
    instruments = instruments_;
    activeInst = instruments.begin();
}

void InstrumentDisplay::UpdateDisplay() {
    display->Fill(false);

    // Drawing waveform onto screen
    for (int i = 0; i < WAVEDISPLAY; i ++) {
        int y0, y1, range;
        range = (*activeInst)->visual[i] * (50.0f / (double) 0x8FFF);
        y0 = (range / 2) + 25;
        y1 = y0 - range;
        display->DrawLine(i, y0, i, y1, true);
    }

    
}