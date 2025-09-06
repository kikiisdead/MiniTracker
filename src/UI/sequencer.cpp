#include "sequencer.h"

void Sequencer::NewPattern() {
    patterns.push_back(new Pattern);
    patterns.back()->index = patterns.size() - 1;
    
    songOrder[currentPattern] = patterns.back()->index;

    activePattern = patterns.at(songOrder[currentPattern]);
    activePattern->numLanes = 4;

    for (int i = 0; i < activePattern->numLanes; i ++) {
        activePattern->lanes.push_back(new Lane);
        currentLane = activePattern->lanes[i];
        currentLane->length = 4;
        currentLane->index = i;
        for (int j = 0; j < activePattern->lanes[i]->length; j++) {
            currentLane->sequence.push_back(new Step);
            InitStep(currentLane->sequence.at(j), j);
        }
    }

    currentLane = activePattern->lanes[0];
    currentStep = currentLane->sequence.at(0);
    activePattern->lanes[0]->sequence.at(0)->instrument = 0;
    activePattern->lanes[0]->sequence.at(2)->instrument = -2;
}

void Sequencer::InitStep(Step* step, int index) {
    step->note = 48;
    step->instrument = -1;
    step->fx = 0;
    step->fxAmount = 0;
    step->selected = false;
    step->paramEdit = Step::i;
    step->index = index;
}

