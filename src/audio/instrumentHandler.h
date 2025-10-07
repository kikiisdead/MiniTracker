#pragma once
#ifndef INSTRUMENT_HANDLER_H
#define INSTRUMENT_HANDLER_H

#include "instrument.h"
// #include "../UI/sequencer.h"
#include "../UI/step.h"
#include "fx/noEffect.h"
#include "fx/effect.h"
#include "fx/filter.h"
#include "fx/distortion.h"
#include "fx/compression.h"
#include "fx/autopan.h"
#include "fx/redux.h"
#include "dev/oled_ssd130x.h"

using MyOledDisplay = OledDisplay<SSD130xI2c128x64Driver>;

/*
the purpose of this class is to abstract the function of the instruments. a way for the sequencer to interface with the instruments. 
only one instance of this object which handles any number of instances of the instrument objects
*/

class InstrumentHandler {
private:
    Instrument* activeInst;
    std::vector<Instrument*>* instVector;
    float samplerate;
    bool preview;
    cFont* MainFont;

public:
    
    std::vector<Effect*> effects;

    InstrumentHandler(){}

    /**
     * Initializes the Instrument handler object
     * @param instVector_ pointer to the global instrument vector (shared between handlers)
     * @param samplerate_ samplerate, used to initialize instruments and effects
     * @param MainFont main font to be displayed, used to initilize effects which have a display component
     */
    void Init(std::vector<Instrument*>* instVector_, float samplerate_, cFont* MainFont) {
        instVector = instVector_;
        activeInst = NULL;
        samplerate = samplerate_;
        this->MainFont = MainFont;
        effects.push_back(new NoEffect);
        effects.back()->Init(samplerate, MainFont);
        effects.push_back(new NoEffect);
        effects.back()->Init(samplerate, MainFont);
        effects.push_back(new NoEffect);
        effects.back()->Init(samplerate, MainFont);
    }

    /**
     * Processes audio in instruments and effects 
     * @param left left audio channel passed by reference
     * @param right right audio channel passed by reference
     */
    void Process(float& left, float& right) {
        if (activeInst == nullptr) {
            left = 0.0f;
            right = 0.0f;
            return;
        } 

        activeInst->Process(left, right);
       
        if (!preview) { // only process with effects if playing from sequence
            for (Effect* effect : effects) {
                effect->Process(left, right);
            }
        }

        if (!activeInst->IsPlaying()) {
            activeInst = nullptr;
        }
    }

    // only triggered when changing next note occurs
    void Update(Step* step) {
        preview = false;
        if (step->instrument != -1) {
            if (step->instrument == -2 && activeInst != nullptr) activeInst->Release();
            else if (step->instrument >= 0) {
                if (step->instrument >= (int) instVector->size()) return;
                activeInst = instVector->at(step->instrument);
                activeInst->Trigger(step->note);
            }
        }
    }

    /**
     * Called from the smaple menu to test a note
     * @param inst instrument to be triggered
     * @param note note to be played
     */
    void Preview(Instrument* inst, int note) {
        activeInst = inst;
        activeInst->Trigger(note);
        preview = true;
    }

    /**
     * Adds an effect tot eh effect vector
     * @param type the type of effect to load and initialize
     */
    void AddEffect(Effect::EFFECT_TYPE type) {
        effects.push_back(GetEffect(type));
        effects.back()->Init(samplerate, MainFont);
    }

    /**
     * Overloads previous to allow for passing of a char buffer on project load
     * @param type the effect type to be loaded
     * @param buf char buffer containing saved data from an effect
     */
    void AddEffect(Effect::EFFECT_TYPE type, char* buf) {
        effects.push_back(GetEffect(type));
        effects.back()->Load(buf, samplerate, MainFont);
    }

    /**
     * Removes an effect from the effect list
     * @param index index of effect to be removed
     */
    void RemoveEffect(size_t index) {
        auto it  = effects.begin();
        std::advance(it, index);
        delete (*it); // freeing memory
        effects.erase(it);
    }

    /**
     * Changes effect by removing previous and adding new
     * @param index index of effect to be swapped
     * @param type the type of effect to be loaded
     */
    void ChangeEffect(size_t index, Effect::EFFECT_TYPE type) {
        if (index >= effects.size() - 1) {
            effects.pop_back();
            effects.push_back(GetEffect(type));
            effects.back()->Init(samplerate, MainFont);
        } else {
            auto it  = effects.begin();
            std::advance(it, index);
            effects.erase(it);
            effects.insert(it, GetEffect(type));
            effects.at(index)->Init(samplerate, MainFont);
        }
    }

    /**
     * creates and returns a new effect based on the type 
     * @param type the type of effect to be created
     */
    Effect* GetEffect(Effect::EFFECT_TYPE type) {
        if (type == Effect::NOFX)             return new NoEffect;
        else if (type == Effect::FILTER)      return new Filter;
        else if (type == Effect::DISTORTION)  return new Distortion;
        else if (type == Effect::COMPRESSION) return new Compression;
        else if (type == Effect::AUTOPAN)     return new Autopan;
        else if (type == Effect::REDUX)       return new Redux;
        else                                  return new NoEffect;
    }

   /**
    * clears the effect vector and frees memory
    * called by CLEAR function in main program on project load
    */
    void ClearFX() {
        for (size_t i = 0; i < effects.size(); i++) {
            RemoveEffect(0);
        }
        effects.clear();
    }

};

#endif