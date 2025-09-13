#ifndef COMPRESSION_H
#define COMPRESSION_H

#include "effect.h"
#include "../../../../DaisyExamples/DaisySP/DaisySP-LGPL/Source/Dynamics/compressor.h"
#include <list>

#define RMS_AVG 5

class Compression : public Effect {
public:
    Compression(){}
    ~Compression(){}

    void Init(float samplerate, cFont* MainFont) {
        compressor.Init(samplerate);
        this->MainFont = MainFont;
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

        display->drawFillRect(x + FX_BUFFER + 8, y + FX_BUFFER + 8, 6, rmsL / 2.0f, MAIN);
        display->drawFillRect(x + FX_BUFFER + 16, y + FX_BUFFER + 8, 6, rmsR / 2.0f, MAIN);
        
    }

    void Increment() {

    }

    void Decrement() {

    }

    void NextParam() {

    }

    void PrevParam(){

    }

private:
    Compressor compressor;
    float attack, release, ratio, makeup, threshold;
    std::list<float> listLeft, listRight;
};

#endif