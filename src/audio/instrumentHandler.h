#ifndef instrumentHandler_h
#define instrumentHandler_h

#include "instrument.h"
#include "../UI/sequencer.h"
#include "../UI/step.h"

/*
the purpose of this class is to abstract the function of the instruments. a way for the sequencer to interface with the instruments. 
only one instance of this object which handles any number of instances of the instrument objects
*/

class InstrumentHandler {
private:
    Instrument* activeInst;
    std::vector<Instrument*>* instVector;
    bool active;

public:
    InstrumentHandler(){}

    void Init(std::vector<Instrument*>* instVector_) {
        instVector = instVector_;
        active = false;
    }

    float Process() {
        if (!active) return 0.0f;
        return activeInst->Process();
    }

    // only triggered when changing next note occurs
    void Update(Step* step) {
        if (step->instrument >= 0) {
            activeInst = instVector->at(step->instrument);
            active = true;
            activeInst->Trigger(step->note);
            // if (step->note < 0) activeInst->Release();
            // else {
            //     activeInst = instVector->at(step->instrument);
            //     active = true;
            //     activeInst->Trigger(step->note);
            // }
        }
    }
};

#endif