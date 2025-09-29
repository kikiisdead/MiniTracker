#ifndef DISTORTION_H
#define DISTORTION_H

#include "effect.h"

class Distortion : public Effect {
public:
    Distortion(){}
    ~Distortion(){}
    
    void Init(float samplerate, cFont* MainFont) {
        overdrive.Init();

        drive = 0.2f;

        overdrive.SetDrive(drive);

        param = 0;
        paramNum = 1;
        selected = false;
        effectType = DISTORTION;
        this->MainFont = MainFont;
    }

    void Process(float& left, float& right) {
        left = overdrive.Process(left);
        right = overdrive.Process(right);
    }

    void Display(cLayer *display, int x, int y) {
        if (selected) display->drawRect(x + FX_BUFFER, y + FX_BUFFER, FX_WIDTH, FX_HEIGHT, 1, ACCENT2);
        else display->drawRect(x + FX_BUFFER, y + FX_BUFFER, FX_WIDTH, FX_HEIGHT, 1, ACCENT1);

        int centerX = x + FX_BUFFER + FX_WIDTH / 2;
        int centerY = y + FX_BUFFER + FX_HEIGHT / 2;

        sColor color = (selected) ? ACCENT2 : ACCENT1;

        float deg = (270.0f * drive);
        float degOffset = (90.0f + 45.0f);

        display->drawCircle(centerX, centerY, 30, color);

        float arcX = cos((deg + degOffset) * (PI_F / 180.0f));
        float arcY = sin((deg + degOffset) * (PI_F / 180.0f));

        if ( drive < 0.80 ) display->drawLine(centerX + arcX * 27, centerY + arcY * 27, centerX + arcX * 15, centerY + arcY * 15, color);
        else display->drawLine(centerX + arcX * 15, centerY + arcY * 15, centerX + arcX * 27.0f, centerY + arcY * 27.0f, color);

        sprintf(strbuff, "OVER");
        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + CHAR_HEIGHT + 8, color);
        sprintf(strbuff, "DRIVE");
        WriteString(display, strbuff, x + FX_BUFFER + 8, y + FX_BUFFER + 2 * (CHAR_HEIGHT + 4) + 4, color);

        for (int i = 0; i < 11; i ++) {
            deg = 270.0f * ((float) i / 10.0f);
            degOffset = (90.0f + 45.0f);
            arcX = cos((deg + degOffset) * (PI_F / 180.0f));
            arcY = sin((deg + degOffset) * (PI_F / 180.0f));

            sprintf(strbuff, "%d", i);
            if ((int)(drive * 10.0f) == i) {
                WriteString(display, strbuff, centerX + arcX * 40 - (CHAR_WIDTH / 2), centerY + arcY * 40 + (CHAR_HEIGHT / 2), ACCENT1);
            } else {
                WriteString(display, strbuff, centerX + arcX * 40 - (CHAR_WIDTH / 2), centerY + arcY * 40 + (CHAR_HEIGHT / 2), MAIN);
            }
        }
    }

    void Increment() {
        drive += 0.01f;
        if (drive >= 1.0f) {
            drive = 1.0f;
        }
        overdrive.SetDrive(drive);
    }
    void Decrement() {
        drive -= 0.01f;
        if (drive <= 0.0f) {
            drive = 0.0f;
        }
        overdrive.SetDrive(drive);
    }

    void GetSnapshot(char *buf) {
        
        void* ptr = &buf[0];

        // *(static_cast<float*>(ptr)) = samplerate;
        // ptr = static_cast<char*>(ptr) + sizeof(samplerate);

        // *((cFont**) ptr) = MainFont;
        // ptr = static_cast<char*>(ptr) + sizeof(MainFont);

        *(static_cast<float*>(ptr)) = drive;
        ptr = static_cast<float*>(ptr) + 1;

    }

    void Load(char* buf, float samplerate, cFont* MainFont) {

        void* ptr = &buf[0];

        this->samplerate = samplerate;
        this->MainFont = MainFont;

        drive = *(static_cast<float*>(ptr));
        ptr = static_cast<float*>(ptr) + 1;

        overdrive.Init();

        param = 0;
        paramNum = 1;
        selected = false;
        effectType = DISTORTION;
        this->MainFont = MainFont;
    }

private:
    Overdrive overdrive;

    float drive;

};

#endif