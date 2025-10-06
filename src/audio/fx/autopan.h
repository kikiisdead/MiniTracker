#ifndef AUTOPAN_H
#define AUTOPAN_H

#include "effect.h"

#define PANSCALE 40.0f * pow(rate, 3.5f)

class Autopan : public Effect {
public:
    Autopan(){};
    ~Autopan(){}

    void Init(float samplerate, cFont* MainFont) {
        // REQUIRED INITIALIZERS 
        param = 0;
        selected = false;
        effectType = AUTOPAN;
        this->MainFont = MainFont;
        paramNum = 4;
        this->samplerate = samplerate;

        // class specific initializers
        amount = 1.0f;
        rate = 0.4f;
        phase = 0.5f;
        shape = Oscillator::WAVE_SIN;

        levL.Init(samplerate);
        levR.Init(samplerate);
        displayL.Init((float) (FX_WIDTH - 16));
        displayR.Init((float) (FX_WIDTH - 16));

        levL.SetAmp(amount);
        levR.SetAmp(amount);
        displayL.SetAmp(amount);
        displayR.SetAmp(amount);

        levL.SetFreq(PANSCALE);
        levR.SetFreq(PANSCALE);
        displayL.SetFreq(PANSCALE * 2.0f);
        displayR.SetFreq(PANSCALE * 2.0f);

        levL.SetWaveform(shape);
        levR.SetWaveform(shape);
        displayL.SetWaveform(shape);
        displayR.SetWaveform(shape);

        levL.Reset();
        levR.Reset();

        levR.PhaseAdd(phase);
    }

    void Process(float& left, float& right) {
        float levelLeft = (levL.Process() + 1.0f) / 2.0f;
        float levelRight = (levR.Process() + 1.0f) / 2.0f;

        left = left * (1.0f - levelLeft);
        right = right * (1.0f - levelRight);
    }

    void Display(cLayer *display, int x, int y){

        sColor color = (selected) ? ACCENT2 : ACCENT1;

        display->drawRect(x + FX_BUFFER, y + FX_BUFFER, FX_WIDTH, FX_HEIGHT, 1, color);

        displayL.Reset();
        displayR.Reset();
        displayR.PhaseAdd(phase);
        float left = 0;
        float right = 0;
        float prevLeft = displayL.Process();
        float prevRight = displayR.Process();

        for (int i = 0; i < FX_WIDTH - 16; i ++) {
            left  = displayL.Process();
            right = displayR.Process();

            int x0 = x + FX_BUFFER + 8 + i;
            int x1 = x0 + 1;

            int ly0 = y + FX_BUFFER + 20 + 8 + prevLeft * 20;
            int ly1 = y + FX_BUFFER + 20 + 8 + left * 20;

            int ry0 = y + FX_BUFFER + 20 + 8+ prevRight * 20;
            int ry1 = y + FX_BUFFER + 20 + 8 + right * 20;

            display->drawLine(x0, ly0, x1, ly1, ACCENT2);
            display->drawLine(x0, ry0, x1, ry1, ACCENT1);

            prevLeft = left;
            prevRight = right;
        }

        sprintf(strbuff, "RATE");
        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 3 * (CHAR_HEIGHT + 4), color);

