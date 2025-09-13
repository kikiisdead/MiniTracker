#ifndef FILTER_H
#define FILTER_H

#include "effect.h"

class Filter : public Effect {
public:

    Filter(){}
    ~Filter(){}

    enum FILTERMODE { LOWPASS, HIGHPASS, BANDPASS, NOTCH };

    void Init(float samplerate, cFont* MainFont) {
        filterL.Init(samplerate);
        filterR.Init(samplerate);

        freq = 800.0f;
        res = 0.2f;
        drive = 0.85f;

        filterL.SetFreq(freq);
        filterR.SetFreq(freq);
        filterL.SetRes(res);
        filterR.SetRes(res);
        filterL.SetDrive(drive);
        filterR.SetDrive(drive);
        filterMode = LOWPASS;
        param = 0;
        selected = false;
        effectType = FILTER;
        this->MainFont = MainFont;
    }

    void Process(float& left, float& right) {
        filterL.Process(left);
        filterR.Process(right);

        if (filterMode == LOWPASS) {
            left = filterL.Low();
            right = filterR.Low();
        } else if (filterMode == HIGHPASS) {
            left = filterL.High();
            right = filterR.High();
        } else if (filterMode == BANDPASS) {
            left = filterL.Band();
            right = filterR.Band();
        } else if (filterMode == NOTCH) {
            left = filterL.Notch();
            right = filterR.Notch();
        }
    }

    void Display(cLayer* display, int x, int y){

        if (selected) display->drawRect(x + FX_BUFFER, y + FX_BUFFER, FX_WIDTH, FX_HEIGHT, 1, ACCENT2);
        else display->drawRect(x + FX_BUFFER, y + FX_BUFFER, FX_WIDTH, FX_HEIGHT, 1, ACCENT1);

        if (filterMode == LOWPASS) {
            display->drawArc(x + FX_BUFFER + FX_WIDTH / 2, y + FX_BUFFER + 28, 20, 0, 90, ACCENT1);
            display->drawLine(x + FX_BUFFER + 8, y + FX_BUFFER + 8, x + FX_BUFFER + FX_WIDTH / 2, y + FX_BUFFER + 8, ACCENT1);
        } else if (filterMode == HIGHPASS) {
            display->drawArc(x + FX_BUFFER + FX_WIDTH / 2, y + FX_BUFFER + 28, 20, 270, 360, ACCENT1);
            display->drawLine(x + FX_BUFFER + FX_WIDTH / 2, y + FX_BUFFER + 8, x + FX_BUFFER + FX_WIDTH - 8, y + FX_BUFFER + 8, ACCENT1);
        } else if (filterMode == BANDPASS) {
            display->drawArc(x + FX_BUFFER + FX_WIDTH / 2, y + FX_BUFFER + 28, 20, 270, 90, ACCENT1);
        } else if (filterMode == NOTCH) {
            display->drawArc(x + FX_BUFFER + (FX_WIDTH / 2) + 20, y + FX_BUFFER + 28, 20, 270, 360, ACCENT1);
            display->drawLine(x + FX_BUFFER + (FX_WIDTH / 2) + 20, y + FX_BUFFER + 8, x + FX_BUFFER + FX_WIDTH - 8, y + FX_BUFFER + 8, ACCENT1);
            display->drawArc(x + FX_BUFFER + (FX_WIDTH / 2) - 20, y + FX_BUFFER + 28, 20, 0, 90, ACCENT1);
            display->drawLine(x + FX_BUFFER + 8, y + FX_BUFFER + 8, x + FX_BUFFER + (FX_WIDTH / 2) - 20, y + FX_BUFFER + 8, ACCENT1);
        }



        sprintf(strbuff, "TYPE");
        if (selected) WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 1 * (CHAR_HEIGHT + 4), ACCENT2);
        else          WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 1 * (CHAR_HEIGHT + 4), ACCENT1);

        if      (filterMode == LOWPASS)  sprintf(strbuff, "LOWPASS");
        else if (filterMode == HIGHPASS) sprintf(strbuff, "HIGHPASS");
        else if (filterMode == BANDPASS) sprintf(strbuff, "BANDPASS");
        else if (filterMode == NOTCH)    sprintf(strbuff, "NOTCH");
        if (param == 0 && selected) WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 2 * (CHAR_HEIGHT + 4), ACCENT1);
        else                        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 2 * (CHAR_HEIGHT + 4), MAIN);


        sprintf(strbuff, "FREQ");
        if (selected) WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 3 * (CHAR_HEIGHT + 4), ACCENT2);
        else          WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 3 * (CHAR_HEIGHT + 4), ACCENT1);

        if (freq < 100.0f) sprintf(strbuff, "%.2fhz", freq);
        else if (freq < 1000.0f) sprintf(strbuff, "%.1fhz", freq);
        else sprintf(strbuff, "%.2fkhz", freq / 1000.0f);
        if (param == 1 && selected) WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 4 * (CHAR_HEIGHT + 4), ACCENT1);
        else                        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 4 * (CHAR_HEIGHT + 4), MAIN);


        sprintf(strbuff, "RESON");
        if (selected) WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 5 * (CHAR_HEIGHT + 4), ACCENT2);
        else          WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 5 * (CHAR_HEIGHT + 4), ACCENT1);

        sprintf(strbuff, "%.2f", res);
        if (param == 2 && selected) WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 6 * (CHAR_HEIGHT + 4), ACCENT1);
        else                        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 6 * (CHAR_HEIGHT + 4), MAIN);


        sprintf(strbuff, "DRIVE");
        if (selected) WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 7 * (CHAR_HEIGHT + 4), ACCENT2);
        else          WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 7 * (CHAR_HEIGHT + 4), ACCENT1);

        sprintf(strbuff, "%.2f", drive);
        if (param == 3 && selected) WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 8 * (CHAR_HEIGHT + 4), ACCENT1);
        else                        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 8 * (CHAR_HEIGHT + 4), MAIN);

    }

    void Increment() {
        if (param == 0) {
            int temp = (int) filterMode + 1;
            if (temp > 3) filterMode = (FILTERMODE) 0;
            else filterMode = (FILTERMODE) temp;
            param = 0;
        } else if (param == 1) {
            freq += pow(freq, 0.8f) / 10.0f;
            filterL.SetFreq(freq);
            filterR.SetFreq(freq);
        } else if (param == 2) {
            res += 0.01;
            filterL.SetRes(res);
            filterR.SetRes(res);
        } else if (param == 3) {
            drive += 0.01;
            filterL.SetDrive(drive);
            filterR.SetDrive(drive);
        }
    }

    void Decrement() {
        if (param == 0) {
            int temp = (int) filterMode - 1;
            if (temp < 0) filterMode = (FILTERMODE) 3;
            else filterMode = (FILTERMODE) temp;
        } else if (param == 1) {
            freq -= pow(freq, 0.8f) / 10.0f;
            filterL.SetFreq(freq);
            filterR.SetFreq(freq);
        } else if (param == 2) {
            res -= 0.01;
            filterL.SetRes(res);
            filterR.SetRes(res);
        } else if (param == 3) {
            drive -= 0.01;
            filterL.SetDrive(drive);
            filterR.SetDrive(drive);
        }
    }

    void NextParam() {
        param += 1;
        if (param > 3) {
            param = 3;
        }
    }

    void PrevParam() {
        param -= 1;
        if (param < 0) {
            param = 0;
        }
    }

private:
    Svf filterL;
    Svf filterR;

    FILTERMODE filterMode;
    float freq;
    float res;
    float drive;


};

#endif