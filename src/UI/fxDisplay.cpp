#include "fxDisplay.h"

void FXDisplay::UpdateDisplay(cLayer* display) {
    
    CURRENT_EFFECT->selected = true;

    display->eraseLayer();
    display->drawFillRect(0, 0, 320, 240, BACKGROUND);

    for (size_t i = 0; i < handler->at(currentLane)->effects.size(); i++) {
        handler->at(currentLane)->effects.at(i)->Display(display, i * (FX_WIDTH + FX_BUFFER), 20);
    }


    display->drawFillRect(0, 0, 320, CHAR_HEIGHT + 7, BACKGROUND);
    display->drawFillRect(currentLane * (320 / 4), 0, 320/4, CHAR_HEIGHT + 7, ACCENT2);

    sprintf(strbuff, "LANE 1");
    if (currentLane == 0) WriteString(display, strbuff, 0 * (320 / 4) + 4, CHAR_HEIGHT + 2, MAIN);
    else                  WriteString(display, strbuff, 0 * (320 / 4) + 4, CHAR_HEIGHT + 2, ACCENT1);

    sprintf(strbuff, "LANE 2");
    if (currentLane == 1) WriteString(display, strbuff, 1 * (320 / 4) + 4, CHAR_HEIGHT + 2, MAIN);
    else                  WriteString(display, strbuff, 1 * (320 / 4) + 4, CHAR_HEIGHT + 2, ACCENT1);

    sprintf(strbuff, "LANE 3");
    if (currentLane == 2) WriteString(display, strbuff, 2 * (320 / 4) + 4, CHAR_HEIGHT + 2, MAIN);
    else                  WriteString(display, strbuff, 2 * (320 / 4) + 4, CHAR_HEIGHT + 2, ACCENT1);

    sprintf(strbuff, "LANE 4");
    if (currentLane == 3) WriteString(display, strbuff, 3 * (320 / 4) + 4, CHAR_HEIGHT + 2, MAIN);
    else                  WriteString(display, strbuff, 3 * (320 / 4) + 4, CHAR_HEIGHT + 2, ACCENT1);
    

    if (changeEffect) {

        int xOffset = 2 * (320/3);
        //turning pixels in right third of screen off
        display->drawFillRect(xOffset, 0, 320/3, 240, BACKGROUND);
        display->drawLine(xOffset, 0, 2 * (320/3), 240, ACCENT2);

        display->drawFillRect(xOffset, (CHAR_HEIGHT + 4) * type + 3, 320/3, CHAR_HEIGHT + 4, ACCENT2);

        sprintf(strbuff, "--");
        if (type == 0) WriteString(display, strbuff, xOffset + 4, (CHAR_HEIGHT + 4) * 1, MAIN);
        else           WriteString(display, strbuff, xOffset + 4, (CHAR_HEIGHT + 4) * 1, MAIN);

        sprintf(strbuff, "FILTER");
        if (type == 1) WriteString(display, strbuff, xOffset + 4, (CHAR_HEIGHT + 4) * 2, MAIN);
        else           WriteString(display, strbuff, xOffset + 4, (CHAR_HEIGHT + 4) * 2, MAIN);

        sprintf(strbuff, "DISTORTION");
        if (type == 2) WriteString(display, strbuff, xOffset + 4, (CHAR_HEIGHT + 4) * 3, MAIN);
        else           WriteString(display, strbuff, xOffset + 4, (CHAR_HEIGHT + 4) * 3, MAIN);

        sprintf(strbuff, "COMPRESSOR");
        if (type == 3) WriteString(display, strbuff, xOffset + 4, (CHAR_HEIGHT + 4) * 4, MAIN);
        else           WriteString(display, strbuff, xOffset + 4, (CHAR_HEIGHT + 4) * 4, MAIN);

        sprintf(strbuff, "AUTOPAN");
        if (type == 4) WriteString(display, strbuff, xOffset + 4, (CHAR_HEIGHT + 4) * 5, MAIN);
        else           WriteString(display, strbuff, xOffset + 4, (CHAR_HEIGHT + 4) * 5, MAIN);

        sprintf(strbuff, "REDUX");
        if (type == 5) WriteString(display, strbuff, xOffset + 4, (CHAR_HEIGHT + 4) * 6, MAIN);
        else           WriteString(display, strbuff, xOffset + 4, (CHAR_HEIGHT + 4) * 6, MAIN);
    }

}

void FXDisplay::AButton() {
    if (!changeEffect) {
        changeEffect = true;
    } else {
        if ( (Effect::EFFECT_TYPE) type != CURRENT_EFFECT->effectType) handler->at(currentLane)->ChangeEffect(currentEffect, (Effect::EFFECT_TYPE) type);
        changeEffect = false;
        EffectNormal();
    }
}

void FXDisplay::BButton(){
    if (changeEffect) {
        changeEffect = false;
        EffectNormal();
    }
}

void FXDisplay::UpButton(){
    if (!changeEffect) CURRENT_EFFECT->PrevParam();
    else {
        type -= 1;
        if (type < 0) type = 0;
    }
}

void FXDisplay::DownButton(){
    if (!changeEffect) CURRENT_EFFECT->NextParam();
    else {
        type += 1;
        if (type > 5) type = 5;
    }
}

void FXDisplay::LeftButton(){
    if (!changeEffect) CURRENT_EFFECT->Decrement();
}

void FXDisplay::RightButton(){
    if (!changeEffect) CURRENT_EFFECT->Increment();
}

void FXDisplay::PlayButton(){

}

void FXDisplay::AltAButton(){

}

void FXDisplay::AltBButton(){

}

void FXDisplay::AltUpButton(){
    CURRENT_EFFECT->selected = false;
    currentLane -= 1;
    if (currentLane < 0) {
        currentLane = 0;
    }
    currentEffect = 0;
    EffectNormal();
}

void FXDisplay::AltDownButton(){
    CURRENT_EFFECT->selected = false;
    currentLane += 1;
    if (currentLane >= handler->size()) {
        currentLane = handler->size() - 1;
    }
    currentEffect = 0;
    EffectNormal();
}

void FXDisplay::AltLeftButton(){
    CURRENT_EFFECT->selected = false;
    currentEffect -= 1;
    if (currentEffect < 0) {
        currentEffect = 0;
    }
    EffectNormal();
}

void FXDisplay::AltRightButton(){
    CURRENT_EFFECT->selected = false;
    currentEffect += 1;
    if (currentEffect >= handler->at(currentLane)->effects.size()) {
        currentEffect = handler->at(currentLane)->effects.size() - 1;
    }
    EffectNormal();
}

void FXDisplay::AltPlayButton(){
    
}