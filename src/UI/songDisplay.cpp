#include "songDisplay.h"

void SongDisplay::Init(Sequencer *sequencer, Node<File>* node, ProjSaverLoader* projSaverLoader, cFont* MainFont, void* (*search_buffer_allocate)(size_t size)) {
    this->sequencer = sequencer;
    vol = 0;
    this->MainFont = MainFont;
    param = SAVE;
    this->projSaverLoader = projSaverLoader;
    projName = std::string("Untitled");
    currentNode = node;
    rootNode = node;
    row = 0;
    scrRow = 0;
    col = 0;
    lev = 0;
    load = false;
    saveas = false;
    tempName.clear();
    this->search_buffer_allocate = search_buffer_allocate;

    keyboard[0][0] = new Letter("a", "A", 1);
    keyboard[0][1] = new Letter("b", "B", 1);
    keyboard[0][2] = new Letter("c", "C", 1);
    keyboard[0][3] = new Letter("d", "D", 1);
    keyboard[0][4] = new Letter("e", "E", 1);
    keyboard[0][5] = new Letter("f", "F", 1);
    keyboard[0][6] = new Letter("g", "G", 1);
    keyboard[0][7] = new Letter("h", "H", 1);
    keyboard[0][8] = new Letter("DEL", "DEL", 2);

    keyboard[1][0] = new Letter("i", "I", 1);
    keyboard[1][1] = new Letter("j", "J", 1);
    keyboard[1][2] = new Letter("k", "K", 1);
    keyboard[1][3] = new Letter("l", "L", 1);
    keyboard[1][4] = new Letter("m", "M", 1);
    keyboard[1][5] = new Letter("n", "N", 1);
    keyboard[1][6] = new Letter("o", "O", 1);
    keyboard[1][7] = new Letter("p", "P", 1);
    keyboard[1][8] = new Letter("SHF", "SHF", 2);

    keyboard[2][0] = new Letter("q", "Q", 1);
    keyboard[2][1] = new Letter("r", "R", 1);
    keyboard[2][2] = new Letter("s", "S", 1);
    keyboard[2][3] = new Letter("t", "T", 1);
    keyboard[2][4] = new Letter("u", "U", 1);
    keyboard[2][5] = new Letter("v", "V", 1);
    keyboard[2][6] = new Letter("w", "W", 1);
    keyboard[2][7] = new Letter("x", "X", 1);
    keyboard[2][8] = new Letter("OK", "OK", 2);

    keyboard[3][0] = new Letter("y", "Y", 1);
    keyboard[3][1] = new Letter("z", "Z", 1);
    keyboard[3][2] = new Letter("SPACE", "SPACE", 8);;
    keyboard[3][3] = nullptr;
    keyboard[3][4] = nullptr;
    keyboard[3][5] = nullptr;
    keyboard[3][6] = nullptr;
    keyboard[3][7] = nullptr;
    keyboard[3][8] = nullptr;

    keyV = 0;
    keyH = 0;
}



void SongDisplay::Process(float& left, float& right) {
    left = left * pow(10.0f, vol / 10.0f);
    right = right * pow(10.0f, vol / 10.0f);
}