void Sequencer::DrawStep(int x, int y, Step* step) {
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
    #if SCREEN_ON
    // Clear
    display_->Fill(false);

    if (songOrder[currentPattern] > -1) {
        // Drawing Steps
        currentStep->selected = true;
        auto laneIt = activePattern->lanes.begin();
        std::advance(laneIt, laneOffset);
        for (int x = 0; x < 2; x++) {
            auto stepIt = (*laneIt)->sequence.begin();
            std::advance(stepIt, currentStep->index);
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
    sprintf(strbuff, "%3d", (int) bpm);
    display_->WriteString(strbuff, Font_4x6, true);

    display_->SetCursor(101, 8);
    sprintf(strbuff, "LANE");
    display_->WriteString(strbuff, Font_4x6, true);

    display_->SetCursor(118, 8);
    if (songOrder[currentPattern] > -1) sprintf(strbuff, "%2d", currentLane->index + 1);
    else sprintf(strbuff, " ");
    display_->WriteString(strbuff, Font_4x6, true);

    display_->SetCursor(101, 15);
    sprintf(strbuff, "LEN");
    display_->WriteString(strbuff, Font_4x6, true);

    display_->SetCursor(114, 15);
    if (songOrder[currentPattern] > -1) sprintf(strbuff, "%3d", currentLane->length);
    else sprintf(strbuff, " ");
    display_->WriteString(strbuff, Font_4x6, true);

    // Drawing Pattern Arrangement
    for (int i = 0; i < 5; i++) {
        int offset = currentPattern + (i - 2);
        if (offset < 0)                                 continue; 
        else if (offset > (int) songOrder.size() - 1)   continue;
        else if (songOrder[offset] < 0)                 continue;
        else {
            display_->SetCursor(114, (24 + (8 * i)));
            sprintf(strbuff, "%d", songOrder[offset]);
            if (offset == currentPattern) {
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

void Sequencer::Update() {
    if (playing_) {
        if (tick.Process()) {
            //lastTrigger = System::GetNow();
            NextStep();
            for (Lane* lane : activePattern->lanes) {
                handler_[lane->index]->Update(lane->sequence.at(currentStep->index)); 
            }
        }
    }
}

void Sequencer::PlayButton() {
    playing_ = (playing_) ? false : true;
    song_ = false;
    if (playing_) {
        lastTrigger = System::GetNow();
        for (Lane* lane : activePattern->lanes) {
            handler_[lane->index]->Update(lane->sequence.at(currentStep->index)); 
        }
    }
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
    currentStep->selected = false;
    if (currentStep->index > 0) {
        currentStep = currentLane->sequence.at(currentStep->index - 1);
    } else {
        currentStep = currentLane->sequence.at(currentLane->length - 1);
    }
    UpdateDisplay();
}

void Sequencer::UpButton() {
    if (!playing_ && !stepEdit_ && songOrder[currentPattern] > -1) PreviousStep();
    else if (stepEdit_ && songOrder[currentPattern] > -1) currentStep->Increment();
    UpdateDisplay();
}

void Sequencer::NextStep() {
    currentStep->selected = false;
    if (currentStep->index < (currentLane->length - 1)) {
        currentStep = currentLane->sequence.at(currentStep->index + 1);
    } else {
        if (song_) {
            NextPattern();
        }
        currentStep = currentLane->sequence.at(0);
    }
    UpdateDisplay();
}

void Sequencer::DownButton() {
    if (!playing_ && !stepEdit_ && songOrder[currentPattern] > -1) NextStep();
    else if (stepEdit_ && songOrder[currentPattern] > -1) currentStep->Decrement();
    UpdateDisplay();
}

void Sequencer::PreviousLane() {
    currentStep->selected = false;
    if (currentLane->index > 0) {
        currentLane = activePattern->lanes.at(currentLane->index - 1);
        currentStep = currentLane->sequence.at(currentStep->index);
        if (currentLane->index < 2) laneOffset = currentLane->index;
    } 
    UpdateDisplay();
}

void Sequencer::LeftButton() {
    if (!stepEdit_ && songOrder[currentPattern] > -1) PreviousLane();
    else if (songOrder[currentPattern] > -1) currentStep->PrevParam();
    UpdateDisplay();
}

void Sequencer::NextLane() {
    currentStep->selected = false;
    if (currentLane->index < (activePattern->numLanes - 1)) {
        currentLane = activePattern->lanes.at(currentLane->index + 1);
        currentStep = currentLane->sequence.at(currentStep->index);
        if (currentLane->index > 1) laneOffset = currentLane->index - 1;
    }
    UpdateDisplay();
}

void Sequencer::RightButton() {
    if (!stepEdit_ && songOrder[currentPattern] > -1) NextLane();
    else if (songOrder[currentPattern] > -1) currentStep->NextParam();
    UpdateDisplay();
}

void Sequencer::NextPattern() {
    currentStep->selected = false;
    currentPattern = (currentPattern >= (int) songOrder.size() - 1) ? 0 : currentPattern + 1;
    activePattern = patterns.at(songOrder[currentPattern]);
    currentStep->selected = true;
    UpdateDisplay();
}

void Sequencer::PreviousPattern() {
    currentStep->selected = false;
    currentPattern = currentPattern + 1;
    if (currentPattern >= (int) songOrder.size()) {
        playing_ = false;
    } else {
        activePattern = patterns.at(songOrder[currentPattern]);
        currentStep->selected = true;
    }
    UpdateDisplay();
}

void Sequencer::AltUpButton() {
    currentStep->selected = false; //deselecting step

    // if the index of pattern were moving away from is -1, remove from songOrder
    if (songOrder[currentPattern] == -1) {
        auto it = songOrder.begin();
        std::advance(it, currentPattern);
        songOrder.erase(it);
    } 
    
    currentPattern -= 1; // decrement index of pattern

    if (currentPattern < 0) {
        songOrder.insert(songOrder.begin(), -1);
        currentPattern = 0; // need to reset it to 0 to shift everything down
    }

    if (songOrder[currentPattern] > -1) {
        activePattern = patterns.at(songOrder[currentPattern]);
        currentLane = activePattern->lanes[0];
        currentStep = currentLane->sequence.at(0);
    }
    UpdateDisplay();
}

void Sequencer::AltDownButton() {
    currentStep->selected = false;
    if (songOrder[currentPattern] == -1) {
        auto it = songOrder.begin();
        std::advance(it, currentPattern);
        songOrder.erase(it);
    } else {
        currentPattern += 1;
    }
    
    if (currentPattern >= (int) songOrder.size()) songOrder.push_back(-1); 
    else { //not seting active pattern to anything to avoid index out of range error
        activePattern = patterns.at(songOrder[currentPattern]);
        currentLane = activePattern->lanes[0];
        currentStep = currentLane->sequence.at(0);
    }
    UpdateDisplay();
}

void Sequencer::AltLeftButton() {
    currentStep->selected = false;
    if (songOrder[currentPattern] < 0) songOrder[currentPattern] = -1;
    else songOrder[currentPattern] -= 1;
    if (songOrder[currentPattern] > -1) {
        activePattern = patterns.at(songOrder[currentPattern]);
        currentLane = activePattern->lanes[0];
        currentStep = currentLane->sequence.at(0);
    }
    UpdateDisplay();
}

void Sequencer::AltRightButton() {
    currentStep->selected = false;
    songOrder[currentPattern] += 1;
    if (songOrder[currentPattern] > ((int) patterns.size() - 1)) NewPattern();
    activePattern = patterns.at(songOrder[currentPattern]);
    currentLane = activePattern->lanes[0];
    currentStep = currentLane->sequence.at(0);
    UpdateDisplay();
}

void Sequencer::AltPlayButton() {
    playing_ = (playing_) ? false : true;
    song_ = (song_) ? false : true;
}

void Sequencer::AltAButton() {
    for (Lane* lane : activePattern->lanes) {
        lane->length += 1;
        lane->sequence.push_back(new Step);
        InitStep(lane->sequence.back(), lane->length - 1);
    }
    UpdateDisplay();
}

void Sequencer::AltBButton() {
    if (currentLane->length <= 1) return;
    if (currentStep->index >= currentLane->length - 1) currentStep = currentLane->sequence.at(currentStep->index - 1);
    for (Lane* lane : activePattern->lanes) {
        lane->length -= 1;
        lane->sequence.erase(--lane->sequence.end()); //get rid of last
    }
    UpdateDisplay();
}