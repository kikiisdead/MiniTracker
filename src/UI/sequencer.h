#pragma once
#ifndef SEQUENCER_H
#define SEQUENCER_H

#include "buttonInterface.h"
#include "daisy_seed.h"
#include "../audio/instrumentHandler.h"
#include "step.h"
#include <vector>
#include "../globals.h"

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

/**
 * Sequencer
 * Interface for the main sequencing page of the project (literally the most important thing)
 * Controls all the sequencing things including triggering
 * Basically the main orchestrator for the audio components of the project
 * 
 * @author Kiyoko Iuchi-Fung
 * @version 0.1.0
 */
class Sequencer : public buttonInterface {
public:

    Sequencer(){};

    /**
     * Update()
     * Sends a step object to the instrument handler at specific rate defined by
     * metro object (tick)
     */
    void Update();

    /**
     * Updates the BPM based on the global BPM held in globals.h
     */
    void SetBPM() {
        __disable_irq();
        bpm_ = BPM;
        tick.SetFreq((BPM / 60.0f) * 4.0f);
        __enable_irq();
    }
    
    /**
     * Initializes display
     * @param display to write to
     * @param handler voice for each lane
     * @param samplerate samplerate to intialize Metro object
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
        SetBPM();
        NewPattern();
        laneOffset = 0;
        this->MainFont = MainFont;
        updateStep = true;
        updateSidebar = true;
        updatePattern = true;
    }
    
    /**
     * Gets the pattern vector pointer from the sequencer (mostly for backwards compatibility)
     * @return the pattern vector pointer 
     */
    std::vector<Pattern*>* GetPatterns() { return patterns; } 

    /**
     * Gets the song order vector pointer from the sequencer (mostly for backwards compatibility)
     * @return the song order vector pointer 
     */
    std::vector<int>* GetSongOrder() { return songOrder; } 

    /**
     * Clears all patterns from the sequencer
     *  - Gets rid of all steps and lanes and patterns
     *  - Gets rid of all ints in song order
     *  - Marks the sequence as unsafe so that steps are not sent to the 
     *    instrument handler to avoid null pointer exception
     */
    void Clear();

    /**
     * Follows the Clear() function once all patterns and song orders
     * are loaded by the project saver loader
     * This marks the sequencer as safe so that steps are once again
     * sent to the instrument handlers
     */
    void Safe();  

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

    /**
     * Called by the Display object to refresh the screen, not called internally 
     * because it interupts audio playback 
     * @param display pointer to display / layer to be written on
     */
    void UpdateDisplay(cLayer *display);

private:
    /**
     * These are the pointers to the vectors dealing with all sequencing
     * basically the main data structures that are used to do the actual
     * sequencing. Not instance variables for Singleton approach and to 
     * make sure that changes propagate throughout the program
     */
    std::vector<InstrumentHandler*>    *handler_;   /**< Pointer to instrument Handler vector */
    std::vector<Pattern*>              *patterns;   /**< Pointer to pattern vector */
    std::vector<int>                   *songOrder;  /**< Pointer to song order vector */
    
    /**
     * Pointers to specific objects withint he above arrays to determine
     * the specific objects that we are using and editing (used in both
     * display, audio playback, and UI editing)
     */
    Pattern    *activePattern;  /**< The active pattern being used */
    Step       *currentStep;    /**< The current step being used */
    Lane       *currentLane;    /**< The current lane being used */
    int         currentPattern; /**< The current pattern within songOrder */

    /**
     * These fields control audio playback specifically the tick rate
     */
    bool    playing_;       /**< Controls whether sequencer is playing or not */
    bool    song_;          /**< Controls whther patterns are triggered sequentially or not */
    float   bpm_;           /**< the bpm retreived from the global CPM */
    Metro   tick;           /**< the tick object that controls the timing of triggers */
    bool    updateStep;     /**< Controls whether it is safe to access steps */
    bool    updatePattern;  /**< Controls whether it is safe to access Patterns */
    bool    updateSidebar;  /**< Controls whether it is safe to access sidebar */
    
    /**
     * Helpful fields that facilitate displaying items 
     */
    char    strbuff[20];    /**< string buffer for use when printing strings to the screen */
    int     laneOffset;     /**< the lane offset position when displaying the lanes */
    bool    stepEdit_;      /**< controls whether steps can be edited or not */
    
    /**
     * Draws a step to the display based on
     * @param display the display being drawn to
     * @param x the x offset
     * @param y the y offset
     * @param step a pointer to the step object being drawn
     */
    void DrawStep( cLayer *display, int x, int y, Step* step);

    /**
     * Draws a square to the display for the song order
     * @param display the display being drawn to
     * @param index the pattern index being written 
     * @param x the x offset
     * @param y the y offset
     * @param fill whether the square is filled or not
     */
    void DrawSquare(cLayer *display, int index, int x, int y, bool fill);

    /**
     * Draws an arrow for the song order UI
     * @param display the display being drawn to
     * @param x the x offset
     * @param y the y offset
     * @param direction the direction the arrow points: 
     *  1 = left, 2 = right, 3 = up, 4 = down
     */
    void DrawArrow(cLayer *display, int x, int y, int direction); 

    /**
     * Turns a note value into a string from a step
     * @param strbuff the string buffer to write to with sprintf
     * @param note the note value to be turned into a string
     */
    void GetNoteString(char* strbuff, int note);

    /**
     * Turns an fx value into a string from a step
     * @param strbuff the string buffer to write to
     * @param fx the fx value to be written
     * @param fxAmount the fxAmount value to be written
     */
    void GetFxString(char* strbuff, int fx, int fxAmount);

    /**
     * Turns an fx value into a string from a step to be displayed
     * @param strbuff the string buffer to write to
     * @param fx the fx value to be written
     */
    void GetFxString(char* strbuff, int fx);

    /**
     * Increments the position of current step to move the sequence
     * forward. Also called by user input
     */
    void NextStep();

    /**
     * Decrements the position of current step to move the sequence
     * backward. Mainly called by user input
     */
    void PreviousStep();

    /**
     * Increments the lane position being displayed to the screen
     * Called by user input
     */
    void NextLane();

    /**
     * Decrements the lane position being displayed to the screen
     * Called by user input
     */
    void PreviousLane();

    /**
     * Increments the pattern position based on the indeces held in
     * the song Order vector
     */
    void NextPattern();

    /**
     * Decrements the pattern position based on the indeces held in
     * the song Order vector
     */
    void PreviousPattern();

    /**
     * Creates a new pattern that is added to the pattern vector
     */
    void NewPattern();

};

#endif