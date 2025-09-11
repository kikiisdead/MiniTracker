#ifndef FILTER_H
#define FILTER_H

#include "effect.h"

class Filter : public Effect {
public:

    Filter(){}
    ~Filter(){}

    enum FILTERMODE { LOWPASS, HIGHPASS, BANDPASS, NOTCH };

    void Init(float samplerate) {
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

    void Display(MyOledDisplay& display, int x, int y){
        // drawing box
        display.DrawRect(x, y, x + FX_WIDTH, y + FX_HEIGHT, true, selected);
        double x_off = ((log10f(freq) - 1.3f)/3.0f);

        if (filterMode == LOWPASS) {
            display.DrawArc(x + (x_off * (FX_WIDTH - 10)), y + 11, 8, 90, -90, !selected);
            display.DrawLine(x + 2, y + 3, x + (x_off * (FX_WIDTH - 10)), y + 3, !selected);
        } else if (filterMode == HIGHPASS) {
            display.DrawArc(x + (x_off * (FX_WIDTH - 10) + 10), y + 11, 8, 90, 90, !selected);
            display.DrawLine(x + FX_WIDTH - 2, y + 3, x + (x_off * (FX_WIDTH - 10) + 10), y + 3, !selected);
        } else if (filterMode == BANDPASS) {
            display.DrawArc(x + (x_off * (FX_WIDTH - 20) + 10), y + 11, 8, 0, 180, !selected);
        } else if (filterMode == NOTCH) {
            double d = (x_off * ((double) FX_WIDTH)) - 1;
            
            if (d - 8 < 0) {
                double theta = acos((8.0f - d)/8.0f);
                int degree = theta * 57.3f;
                display.DrawArc(x + (x_off * (FX_WIDTH)) - 8, y + 11, 8, degree, -degree, !selected);
            }
            else {
                display.DrawArc(x + (x_off * (FX_WIDTH)) - 8, y + 11, 8, 90, -90, !selected);
                display.DrawLine(x + 2, y + 3, x + (x_off * (FX_WIDTH)) - 8, y + 3, !selected);
            }

            d = ((double) FX_WIDTH) - (x_off * ((double) FX_WIDTH));
            if (d <= 8) {
                double theta = acos((8.0f - d)/8.0f);
                int degree = theta * 57.3f;
                display.DrawArc(x + (x_off * (FX_WIDTH)) + 8, y + 11, 8, 180 - degree, degree, !selected);
            } else {
                display.DrawArc(x + (x_off * (FX_WIDTH)) + 8, y + 11, 8, 90, 90, !selected);
                display.DrawLine(x + FX_WIDTH - 2, y + 3, x + (x_off * (FX_WIDTH)) + 9, y + 3, !selected);
            }
        }

        display.SetCursor(x + 3, y + 12);
        sprintf(strbuff, "FREQ");
        display.WriteString(strbuff, Font_4x6, !selected);

        display.SetCursor(x + 3, y + 18);
        if (freq < 1000.0f) sprintf(strbuff, "%.2fHZ", freq);
        else sprintf(strbuff, "%.2fKHZ", freq / 1000.0f);
        display.WriteString(strbuff, Font_4x6, !selected);

        display.SetCursor(x + 3, y + 26);
        sprintf(strbuff, "RESO");
        display.WriteString(strbuff, Font_4x6, !selected);

        display.SetCursor(x + 3, y + 32);
        sprintf(strbuff, "%.2f", res);
        display.WriteString(strbuff, Font_4x6, !selected);

        display.SetCursor(x + 3, y + 40);
        sprintf(strbuff, "DRIVE");
        display.WriteString(strbuff, Font_4x6, !selected);

        display.SetCursor(x + 3, y + 46);
        sprintf(strbuff, "%.2f", drive);
        display.WriteString(strbuff, Font_4x6, !selected);

        if (param != 0 && selected) DrawArrow(display, x + 24, y + (14 * param), 0, !selected);

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