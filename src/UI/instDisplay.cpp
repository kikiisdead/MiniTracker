#include "instDisplay.h"

void InstrumentDisplay::Init(std::vector<Instrument*> instruments_, InstrumentHandler* handler_) {
    //display = display_;
    instruments = instruments_;
    handler = handler_;
    activeInst = instruments.begin();
    currentSlice = 0;
    sliceEdit = true;
}

void InstrumentDisplay::UpdateDisplay(MyOledDisplay &display) {
    display.Fill(false);

    /**
     * Drawing selector for current slice being edited
     */
    int selBeg, selEnd;
    selBeg = (*activeInst)->slices.at(currentSlice) * WAVEWIDTH;

    if (currentSlice + 1 < (*activeInst)->slices.size()) {
        selEnd = (*activeInst)->slices.at(currentSlice + 1) * WAVEWIDTH;
    } else {
        selEnd = WAVEWIDTH - 1;
    }
    display.DrawRect(selBeg, 0, selEnd, WAVEHEIGHT, true, true);

    /**
     * Drawing slice lines
     */
    for (double slice : (*activeInst)->slices) {
        int x = slice * WAVEWIDTH;
        if (x > selBeg && x <= selEnd) display.DrawLine(x, 0, x, WAVEHEIGHT, false);
        else display.DrawLine(x, 0, x, WAVEHEIGHT, true);
    }

    /**
     * Drawing waveform
     */
    for (int i = 0; i < WAVEWIDTH; i ++) {
        int y0, y1, range;
        range = (*activeInst)->visual[i] * ((double) WAVEHEIGHT / (double) 0x8FFF);
        y0 = (range / 2) + (WAVEHEIGHT / 2);
        y1 = y0 - range;
        if (i >= selBeg && i < selEnd) display.DrawLine(i, y0, i, y1, false);
        else display.DrawLine(i, y0, i, y1, true);
    }

    /**
     * Drawing bottom UI
     */
    Instrument::param edit = (*activeInst)->GetEdit();

    if (!sliceEdit) DrawArrow(display, ((int)edit * 20) + 13, 53);

    sprintf(strbuff, "PIT");
    WriteString(display, 0, 51, true);
    sprintf(strbuff, "%dST", static_cast<int>((*activeInst)->GetPitch()));
    WriteString(display, 0, 58, true);

    sprintf(strbuff, "ATT");
    WriteString(display, 20, 51, true);

    sprintf(strbuff, "%dMS", static_cast<int>((*activeInst)->GetAttack() * 1000.0f));
    WriteString(display, 20, 58, true);

    sprintf(strbuff, "DEC");
    WriteString(display, 40, 51, true);

    sprintf(strbuff, "%dMS", static_cast<int>((*activeInst)->GetDecay() * 1000.0f));
    WriteString(display, 40, 58, true);

    sprintf(strbuff, "SUS");
    WriteString(display, 60, 51, true);


    sprintf(strbuff, "%d", static_cast<int>((*activeInst)->GetSustain() * 100.0f));
    WriteString(display, 60, 58, true);

    sprintf(strbuff, "REL");
    WriteString(display, 80, 51, true);

    sprintf(strbuff, "%dMS", static_cast<int>((*activeInst)->GetRelease() * 1000.0f));
    WriteString(display, 80, 58, true);

    /**
     * Drawing Instruments
     */
    int yOffset = 1;
    for (Instrument* inst : instruments) {
        sprintf(strbuff, "%s", inst->GetName());
        if (inst == (*activeInst)) {
            display.DrawRect(96, yOffset - 1, 128, yOffset + 5, true, true);
            WriteString(display, 98, yOffset, false);
        } else {
            WriteString(display, 98, yOffset, true);
        }
        yOffset += 8;
    }
    
}

void InstrumentDisplay::WriteString(MyOledDisplay &display, uint16_t x, uint16_t y, bool on) {
    display.SetCursor(x, y);
    display.WriteString(strbuff, Font_4x6, on);
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