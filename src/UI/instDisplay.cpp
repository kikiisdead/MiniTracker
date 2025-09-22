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
    display->eraseLayer();

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
            //range = (*activeInst)->visual[i] * ((double) WAVEHEIGHT / (double) 0x8FFF);
            range = ACTIVEINST->visual[i] * ((double) WAVEHEIGHT / (double) 0x8FFF);
            y1 = (range / 2.0f) + (WAVEHEIGHT / 2.0f);
            y0 = y1 - range;

            display->drawLine(i, y0, i, y1, ACCENT1);
        }

        /**
         * Drawing slice lines
         */

        //auto it = (*activeInst)->slices.begin();
        auto it = ACTIVEINST->slices.begin();
        // while (it != (*activeInst)->slices.end()) {
        while (it != ACTIVEINST->slices.end()) {
            int x = *it * WAVEWIDTH;
            //if ((*activeInst)->slices.begin() + currentSlice == it && sliceEdit) display->drawFillRect(x, 0, 1, WAVEHEIGHT, ACCENT2);
            if (ACTIVEINST->slices.begin() + currentSlice == it && sliceEdit) display->drawFillRect(x, 0, 1, WAVEHEIGHT, ACCENT2);
            else display->drawFillRect(x, 0, 1, WAVEHEIGHT, ALTACCENT2);
            it++;
        }

        /**
         * Drawing pitch and gain
         */
        // Instrument::param edit = (*activeInst)->GetEdit();
        Instrument::param edit = ACTIVEINST->GetEdit();
        

        sprintf(strbuff, "PITCH");
        WriteString(display, strbuff, 0, WAVEHEIGHT + CHAR_HEIGHT + 4, ACCENT2);

        //sprintf(strbuff, "%.2fst", (*activeInst)->GetPitch());
        sprintf(strbuff, "%.2fst", ACTIVEINST->GetPitch());
        if (edit == Instrument::p && !sliceEdit) WriteString(display, strbuff, 0, WAVEHEIGHT + (CHAR_HEIGHT + 4) * 2, ACCENT1);
        else                                     WriteString(display, strbuff, 0, WAVEHEIGHT + (CHAR_HEIGHT + 4) * 2, MAIN);

        sprintf(strbuff, "GAIN");
        WriteString(display, strbuff, 0, WAVEHEIGHT + (CHAR_HEIGHT + 4) * 3, ACCENT2);

        // sprintf(strbuff, "%.2fdB", (*activeInst)->GetGain());
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
    
        // float attack = (*activeInst)->GetAttack();
        // float decay = (*activeInst)->GetDecay();
        // float release = (*activeInst)->GetRelease();

        float attack = ACTIVEINST->GetAttack();
        float decay = ACTIVEINST->GetDecay();
        float release = ACTIVEINST->GetRelease();
        
        float total = attack + decay + release;

        int attX = ((attack / total) * 0.75f) * (float) adsrWidth;
        int decX = ((decay / total) * 0.75f) * (float) adsrWidth;
        int susX = 0.25f * (float) adsrWidth;

        // int susY = (float) (*activeInst)->GetSustain() * (float) adsrHeight;
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

            // float att = (*activeInst)->GetAttack() * 1000.0f;
            float att = ACTIVEINST->GetAttack() * 1000.0f;

            if (att < 100.0f)       sprintf(strbuff, "%.2fms", att);
            else if (att < 1000.0f) sprintf(strbuff, "%.1fms", att);
            else sprintf(strbuff, "%.2fs", att / 1000.0f);

            WriteString(display, strbuff, xOffset, WAVEHEIGHT + (CHAR_HEIGHT + 4) * 2, ACCENT1);
        } else if (edit == Instrument::d && !sliceEdit) {
            sprintf(strbuff, "DEC");
            WriteString(display, strbuff, xOffset, WAVEHEIGHT + CHAR_HEIGHT + 4, ACCENT2);

            // float dec = (*activeInst)->GetDecay() * 1000.0f;
            float dec = ACTIVEINST->GetDecay() * 1000.0f;

            if (dec < 100.0f)       sprintf(strbuff, "%.2fms", dec);
            else if (dec < 1000.0f) sprintf(strbuff, "%.1fms", dec);
            else sprintf(strbuff, "%.2fs", dec / 1000.0f);

            WriteString(display, strbuff, xOffset, WAVEHEIGHT + (CHAR_HEIGHT + 4) * 2, ACCENT1);
        } else if (edit == Instrument::s && !sliceEdit) {
            sprintf(strbuff, "SUS");
            WriteString(display, strbuff, xOffset, WAVEHEIGHT + CHAR_HEIGHT + 4, ACCENT2);
            // sprintf(strbuff, "%.2f%%", (*activeInst)->GetSustain());
            sprintf(strbuff, "%.2f%%", ACTIVEINST->GetSustain());
            WriteString(display, strbuff, xOffset, WAVEHEIGHT + (CHAR_HEIGHT + 4) * 2, ACCENT1);
        } else if (edit == Instrument::r && !sliceEdit) {
            sprintf(strbuff, "REL");
            WriteString(display, strbuff, xOffset, WAVEHEIGHT + CHAR_HEIGHT + 4, ACCENT2);

            // float rel = (*activeInst)->GetRelease() * 1000.0f;
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
            // if (inst == (*activeInst)) {
            if (i == active) {
                display->drawFillRect(WAVEWIDTH, yOffset - CHAR_HEIGHT, 100, CHAR_HEIGHT + 4, ACCENT2);
                WriteString(display, strbuff, WAVEWIDTH + 2, yOffset, MAIN);
            } else {
                WriteString(display, strbuff, WAVEWIDTH + 2, yOffset, ACCENT1);
            }
            yOffset += CHAR_HEIGHT + 8;
        }
    }
}

