#ifndef INSTRUMENT_HANDLER_H
#define INSTRUMENT_HANDLER_H

#include "instrument.h"
// #include "../UI/sequencer.h"
#include "../UI/step.h"
#include "fx/noEffect.h"
#include "fx/effect.h"
#include "fx/filter.h"
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

public:
    

    std::vector<Effect*> effects;

    InstrumentHandler(){}

    void Init(std::vector<Instrument*>* instVector_, float samplerate_) {
        instVector = instVector_;
        activeInst = NULL;
        samplerate = samplerate_;
        effects.push_back(new NoEffect);
        effects.back()->Init(samplerate);
        effects.push_back(new NoEffect);
        effects.back()->Init(samplerate);
        effects.push_back(new NoEffect);
        effects.back()->Init(samplerate);
    }

    float Process() {
        if (activeInst == NULL) return 0.0f;

        float outL = activeInst->Process();
        float outR = 0.0f;
        if (!preview) { // only process with effects if playing from sequence
            for (Effect* effect : effects) {
                effect->Process(outL, outR);
            }
        }
        return outL;
    }

    // only triggered when changing next note occurs
    void Update(Step* step) {
        preview = false;
        if (step->instrument != -1) {
            if (step->instrument == -2 && activeInst != NULL) activeInst->Release();
            else if (step->instrument >= 0) {
                activeInst = instVector->at(step->instrument);
                activeInst->Trigger(step->note);
            }
        }
    }

    void Preview(Instrument* inst, int note) {
        activeInst = inst;
        activeInst->Trigger(note);
        preview = true;
    }

    void AddEffect(Effect::EFFECT_TYPE type) {
        effects.push_back(GetEffect(type));
        effects.back()->Init(samplerate);
    }

    void RemoveEffect(size_t index) {
        auto it  = effects.begin();
        std::advance(it, index);
        effects.erase(it);
    }

    void ChangeEffect(size_t index, Effect::EFFECT_TYPE type) {
        if (index >= effects.size() - 1) {
            effects.pop_back();
            effects.push_back(GetEffect(type));
            effects.back()->Init(samplerate);
        } else {
            auto it  = effects.begin();
            std::advance(it, index);
            effects.erase(it);
            effects.insert(it, GetEffect(type));
            effects.at(index)->Init(samplerate);
        }
    }

    Effect* GetEffect(Effect::EFFECT_TYPE type) {
        if (type == Effect::NOFX)        return new NoEffect;
        else if (type == Effect::FILTER) return new Filter;
        else                     return new NoEffect;
    }

};

#endif