#ifndef step_h
#define step_h



class Step {
public:
    enum param {i, n, f, fa};

    int instrument;
    int note;
    int fx;
    int fxAmount;
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