void InstrumentDisplay::WriteString(cLayer* display, char* strbuff, int x, int y, DadGFX::sColor color) {
    display->setCursor(x, y);
    display->setFont(MainFont);
    display->setTextFrontColor(color);
    display->drawText(strbuff);
}

void InstrumentDisplay::AButton() {
    if (!sliceEdit) return;
    // auto it = (*activeInst)->slices.begin();
    auto it = ACTIVEINST->slices.begin();
    double val;
    // if (currentSlice + 1 < (*activeInst)->slices.size()) {
    if (currentSlice + 1 < ACTIVEINST->slices.size()) {
        std::advance(it, currentSlice + 1);
        // val = (*activeInst)->slices.at(currentSlice) + (((*activeInst)->slices.at(currentSlice + 1) - (*activeInst)->slices.at(currentSlice)) / (double) 2.0);
        val = ACTIVEINST->slices.at(currentSlice) + ((ACTIVEINST->slices.at(currentSlice + 1) - ACTIVEINST->slices.at(currentSlice)) / (double) 2.0);
        // (*activeInst)->slices.insert(it, val);
        ACTIVEINST->slices.insert(it, val);
    } else {
        // val = (*activeInst)->slices.at(currentSlice) + (((double) 1.0 - (*activeInst)->slices.at(currentSlice)) / (double) 2.0);
        val = ACTIVEINST->slices.at(currentSlice) + (((double) 1.0 - ACTIVEINST->slices.at(currentSlice)) / (double) 2.0);
        // (*activeInst)->slices.push_back(val);
        ACTIVEINST->slices.push_back(val);
    }
};

void InstrumentDisplay::BButton(){
    if (!sliceEdit) return;
    //auto it = (*activeInst)->slices.begin();
    auto it = ACTIVEINST->slices.begin();
    std::advance(it, currentSlice);
    // if ((*activeInst)->slices.size() > 1) {
    if (ACTIVEINST->slices.size() > 1) {
        // (*activeInst)->slices.erase(it);
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
        // (*activeInst)->Increment();
        ACTIVEINST->Increment();
    }
};

void InstrumentDisplay::DownButton(){
    if (sliceEdit) {
        // if (currentSlice < (*activeInst)->slices.size() - 1) {
        if (currentSlice < ACTIVEINST->slices.size() - 1) {
            currentSlice++;
        }
        
    } else {
        // (*activeInst)->Decrement();
        ACTIVEINST->Decrement();
    }
};

void InstrumentDisplay::LeftButton(){
    if (sliceEdit) {
        // (*activeInst)->slices.at(currentSlice) -= (double) 0.002;
        ACTIVEINST->slices.at(currentSlice) -= (double) 0.002;
        // if ((*activeInst)->slices.at(currentSlice) < 0.0) (*activeInst)->slices.at(currentSlice) = (double) 0.0;
        if (ACTIVEINST->slices.at(currentSlice) < 0.0) ACTIVEINST->slices.at(currentSlice) = (double) 0.0;
    } else {
        // (*activeInst)->PrevParam();
        ACTIVEINST->PrevParam();
    }
};

void InstrumentDisplay::RightButton(){
    if (sliceEdit) {
        // (*activeInst)->slices.at(currentSlice) += (double) 0.002;
        ACTIVEINST->slices.at(currentSlice) += (double) 0.002;
        // if ((*activeInst)->slices.at(currentSlice) > 1.0) (*activeInst)->slices.at(currentSlice) = (double) 1.0;
        if (ACTIVEINST->slices.at(currentSlice) > 1.0) ACTIVEINST->slices.at(currentSlice) = (double) 1.0;
    } else {
        // (*activeInst)->NextParam();
        ACTIVEINST->NextParam();
    }
};

void InstrumentDisplay::PlayButton(){
    // handler->Preview((*activeInst), currentSlice + 48);
    handler->Preview(ACTIVEINST, currentSlice + 48);
};

void InstrumentDisplay::AltAButton(){};
void InstrumentDisplay::AltBButton(){};

void InstrumentDisplay::AltUpButton(){
    // if (activeInst != instruments.begin()) {
    //     activeInst--;
    // }
    active -= 1;
    if (active < 0) {
        active = 0;
    }
};

void InstrumentDisplay::AltDownButton(){
    // if (activeInst != instruments.end()) {
    //     activeInst++;
    // }
    active += 1;
    if (active >= (int) instruments->size()) {
        active = instruments->size() - 1;
    }
};

void InstrumentDisplay::AltLeftButton(){};
void InstrumentDisplay::AltRightButton(){};

void InstrumentDisplay::AltPlayButton(){
    sliceEdit = !sliceEdit;
};