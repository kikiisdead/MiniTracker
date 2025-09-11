#ifndef POP_UP_MENU_H
#define POP_UP_MENU_H

#include "buttonInterface.h"

/**
 * Pop Up Menu to be created on the stack and cleared as function clears
 * Auto scope as opposed to persistent
 */
class PopUpMenu : public buttonInterface {

public:

    PopUpMenu(int width, int height, int x, int y, char* name, char** items) {
        this->width = width;
        this->height = height;
        this->x = x;
        this->y = y;
        this->name = name;
        this->items = items;
    }

    void AButton() {};
    void BButton() {};
    void UpButton() {};
    void DownButton() {};
    void LeftButton() {};
    void RightButton() {};
    void PlayButton() {};

    void AltAButton() {};
    void AltBButton() {};
    void AltUpButton() {};
    void AltDownButton() {};
    void AltLeftButton() {};
    void AltRightButton() {};
    void AltPlayButton() {};

    void UpdateDisplay(MyOledDisplay &display) {

    }

private:
    int width;
    int height;
    int x;
    int y;
    char* name;
    char** items;

};

#endif