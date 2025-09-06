#include "display.h"

void Display::Init(MyOledDisplay* display, Sequencer* sequencer_) {
        display_ = display;
        sequencer = sequencer_;
    }

void Display::DrawStep(int x, int y, Step* step) {
    // Writing instrument
    display_->SetCursor(x + 2, y + 2);
    if (step->instrument < 0) sprintf(strbuff, "--");
    else if (step->instrument < 10) sprintf(strbuff, "0%d", step->instrument);
    else sprintf(strbuff, "%d", step->instrument);
    display_->WriteString(strbuff, Font_4x6, true);

    // Writing note
    display_->SetCursor(x + 13, y + 2);
    if (step->instrument == -1) sprintf(strbuff, "---");
    else if (step->instrument == -2) sprintf(strbuff, "OFF");
    else GetNoteString(strbuff, step->note);
    display_->WriteString(strbuff, Font_4x6, true);

    // Writing fx
    display_->SetCursor(x + 28, y + 2);
    if (step->instrument < 0 || step->note < 0) sprintf(strbuff, "---");
    else GetFxString(strbuff, step->fx, step->fxAmount);
    display_->WriteString(strbuff, Font_4x6, true);

    // Drawing select things
    if (step->selected) {
        display_->DrawPixel(x, y, true);
        display_->DrawPixel(x+1, y, true);
        display_->DrawPixel(x, y+1, true);

        display_->DrawPixel(x+40, y, true);
        display_->DrawPixel(x+39, y, true);
        display_->DrawPixel(x+40, y+1, true);

        display_->DrawPixel(x, y+8, true);
        display_->DrawPixel(x+1, y+8, true);
        display_->DrawPixel(x, y+7, true);

        display_->DrawPixel(x+40, y+8, true);
        display_->DrawPixel(x+39, y+8, true);
        display_->DrawPixel(x+40, y+7, true);

        if (sequencer->stepEdit_) {
            int xOffset = 0;
            if (step->paramEdit == Step::i) xOffset = 5;
            else if (step->paramEdit == Step::n) xOffset = 17;
            else if (step->paramEdit == Step::f) xOffset = 29;
            else if (step->paramEdit == Step::fa) xOffset = 35;

            display_->DrawPixel(x + xOffset, y - 1, true);
            display_->DrawPixel(x + xOffset + 1, y, true);
            display_->DrawPixel(x + xOffset - 1, y, true);

            display_->DrawPixel(x + xOffset, y + 9, true);
            display_->DrawPixel(x + xOffset + 1, y + 8, true);
            display_->DrawPixel(x + xOffset - 1, y + 8, true);
        }
    }

}

void Display::DrawSquare(int x, int y, bool fill) {
    if (fill) {
        for (int i = x; i < x + 7; i++) {
            for (int j = y; j < y + 7; j++) {
                display_->DrawPixel(i, j, true);
            }
        }
    } else {
        display_->DrawPixel(x, y, true);
        display_->DrawPixel(x+1, y, true);
        display_->DrawPixel(x, y+1, true);

        display_->DrawPixel(x+6, y, true);
        display_->DrawPixel(x+5, y, true);
        display_->DrawPixel(x+6, y+1, true);

        display_->DrawPixel(x, y+6, true);
        display_->DrawPixel(x+1, y+6, true);
        display_->DrawPixel(x, y+5, true);

        display_->DrawPixel(x+6, y+6, true);
        display_->DrawPixel(x+5, y+6, true);
        display_->DrawPixel(x+6, y+5, true);
    }
}

void Display::DrawArrow(int x, int y, int direction) {
    for (int i = 0; i < 3; i++) {
        if (direction == 1) {
            display_->DrawPixel(x + i, y + i, true);
            display_->DrawPixel(x + i, y - i, true);
        } else if (direction == 2) {
            display_->DrawPixel(x - i, y + i, true);
            display_->DrawPixel(x - i, y - i, true);
        } else if (direction == 3) {
            display_->DrawPixel(x + i, y + i, true);
            display_->DrawPixel(x - i, y + i, true);
        } else if (direction == 4) {
            display_->DrawPixel(x + i, y - i, true);
            display_->DrawPixel(x - i, y - i, true);
        }
    }
}

void Display::GetFxString(char* strbuff, int fx, int fxAmount) {
    if (fx == 0) sprintf(strbuff, "---");
    else {
        if (fx == 1) sprintf(strbuff, "R");
        else if (fx == 2) sprintf(strbuff, "U");
        else if (fx == 3) sprintf(strbuff, "D");
        else if (fx == 4) sprintf(strbuff, "I");
        else if (fx == 5) sprintf(strbuff, "O");
        else sprintf(strbuff, "F");

        if (fxAmount < 10) sprintf(strbuff + strlen(strbuff), "-%d", fxAmount);
        else sprintf(strbuff + strlen(strbuff), "%d", fxAmount);
    }
}

