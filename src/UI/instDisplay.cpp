#include "instDisplay.h"

void InstrumentDisplay::Init(std::vector<Instrument*> instruments_, InstrumentHandler* handler_, cFont *MainFont) {
    //display = display_;
    instruments = instruments_;
    handler = handler_;
    activeInst = instruments.begin();
    currentSlice = 0;
    sliceEdit = true;
    this->MainFont = MainFont;
}

void InstrumentDisplay::UpdateDisplay(cLayer* display) {
    display->eraseLayer();

    /**
     * Drawing waveform
     */
    for (int i = 0; i < WAVEWIDTH; i ++) {
        int y0, y1;
        double range;
        range = (*activeInst)->visual[i] * ((double) WAVEHEIGHT / (double) 0x8FFF);
        y1 = (range / 2.0f) + (WAVEHEIGHT / 2.0f);
        y0 = y1 - range;

        display->drawLine(i, y0, i, y1, ACCENT1);
    }

    /**
     * Drawing slice lines
     */

    auto it = (*activeInst)->slices.begin();
    while (it != (*activeInst)->slices.end()) {
        int x = *it * WAVEWIDTH;
        if ((*activeInst)->slices.begin() + currentSlice == it && sliceEdit) display->drawFillRect(x, 0, 1, WAVEHEIGHT, ACCENT2);
        else display->drawFillRect(x, 0, 1, WAVEHEIGHT, ALTACCENT2);
        it++;
    }

    /**
     * Drawing pitch and gain
     */
    Instrument::param edit = (*activeInst)->GetEdit();

    sprintf(strbuff, "PITCH");
    WriteString(display, strbuff, 0, WAVEHEIGHT + CHAR_HEIGHT + 4, ACCENT2);

    sprintf(strbuff, "%.2fst", (*activeInst)->GetPitch());
    if (edit == Instrument::p && !sliceEdit) WriteString(display, strbuff, 0, WAVEHEIGHT + (CHAR_HEIGHT + 4) * 2, ACCENT1);
    else                                     WriteString(display, strbuff, 0, WAVEHEIGHT + (CHAR_HEIGHT + 4) * 2, MAIN);

    sprintf(strbuff, "GAIN");
    WriteString(display, strbuff, 0, WAVEHEIGHT + (CHAR_HEIGHT + 4) * 3, ACCENT2);

    sprintf(strbuff, "%.2fdB", (*activeInst)->GetGain());
    if (edit == Instrument::g && !sliceEdit) WriteString(display, strbuff, 0, WAVEHEIGHT + (CHAR_HEIGHT + 4) * 4, ACCENT1);
    else                                     WriteString(display, strbuff, 0, WAVEHEIGHT + (CHAR_HEIGHT + 4) * 4, MAIN);


    /**
     * Drawing ADSR diagram
     */
    int adsrHeight = (CHAR_HEIGHT + 4) * 4;
    int adsrWidth = 0;
    int xOffset = CHAR_WIDTH * 8 + 4;

    if (!sliceEdit && (int) edit > 1) {
        adsrWidth = 112;
    } else {
        adsrWidth = WAVEWIDTH - xOffset;
    }
   
    float attack = (*activeInst)->GetAttack();
    float decay = (*activeInst)->GetDecay();
    float release = (*activeInst)->GetRelease();
    
    float total = attack + decay + release;

    int attX = ((attack / total) * 0.75f) * (float) adsrWidth;
    int decX = ((decay / total) * 0.75f) * (float) adsrWidth;
    int susX = 0.25f * (float) adsrWidth;

    int susY = (float) (*activeInst)->GetSustain() * (float) adsrHeight;

    /**
     * Drawing ADSR values when selected
     */
    if (edit == Instrument::a && !sliceEdit) display->drawLine(xOffset, 240, xOffset + attX, 239 - adsrHeight, ACCENT2);
    else                                     display->drawLine(xOffset, 240, xOffset + attX, 239 - adsrHeight, MAIN);

    if (edit == Instrument::d && !sliceEdit) display->drawLine(xOffset + attX, 239 - adsrHeight, xOffset + attX + decX, 239 - susY, ACCENT2);
    else                                     display->drawLine(xOffset + attX, 239 - adsrHeight, xOffset + attX + decX, 239 - susY, MAIN);

    if (edit == Instrument::s && !sliceEdit) display->drawLine(xOffset + attX + decX, 239 - susY, xOffset + attX + decX + susX, 239 - susY, ACCENT2);
    else                                     display->drawLine(xOffset + attX + decX, 239 - susY, xOffset + attX + decX + susX, 239 - susY, MAIN);

    if (edit == Instrument::r && !sliceEdit) display->drawLine(xOffset + attX + decX + susX, 239 - susY, xOffset + adsrWidth, 240, ACCENT2);
    else                                     display->drawLine(xOffset + attX + decX + susX, 239 - susY, xOffset + adsrWidth, 240, MAIN);

    xOffset += adsrWidth;

    if (edit == Instrument::a && !sliceEdit) {
        sprintf(strbuff, "ATT");
        WriteString(display, strbuff, xOffset, WAVEHEIGHT + CHAR_HEIGHT + 4, ACCENT2);

        float att = (*activeInst)->GetAttack() * 1000.0f;

        if (att < 100.0f)       sprintf(strbuff, "%.2fms", att);
        else if (att < 1000.0f) sprintf(strbuff, "%.1fms", att);
        else sprintf(strbuff, "%.2fs", att / 1000.0f);

        WriteString(display, strbuff, xOffset, WAVEHEIGHT + (CHAR_HEIGHT + 4) * 2, ACCENT1);
    } else if (edit == Instrument::d && !sliceEdit) {
        sprintf(strbuff, "DEC");
        WriteString(display, strbuff, xOffset, WAVEHEIGHT + CHAR_HEIGHT + 4, ACCENT2);

        float dec = (*activeInst)->GetDecay() * 1000.0f;

        if (dec < 100.0f)       sprintf(strbuff, "%.2fms", dec);
        else if (dec < 1000.0f) sprintf(strbuff, "%.1fms", dec);
        else sprintf(strbuff, "%.2fs", dec / 1000.0f);

        WriteString(display, strbuff, xOffset, WAVEHEIGHT + (CHAR_HEIGHT + 4) * 2, ACCENT1);
    } else if (edit == Instrument::s && !sliceEdit) {
        sprintf(strbuff, "SUS");
        WriteString(display, strbuff, xOffset, WAVEHEIGHT + CHAR_HEIGHT + 4, ACCENT2);
        sprintf(strbuff, "%.2f%%", (*activeInst)->GetSustain());
        WriteString(display, strbuff, xOffset, WAVEHEIGHT + (CHAR_HEIGHT + 4) * 2, ACCENT1);
    } else if (edit == Instrument::r && !sliceEdit) {
        sprintf(strbuff, "REL");
        WriteString(display, strbuff, xOffset, WAVEHEIGHT + CHAR_HEIGHT + 4, ACCENT2);

        float rel = (*activeInst)->GetRelease() * 1000.0f;

        if (rel < 100.0f)       sprintf(strbuff, "%.2fms", rel);
        else if (rel < 1000.0f) sprintf(strbuff, "%.1fms", rel);
        else sprintf(strbuff, "%.2fs", rel / 1000.0f);

        WriteString(display, strbuff, xOffset, WAVEHEIGHT + (CHAR_HEIGHT + 4) * 2, ACCENT1);
    }

    /**
     * Drawing Instruments
     */
    int yOffset = CHAR_HEIGHT + 4;
    for (Instrument* inst : instruments) {
        sprintf(strbuff, "%s", inst->GetName());
        if (inst == (*activeInst)) {
            display->drawFillRect(WAVEWIDTH, yOffset - CHAR_HEIGHT, 100, CHAR_HEIGHT + 4, ACCENT2);
            WriteString(display, strbuff, WAVEWIDTH + 2, yOffset, MAIN);
        } else {
            WriteString(display, strbuff, WAVEWIDTH + 2, yOffset, ACCENT1);
        }
        yOffset += CHAR_HEIGHT + 8;
    }
    
}

