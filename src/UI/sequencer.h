#pragma once
#ifndef SEQUENCER_H
#define SEQUENCER_H

#include "buttonInterface.h"
#include "daisy_seed.h"
#include "../audio/instrumentHandler.h"
#include "step.h"
#include <vector>

#define OFF -1

using namespace daisy;


/**
 * Lane described in 8 + x * 20
 * 4 bytes for Lane header
 * 4 bytes for length
 * x * 20 bytes for num of steps
 */
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

/**
 * pattern described in 8 bytes + size of all lanes in pattern
 * 4 bytes pattern label
 * 4 bytes numlanes
 * then all the lanes
 */
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
    std::vector<InstrumentHandler*>* handler_;
    std::vector<Pattern*>* patterns; // holds all the possible patterns made
    std::vector<int>* songOrder; // holds the order of the patterns
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

    void NextStep();
    void PreviousStep();
    void NextLane();
    void PreviousLane();
    void NextPattern();
    void PreviousPattern();

    void NewPattern();

public:

    Sequencer(){};

    /**
     * Sends triggers to the instrument handlers and moves sequencer forward
     */
    void Update();

    

    /**
     * Updates the BPM
     * \param bpm new bpm that will be changed to
     */
    void SetBPM(float bpm_) {
        bpm = bpm_;
        tick.SetFreq((bpm / 60.0f) * 4.0f);
        triggerTime = (1.0f / ((bpm / 60.0f) * 4.0f)) * 1000;
    }

    /**
     * Returns the BPM
     */
    float GetBPM() { return bpm; }
    
    /**
     * Initializes display
     * \param display to write to
     * \param handler voice for each lane
     * \param samplerate samplerate to intialize Metro object
     */
    void Init(std::vector<InstrumentHandler*>* handler, float samplerate, cFont* MainFont, std::vector<Pattern*>* patterns, std::vector<int>* songOrder) {
        //display_ = display;
        this->songOrder = songOrder;
        this->patterns = patterns;
        handler_ = handler;
        playing_ = false;
        stepEdit_ = false;
        song_ = false;
        songOrder->push_back(0);
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
    
    std::vector<Pattern*>* GetPatterns() { return patterns; } 

    std::vector<int>* GetSongOrder() { return songOrder; } 

    void Clear();

    void Safe();

    /**
     * Called by the Display object to refresh the screen, not called internally because it interupts audio playback 
     */
    void UpdateDisplay(cLayer *display);


    

    /**
     * INHERITED FROM BUTTON INTERFACE
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