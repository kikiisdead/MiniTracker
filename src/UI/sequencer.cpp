#include "sequencer.h"

void Sequencer::NewPattern() {
    patterns.push_back(new Pattern);
    patterns.back()->index = patterns.size() - 1;
    
    songOrder[selector[0]] = patterns.back()->index;

    activePattern = patterns.at(songOrder[selector[0]]);
    activePattern->numLanes = 4;

    for (int i = 0; i < activePattern->numLanes; i ++) {
        activePattern->lanes[i] = new Lane;
        activePattern->lanes[i]->length = 32;
        for (int j = 0; j < activePattern->lanes[i]->length; j++) {
            activePattern->lanes[i]->sequence.push_back(new Step);
            InitStep(activePattern->lanes[i]->sequence.at(j));
        }
    }
}

void Sequencer::InitStep(Step* step) {
    step->note = 48;
    step->instrument = -1;
    step->fx = 0;
    step->fxAmount = 0;
    step->selected = false;
    step->paramEdit = Step::i;
}

void Sequencer::DrawStep(int x, int y, Step* step) {
    // Writing instrument
    display_->SetCursor(x + 2, y + 2);
    if (step->instrument < 0 || step->note < 0) sprintf(strbuff, "--");
    else if (step->instrument < 10) sprintf(strbuff, "0%d", step->instrument);
    else sprintf(strbuff, "%d", step->instrument);
    display_->WriteString(strbuff, Font_4x6, true);

    // Writing note
    display_->SetCursor(x + 13, y + 2);
    if (step->instrument < 0) sprintf(strbuff, "---");
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

        if (stepEdit_) {
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

void Sequencer::DrawSquare(int x, int y, bool fill) {
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

void Sequencer::DrawArrow(int x, int y, int direction) {
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

void Sequencer::GetFxString(char* strbuff, int fx, int fxAmount) {
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

void Sequencer::GetNoteString(char* strbuff, int note) {
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

void Sequencer::UpdateDisplay() {
    // Clear
    display_->Fill(false);

    if (song_) display_->DrawPixel(0, 0, true);

    if (songOrder[selector[0]] > -1) {
        // Drawing Steps
        activePattern->lanes[selector[1]]->sequence.at(selector[2])->selected = true;
        for (int x = 0; x < 2; x++) {
            for (int y = 0; y < 7; y++) {
                int offset = selector[2] - (3 - y);
                if (offset < 0) {
                    continue;
                }
                else if (offset > activePattern->lanes[laneOffset + x]->length - 1) {
                    continue;
                }
                else {
                    DrawStep(((43 * x) + 14), (9 * y), activePattern->lanes[laneOffset + x]->sequence.at(offset));
                }
            }
        } 

        // Drawing Line Numbers
        for (int i = 0; i < 7; i++) {
            int offset = selector[2] - (3 - i);
            if (offset < 0) {
                continue;
            }
            else if (offset > activePattern->lanes[0]->length - 1) {
                continue;
            }
            else if (offset % 4 == 0) {
                display_->SetCursor(0, ((9 * i) + 2));
                sprintf(strbuff, "%3d", offset);
                display_->WriteString(strbuff, Font_4x6, true);
            }
        }
    }
    
    //Drawing Sidebar
    display_->SetCursor(101, 1);
    sprintf(strbuff, "BPM");
    display_->WriteString(strbuff, Font_4x6, true);

    display_->SetCursor(114, 1);
    sprintf(strbuff, "%3d", (int) bpm);
    display_->WriteString(strbuff, Font_4x6, true);

    display_->SetCursor(101, 8);
    sprintf(strbuff, "LANE");
    display_->WriteString(strbuff, Font_4x6, true);

    display_->SetCursor(118, 8);
    if (songOrder[selector[0]] > -1) sprintf(strbuff, "%2d", selector[1] + 1);
    else sprintf(strbuff, " ");
    display_->WriteString(strbuff, Font_4x6, true);

    display_->SetCursor(101, 15);
    sprintf(strbuff, "LEN");
    display_->WriteString(strbuff, Font_4x6, true);

    display_->SetCursor(114, 15);
    if (songOrder[selector[0]] > -1) sprintf(strbuff, "%3d", activePattern->lanes[0]->length);
    else sprintf(strbuff, " ");
    display_->WriteString(strbuff, Font_4x6, true);

    // Drawing Pattern Arrangement
    if (selector[0] - 2 >= 0) DrawSquare(112, 23, false);
    if (selector[0] - 1 >= 0) DrawSquare(112, 31, false);
    DrawSquare(112, 39, true);
    DrawSquare(112, 47, false);
    DrawSquare(112, 55, false);

    for (int i = 0; i < 5; i++) {
        int offset = selector[0] + (i - 2);
        if (offset < 0)                         continue; 
        else if (offset > (int) songOrder.size() - 1) continue;
        else if (songOrder[offset] < 0)         continue;
        else {
            display_->SetCursor(114, (24 + (8 * i)));
            sprintf(strbuff, "%d", songOrder[offset]);
            if (offset == selector[0]) display_->WriteString(strbuff, Font_4x6, false);
            else display_->WriteString(strbuff, Font_4x6, true);
        }
    }

    DrawArrow(122, 42, 2);
    DrawArrow(108, 42, 1);
    DrawArrow(103, 38, 3);
    DrawArrow(103, 46, 4);

    display_->Update();
}

void Sequencer::Update() {
    if (playing_) {
        if (tick.Process()) {
            NextStep();
            for (int i = 0; i < 4; i++) {
                handler_[i].Update(activePattern->lanes[i]->sequence.at(selector[2]));
            }
        }
    }
}

void Sequencer::PlayButton() {
    playing_ = (playing_) ? false : true;
    song_ = false;
}

void Sequencer::AButton() {
    stepEdit_ = true;
    UpdateDisplay();   
}

void Sequencer::BButton() {
    stepEdit_ = false;
    UpdateDisplay();
}

void Sequencer::PreviousStep() {
    activePattern->lanes[selector[1]]->sequence.at(selector[2])->selected = false;
    selector[2] = (selector[2] <= 0) ? activePattern->lanes[1]->length - 1 : selector[2] - 1;
    UpdateDisplay();
}

void Sequencer::UpButton() {
    if (!playing_ && !stepEdit_ && songOrder[selector[0]] > -1) PreviousStep();
    else if (stepEdit_ && songOrder[selector[0]] > -1) activePattern->lanes[selector[1]]->sequence.at(selector[2])->Increment();
    UpdateDisplay();
}

void Sequencer::NextStep() {
    activePattern->lanes[selector[1]]->sequence.at(selector[2])->selected = false;
    if (selector[2] >= activePattern->lanes[selector[1]]->length - 1) {
        selector[2] = 0;
        if (song_) {
            NextPattern();
        }
    } else {
        selector[2] += 1;
    }
    UpdateDisplay();
}

void Sequencer::DownButton() {
    if (!playing_ && !stepEdit_ && songOrder[selector[0]] > -1) NextStep();
    else if (stepEdit_ && songOrder[selector[0]] > -1) activePattern->lanes[selector[1]]->sequence.at(selector[2])->Decrement();
    UpdateDisplay();
}

void Sequencer::PreviousLane() {
    activePattern->lanes[selector[1]]->sequence.at(selector[2])->selected = false;
    selector[1] = (selector[1] <= 0) ? 0 : selector[1] - 1;
    if (selector[1] < 2) laneOffset = selector[1];
    UpdateDisplay();
}

void Sequencer::LeftButton() {
    if (!stepEdit_ && songOrder[selector[0]] > -1) PreviousLane();
    else if (songOrder[selector[0]] > -1) activePattern->lanes[selector[1]]->sequence.at(selector[2])->PrevParam();
    UpdateDisplay();
}

void Sequencer::NextLane() {
    activePattern->lanes[selector[1]]->sequence.at(selector[2])->selected = false;
    selector[1] = (selector[1] >= activePattern->numLanes - 1) ? activePattern->numLanes - 1 : selector[1] + 1;
    if (selector[1] > 1) laneOffset = selector[1] - 1;
    UpdateDisplay();
}

void Sequencer::RightButton() {
    if (!stepEdit_ && songOrder[selector[0]] > -1) NextLane();
    else if (songOrder[selector[0]] > -1) activePattern->lanes[selector[1]]->sequence.at(selector[2])->NextParam();
    UpdateDisplay();
}

void Sequencer::NextPattern() {
    activePattern->lanes[selector[1]]->sequence.at(selector[2])->selected = false;
    selector[0] = (selector[0] >= (int) songOrder.size() - 1) ? 0 : selector[0] + 1;
    activePattern = patterns.at(songOrder[selector[0]]);
    activePattern->lanes[selector[1]]->sequence.at(selector[2])->selected = true;
    UpdateDisplay();
}

void Sequencer::PreviousPattern() {
    activePattern->lanes[selector[1]]->sequence.at(selector[2])->selected = false;
    selector[0] = selector[0] + 1;
    if (selector[0] >= (int) songOrder.size()) {
        playing_ = false;
    } else {
        activePattern = patterns.at(songOrder[selector[0]]);
        activePattern->lanes[selector[1]]->sequence.at(selector[2])->selected = true;
    }
    UpdateDisplay();
}

void Sequencer::AltUpButton() {
    activePattern->lanes[selector[1]]->sequence.at(selector[2])->selected = false; //deselecting step
    // if the index of pattern were moving away from is -1, remove from songOrder
    if (songOrder[selector[0]] == -1) {
        auto it = songOrder.begin();
        std::advance(it, selector[0]);
        songOrder.erase(it);
    } else {
        selector[0] = (selector[0] <= 0) ? 0 : selector[0] - 1; // advance selector
    }

    if (songOrder[selector[0]] > -1) {
        activePattern = patterns.at(songOrder[selector[0]]);
        activePattern->lanes[selector[1]]->sequence.at(selector[2])->selected = true;
    }
    UpdateDisplay();
}

void Sequencer::AltDownButton() {
    activePattern->lanes[selector[1]]->sequence.at(selector[2])->selected = false;
    if (songOrder[selector[0]] == -1) {
        auto it = songOrder.begin();
        std::advance(it, selector[0]);
        songOrder.erase(it);
    } else {
        selector[0] += 1;
    }
    
    if (selector[0] >= (int) songOrder.size()) songOrder.push_back(-1); 
    else { //not seting active pattern to anything to avoid index out of range error
        activePattern = patterns.at(songOrder[selector[0]]);
        activePattern->lanes[selector[1]]->sequence.at(selector[2])->selected = true;
    }
    UpdateDisplay();
}

void Sequencer::AltLeftButton() {
    activePattern->lanes[selector[1]]->sequence.at(selector[2])->selected = false;
    if (songOrder[selector[0]] < 0) songOrder[selector[0]] = -1;
    else songOrder[selector[0]] -= 1;
    if (songOrder[selector[0]] > -1) {
        activePattern = patterns.at(songOrder[selector[0]]);
        activePattern->lanes[selector[1]]->sequence.at(selector[2])->selected = true;
    }
    UpdateDisplay();
}

void Sequencer::AltRightButton() {
    activePattern->lanes[selector[1]]->sequence.at(selector[2])->selected = false;
    songOrder[selector[0]] += 1;
    if (songOrder[selector[0]] > ((int) patterns.size() - 1)) NewPattern();
    activePattern = patterns.at(songOrder[selector[0]]);
    activePattern->lanes[selector[1]]->sequence.at(selector[2])->selected = true;
    UpdateDisplay();
}

void Sequencer::AltPlayButton() {
    playing_ = (playing_) ? false : true;
    song_ = (song_) ? false : true;
}