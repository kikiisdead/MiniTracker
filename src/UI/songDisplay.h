#pragma once

#ifndef SONG_DISPLAY_H
#define SONG_DISPLAY_H

#include "buttonInterface.h"
#include "sequencer.h"
#include "../sd/dirLoader.h"
#include "../sd/projSaverLoader.h"

/**
 * Helper class for the keyboard letters
 */
class Letter {
public:
    Letter(const char* dataLow, const char* dataHigh, int size) {
        this->dataLow = dataLow;
        this->dataHigh = dataHigh;
        selected = false;
        capital = false;
        this->size = size;
    }

    const char* GetVal() {
        if (capital) return dataHigh;
        else return dataLow;
    }

    void Display(cLayer* display, cFont* MainFont, int x, int y) {

        int width = 24 * size;
        if (selected) display->drawFillRect(x, y, width + 1, 25, ACCENT2);
        display->drawRect(x, y, width + 1, 25, 1, MAIN);

        int len = strlen(dataLow);
        int xOffset = (width / 2) - ((len * CHAR_WIDTH) / 2);
        int yOffset = CHAR_HEIGHT + ((25 / 2) - (CHAR_HEIGHT / 2));

        display->setCursor(x + xOffset, y + yOffset);
        display->setFont(MainFont);
        display->setTextFrontColor(MAIN);
        if (capital) display->drawText(dataHigh);
        else display->drawText(dataLow);
    }

    const char* dataLow;
    const char* dataHigh;
    bool selected;
    bool capital;
    int size;
};

class SongDisplay : public buttonInterface {
public:
    SongDisplay(){}
    ~SongDisplay(){}

    enum PARAM { SAVE, SAVEAS, LOAD, VOL, BPM };

    /**
     * Initialize the Song Display
     */
    void Init(Sequencer *sequencer, Node<File>* node, ProjSaverLoader* projSaverLoader, cFont* MainFont, void* (*search_buffer_allocate)(size_t size));

    /**
     * Process to apply gain
     */
    void Process(float& left, float& right);

    /**
     * INHERITED FROM BUTTON INTERFACE
     */
    void AButton();
    void BButton();
    void UpButton();
    void DownButton();
    void LeftButton();
    void RightButton();
    void PlayButton(){};

    void AltAButton(){};
    void AltBButton(){};
    void AltUpButton(){};
    void AltDownButton(){};
    void AltLeftButton(){};
    void AltRightButton(){};
    void AltPlayButton(){};

    void UpdateDisplay(cLayer* display);

private:
    Sequencer *sequencer;
    float vol;
    char strbuff[256];
    cFont *MainFont;
    PARAM param;
    ProjSaverLoader* projSaverLoader;
    std::string projName;

    Node<File>* currentNode;
    Node<File>* rootNode;
    Letter* keyboard[4][9];
    std::string tempName;

    void* (*search_buffer_allocate)(size_t size);

    int keyV, keyH;

    bool load, saveas;

    int row, col, lev, scrRow;

    void WriteString(cLayer* display, char* strbuff, int x, int y, DadGFX::sColor color);

    void NextParam();
    void PrevParam();
    void Increment();
    void Decrement();  
    
};


#endif