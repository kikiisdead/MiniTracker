#ifndef FILTER_H
#define FILTER_H

#include "effect.h"

#define FILTERSCALE 19980.0f * pow(freq, 2.5f) + 20.0f

class Filter : public Effect {
public:

    Filter(){}
    ~Filter(){}

    enum FILTERMODE { LOWPASS, HIGHPASS, BANDPASS, NOTCH };

    void Init(float samplerate, cFont* MainFont) {
        filterL.Init(samplerate);
        filterR.Init(samplerate);

        freq = 0.5f;
        res = 0.2f;
        drive = 0.85f;

        filterL.SetFreq(FILTERSCALE);
        filterR.SetFreq(FILTERSCALE);
        filterL.SetRes(res);
        filterR.SetRes(res);
        filterL.SetDrive(drive);
        filterR.SetDrive(drive);
        filterMode = LOWPASS;
        param = 0;
        paramNum = 4;
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

        sColor color = (selected) ? ACCENT2 : ACCENT1;

        if (filterMode == LOWPASS) {
            display->drawArc(x + FX_BUFFER + FX_WIDTH / 2, y + FX_BUFFER + 28, 20, 0, 90, color);
            display->drawLine(x + FX_BUFFER + 8, y + FX_BUFFER + 8, x + FX_BUFFER + FX_WIDTH / 2, y + FX_BUFFER + 8, color);
        } else if (filterMode == HIGHPASS) {
            display->drawArc(x + FX_BUFFER + FX_WIDTH / 2, y + FX_BUFFER + 28, 20, 270, 360, color);
            display->drawLine(x + FX_BUFFER + FX_WIDTH / 2, y + FX_BUFFER + 8, x + FX_BUFFER + FX_WIDTH - 8, y + FX_BUFFER + 8, color);
        } else if (filterMode == BANDPASS) {
            display->drawArc(x + FX_BUFFER + FX_WIDTH / 2, y + FX_BUFFER + 28, 20, 270, 90, color);
        } else if (filterMode == NOTCH) {
            display->drawArc(x + FX_BUFFER + (FX_WIDTH / 2) + 20, y + FX_BUFFER + 28, 20, 270, 360, color);
            display->drawLine(x + FX_BUFFER + (FX_WIDTH / 2) + 20, y + FX_BUFFER + 8, x + FX_BUFFER + FX_WIDTH - 8, y + FX_BUFFER + 8, color);
            display->drawArc(x + FX_BUFFER + (FX_WIDTH / 2) - 20, y + FX_BUFFER + 28, 20, 0, 90, color);
            display->drawLine(x + FX_BUFFER + 8, y + FX_BUFFER + 8, x + FX_BUFFER + (FX_WIDTH / 2) - 20, y + FX_BUFFER + 8, color);
        }



        sprintf(strbuff, "TYPE");
        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 1 * (CHAR_HEIGHT + 4), color);

        if      (filterMode == LOWPASS)  sprintf(strbuff, "LOWPASS");
        else if (filterMode == HIGHPASS) sprintf(strbuff, "HIGHPASS");
        else if (filterMode == BANDPASS) sprintf(strbuff, "BANDPASS");
        else if (filterMode == NOTCH)    sprintf(strbuff, "NOTCH");
        if (param == 0 && selected) WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 2 * (CHAR_HEIGHT + 4), ACCENT1);
        else                        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 2 * (CHAR_HEIGHT + 4), MAIN);


        sprintf(strbuff, "FREQ");
        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 3 * (CHAR_HEIGHT + 4), color);

        if (FILTERSCALE < 100.0f) sprintf(strbuff, "%.2fhz", FILTERSCALE);
        else if (FILTERSCALE < 1000.0f) sprintf(strbuff, "%.1fhz", FILTERSCALE);
        else sprintf(strbuff, "%.2fkhz", ((FILTERSCALE) / 1000.0f));
        if (param == 1 && selected) WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 4 * (CHAR_HEIGHT + 4), ACCENT1);
        else                        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 4 * (CHAR_HEIGHT + 4), MAIN);


        sprintf(strbuff, "RESON");
        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 5 * (CHAR_HEIGHT + 4), color);

        sprintf(strbuff, "%.2f", res);
        if (param == 2 && selected) WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 6 * (CHAR_HEIGHT + 4), ACCENT1);
        else                        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 6 * (CHAR_HEIGHT + 4), MAIN);


        sprintf(strbuff, "DRIVE");
        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 7 * (CHAR_HEIGHT + 4), color);

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
            freq += 0.01;
            if (freq > 1) {
                freq = 1;
            }
            filterL.SetFreq(FILTERSCALE);
            filterR.SetFreq(FILTERSCALE);
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
            freq -= 0.01;
            if (freq < 0) {
                freq = 0;
            }
            filterL.SetFreq(FILTERSCALE);
            filterR.SetFreq(FILTERSCALE);
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

    void GetSnapshot(char *buf) {
        
        void* ptr = &buf[0];

        *(static_cast<uint32_t*>(ptr)) = (uint32_t) filterMode;
        ptr = static_cast<uint32_t*>(ptr) + 1;

        *(static_cast<float*>(ptr)) = freq;
        ptr = static_cast<float*>(ptr) + 1;

        *(static_cast<float*>(ptr)) = res;
        ptr = static_cast<float*>(ptr) + 1;
        
        *(static_cast<float*>(ptr)) = drive;
        ptr = static_cast<float*>(ptr) + 1;

    }

    void Load(char* buf, float samplerate, cFont* MainFont) {
        void* ptr = &buf[0];

        this->samplerate = samplerate;

        this->MainFont = MainFont;

        filterL.Init(samplerate);
        filterR.Init(samplerate);

        filterMode = (FILTERMODE) *(static_cast<uint32_t*>(ptr));
        ptr = static_cast<uint32_t*>(ptr) + 1;

        freq = *(static_cast<float*>(ptr));
        ptr = static_cast<float*>(ptr) + 1;

        res = *(static_cast<float*>(ptr));
        ptr = static_cast<float*>(ptr) + 1;
        
        drive = *(static_cast<float*>(ptr));
        ptr = static_cast<float*>(ptr) + 1;


        filterL.SetFreq(FILTERSCALE);
        filterR.SetFreq(FILTERSCALE);
        filterL.SetRes(res);
        filterR.SetRes(res);
        filterL.SetDrive(drive);
        filterR.SetDrive(drive);
        param = 0;
        paramNum = 4;
        selected = false;
        effectType = FILTER;
        this->MainFont = MainFont;
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