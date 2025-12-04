#include "sequencer.h"

void Sequencer::NewPattern() {
    patterns->push_back(new Pattern);

    patterns->back()->setIndex(patterns->size() - 1);
    
    songOrder->at(currentPattern) = patterns->back()->getIndex();

    activePattern = patterns->at(songOrder->at(currentPattern));

    for (int i = 0; i < 64; i ++) {
        activePattern->push(i);
    }

    currentLane = 0;
    currentStep = activePattern->get_current(currentLane);
}

void Sequencer::Clear() {
    patterns->clear();
    songOrder->clear();
    playing_ = false;
    stepEdit_ = false;
    song_ = false;
    currentPattern = 0;
    laneOffset = 0;
    updateStep = true;
    updateSidebar = true;
    updatePattern = true;
    currentLane = 0;
    currentStep = nullptr;
    activePattern = nullptr;
}

void Sequencer::Safe() {

    playing_ = false;
    stepEdit_ = false;
    song_ = false;
    currentPattern = 0;
    laneOffset = 0;
    updateStep = true;
    updateSidebar = true;
    updatePattern = true;

    activePattern = patterns->at(songOrder->at(currentPattern));
    currentLane = 0;
    currentStep = activePattern->get_current()->steps[currentLane];
}

void Sequencer::DrawStep(cLayer* display, int x, int y, Step* step, bool active ) {

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
    
    sColor main = (active) ? MAIN : ALTMAIN;

    // Writing instrument
    if (step->instrument < 0) sprintf(strbuff, "--");
    else sprintf(strbuff, "%02d", step->instrument);

    if (step->selected && stepEdit_ && step->paramEdit == Step::i) 
        WriteString(display, x + 2, y + 2 + CHAR_HEIGHT, BACKGROUND);
    else 
        WriteString(display, x + 2, y + 2 + CHAR_HEIGHT, main);

    // Writing note
    if (step->instrument == -1) sprintf(strbuff, "---");
    else if (step->instrument == -2) sprintf(strbuff, "OFF");
    else GetNoteString(strbuff, step->note);

    if (step->selected && stepEdit_ && step->paramEdit == Step::n) 
        WriteString(display, x + (CHAR_WIDTH * 3) + 2, y + 2 + CHAR_HEIGHT, BACKGROUND);
    else 
        WriteString(display, x + (CHAR_WIDTH * 3) + 2, y + 2 + CHAR_HEIGHT, main);

    // Writing fx
    if (step->instrument < 0) sprintf(strbuff, "-");
    else GetFxString(strbuff, step->fx);
    if (step->selected && stepEdit_ && step->paramEdit == Step::f) 
        WriteString(display, x + (CHAR_WIDTH * 7) + 2, y + 2 + CHAR_HEIGHT, BACKGROUND);
    else 
        WriteString(display, x + (CHAR_WIDTH * 7) + 2, y + 2 + CHAR_HEIGHT, main);

     // Writing fx amount
    if (step->instrument < 0 || step->fx <= 0) sprintf(strbuff, "--");
    else sprintf(strbuff, "%02x", step->fxAmount);
    if (step->selected && stepEdit_ && step->paramEdit == Step::fa) 
        WriteString(display, x + (CHAR_WIDTH * 8) + 2, y + 2 + CHAR_HEIGHT, BACKGROUND);
    else 
        WriteString(display, x + (CHAR_WIDTH * 8) + 2, y + 2 + CHAR_HEIGHT, main);

}

