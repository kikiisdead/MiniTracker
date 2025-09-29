#include "sequencer.h"

void Sequencer::NewPattern() {
    patterns->push_back(new Pattern);

    patterns->back()->index = patterns->size() - 1;
    patterns->back()->numLanes = handler_->size();
    
    songOrder->at(currentPattern) = patterns->back()->index;

    activePattern = patterns->at(songOrder->at(currentPattern));

    for (int i = 0; i < activePattern->numLanes; i ++) {
        activePattern->lanes.push_back(new Lane);
        currentLane = activePattern->lanes[i];
        currentLane->length = 64;
        currentLane->index = i;
        for (int j = 0; j < activePattern->lanes[i]->length; j++) {
            currentLane->sequence.push_back(new Step(j));
        }
    }

    currentLane = activePattern->lanes[0];
    currentStep = currentLane->sequence.at(0);
}

void Sequencer::Clear() {
    patterns->clear();
    songOrder->clear();
    playing_ = false;
    stepEdit_ = false;
    song_ = false;
    currentPattern = 0;
    lastTrigger = 0;
    laneOffset = 0;
    updateStep = true;
    updateSidebar = true;
    updatePattern = true;
    currentLane = nullptr;
    currentStep = nullptr;
    activePattern = nullptr;
}

void Sequencer::Safe() {

    playing_ = false;
    stepEdit_ = false;
    song_ = false;
    currentPattern = 0;
    lastTrigger = 0;
    laneOffset = 0;
    updateStep = true;
    updateSidebar = true;
    updatePattern = true;

    activePattern = patterns->at(songOrder->at(currentPattern));
    currentLane = activePattern->lanes[0];
    currentStep = currentLane->sequence.at(0);
}

void Sequencer::DrawStep(cLayer* display, int x, int y, Step* step) {

    // Drawing select things
    if (step->selected) {
        if (stepEdit_) {
            if      (step->paramEdit == Step::i) display->drawFillRect(x, y, 4 + CHAR_WIDTH * 2, CHAR_HEIGHT + 8, ACCENT1);
            else if (step->paramEdit == Step::n) display->drawFillRect(x + (CHAR_WIDTH * 3), y, 4 + CHAR_WIDTH * 3, CHAR_HEIGHT + 8, ACCENT1);
            else if (step->paramEdit == Step::f) display->drawFillRect(x + (CHAR_WIDTH * 7), y, 4 + CHAR_WIDTH, CHAR_HEIGHT + 8, ACCENT1);
            else if (step->paramEdit == Step::fa) display->drawFillRect(x+ (CHAR_WIDTH * 8), y, 4 + CHAR_WIDTH * 2, CHAR_HEIGHT + 8, ACCENT1);
        }

        display->drawFillRect(x, y, 5, 1, MAIN);
        display->drawFillRect(x, y, 1, 5, MAIN);

        display->drawFillRect(x + (10 * CHAR_WIDTH) - 1, y, 5, 1, MAIN);
        display->drawFillRect(x + (10 * CHAR_WIDTH) + 3, y, 1, 5, MAIN);

        display->drawFillRect(x, y + CHAR_HEIGHT + 6, 5, 1, MAIN);
        display->drawFillRect(x, y + CHAR_HEIGHT + 2, 1, 5, MAIN);

        display->drawFillRect(x + (10 * CHAR_WIDTH) - 1, y + CHAR_HEIGHT + 6, 5, 1, MAIN);
        display->drawFillRect(x + (10 * CHAR_WIDTH) + 3, y + CHAR_HEIGHT + 2, 1, 5, MAIN);  
    }
    
    // Writing instrument
    if (step->instrument < 0) sprintf(strbuff, "--");
    else if (step->instrument < 10) sprintf(strbuff, "0%d", step->instrument);
    else sprintf(strbuff, "%d", step->instrument);
    if (step->selected && stepEdit_ && step->paramEdit == Step::i) WriteString(display, strbuff, x + 2, y + 2 + CHAR_HEIGHT, BACKGROUND);
    else WriteString(display, strbuff, x + 2, y + 2 + CHAR_HEIGHT, MAIN);

    // Writing note
    if (step->instrument == -1) sprintf(strbuff, "---");
    else if (step->instrument == -2) sprintf(strbuff, "OFF");
    else GetNoteString(strbuff, step->note);
    if (step->selected && stepEdit_ && step->paramEdit == Step::n) WriteString(display, strbuff, x + (CHAR_WIDTH * 3) + 2, y + 2 + CHAR_HEIGHT, BACKGROUND);
    else WriteString(display, strbuff, x + (CHAR_WIDTH * 3) + 2, y + 2 + CHAR_HEIGHT, MAIN);

    // Writing fx
    if (step->instrument < 0) sprintf(strbuff, "-");
    else GetFxString(strbuff, step->fx);
    if (step->selected && stepEdit_ && step->paramEdit == Step::f) WriteString(display, strbuff, x + (CHAR_WIDTH * 7) + 2, y + 2 + CHAR_HEIGHT, BACKGROUND);
    else WriteString(display, strbuff, x + (CHAR_WIDTH * 7) + 2, y + 2 + CHAR_HEIGHT, MAIN);

     // Writing fx amount
    if (step->instrument < 0 || step->fx <= 0) sprintf(strbuff, "--");
    else if (step->fxAmount < 10) sprintf(strbuff, "0%d", step->fxAmount);
    else sprintf(strbuff, "%d", step->fxAmount);
    if (step->selected && stepEdit_ && step->paramEdit == Step::fa) WriteString(display, strbuff, x + (CHAR_WIDTH * 8) + 2, y + 2 + CHAR_HEIGHT, BACKGROUND);
    else WriteString(display, strbuff, x + (CHAR_WIDTH * 8) + 2, y + 2 + CHAR_HEIGHT, MAIN);

}

