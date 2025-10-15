#pragma once
#ifndef step_h
#define step_h

/**
 * @class Step
 * A class that holds all the important functionality of individual steps
 * does not display anything but has information that helps tell the UI
 * how to display that specific step
 * 
 * @author Kiyoko Iuchi-Fung
 * @version 0.1.0
 */

class Step {
public:
    /**
     * Constructor with the index it exists at in the step vector
     * @param index the index of the step within the step vector
     */
    Step(int index) {
        this->note = 48;
        this->instrument = -1;
        this->fx = 0;
        this->fxAmount = 0;
        this->selected = false;
        this->paramEdit = Step::i;
        this->index = index;
    }

    /**
     * A more verbose constructor that is made for project loading where 
     * steps start with pre assigned values from the file and not default 
     * values
     * @param instrument the instrument field of the step
     * @param note the note field of the step
     * @param fx the fx field of the step
     * @param fxAmount the fxAmount field of the step
     * @param index the index of the step within the step vector
     */
    Step(int8_t instrument, uint8_t note, uint8_t fx, uint8_t fxAmount, int index) {
        this->note = note;
        this->instrument = instrument;
        this->fx = fx;
        this->fxAmount = fxAmount;
        this->selected = false;
        this->paramEdit = Step::i;
        this->index = index;
    }

    /**
     * param holds all the possibe parameters that can be edited
     */
    enum param {i, n, f, fa};

    /**
     * The actual important data fields
     */
    int8_t  instrument; /**< Instrument to be triggered */
    uint8_t note;       /**< Note to be played */
    uint8_t fx;         /**< Step fx to be applied */
    uint8_t fxAmount;   /**< How much fx applied */

    /**
     * These fields have to do with UI access and display
     */
    bool selected;      /**< Describes whether the step is selected or not */
    int index;          /**< Position within the step vector */
    param paramEdit;    /**< Param to be edited */

    /**
     * Increments paramEdit within some constraints
     * called from within UI to change which parameter is 
     * edited by Increment() and Decrement() functions
     */
    void NextParam() {
        if ((int) paramEdit < 3) paramEdit = (param) ((int) paramEdit + 1);
        else paramEdit = (param) 3;
    }

    /**
     * Decrements paramEdit within some constraints
     * called from within UI to change which parameter is 
     * edited by Increment() and Decrement() functions
     */
    void PrevParam() {
        if ((int) paramEdit > 0) paramEdit = (param) ((int) paramEdit - 1);
        else paramEdit = (param) 0;
    }

    /**
     * Increments the specific parameter selected by paramEdit
     */
    void Increment() {
        if (paramEdit == i) instrument += 1;
        else if (paramEdit == n) note += 1;
        else if (paramEdit == f) fx += 1;
        else if (paramEdit == fa) fxAmount += 1;
    }

    /**
     * Decrements the specific parameter selected by paramEdit
     */
    void Decrement () {
        if (paramEdit == i) instrument = (instrument <= -2) ? -2 : instrument - 1;
        else if (paramEdit == n) note = (note <= 0) ? 0 : note - 1;
        else if (paramEdit == f) fx = (fx <= 0) ? 0 : fx - 1;
        else if (paramEdit == fa) fxAmount = (fxAmount <= 0) ? 0 : fxAmount - 1;
    }
};



#endif