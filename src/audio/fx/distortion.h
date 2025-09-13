#ifndef DISTORTION_H
#define DISTORTION_H

#include "effect.h"

class Distortion : public Effect {
public:

    void Init(float samplerate, cFont* MainFont) {
        overdrive.Init();

        drive = 0.2f;

        overdrive.SetDrive(drive);

        param = 0;
        selected = false;
        effectType = DISTORTION;
        this->MainFont = MainFont;
    }
    void Process(float& left, float& right) {
        left = overdrive.Process(left);
        right = overdrive.Process(right);
    }

    void Display(cLayer *display, int x, int y) {
        
    }

    void Increment() {
        drive += 0.01f;
        if (drive >= 1.0f) {
            drive = 1.0f;
        }
    }
    void Decrement() {
        drive -= 0.01f;
        if (drive <= 0.0f) {
            drive = 0.0f;
        }
    }

    void NextParam() {}
    void PrevParam() {}

private:
    Overdrive overdrive;

    float drive;

};

#endif