void InstrumentDisplay::WriteString(MyOledDisplay &display, uint16_t x, uint16_t y, bool on) {
    display.SetCursor(x, y);
    display.WriteString(strbuff, Font_4x6, on);
}

void InstrumentDisplay::WriteString(cLayer* display, char* strbuff, int x, int y, DadGFX::sColor color) {
    display->setCursor(x, y);
    display->setFont(MainFont);
    display->setTextFrontColor(color);
    display->drawText(strbuff);
}

void InstrumentDisplay::DrawArrow(MyOledDisplay &display, int x, int y) {
    display.DrawPixel(x, y, true);
    display.DrawPixel(x + 1, y - 1, true);
    display.DrawPixel(x + 1, y + 1, true);
}


void InstrumentDisplay::AButton() {
    if (!sliceEdit) return;
    auto it = (*activeInst)->slices.begin();
    double val;
    if (currentSlice + 1 < (*activeInst)->slices.size()) {
        std::advance(it, currentSlice + 1);
        val = (*activeInst)->slices.at(currentSlice) + (((*activeInst)->slices.at(currentSlice + 1) - (*activeInst)->slices.at(currentSlice)) / (double) 2.0);
        (*activeInst)->slices.insert(it, val);
    } else {
        val = (*activeInst)->slices.at(currentSlice) + (((double) 1.0 - (*activeInst)->slices.at(currentSlice)) / (double) 2.0);
        (*activeInst)->slices.push_back(val);
    }
};