void SongDisplay::UpdateDisplay(cLayer* display) {
    display->eraseLayer(BACKGROUND);
    
    /**
     * Drawing Save Save as and Load
     */
    display->drawLine(0, 200, 320, 200, ACCENT2);
    display->drawLine(80, 200, 80, 240, ACCENT2);
    display->drawLine(160, 200, 160, 240, ACCENT2);
    display->drawLine(240, 200, 240, 240, ACCENT2);
    display->drawLine(280, 200, 280, 240, ACCENT2);

    if ((int) param < 3) {
        display->drawFillRect(80 * (int) param, 200, 80, 40, ACCENT2);
    }

    sprintf(strbuff, "SAVE");
    WriteString(display, strbuff, 40 - (2 * CHAR_WIDTH), 220 + (0.5f * (float) CHAR_HEIGHT), MAIN);

    sprintf(strbuff, "SAVE AS");
    WriteString(display, strbuff, 120 - (3.5f * (float) CHAR_WIDTH), 220 + (0.5f * (float) CHAR_HEIGHT), MAIN);

    sprintf(strbuff, "LOAD");
    WriteString(display, strbuff, 200 - (2 * CHAR_WIDTH), 220 + (0.5f * (float) CHAR_HEIGHT), MAIN);

    /**
     * Drawing VOL and BPM
     */
    float height = (vol + 70.0f) / 76.0f;

    display->drawFillRect(243, 1 + (1.0f - height) * 200, 35, height * 200, ACCENT1);

    if (param == VOL) display->drawFillRect(240, 200, 40, 40, ACCENT2);

    sprintf(strbuff, "VOL");
    WriteString(display, strbuff, 244, 200 + CHAR_HEIGHT + 4, MAIN);

    sprintf(strbuff, "%.2f", vol);
    WriteString(display, strbuff, 244, 200 + 2 * (CHAR_HEIGHT + 4), MAIN);

    __disable_irq();
    float bpm_ = BPM;
    __enable_irq();
    
    height = bpm_ / 300.0f;

    display->drawFillRect(283, 1 + (1.0f - height) * 200, 35, height * 200, ACCENT1);

    if (param == BPM) display->drawFillRect(280, 200, 40, 40, ACCENT2);

    sprintf(strbuff, "BPM");
    WriteString(display, strbuff, 284, 200 + CHAR_HEIGHT + 4, MAIN);

    sprintf(strbuff, "%.0f", bpm_);
    WriteString(display, strbuff, 284, 200 + 2 * (CHAR_HEIGHT + 4), MAIN);

    if (!load && !saveas) {
        sprintf(strbuff, "%s", projName.c_str());
        WriteString(display, strbuff, 4, (CHAR_HEIGHT + 4), MAIN);
    }

    /**
     * LOAD MENU
     */
    if (load) {
        /**
         * Drawing selector
         */
        display->drawFillRect(col * 80 + 1, scrRow * (CHAR_HEIGHT + 8) + 2, 240, CHAR_HEIGHT + 8, ACCENT2);

        int y = CHAR_HEIGHT + 4;
        int x = 4;
    
        /**
         * Drawing files in open dir
         */
        y = CHAR_HEIGHT + 4;

        for (int i = 0; i < 8; i ++) {
            int pos = row + (i - scrRow);

            if (pos >= (int) currentNode->children.size()) continue;
            else if (pos < 0) continue;

            Node<File>* node = currentNode->children.at(pos);
            sprintf(strbuff, "%.24s", *node->data.name);
            
            WriteString(display, strbuff, x, y, MAIN);
            y += CHAR_HEIGHT + 8; 
        }
    }
    
    /**
     * SAVE AS MENU
     */
    if (saveas) {

        /**
         * Temp Name being written
         */
        sprintf(strbuff, "%s%s", tempName.c_str(), "_");
        WriteString(display, strbuff, 4, (CHAR_HEIGHT + 4), MAIN);

        if (keyboard[keyV][keyH] != nullptr) keyboard[keyV][keyH]->selected = true;

        for (int v = 0; v < 4; v++) {
            for (int h = 0; h < 9; h++) {
                int y = v * 24 + 101;
                int x = h * 24;
                if (keyboard[v][h] != nullptr) {
                    keyboard[v][h]->Display(display, MainFont, x, y);
                }
            }
        }
    }
}

void SongDisplay::NextParam() {
    if (param == SAVE) param = SAVEAS;
    else if (param == SAVEAS) param = LOAD;
    else if (param == LOAD) param = VOL;
    else if (param == VOL) param = TEMPO;
}

void SongDisplay::PrevParam() {
    if (param == SAVEAS) param = SAVE;
    else if (param == LOAD) param = SAVEAS;
    else if (param == VOL) param = LOAD;
    else if (param == TEMPO) param = VOL;
}

void SongDisplay::Increment() {
    if (param == VOL) {
        vol += 0.5f;
        if (vol > 6.0f) {
            vol = 6.0f;
        }
    }
    else if (param == TEMPO) {
        __disable_irq();
        BPM += 1;
        if (BPM > 300) {
            BPM = 300;
        }
        __enable_irq();
        sequencer->SetBPM();
    }
}

void SongDisplay::Decrement() {
    if (param == VOL) {
        vol -= 0.5f;
        if (vol < -70.0f) {
            vol = -70.0f;
        }
    }
    else if (param == TEMPO) {
        __disable_irq();
        BPM -= 1;
        if (BPM < 20) {
            BPM = 20;
        }
        __enable_irq();
        sequencer->SetBPM();
    }
}