void Sequencer::DrawSquare(cLayer *display, int index, int x, int y, bool fill) {
    if (fill) {
        display->drawFillRect(x, y, 22, 22, ACCENT2);
        if (index < 10) 
            WriteString(display, x + (22 - CHAR_WIDTH) / 2, y + CHAR_HEIGHT + (22 - CHAR_HEIGHT) / 2, MAIN, "%d", index);
        else 
            WriteString(display, x + (22 - (CHAR_WIDTH * 2)) / 2, y + CHAR_HEIGHT + (22 - CHAR_HEIGHT) / 2, MAIN, "%d", index);
    } else {
        display->drawFillRect(x, y, 22, 22, ALTACCENT2);
        if (index < 10) 
            WriteString(display, x + (22 - CHAR_WIDTH) / 2, y + CHAR_HEIGHT + (22 - CHAR_HEIGHT) / 2, MAIN, "%d", index);
        else 
            WriteString(display, x + (22 - (CHAR_WIDTH * 2)) / 2, y + CHAR_HEIGHT + (22 - CHAR_HEIGHT) / 2, MAIN, "%d", index);
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
        if      (fx == 1)   sprintf(strbuff, "R");  // fx 1 - ROLL
        else if (fx == 2)   sprintf(strbuff, "U");  // fx 2 - PITCH UP
        else if (fx == 3)   sprintf(strbuff, "D");  // fx 3 - PITCH DOWN
        else if (fx == 4)   sprintf(strbuff, "V");  // fx 4 - VOLUME
        else if (fx == 5)   sprintf(strbuff, "B");  // fx 5 - BACKWARDS
        else                sprintf(strbuff, "F");  // fx 6 - FX AUTOMATION
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

void Sequencer::UpdateDisplay(cLayer *display) {
    /**
     * CLEAR
     */
    display->eraseLayer(BACKGROUND);

    /**
     * Drawing Steps
     */
    if (songOrder->at(currentPattern) > -1) {
        // clear previous stuff
        display->drawFillRect(0, 0, (28 * 9), 240, BACKGROUND);

        currentStep->selected = true;

        
        StepCluster<LANE_NUMBER>* clust = activePattern->get_current();
        for (int i = 0; i < 5; i ++) {
            clust = clust->prev;
        }


        int index = activePattern->get_current()->steps[0]->index;
        int top = 5 - index;
        int bottom = 7 - (activePattern->getSize() - index);

        for (int y = 0; y < 12; y++) {
            for (int x = 0; x < 2; x++) {
                Step* step = clust->steps[laneOffset + x];

                if (bottom > 0 && y >= 12 - bottom) {
                    DrawStep(display, (((CHAR_WIDTH * 12) * x) + (CHAR_WIDTH * 4)), ((CHAR_HEIGHT + 8) * y), step, false);
                } else if (y < top) {
                    DrawStep(display, (((CHAR_WIDTH * 12) * x) + (CHAR_WIDTH * 4)), ((CHAR_HEIGHT + 8) * y), step, false);
                } else {
                    DrawStep(display, (((CHAR_WIDTH * 12) * x) + (CHAR_WIDTH * 4)), ((CHAR_HEIGHT + 8) * y), step, true);
                }
                
                if (step->index % 4 == 0 && x == 0) {
                    if (step->index % 16 == 0) 
                        WriteString(display, 0, ((CHAR_HEIGHT + 8) * (y + 1) - (CHAR_HEIGHT / 2)), ACCENT2, "%3d", step->index);
                    else 
                        WriteString(display, 0, ((CHAR_HEIGHT + 8) * (y + 1) - (CHAR_HEIGHT / 2)), ACCENT1, "%3d", step->index);
                }   
            }
            clust = clust->next;
        } 
    }
    
    /**
     * Drawing Sidebar
     */
    display->drawFillRect(28*9, 0, 100, (CHAR_HEIGHT + 4) * 3 + 4, BACKGROUND);

    WriteString(display, 320 - (CHAR_WIDTH * 7), CHAR_HEIGHT + 4, ACCENT1, "BPM %3d", (int) bpm_);

    
    if (songOrder->at(currentPattern) > -1) 
        WriteString(display, 320 - (CHAR_WIDTH * 7), (CHAR_HEIGHT + 4) * 2, ACCENT1, "LANE%3d", currentLane + 1);
    else 
        WriteString(display, 320 - (CHAR_WIDTH * 7), (CHAR_HEIGHT + 4) * 2, ACCENT1, "LANE");

    
    if (songOrder->at(currentPattern) > -1) WriteString(display, 320 - (CHAR_WIDTH * 7), (CHAR_HEIGHT + 4) * 3, ACCENT1, "LEN%4d", activePattern->getSize());
    else WriteString(display, 320 - (CHAR_WIDTH * 7), (CHAR_HEIGHT + 4) * 3, ACCENT1, "LEN");
    
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
            for (int i = 0; i < LANE_NUMBER; i ++) {
                handler_->at(i)->Update(activePattern->get_current(i)); 
            }
        }
    }
}

void Sequencer::PlayButton() {
    playing_ = (playing_) ? false : true;
    song_ = false;
    if (playing_) {
        tick.Reset();
        for (int i = 0; i < LANE_NUMBER; i ++) {
            handler_->at(i)->Update(activePattern->get_current(i)); 
        }
    } else {
        for (int i = 0; i < LANE_NUMBER; i ++) {
            handler_->at(i)->Update(activePattern->get_current(i)); 
        }
    }
}

void Sequencer::AButton() {
    stepEdit_ = !stepEdit_;
    if (currentStep->instrument == -2 && stepEdit_) 
        currentStep->instrument = 0;
    updateStep = true;
}

void Sequencer::BButton() {
    currentStep->instrument = -1;
    updateStep = true;
}

