#pragma once
#ifndef step_h
#define step_h

/**
 * Step can be represented in 16 bytes including one byte telling program that it is a Step
 */

class Step {
public:
    Step(int index) {
        this->note = 48;
        this->instrument = -1;
        this->fx = 0;
        this->fxAmount = 0;
        this->selected = false;
        this->paramEdit = Step::i;
        this->index = index;
    }

    // verbose constructor for loading
    Step(int8_t instrument, uint8_t note, uint8_t fx, uint8_t fxAmount, int index) {
        this->note = note;
        this->instrument = instrument;
        this->fx = fx;
        this->fxAmount = fxAmount;
        this->selected = false;
        this->paramEdit = Step::i;
        this->index = index;
    }

    enum param {i, n, f, fa};

    int8_t instrument;
    uint8_t note;
    uint8_t fx;
    uint8_t fxAmount;
    bool selected;
    int index;
    // bool active;
    param paramEdit;

    void NextParam() {
        if ((int) paramEdit < 3) paramEdit = (param) ((int) paramEdit + 1);
        else paramEdit = (param) 3;
    }

    void PrevParam() {
        if ((int) paramEdit > 0) paramEdit = (param) ((int) paramEdit - 1);
        else paramEdit = (param) 0;
    }

    void Increment() {
        if (paramEdit == i) instrument += 1;
        else if (paramEdit == n) note += 1;
        else if (paramEdit == f) fx += 1;
        else if (paramEdit == fa) fxAmount += 1;
    }

    void Decrement () {
        if (paramEdit == i) instrument = (instrument <= -2) ? -2 : instrument - 1;
        else if (paramEdit == n) note = (note <= 0) ? 0 : note - 1;
        else if (paramEdit == f) fx = (fx <= 0) ? 0 : fx - 1;
        else if (paramEdit == fa) fxAmount = (fxAmount <= 0) ? 0 : fxAmount - 1;
    }
};

#endif