        sprintf(strbuff, "%.2fhz", PANSCALE);
        if (param == 0 && selected) WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 4 * (CHAR_HEIGHT + 4), ACCENT1);
        else                        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 4 * (CHAR_HEIGHT + 4), MAIN);


        sprintf(strbuff, "AMOUNT");
        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 5 * (CHAR_HEIGHT + 4), color);

        sprintf(strbuff, "%.1f%%", amount * 100.0f);
        if (param == 1 && selected) WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 6 * (CHAR_HEIGHT + 4), ACCENT1);
        else                        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 6 * (CHAR_HEIGHT + 4), MAIN);


        sprintf(strbuff, "PHASE");
        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 7 * (CHAR_HEIGHT + 4), color);

        sprintf(strbuff, "%ddeg", (int)(phase * 360.0f));
        if (param == 2 && selected) WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 8 * (CHAR_HEIGHT + 4), ACCENT1);
        else                        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 8 * (CHAR_HEIGHT + 4), MAIN);

        sprintf(strbuff, "SHAPE");
        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 9 * (CHAR_HEIGHT + 4), color);

        if      (shape == Oscillator::WAVE_SIN)  sprintf(strbuff, "SINE");
        else if (shape == Oscillator::WAVE_TRI)  sprintf(strbuff, "TRIANGLE");
        else if (shape == Oscillator::WAVE_SAW)  sprintf(strbuff, "SAW");
        else if (shape == Oscillator::WAVE_RAMP) sprintf(strbuff, "RAMP");
        else                                     sprintf(strbuff, "SQUARE");
        if (param == 3 && selected) WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 10 * (CHAR_HEIGHT + 4), ACCENT1);
        else                        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 10 * (CHAR_HEIGHT + 4), MAIN);
    }

    void Increment(){
        if (param == 0) {
            rate += 0.01f;
            if (rate > 1.0f) {
                rate = 1.0f;
            }
            levL.SetFreq(PANSCALE);
            levR.SetFreq(PANSCALE);
            displayL.SetFreq(PANSCALE * 2.0f);
            displayR.SetFreq(PANSCALE * 2.0f);
        } else if (param == 1) {
            amount += 0.01f;
            if (amount > 1.0f) {
                amount = 1.0f;
            }
            levL.SetAmp(amount);
            levR.SetAmp(amount);
            displayL.SetAmp(amount);
            displayR.SetAmp(amount);
        } else if (param == 2) {
            phase += 5.0f / 360.0f;
            if (phase > 1.0f) {
                phase = 1.0f;
            } else {
                levR.PhaseAdd(5.0f / 360.0f);
            }
        } else if (param == 3) {
            shape += 1;
            if (shape > Oscillator::WAVE_SQUARE) {
                shape = Oscillator::WAVE_SQUARE;
            }
            levL.SetWaveform(shape);
            levR.SetWaveform(shape);
            displayL.SetWaveform(shape);
            displayR.SetWaveform(shape);
        }
    }

    void Decrement(){
        if (param == 0) {
            rate -= 0.01f;
            if (rate < 0.0f) {
                rate = 0.0f;
            }
            levL.SetFreq(PANSCALE);
            levR.SetFreq(PANSCALE);
            displayL.SetFreq(PANSCALE * 2.0f);
            displayR.SetFreq(PANSCALE * 2.0f);
        } else if (param == 1) {
            amount -= 0.01f;
            if (amount < 0.0f) {
                amount = 0.0f;
            }
            levL.SetAmp(amount);
            levR.SetAmp(amount);
            displayL.SetAmp(amount);
            displayR.SetAmp(amount);
        } else if (param == 2) {
            phase -= 5.0f / 360.0f;
            if (phase < 0.0f) {
                phase = 0.0f;
            } else {
                levR.PhaseAdd(-5.0f / 360.0f);
            }
        } else if (param == 3) {
        shape -= 1;
            if (shape < Oscillator::WAVE_SIN) {
                shape = Oscillator::WAVE_SIN;
            }
            levL.SetWaveform(shape);
            levR.SetWaveform(shape);
            displayL.SetWaveform(shape);
            displayR.SetWaveform(shape);
        }
    }

    void GetSnapshot(char *buf) {
        
        void* ptr = &buf[0];

        *(static_cast<float*>(ptr)) = rate;
        ptr = static_cast<float*>(ptr) + 1;

        *(static_cast<float*>(ptr)) = amount;
        ptr = static_cast<float*>(ptr) + 1;

        *(static_cast<float*>(ptr)) = phase;
        ptr = static_cast<float*>(ptr) + 1;
        
        *(static_cast<int*>(ptr)) = shape;
        ptr = static_cast<int*>(ptr) + 1;

    }

    void Load(char* buf, float samplerate, cFont* MainFont) {
        void* ptr = &buf[0];

        this->samplerate = samplerate;

        this->MainFont = MainFont;

        rate = *(static_cast<float*>(ptr));
        ptr = static_cast<float*>(ptr) + 1;

        amount = *(static_cast<float*>(ptr));
        ptr = static_cast<float*>(ptr) + 1;

        phase = *(static_cast<float*>(ptr));
        ptr = static_cast<float*>(ptr) + 1;
        
        shape = *(static_cast<int*>(ptr));
        ptr = static_cast<int*>(ptr) + 1;

        param = 0;
        selected = false;
        effectType = AUTOPAN;
        paramNum = 4;

        levL.Init(samplerate);
        levR.Init(samplerate);
        displayL.Init((float) (FX_WIDTH - 16));
        displayR.Init((float) (FX_WIDTH - 16));

        levL.SetAmp(amount);
        levR.SetAmp(amount);
        displayL.SetAmp(amount);
        displayR.SetAmp(amount);

        levL.SetFreq(PANSCALE);
        levR.SetFreq(PANSCALE);
        displayL.SetFreq(PANSCALE * 2.0f);
        displayR.SetFreq(PANSCALE * 2.0f);

        levL.SetWaveform(shape);
        levR.SetWaveform(shape);
        displayL.SetWaveform(shape);
        displayR.SetWaveform(shape);

        levL.Reset();
        levR.Reset();

        levR.PhaseAdd(phase);
    }

private:
    Oscillator levL;
    Oscillator levR;

    Oscillator displayL;
    Oscillator displayR;

    float amount, rate, phase;
    int shape;
};

#endif