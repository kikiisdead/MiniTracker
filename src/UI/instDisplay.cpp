#include "instDisplay.h"

void InstrumentDisplay::Init(std::vector<Instrument*>* instruments_, InstrumentHandler* handler_, cFont *MainFont) {
    instruments = instruments_;
    handler = handler_;
    active = -1;
    currentSlice = 0;
    sliceEdit = true;
    this->MainFont = MainFont;
}

void InstrumentDisplay::UpdateDisplay(cLayer* display) {
    display->eraseLayer(BACKGROUND);

    if (!instruments->empty() && active < 0) {
        active = 0;
    } else if (!instruments->empty() && active >= (int) instruments->size()) {
        active = instruments->size() - 1;
    }

    if (!instruments->empty()) {
        /**
         * Drawing waveform
         */
        for (int i = 0; i < WAVEWIDTH; i ++) {
            int y0, y1;
            double range;
            range = ACTIVEINST->visual[i] * ((double) WAVEHEIGHT / (double) 0x8FFF);
            y1 = (range / 2.0f) + (WAVEHEIGHT / 2.0f);
            y0 = y1 - range;

            display->drawLine(i, y0, i, y1, ACCENT1);
        }

        /**
         * Drawing slice lines
         */
        auto it = ACTIVEINST->slices.begin();
        while (it != ACTIVEINST->slices.end()) {
            int x = *it * WAVEWIDTH;
            if (ACTIVEINST->slices.begin() + currentSlice == it && sliceEdit) display->drawFillRect(x, 0, 1, WAVEHEIGHT, ACCENT2);
            else display->drawFillRect(x, 0, 1, WAVEHEIGHT, ALTACCENT2);
            it++;
        }

        /**
         * Drawing pitch and gain
         */
        Instrument::param edit = ACTIVEINST->GetEdit();
        

        sprintf(strbuff, "PITCH");
        WriteString(display, strbuff, 0, WAVEHEIGHT + CHAR_HEIGHT + 4, ACCENT2);

        sprintf(strbuff, "%.2fst", ACTIVEINST->GetPitch());
        if (edit == Instrument::p && !sliceEdit) WriteString(display, strbuff, 0, WAVEHEIGHT + (CHAR_HEIGHT + 4) * 2, ACCENT1);
        else                                     WriteString(display, strbuff, 0, WAVEHEIGHT + (CHAR_HEIGHT + 4) * 2, MAIN);

        sprintf(strbuff, "GAIN");
        WriteString(display, strbuff, 0, WAVEHEIGHT + (CHAR_HEIGHT + 4) * 3, ACCENT2);

        sprintf(strbuff, "%.2fdB", ACTIVEINST->GetGain());
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

        float attack = ACTIVEINST->GetAttack();
        float decay = ACTIVEINST->GetDecay();
        float release = ACTIVEINST->GetRelease();
        
        float total = attack + decay + release;

        int attX = ((attack / total) * 0.75f) * (float) adsrWidth;
        int decX = ((decay / total) * 0.75f) * (float) adsrWidth;
        int susX = 0.25f * (float) adsrWidth;

        int susY = (float) ACTIVEINST->GetSustain() * (float) adsrHeight;

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

            float att = ACTIVEINST->GetAttack() * 1000.0f;

            if (att < 100.0f)       sprintf(strbuff, "%.2fms", att);
            else if (att < 1000.0f) sprintf(strbuff, "%.1fms", att);
            else sprintf(strbuff, "%.2fs", att / 1000.0f);

            WriteString(display, strbuff, xOffset, WAVEHEIGHT + (CHAR_HEIGHT + 4) * 2, ACCENT1);
        } else if (edit == Instrument::d && !sliceEdit) {
            sprintf(strbuff, "DEC");
            WriteString(display, strbuff, xOffset, WAVEHEIGHT + CHAR_HEIGHT + 4, ACCENT2);

            float dec = ACTIVEINST->GetDecay() * 1000.0f;

            if (dec < 100.0f)       sprintf(strbuff, "%.2fms", dec);
            else if (dec < 1000.0f) sprintf(strbuff, "%.1fms", dec);
            else sprintf(strbuff, "%.2fs", dec / 1000.0f);

            WriteString(display, strbuff, xOffset, WAVEHEIGHT + (CHAR_HEIGHT + 4) * 2, ACCENT1);
        } else if (edit == Instrument::s && !sliceEdit) {
            sprintf(strbuff, "SUS");
            WriteString(display, strbuff, xOffset, WAVEHEIGHT + CHAR_HEIGHT + 4, ACCENT2);
            sprintf(strbuff, "%.2f%%", ACTIVEINST->GetSustain());
            WriteString(display, strbuff, xOffset, WAVEHEIGHT + (CHAR_HEIGHT + 4) * 2, ACCENT1);
        } else if (edit == Instrument::r && !sliceEdit) {
            sprintf(strbuff, "REL");
            WriteString(display, strbuff, xOffset, WAVEHEIGHT + CHAR_HEIGHT + 4, ACCENT2);

            float rel = ACTIVEINST->GetRelease() * 1000.0f;

            if (rel < 100.0f)       sprintf(strbuff, "%.2fms", rel);
            else if (rel < 1000.0f) sprintf(strbuff, "%.1fms", rel);
            else sprintf(strbuff, "%.2fs", rel / 1000.0f);

            WriteString(display, strbuff, xOffset, WAVEHEIGHT + (CHAR_HEIGHT + 4) * 2, ACCENT1);
        }

        /**
         * Drawing Instruments
         */
        int yOffset = CHAR_HEIGHT + 4;
        for (int i = 0; i < (int) instruments->size(); i++) {
            sprintf(strbuff, "%s", instruments->at(i)->GetName());
            if (i == active) {
                display->drawFillRect(WAVEWIDTH, yOffset - (CHAR_HEIGHT + 4), 100, CHAR_HEIGHT + 8, ACCENT2);
                WriteString(display, strbuff, WAVEWIDTH + 2, yOffset, MAIN);
            } else {
                WriteString(display, strbuff, WAVEWIDTH + 2, yOffset, ACCENT1);
            }
            yOffset += CHAR_HEIGHT + 8;
        }
    }
    else {
        display->drawRect(80, 60, 160, 120, 1, ACCENT2);
        sprintf(strbuff, "NO SAMPLES");
        WriteString(display, strbuff, 160 - (5 * CHAR_WIDTH), 120 - 2, MAIN);
        sprintf(strbuff, "LOADED");
        WriteString(display, strbuff, 160 - (3 * CHAR_WIDTH), 120 + CHAR_HEIGHT + 2, MAIN);
    }
}