void SongDisplay::AButton(){
    if (load) {
        std::string name = std::string(*currentNode->children.at(row)->data.name);
        if (name.find(".mtrk") != std::string::npos) {
            projSaverLoader->LoadProject(*currentNode->children.at(row)->data.name);
            projName = name;
        }
        load = false;
    } else if (saveas) {
        std::string val = std::string(keyboard[keyV][keyH]->GetVal());

        if (val.compare("DEL") == 0) {
            if (!tempName.empty()) tempName.pop_back();
        } else if (val.compare("SHF") == 0) {
            for (int v = 0; v < 4; v++) {
                for (int h = 0; h < 9; h++) {
                    keyboard[v][h]->capital = !keyboard[v][h]->capital;
                }
            }
        } else if (val.compare("SPACE") == 0) {
            tempName += " ";
        } else if (val.compare("OK") == 0) {
            projName = tempName + std::string(".mtrk");
            projSaverLoader->SaveProject(projName.c_str());
            rootNode->children.push_back(new Node<File>);
            void* ptr = search_buffer_allocate(256);
            rootNode->children.back()->data.name = (char (*)[256]) ptr;
            sprintf(*rootNode->children.back()->data.name, "%s%s", tempName.c_str(), ".mtrk");
            rootNode->children.back()->data.attrib = 0;
            saveas = false;
        } else {
            tempName += val;
        }
    } else {
        if (param == SAVE) {
            if (projName.compare("Untitled") != 0) projSaverLoader->SaveProject(projName.c_str());
        }
        else if (param == LOAD) {
            load = true;
        }
        else if (param == SAVEAS) {
            saveas = true;
            tempName.clear();
        }
    }   
}

void SongDisplay::BButton(){
    if (load) {
        if (currentNode->parent != nullptr) {
            currentNode = currentNode->parent;
            lev -= 1;
            if (lev < 0) {
                lev = 0;
            }
            if (lev > 2) {
                col = 2;
            } else {
                col = lev;
            }
            row = 0;
            scrRow = 0;
        } else {
            load = false;
        }
    }
    if (saveas) {
        saveas = false;
    }
}

void SongDisplay::UpButton(){ 
    if (load) {
        row -= 1;
        scrRow -= 1;
        if (row < 0) {
            row = 0;
        }
        if (scrRow < 0) {
            scrRow = 0;
        }
    } else if (saveas) {
        keyboard[keyV][keyH]->selected = false;
        keyV -= 1;
        if (keyV < 0) {
            keyV = 0;
        }
        while (keyboard[keyV][keyH] == nullptr) {
            keyH -= 1;
        }
        keyboard[keyV][keyH]->selected = true;
    } else {
        Increment(); 
    }
    
}

void SongDisplay::DownButton(){ 
    if (load) {
        row += 1;
        scrRow += 1;
        if (row >= (int) currentNode->children.size()) {
            row = currentNode->children.size() - 1;
        }
        if (scrRow > 7) {
            scrRow = 7;
        }
        if (scrRow > row) {
            scrRow = row;
        }
    } else if (saveas) {
        keyboard[keyV][keyH]->selected = false;
        keyV += 1;
        if (keyV > 3) {
            keyV = 3;
        }
        while (keyboard[keyV][keyH] == nullptr) {
            keyH -= 1;
        }
        keyboard[keyV][keyH]->selected = true;
    } else {
        Decrement(); 
    }
}

void SongDisplay::LeftButton(){ 
    if (load) {

    } else if (saveas) {
        keyboard[keyV][keyH]->selected = false;
        keyH -= 1;
        if (keyH < 0) {
            keyH = 0;
        }
        while (keyboard[keyV][keyH] == nullptr) {
            keyH -= 1;
        }
        keyboard[keyV][keyH]->selected = true;
    } else {
        PrevParam(); 
    } 
}

void SongDisplay::RightButton(){ 
    if (load) {

    } else if (saveas) {
        keyboard[keyV][keyH]->selected = false;
        keyH += 1;
        if (keyH > 8) {
            keyH = 8;
        }
        while (keyboard[keyV][keyH] == nullptr) {
            keyH -= 1;
        }
        keyboard[keyV][keyH]->selected = true;
    } else {
        NextParam(); 
    }
}