void Sequencer::DrawSquare(cLayer *display, int index, int x, int y, bool fill) {
    sprintf(strbuff, "%d", index);
    if (fill) {
        display->drawFillRect(x, y, 22, 22, ACCENT2);
        if (index < 10) WriteString(display, strbuff, x + (22 - CHAR_WIDTH) / 2, y + CHAR_HEIGHT + (22 - CHAR_HEIGHT) / 2, MAIN);
        else WriteString(display, strbuff, x + (22 - (CHAR_WIDTH * 2)) / 2, y + CHAR_HEIGHT + (22 - CHAR_HEIGHT) / 2, MAIN);
    } else {
        display->drawFillRect(x, y, 22, 22, ALTACCENT2);
        if (index < 10) WriteString(display, strbuff, x + (22 - CHAR_WIDTH) / 2, y + CHAR_HEIGHT + (22 - CHAR_HEIGHT) / 2, ALTMAIN);
        else WriteString(display, strbuff, x + (22 - (CHAR_WIDTH * 2)) / 2, y + CHAR_HEIGHT + (22 - CHAR_HEIGHT) / 2, ALTMAIN);
    }
}

void Sequencer::DrawArrow(cLayer *display, int x, int y, int direction) {
    if (direction == 1) {
        display->drawLine(x, y, x + 5, y + 5, MAIN);
        display->drawLine(x, y, x + 5, y - 5, MAIN);
    } else if (direction == 2) {
        display->drawLine(x, y, x - 5, y + 5, MAIN);
        display->drawLine(x, y, x - 5, y - 5, MAIN);
    } else if (direction == 3) {
        display->drawLine(x, y, x + 5, y + 5, MAIN);
        display->drawLine(x, y, x - 5, y + 5, MAIN);
    } else if (direction == 4) {
        display->drawLine(x, y, x + 5, y - 5, MAIN);
        display->drawLine(x, y, x - 5, y - 5, MAIN);
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

void Sequencer::GetFxString(char* strbuff, int fx) {
    if (fx == 0) sprintf(strbuff, "-");
    else {
        if (fx == 1) sprintf(strbuff, "R");
        else if (fx == 2) sprintf(strbuff, "U");
        else if (fx == 3) sprintf(strbuff, "D");
        else if (fx == 4) sprintf(strbuff, "I");
        else if (fx == 5) sprintf(strbuff, "O");
        else sprintf(strbuff, "F");
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

void Sequencer::WriteString(cLayer* display, char* strbuff, int x, int y, DadGFX::sColor color) {
    display->setCursor(x, y);
    display->setFont(MainFont);
    display->setTextFrontColor(color);
    display->drawText(strbuff);
}

void Sequencer::UpdateDisplay(cLayer *display) {
    /**
     * CLEAR
     */
    display->eraseLayer();



    /**
     * Drawing Steps
     */
    if (songOrder->at(currentPattern) > -1) {
        // clear previous stuff
        display->drawFillRect(0, 0, (28 * 9), 240, BACKGROUND);

        currentStep->selected = true;
        auto laneIt = activePattern->lanes.begin();
        std::advance(laneIt, laneOffset);
        for (int x = 0; x < 2; x++) {
            auto stepIt = (*laneIt)->sequence.begin();
            std::advance(stepIt, currentStep->index);
            for (int y = 0; y < 12; y++) {
                auto offset = stepIt + (y - 5);
                if (offset < (*laneIt)->sequence.begin()) {
                    continue;
                }
                else if (offset >= (*laneIt)->sequence.end()) {
                    continue;
                }
                else {
                    DrawStep(display, (((CHAR_WIDTH * 12) * x) + (CHAR_WIDTH * 4)), ((CHAR_HEIGHT + 8) * y), *offset);
                    if ((*offset)->index % 4 == 0 && x == 0) {
                        sprintf(strbuff, "%3d", (*offset)->index);
                        if ((*offset)->index % 16 == 0) WriteString(display, strbuff, 0, ((CHAR_HEIGHT + 8) * (y + 1) - (CHAR_HEIGHT / 2)), ACCENT2);
                        else WriteString(display, strbuff, 0, ((CHAR_HEIGHT + 8) * (y + 1) - (CHAR_HEIGHT / 2)), ACCENT1);
                    }
                }
            }
            std::advance(laneIt, 1);
        } 
    }
    
    /**
     * Drawing Sidebar
     */
    display->drawFillRect(28*9, 0, 100, (CHAR_HEIGHT + 4) * 3 + 4, BACKGROUND);

    sprintf(strbuff, "BPM %3d", (int) bpm);
    WriteString(display, strbuff, 320 - (CHAR_WIDTH * 7), CHAR_HEIGHT + 4, ACCENT1);

    
    if (songOrder->at(currentPattern) > -1) sprintf(strbuff, "LANE%3d", currentLane->index + 1);
    else sprintf(strbuff, "LANE");
    WriteString(display, strbuff, 320 - (CHAR_WIDTH * 7), (CHAR_HEIGHT + 4) * 2, ACCENT1);

    
    if (songOrder->at(currentPattern) > -1) sprintf(strbuff, "LEN%4d", currentLane->length);
    else sprintf(strbuff, "LEN");
    WriteString(display, strbuff, 320 - (CHAR_WIDTH * 7), (CHAR_HEIGHT + 4) * 3, ACCENT1);
    

    /**
     * Drawing Pattern arrangement
     */
    display->drawFillRect(28*9, (CHAR_HEIGHT + 4) * 3 + 4, 100, 240, BACKGROUND);

    auto it = songOrder->begin() + currentPattern;
    for (int i = 0; i < 7; i++) {
        auto offset = it + (i - 3);
        if (offset < songOrder->begin())     continue; 
        else if (offset >= songOrder->end()) continue;
        else if (*offset < 0)               continue;
        else {
            
            sprintf(strbuff, "%d", *offset);
            if (offset == songOrder->begin() + currentPattern) {
                DrawSquare(display, *offset, 320 - (CHAR_WIDTH * 4), (CHAR_HEIGHT + 4) * 4 + (28 * i), true);
            }
            else {
                DrawSquare(display, *offset, 320 - (CHAR_WIDTH * 4), (CHAR_HEIGHT + 4) * 4 + (28 * i), false);
            }
        }
    }

    DrawArrow(display, 320 - (CHAR_WIDTH * 4) + 7 + 24, (CHAR_HEIGHT + 4) * 4 + (28 * 3) + 11, 2);
    DrawArrow(display, 320 - (CHAR_WIDTH * 4) - 11, (CHAR_HEIGHT + 4) * 4 + (28 * 3) + 11, 1);
    DrawArrow(display, 320 - (CHAR_WIDTH * 7), (CHAR_HEIGHT + 4) * 4 + (28 * 3) - 9 + 11, 3);
    DrawArrow(display, 320 - (CHAR_WIDTH * 7), (CHAR_HEIGHT + 4) * 4 + (28 * 3) + 9 + 11, 4);
    
}

void Sequencer::Update() {
    if (playing_) {
        if (tick.Process()) {
            //lastTrigger = System::GetNow();
            NextStep();
            for (Lane* lane : activePattern->lanes) {
                handler_->at(lane->index)->Update(lane->sequence.at(currentStep->index)); 
            }
        }
    }
}

void Sequencer::PlayButton() {
    playing_ = (playing_) ? false : true;
    song_ = false;
    if (playing_) {
        for (Lane* lane : activePattern->lanes) {
            handler_->at(lane->index)->Update(lane->sequence.at(currentStep->index)); 
        }
    }
}

void Sequencer::AButton() {
    stepEdit_ = true;
    updateStep = true;
}

void Sequencer::BButton() {
    stepEdit_ = false;
    updateStep = true;
}

void Sequencer::PreviousStep() {
    currentStep->selected = false;
    if (currentStep->index > 0) {
        currentStep = currentLane->sequence.at(currentStep->index - 1);
    } else {
        currentStep = currentLane->sequence.at(currentLane->length - 1);
    }
    updateStep = true;
}

void Sequencer::UpButton() {
    if (!playing_ && !stepEdit_ && songOrder->at(currentPattern) > -1) PreviousStep();
    else if (stepEdit_ && songOrder->at(currentPattern) > -1) currentStep->Increment();
    updateStep = true;
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
    updateStep = true;
}

void Sequencer::DownButton() {
    if (!playing_ && !stepEdit_ && songOrder->at(currentPattern) > -1) NextStep();
    else if (stepEdit_ && songOrder->at(currentPattern) > -1) currentStep->Decrement();
    updateStep = true;
}

void Sequencer::PreviousLane() {
    currentStep->selected = false;
    if (currentLane->index > 0) {
        currentLane = activePattern->lanes.at(currentLane->index - 1);
        currentStep = currentLane->sequence.at(currentStep->index);
        if (currentLane->index < 2) laneOffset = currentLane->index;
    } 
    updateStep = true;
    updateSidebar = true;
}

void Sequencer::LeftButton() {
    if (!stepEdit_ && songOrder->at(currentPattern) > -1) PreviousLane();
    else if (songOrder->at(currentPattern) > -1) currentStep->PrevParam();
    updateStep = true;
}

void Sequencer::NextLane() {
    currentStep->selected = false;
    if (currentLane->index < (activePattern->numLanes - 1)) {
        currentLane = activePattern->lanes.at(currentLane->index + 1);
        currentStep = currentLane->sequence.at(currentStep->index);
        if (currentLane->index > 1) laneOffset = currentLane->index - 1;
    }
    updateStep = true;
    updateSidebar = true;
}

void Sequencer::RightButton() {
    if (!stepEdit_ && songOrder->at(currentPattern) > -1) NextLane();
    else if (songOrder->at(currentPattern) > -1) currentStep->NextParam();
    updateStep = true;
}

void Sequencer::NextPattern() {
    currentStep->selected = false;
    currentPattern = (currentPattern >= (int) songOrder->size() - 1) ? 0 : currentPattern + 1;
    activePattern = patterns->at(songOrder->at(currentPattern));
    currentLane = activePattern->lanes.at(currentLane->index);
    currentStep = currentLane->sequence.at(0);
    currentStep->selected = true;
    updateStep = true;
    updatePattern = true;
    updateSidebar = true;
}

void Sequencer::PreviousPattern() {
    currentStep->selected = false;
    currentPattern = currentPattern + 1;
    if (currentPattern >= (int) songOrder->size()) {
        playing_ = false;
    } else {
        activePattern = patterns->at(songOrder->at(currentPattern));
        currentLane = activePattern->lanes.at(currentLane->index);
        currentStep = currentLane->sequence.at(0);
        currentStep->selected = true;
    }
    updateStep = true;
    updatePattern = true;
    updateSidebar = true;
}

void Sequencer::AltUpButton() {
    currentStep->selected = false; //deselecting step

    // if the index of pattern were moving away from is -1, remove from songOrder
    if (songOrder->at(currentPattern) == -1) {
        auto it = songOrder->begin();
        std::advance(it, currentPattern);
        songOrder->erase(it);
    } 
    
    currentPattern -= 1; // decrement index of pattern

    if (currentPattern < 0) {
        songOrder->insert(songOrder->begin(), -1);
        currentPattern = 0; // need to reset it to 0 to shift everything down
    }

    if (songOrder->at(currentPattern) > -1) {
        activePattern = patterns->at(songOrder->at(currentPattern));
        currentLane = activePattern->lanes[0];
        currentStep = currentLane->sequence.at(0);
    }
    updatePattern = true;
}

void Sequencer::AltDownButton() {
    currentStep->selected = false;
    if (songOrder->at(currentPattern) == -1) {
        auto it = songOrder->begin();
        std::advance(it, currentPattern);
        songOrder->erase(it);
    } else {
        currentPattern += 1;
    }
    
    if (currentPattern >= (int) songOrder->size()) songOrder->push_back(-1); 
    else { //not seting active pattern to anything to avoid index out of range error
        activePattern = patterns->at(songOrder->at(currentPattern));
        currentLane = activePattern->lanes[0];
        currentStep = currentLane->sequence.at(0);
    }
    updatePattern = true;
}

void Sequencer::AltLeftButton() {
    currentStep->selected = false;
    if (songOrder->at(currentPattern) < 0) songOrder->at(currentPattern) = -1;
    else songOrder->at(currentPattern) -= 1;
    if (songOrder->at(currentPattern) > -1) {
        activePattern = patterns->at(songOrder->at(currentPattern));
        currentLane = activePattern->lanes[0];
        currentStep = currentLane->sequence.at(0);
    }
    updatePattern = true;
}

void Sequencer::AltRightButton() {
    currentStep->selected = false;
    songOrder->at(currentPattern) += 1;
    if (songOrder->at(currentPattern) > ((int) patterns->size() - 1)) NewPattern();
    activePattern = patterns->at(songOrder->at(currentPattern));
    currentLane = activePattern->lanes[0];
    currentStep = currentLane->sequence.at(0);
    updatePattern = true;
}

void Sequencer::AltPlayButton() {
    playing_ = (playing_) ? false : true;
    song_ = (song_) ? false : true;
    if (playing_) {
        for (Lane* lane : activePattern->lanes) {
            handler_->at(lane->index)->Update(lane->sequence.at(currentStep->index)); 
        }
    }
}

void Sequencer::AltAButton() {
    for (Lane* lane : activePattern->lanes) {
        lane->length += 1;
        lane->sequence.push_back(new Step(lane->length - 1));
    }
    updateStep = true;
    updateSidebar = true;
}

void Sequencer::AltBButton() {
    if (currentLane->length <= 1) return;
    if (currentStep->index >= currentLane->length - 1) currentStep = currentLane->sequence.at(currentStep->index - 1);
    for (Lane* lane : activePattern->lanes) {
        lane->length -= 1;
        lane->sequence.erase(--lane->sequence.end()); //get rid of last
    }
    updateStep = true;
    updateSidebar = true;
}