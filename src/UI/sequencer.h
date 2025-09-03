#ifndef sequencer_h
#define sequencer_h

#include "menu.h"
#include "daisy_seed.h"
#include "../audio/instrumentHandler.h"
#include "step.h"
#include "dev/oled_ssd130x.h"
#include <vector>
#include <list>

#define OFF -1

using namespace daisy;
using MyOledDisplay = OledDisplay<SSD130xI2c128x64Driver>;

struct Lane {
    std::vector<Step*> sequence;
    int length;
};

struct Pattern {
    Lane* lanes[4];
    int numLanes;
    int index;
};

class Sequencer : public Menu {
private:
    MyOledDisplay* display_;
    InstrumentHandler* handler_;
    std::vector<Pattern*> patterns; // holds all the possible patterns made
    std::vector<int> songOrder; // holds the order of the patterns
    Pattern* activePattern;
    int selector[3]; //selector[0] is index of pattern in song order, selector[1] is lane, selector[2] is step
    bool playing_;
    bool stepEdit_;
    bool song_;
    float bpm;
    float timePerTick;
    char strbuff[20];
    int laneOffset;
    Metro tick;
    
    void DrawStep(int x, int y, Step* step);
    void DrawSquare(int x, int y, bool fill);
    // 1 = left, 2 = right, 3 = up, 4 = down
    void DrawArrow(int x, int y, int direction); 
    void GetNoteString(char* strbuff, int note);
    void GetFxString(char* strbuff, int fx, int fxAmount);
    void InitStep(Step* step);

    void NextStep();
    void PreviousStep();
    void NextLane();
    void PreviousLane();
    void NextPattern();
    void PreviousPattern();

    void UpdateDisplay();

    void NewPattern();

public:
    Sequencer(){};

    // to be called continuously from the audio callback
    void Update();

    void SetBPM(float bpm_) {
        bpm = bpm_;
        tick.SetFreq((bpm / 60.0f) * 4.0f);
    }
    
    // Initializes the sequencer
    // will need to be passed 
    void Init(MyOledDisplay* display, InstrumentHandler* handler, float samplerate) {
        display_ = display;
        handler_ = handler;
        playing_ = false;
        stepEdit_ = false;
        song_ = false;
        songOrder.push_back(0);
        selector[0] = 0;
        selector[1] = 0;
        selector[2] = 0;
        tick.Init(1.0f, samplerate);
        SetBPM(128);
        NewPattern();
        laneOffset = 0;
        UpdateDisplay();
    }
    
    // sequencer controls
    void AButton();
    void BButton();
    void UpButton();
    void DownButton();
    void LeftButton();
    void RightButton();
    void PlayButton();

    void AltAButton(){};
    void AltBButton(){};
    void AltUpButton();
    void AltDownButton();
    void AltLeftButton();
    void AltRightButton();
    void AltPlayButton();
};

#endif