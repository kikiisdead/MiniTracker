#ifndef COMPRESSION_H
#define COMPRESSION_H

#include "effect.h"
#include "../../../../DaisyExamples/DaisySP/DaisySP-LGPL/Source/Dynamics/compressor.h"
#include <list>

#define RMS_AVG 100

class Compression : public Effect {
public:
    Compression(){}
    ~Compression(){}

    void Init(float samplerate, cFont* MainFont) {
        compressor.Init(samplerate);
        attack = 0.005f;
        release = 0.125f;
        ratio = 1.0f;
        threshold = 0.0f;
        makeup = 0.0f;
        compressor.SetAttack(attack);
        compressor.SetRelease(release);
        compressor.SetRatio(ratio);
        compressor.SetThreshold(threshold);
        compressor.SetMakeup(makeup); 
        compressor.AutoMakeup(false);

        param = 0;
        paramNum = 5;
        selected = false;
        effectType = COMPRESSION;
        this->MainFont = MainFont;
    }

    void Process(float& left, float& right) {

        listLeft.push_back(left);
        listRight.push_back(right);

        if (listLeft.size() > RMS_AVG) {
            listLeft.pop_front();
            listRight.pop_front();
        }

        float avg = (left + right) / 2.0f; // get average

        compressor.Process(avg); // turn avg into some amount of gain reduction

        left = compressor.Apply(left); // apply that gain reduction to both sides equally
        right = compressor.Apply(right);
    }

    void Display(cLayer* display, int x, int y) {
        if (selected) display->drawRect(x + FX_BUFFER, y + FX_BUFFER, FX_WIDTH, FX_HEIGHT, 1, ACCENT2);
        else display->drawRect(x + FX_BUFFER, y + FX_BUFFER, FX_WIDTH, FX_HEIGHT, 1, ACCENT1);

        double sumL = 0;
        double sumR = 0;

        for (float left : listLeft) {
            sumL += pow(left, 2.0f);
        }

        for (float right : listRight) {
            sumR += pow(right, 2.0f);
        }

        int rmsL = f2u8(sqrt(sumL)); // scaled 0 to 255
        int rmsR = f2u8(sqrt(sumR));

        sColor color = (selected) ? ACCENT1 : MAIN;

        display->drawFillRect(x + FX_BUFFER + FX_WIDTH - 22, y + FX_BUFFER + FX_HEIGHT - (rmsL / 2) - 8 + 40, 6, (rmsL / 2) - 40, color);
        display->drawFillRect(x + FX_BUFFER + FX_WIDTH - 14, y + FX_BUFFER + FX_HEIGHT - (rmsR / 2) - 8 + 40, 6, (rmsR / 2) - 40, color);



        color = (selected) ? ACCENT2 : ACCENT1;

        sprintf(strbuff, "COMPRESS");
        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + CHAR_HEIGHT + 8, color);