void InstrumentDisplay::AButton() {
    if (!sliceEdit) return;
    auto it = ACTIVEINST->slices.begin();
    double val;
    if (currentSlice + 1 < ACTIVEINST->slices.size()) {
        std::advance(it, currentSlice + 1);
        val = ACTIVEINST->slices.at(currentSlice) + ((ACTIVEINST->slices.at(currentSlice + 1) - ACTIVEINST->slices.at(currentSlice)) / (double) 2.0);
        ACTIVEINST->slices.insert(it, val);
    } else {
        val = ACTIVEINST->slices.at(currentSlice) + (((double) 1.0 - ACTIVEINST->slices.at(currentSlice)) / (double) 2.0);
        ACTIVEINST->slices.push_back(val);
    }
};

void InstrumentDisplay::BButton(){
    if (!sliceEdit) return;
    auto it = ACTIVEINST->slices.begin();
    std::advance(it, currentSlice);
    if (ACTIVEINST->slices.size() > 1) {
        ACTIVEINST->slices.erase(it);
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
        ACTIVEINST->Increment();
    }
};

void InstrumentDisplay::DownButton(){
    if (sliceEdit) {
        if (currentSlice < ACTIVEINST->slices.size() - 1) {
            currentSlice++;
        }
        
    } else {
        ACTIVEINST->Decrement();
    }
};

void InstrumentDisplay::LeftButton(){
    if (sliceEdit) {
        ACTIVEINST->slices.at(currentSlice) -= (double) 0.002;
        if (ACTIVEINST->slices.at(currentSlice) < 0.0) ACTIVEINST->slices.at(currentSlice) = (double) 0.0;
    } else {
        ACTIVEINST->PrevParam();
    }
};

void InstrumentDisplay::RightButton(){
    if (sliceEdit) {
        ACTIVEINST->slices.at(currentSlice) += (double) 0.002;
        if (ACTIVEINST->slices.at(currentSlice) > 1.0) ACTIVEINST->slices.at(currentSlice) = (double) 1.0;
    } else {
        ACTIVEINST->NextParam();
    }
};

void InstrumentDisplay::PlayButton(){
    handler->Preview(ACTIVEINST, currentSlice + 48);
};

void InstrumentDisplay::AltAButton(){};
void InstrumentDisplay::AltBButton(){};

void InstrumentDisplay::AltUpButton(){
    active -= 1;
    if (active < 0) {
        active = 0;
    }
    currentSlice = 0;
};

void InstrumentDisplay::AltDownButton(){
    active += 1;
    if (active >= (int) instruments->size()) {
        active = instruments->size() - 1;
    }
    currentSlice = 0;
};

void InstrumentDisplay::AltLeftButton(){};
void InstrumentDisplay::AltRightButton(){};

void InstrumentDisplay::AltPlayButton(){
    sliceEdit = !sliceEdit;
};