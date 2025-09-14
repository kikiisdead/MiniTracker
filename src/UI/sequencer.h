#ifndef SEQUENCER_H
#define SEQUENCER_H

#include "buttonInterface.h"
#include "daisy_seed.h"
#include "../audio/instrumentHandler.h"
#include "step.h"
#include "dev/oled_ssd130x.h"
#include <vector>

#define OFF -1

using namespace daisy;
using MyOledDisplay = OledDisplay<SSD130xI2c128x64Driver>;

class Lane {
public:
    Lane(){}
    ~Lane(){
        for (Step* step : sequence) {
            delete step;
        }
        delete &sequence;
    }
    std::vector<Step*> sequence;
    int length;
    int index;
};

class Pattern {
public:
    Pattern(){}
    ~Pattern(){
        for (Lane* lane : lanes) {
            delete lane;
        }
        delete &lanes;
    }
    std::vector<Lane*> lanes;
    int numLanes;
    int index;
};

class Sequencer : public buttonInterface {
private:
    // MyOledDisplay* display_;
    std::vector<InstrumentHandler*> handler_;
    std::vector<Pattern*> patterns; // holds all the possible patterns made
    std::vector<int> songOrder; // holds the order of the patterns
    Pattern* activePattern;

    cFont* MainFont;

    Step* currentStep;
    Lane* currentLane;
    int currentPattern;

    bool playing_;
    bool stepEdit_;
    bool song_;
    bool updateStep, updatePattern, updateSidebar;
    float bpm;
    float timePerTick;
    char strbuff[20];
    int laneOffset;
    uint32_t lastTrigger;
    uint32_t triggerTime;
    Metro tick;
    
    void DrawStep( cLayer *display, int x, int y, Step* step);

    void DrawSquare(cLayer *display, int index, int x, int y, bool fill);

    void DrawArrow(cLayer *display, int x, int y, int direction); // 1 = left, 2 = right, 3 = up, 4 = down

    void WriteString(cLayer* display, char* strbuff, int x, int y, DadGFX::sColor color);

    void GetNoteString(char* strbuff, int note);
    void GetFxString(char* strbuff, int fx, int fxAmount);
    void GetFxString(char* strbuff, int fx);
    void InitStep(Step* step, int index);

    void NextStep();
    void PreviousStep();
    void NextLane();
    void PreviousLane();
    void NextPattern();
    void PreviousPattern();

    void NewPattern();

public:
    Sequencer(){};

    // to be called continuously from the audio callback
    void Update();

    void SetBPM(float bpm_) {
        bpm = bpm_;
        tick.SetFreq((bpm / 60.0f) * 4.0f);
        triggerTime = (1.0f / ((bpm / 60.0f) * 4.0f)) * 1000;
    }
    
    /**
     * Initializes display
     * \param display to write to
     * \param handler voice for each lane
     * \param samplerate samplerate to intialize Metro object
     */
    void Init(std::vector<InstrumentHandler*> handler, float samplerate, cFont* MainFont) {
        //display_ = display;
        handler_ = handler;
        playing_ = false;
        stepEdit_ = false;
        song_ = false;
        songOrder.push_back(0);
        currentPattern = 0;
        tick.Init(1.0f, samplerate);
        SetBPM(178.0f);
        NewPattern();
        lastTrigger = 0;
        laneOffset = 0;
        this->MainFont = MainFont;
        updateStep = true;
        updateSidebar = true;
        updatePattern = true;
    }
    

    /**
     * Called by the Display object to refresh the screen, not called internally because it interupts audio playback 
     */
    void UpdateDisplay(cLayer *display);

    /**
     * Implementations of virtual parent functions.
     */
    void AButton();
    void BButton();
    void UpButton();
    void DownButton();
    void LeftButton();
    void RightButton();
    void PlayButton();

    void AltAButton();
    void AltBButton();
    void AltUpButton();
    void AltDownButton();
    void AltLeftButton();
    void AltRightButton();
    void AltPlayButton();
};

#endif