        sprintf(strbuff, "THRESH");
        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 1 * (CHAR_HEIGHT + 4), color);

        if (abs(threshold) < 10) sprintf(strbuff, "%.2fdB", threshold);
        else sprintf(strbuff, "%.1fdB", threshold);
        if (param == 0 && selected) WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 2 * (CHAR_HEIGHT + 4), ACCENT1);
        else                        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 2 * (CHAR_HEIGHT + 4), MAIN);


        sprintf(strbuff, "RATIO");
        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 3 * (CHAR_HEIGHT + 4), color);

        sprintf(strbuff, "%.d:1", (int) ratio);
        if (param == 1 && selected) WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 4 * (CHAR_HEIGHT + 4), ACCENT1);
        else                        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 4 * (CHAR_HEIGHT + 4), MAIN);


        sprintf(strbuff, "ATTACK");
        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 5 * (CHAR_HEIGHT + 4), color);

        if (attack < 0.100) sprintf(strbuff, "%.2fms", attack * 1000.0f);
        else sprintf(strbuff, "%.1fms", attack * 1000.0f);
        if (param == 2 && selected) WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 6 * (CHAR_HEIGHT + 4), ACCENT1);
        else                        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 6 * (CHAR_HEIGHT + 4), MAIN);


        sprintf(strbuff, "RELEASE");
        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 7 * (CHAR_HEIGHT + 4), color);

        if (release < 0.100) sprintf(strbuff, "%.2fms", release * 1000.0f);
        else sprintf(strbuff, "%.1fms", release * 1000.0f);
        if (param == 3 && selected) WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 8 * (CHAR_HEIGHT + 4), ACCENT1);
        else                        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 8 * (CHAR_HEIGHT + 4), MAIN);

        sprintf(strbuff, "MAKEUP");
        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 9 * (CHAR_HEIGHT + 4), color);

        sprintf(strbuff, "%.2fdB", makeup);
        if (param == 4 && selected) WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 10 * (CHAR_HEIGHT + 4), ACCENT1);
        else                        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 34 + 10 * (CHAR_HEIGHT + 4), MAIN);
        
    }

    void Increment() {
        if (param == 0) {
            threshold += 0.5f;
            if (threshold > 0) threshold = 0;
            compressor.SetThreshold(threshold);
        }
        else if (param == 1) {
            ratio += 1.0f;
            compressor.SetRatio(ratio);
        }
        else if (param == 2) {
            if (attack < 0.05) attack += 0.001f;
            else attack += 0.005f;
            compressor.SetAttack(attack);
        }
        else if (param == 3) {
            if (release < 0.05) release += 0.001f;
            else release += 0.005f;
            compressor.SetRelease(release);
        }
        else if (param == 4) {
            makeup += 0.5f;
            compressor.SetMakeup(makeup);
        }
    }

    void Decrement() {
        if (param == 0) {
            threshold -= 0.5f;
            compressor.SetThreshold(threshold);
        }
        else if (param == 1) {
            ratio -= 1.0f;
            if (ratio < 1) ratio = 1;
            compressor.SetRatio(ratio);
        }
        else if (param == 2) {
            if (attack < 0.05) attack -= 0.001f;
            else attack -= 0.005f;
            if (attack < 0.001f) attack = 0.001f;
            compressor.SetAttack(attack);
        }
        else if (param == 3) {
            if (release < 0.05) release -= 0.001f;
            else release -= 0.005f;
            if (release < 0.001f) release = 0.001f;
            compressor.SetRelease(release);
        }
        else if (param == 4) {
            makeup -= 0.5f;
            if (makeup < 0) makeup = 0;
            compressor.SetMakeup(makeup);
        }
    }

    void GetSnapshot(char *buf) {
        
        void* ptr = &buf[0];

        *(static_cast<float*>(ptr)) = threshold;
        ptr = static_cast<float*>(ptr) + 1;

        *(static_cast<float*>(ptr)) = ratio;
        ptr = static_cast<float*>(ptr) + 1;

        *(static_cast<float*>(ptr)) = attack;
        ptr = static_cast<float*>(ptr) + 1;
        
        *(static_cast<float*>(ptr)) = release;
        ptr = static_cast<float*>(ptr) + 1;

        *(static_cast<float*>(ptr)) = makeup;
        ptr = static_cast<float*>(ptr) + 1;
    }

    void Load(char* buf, float samplerate, cFont* MainFont) {
        void* ptr = &buf[0];

        this->samplerate = samplerate;

        this->MainFont = MainFont;

        compressor.Init(samplerate);

        threshold = *(static_cast<float*>(ptr));
        ptr = static_cast<float*>(ptr) + 1;

        ratio = *(static_cast<float*>(ptr));
        ptr = static_cast<float*>(ptr) + 1;

        attack = *(static_cast<float*>(ptr));
        ptr = static_cast<float*>(ptr) + 1;
        
        release = *(static_cast<float*>(ptr));
        ptr = static_cast<float*>(ptr) + 1;

        makeup = *(static_cast<float*>(ptr));
        ptr = static_cast<float*>(ptr) + 1;

        compressor.SetAttack(attack);
        compressor.SetRelease(release);
        compressor.SetRatio(ratio);
        compressor.SetThreshold(threshold);
        compressor.SetMakeup(makeup); 
        compressor.AutoMakeup(false);

        param = 0;
        paramNum = 5;
        selected = false;
        effectType = COMPRESSION;
    }

private:
    Compressor compressor;
    float attack, release, ratio, makeup, threshold;
    std::list<float> listLeft, listRight;
};

#endif