void Display::GetNoteString(char* strbuff, int note) {
    int root = note % 12;
    int octave = (note - root) / 12;
    switch (root) {
        case 0: sprintf(strbuff, "C-%d", octave); break;
        case 1: sprintf(strbuff, "Db%d", octave); break;
        case 2: sprintf(strbuff, "D-%d", octave); break;
        case 3: sprintf(strbuff, "Eb%d", octave); break;
        case 4: sprintf(strbuff, "E-%d", octave); break;
        case 5: sprintf(strbuff, "F-%d", octave); break;
        case 6: sprintf(strbuff, "Gb%d", octave); break;
        case 7: sprintf(strbuff, "G-%d", octave); break;
        case 8: sprintf(strbuff, "Ab%d", octave); break;
        case 9: sprintf(strbuff, "A-%d", octave); break;
        case 10: sprintf(strbuff, "Bb%d", octave); break;
        case 11: sprintf(strbuff, "B-%d", octave); break;
        default: sprintf(strbuff, "OFF"); break;
    }
}

void Display::UpdateDisplay() {
    #if SCREEN_ON
    // Clear
    display_->Fill(false);

    if (sequencer->songOrder[sequencer->currentPattern] > -1) {
        // Drawing Steps
        sequencer->currentStep->selected = true;
        auto laneIt = sequencer->activePattern->lanes.begin();
        std::advance(laneIt, sequencer->laneOffset);
        for (int x = 0; x < 2; x++) {
            auto stepIt = (*laneIt)->sequence.begin();
            std::advance(stepIt, sequencer->currentStep->index);
            for (int y = 0; y < 7; y++) {
                auto offset = stepIt + (y - 3);
                if (offset < (*laneIt)->sequence.begin()) {
                    continue;
                }
                else if (offset >= (*laneIt)->sequence.end()) {
                    continue;
                }
                else {
                    DrawStep(((43 * x) + 14), (9 * y), *offset);
                    if ((*offset)->index % 4 == 0 && x == 0) {
                        display_->SetCursor(0, ((9 * y) + 2));
                        sprintf(strbuff, "%3d", (*offset)->index);
                        display_->WriteString(strbuff, Font_4x6, true);
                    }
                }
            }
            std::advance(laneIt, 1);
        } 
    }
    
    //Drawing Sidebar
    display_->SetCursor(101, 1);
    sprintf(strbuff, "BPM");
    display_->WriteString(strbuff, Font_4x6, true);

    display_->SetCursor(114, 1);
    sprintf(strbuff, "%3d", (int) sequencer->bpm);
    display_->WriteString(strbuff, Font_4x6, true);

    display_->SetCursor(101, 8);
    sprintf(strbuff, "LANE");
    display_->WriteString(strbuff, Font_4x6, true);

    display_->SetCursor(118, 8);
    if (sequencer->songOrder[sequencer->currentPattern] > -1) sprintf(strbuff, "%2d", sequencer->currentLane->index + 1);
    else sprintf(strbuff, " ");
    display_->WriteString(strbuff, Font_4x6, true);

    display_->SetCursor(101, 15);
    sprintf(strbuff, "LEN");
    display_->WriteString(strbuff, Font_4x6, true);

    display_->SetCursor(114, 15);
    if (sequencer->songOrder[sequencer->currentPattern] > -1) sprintf(strbuff, "%3d", sequencer->currentLane->length);
    else sprintf(strbuff, " ");
    display_->WriteString(strbuff, Font_4x6, true);

    // Drawing Pattern Arrangement
    for (int i = 0; i < 5; i++) {
        int offset = sequencer->currentPattern + (i - 2);
        if (offset < 0)                                 continue; 
        else if (offset > (int) sequencer->songOrder.size() - 1)   continue;
        else if (sequencer->songOrder[offset] < 0)                 continue;
        else {
            display_->SetCursor(114, (24 + (8 * i)));
            sprintf(strbuff, "%d", sequencer->songOrder[offset]);
            if (offset == sequencer->currentPattern) {
                DrawSquare(112, 23 + (8 * i), true);
                display_->WriteString(strbuff, Font_4x6, false);
            }
            else {
                DrawSquare(112, 23 + (8 * i), false);
                display_->WriteString(strbuff, Font_4x6, true);
            }
        }
    }

    DrawArrow(122, 42, 2);
    DrawArrow(108, 42, 1);
    DrawArrow(103, 38, 3);
    DrawArrow(103, 46, 4);

    display_->Update();

    #endif
}