#ifndef instrumentHandler_h
#define instrumentHandler_h

#include "instrument.h"
// #include "../UI/sequencer.h"
#include "../UI/step.h"

/*
the purpose of this class is to abstract the function of the instruments. a way for the sequencer to interface with the instruments. 
only one instance of this object which handles any number of instances of the instrument objects
*/

class InstrumentHandler {
private:
    Instrument* activeInst;
    std::vector<Instrument*>* instVector;

public:
    InstrumentHandler(){}

    void Init(std::vector<Instrument*>* instVector_) {
        instVector = instVector_;
        activeInst = NULL;
    }

    float Process() {
        if (activeInst == NULL) return 0.0f;
        else return activeInst->Process();
    }

    // only triggered when changing next note occurs
    void Update(Step* step) {
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
    }
};

#endif