void InstrumentDisplay::BButton(){
    if (!sliceEdit) return;
    auto it = (*activeInst)->slices.begin();
    std::advance(it, currentSlice);
    if ((*activeInst)->slices.size() > 1) {
        (*activeInst)->slices.erase(it);
        if (currentSlice > 0) {
            currentSlice -= 1;
        } else {
            currentSlice = 0;
        }
    }
};

void InstrumentDisplay::UpButton(){
    if (sliceEdit) {
        if (currentSlice > 0) {
            currentSlice--;
        }
    } else {
        (*activeInst)->Increment();
    }
};

void InstrumentDisplay::DownButton(){
    if (sliceEdit) {
        if (currentSlice < (*activeInst)->slices.size() - 1) {
            currentSlice++;
        }
        
    } else {
        (*activeInst)->Decrement();
    }
};

void InstrumentDisplay::LeftButton(){
    if (sliceEdit) {
        (*activeInst)->slices.at(currentSlice) -= (double) 0.002;
        if ((*activeInst)->slices.at(currentSlice) < 0.0) (*activeInst)->slices.at(currentSlice) = (double) 0.0;
    } else {
     (*activeInst)->PrevParam();
    }
};

void InstrumentDisplay::RightButton(){
    if (sliceEdit) {
        (*activeInst)->slices.at(currentSlice) += (double) 0.002;
        if ((*activeInst)->slices.at(currentSlice) > 1.0) (*activeInst)->slices.at(currentSlice) = (double) 1.0;
    } else {
        (*activeInst)->NextParam();
    }
};

void InstrumentDisplay::PlayButton(){
    handler->Preview((*activeInst), currentSlice + 48);
};

void InstrumentDisplay::AltAButton(){};
void InstrumentDisplay::AltBButton(){};

void InstrumentDisplay::AltUpButton(){
    if (activeInst != instruments.begin()) {
        activeInst--;
    }
};

void InstrumentDisplay::AltDownButton(){
    if (activeInst != instruments.end()) {
        activeInst++;
    }
};

void InstrumentDisplay::AltLeftButton(){};
void InstrumentDisplay::AltRightButton(){};

void InstrumentDisplay::AltPlayButton(){
    sliceEdit = !sliceEdit;
};