void Sequencer::PreviousStep() {
    currentStep->selected = false;
    activePattern->decrement();
    currentStep = activePattern->get_current(currentLane);
    updateStep = true;
}

void Sequencer::UpButton() {
    if (!playing_ && !stepEdit_ && songOrder->at(currentPattern) > -1) PreviousStep();
    else if (stepEdit_ && songOrder->at(currentPattern) > -1) currentStep->Increment();
    updateStep = true;
}

void Sequencer::NextStep() {
    currentStep->selected = false;
    if (activePattern->is_tail() && song_) {
        NextPattern();
        activePattern->reset();
    } else {
        activePattern->increment();
    }

    currentStep = activePattern->get_current(currentLane);
    
    updateStep = true;
}

void Sequencer::DownButton() {
    if (!playing_ && !stepEdit_ && songOrder->at(currentPattern) > -1) NextStep();
    else if (stepEdit_ && songOrder->at(currentPattern) > -1) currentStep->Decrement();
    updateStep = true;
}

void Sequencer::PreviousLane() {
    currentStep->selected = false;
    if (currentLane > 0) {
        currentLane -= 1;
        if (currentLane < 0) {
            currentLane = 0;
        }
        currentStep = activePattern->get_current(currentLane);
        if (currentLane < 2) laneOffset = currentLane;
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
    if (currentLane < (LANE_NUMBER - 1)) {
        currentLane += 1;
        if (currentLane >= LANE_NUMBER) {
            currentLane = LANE_NUMBER - 1;
        }
        currentStep = activePattern->get_current(currentLane);
        if (currentLane> 1) laneOffset = currentLane - 1;
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
    currentLane = 0;
    activePattern->reset();
    currentStep = activePattern->get_current(currentLane);
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
        currentLane = 0;
        activePattern->reset();
        currentStep = activePattern->get_current(currentLane);
        currentStep->selected = true;
    }
    updateStep = true;
    updatePattern = true;
    updateSidebar = true;
}

void Sequencer::AltUpButton() {
    if (!playing_ && stepEdit_ && songOrder->at(currentPattern) > -1) {
        PreviousStep();
        return;
    }

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
        currentLane = 0;
        activePattern->reset();
        currentStep = activePattern->get_current(currentLane);
    }
    updatePattern = true;
}

void Sequencer::AltDownButton() {
    if (!playing_ && stepEdit_ && songOrder->at(currentPattern) > -1) {
        NextStep();
        return;
    }

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
        currentLane = 0;
        activePattern->reset();
        currentStep = activePattern->get_current(currentLane);
    }
    updatePattern = true;
}

void Sequencer::AltLeftButton() {
    currentStep->selected = false;
    if (songOrder->at(currentPattern) < 0) songOrder->at(currentPattern) = -1;
    else songOrder->at(currentPattern) -= 1;
    if (songOrder->at(currentPattern) > -1) {
        activePattern = patterns->at(songOrder->at(currentPattern));
        currentLane = 0;
        activePattern->reset();
        currentStep = activePattern->get_current(currentLane);
    }
    updatePattern = true;
}

void Sequencer::AltRightButton() {
    currentStep->selected = false;
    songOrder->at(currentPattern) += 1;
    if (songOrder->at(currentPattern) > ((int) patterns->size() - 1)) NewPattern();
    activePattern = patterns->at(songOrder->at(currentPattern));
    currentLane = 0;
    activePattern->reset();
    currentStep = activePattern->get_current(currentLane);
    updatePattern = true;
}

void Sequencer::AltPlayButton() {
    playing_ = (playing_) ? false : true;
    song_ = (song_) ? false : true;
    if (playing_) {
        tick.Reset();
        for (int i = 0; i < LANE_NUMBER; i ++) {
            handler_->at(i)->Update(activePattern->get_current(i)); 
        }
    } else {
        for (int i = 0; i < LANE_NUMBER; i ++) {
            handler_->at(i)->Update(activePattern->get_current(i)); 
        }
    }
}

void Sequencer::AltAButton() {
    int size = activePattern->getSize();

    for (int i = 0; i < size; i ++) {
        activePattern->push(size + i);
    }

    updateStep = true;
    updateSidebar = true;
}

void Sequencer::AltBButton() {
    int size = activePattern->getSize();

    if (size <= 1) 
        return;
    
    if (currentStep->index >= size / 2) 
        currentStep = activePattern->get_current(size / 2);
    
    for (int i = 0; i < size / 2; i ++) {
        StepCluster<LANE_NUMBER>* back = activePattern->remove();
        delete back;
    }
    
    updateStep = true;
    updateSidebar = true;
}