#include "fxDisplay.h"

void FXDisplay::UpdateDisplay(MyOledDisplay &display) {
    display.Fill(false);

    for (size_t i = 0; i < handler.at(currentLane)->effects.size(); i++) {
        handler.at(currentLane)->effects.at(i)->Display(display, 2 + i * (FX_WIDTH + 3), 9);
    }

    display.DrawRect(currentLane * 32, 0, currentLane * 32 + 31, 6, true, true);

    display.SetCursor(1, 1);
    sprintf(strbuff, "LANE 1");
    if (currentLane == 0) display.WriteString(strbuff, Font_4x6, false);
    else display.WriteString(strbuff, Font_4x6, true);

    display.SetCursor(33, 1);
    sprintf(strbuff, "LANE 2");
    if (currentLane == 1) display.WriteString(strbuff, Font_4x6, false);
    else display.WriteString(strbuff, Font_4x6, true);

    display.SetCursor(65, 1);
    sprintf(strbuff, "LANE 3");
    if (currentLane == 2) display.WriteString(strbuff, Font_4x6, false);
    else display.WriteString(strbuff, Font_4x6, true);

    display.SetCursor(97, 1);
    sprintf(strbuff, "LANE 4");
    if (currentLane == 3) display.WriteString(strbuff, Font_4x6, false);
    else display.WriteString(strbuff, Font_4x6, true);

    if (changeEffect) {
        //turning pixels in right third of screen off
        display.DrawRect(2 * (128 / 3), 0, 127, 63, false, true);
        display.DrawLine(2 * (128 / 3), 0, 2 * (128 / 3), 63, true);

        display.DrawRect(2 * (128 / 3), 7 * type + 1, 127, 7 * type + 7, true, true);

        sprintf(strbuff, "--");
        WriteString(display, 2 * (128 / 3) + 2, 2, !(type == 0));

        sprintf(strbuff, "FILTER");
        WriteString(display, 2 * (128 / 3) + 2, 9, !(type == 1));
    }

}

void FXDisplay::WriteString(MyOledDisplay &display, uint16_t x, uint16_t y, bool on) {
    display.SetCursor(x, y);
    display.WriteString(strbuff, Font_4x6, on);
}

void FXDisplay::AButton() {
    if (!changeEffect) {
        changeEffect = true;
    } else {
        if ( (Effect::EFFECT_TYPE) type != CURRENT_EFFECT->effectType) handler.at(currentLane)->ChangeEffect(currentEffect, (Effect::EFFECT_TYPE) type);
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
        if (type > 1) type = 1;
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
    if (currentLane >= handler.size()) {
        currentLane = handler.size() - 1;
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
    if (currentEffect >= handler.at(currentLane)->effects.size()) {
        currentEffect = handler.at(currentLane)->effects.size() - 1;
    }
    EffectNormal();
}

void FXDisplay::AltPlayButton(){
    
}