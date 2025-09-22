#include "sampDisplay.h"

void SampDisplay::Init(WaveFileLoader* fileLoader, std::vector<Instrument*>* instruments, cFont *MainFont, size_t* bufferIndex) {
    this->fileLoader = fileLoader;
    this->instruments = instruments;
    this->MainFont = MainFont;
    this->bufferIndex = bufferIndex;
    currentNode = fileLoader->GetRootNode();
    row = 0;
    scrRow = 0;
    col = 0;
    lev = 0;
}

void SampDisplay::AButton() {

    if (currentNode->down.at(row)->data.attrib & (AM_DIR)) {
        currentNode = currentNode->down.at(row);
        lev += 1;
        if (lev > 2) {
            col = 2;
        } else {
            col = lev;
        }
        row = 0;
        scrRow = 0;
    } else {
        Instrument* inst = fileLoader->CreateInstrument(currentNode->down.at(row));
        if (inst != nullptr) instruments->push_back(inst);
    }
}

void SampDisplay::BButton(){
    if (currentNode->up != nullptr) {
        currentNode = currentNode->up;
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
    }

}

void SampDisplay::UpButton(){
    row -= 1;
    scrRow -= 1;
    if (row < 0) {
        row = 0;
    }
    if (scrRow < 0) {
        scrRow = 0;
    }
}

void SampDisplay::DownButton(){
    row += 1;
    scrRow += 1;
    if (row >= (int) currentNode->down.size()) {
        row = currentNode->down.size() - 1;
    }
    if (scrRow > 10) {
        scrRow = 10;
    }
    if (scrRow > row) {
        scrRow = row;
    }
}

void SampDisplay::LeftButton(){}
void SampDisplay::RightButton(){}
void SampDisplay::PlayButton(){}

void SampDisplay::AltAButton(){}
void SampDisplay::AltBButton(){}
void SampDisplay::AltUpButton(){}
void SampDisplay::AltDownButton(){}
void SampDisplay::AltLeftButton(){}
void SampDisplay::AltRightButton(){}
void SampDisplay::AltPlayButton(){}

void SampDisplay::UpdateDisplay(cLayer* display){
    display->eraseLayer();

    double sdramUsage = (double) *bufferIndex / (double) MAX_BUFFER_SIZE;

    display->drawLine(0, 20, 240, 20, ACCENT2);

    display->drawFillRect(0, 0, sdramUsage * 240.0f, 20, ACCENT2);

    sprintf(strbuff, "MEM USAGE");
    WriteString(display, strbuff, 4, CHAR_HEIGHT + 4, MAIN);


    display->drawLine(80, 21, 80, 240, MAIN);
    display->drawLine(160, 21, 160, 240, MAIN);

    /**
     * Drawing selector
     */
    display->drawFillRect(col * 80 + 1, scrRow * (CHAR_HEIGHT + 8) + 20, 79, CHAR_HEIGHT + 8, ACCENT2);

    int y = CHAR_HEIGHT + 4 + 18;
    int x = 4;
    if (currentNode->up != nullptr) {

        if (currentNode->up->up != nullptr) {
            for (Node<File>* node : currentNode->up->up->down) {
                std::string path = std::string(node->data.name);
                size_t index = path.find_last_of('/') + 1; 
                std::string name = path.substr(index, path.length() - index);
                if (node->data.attrib & (AM_DIR)) sprintf(strbuff, "/%.7s", name.c_str());
                else sprintf(strbuff, "%.8s", name.c_str());

                if (node == currentNode->up) {
                    display->drawFillRect(x - 3, y - (CHAR_HEIGHT + 2), 79, CHAR_HEIGHT + 8, ACCENT2);
                }

                WriteString(display, strbuff, x, y, MAIN);
                y += CHAR_HEIGHT + 8;
            }
            x += 80;
        }

        y = CHAR_HEIGHT + 4 + 18;
        for (Node<File>* node : currentNode->up->down) {
            std::string path = std::string(node->data.name);
            size_t index = path.find_last_of('/') + 1; 
            std::string name = path.substr(index, path.length() - index);
            if (node->data.attrib & (AM_DIR)) sprintf(strbuff, "/%.7s", name.c_str());
            else sprintf(strbuff, "%.8s", name.c_str());

            if (node == currentNode) {
                display->drawFillRect(x - 3, y - (CHAR_HEIGHT + 2), 79, CHAR_HEIGHT + 8, ACCENT2);
            }

            WriteString(display, strbuff, x, y, MAIN);
            y += CHAR_HEIGHT + 8;
        }
        x += 80;
    }
   
    /**
     * Drawing files in open dir
     */
    y = CHAR_HEIGHT + 4 + 18;

    for (int i = 0; i < 11; i ++) {
        int pos = row + (i - scrRow);

        if (pos >= (int) currentNode->down.size()) continue;
        else if (pos < 0) continue;

        Node<File>* node = currentNode->down.at(pos);

        std::string path = std::string(node->data.name);
        size_t index = path.find_last_of('/') + 1; 
        std::string name = path.substr(index, path.length() - index);
        if (node->data.attrib & (AM_DIR)) sprintf(strbuff, "/%.7s", name.c_str());
        else sprintf(strbuff, "%.8s", name.c_str());
        WriteString(display, strbuff, x, y, MAIN);
        y += CHAR_HEIGHT + 8; 
    }

    // for (Node<File>* node : currentNode->down) {
    //     std::string path = std::string(node->data.name);
    //     size_t index = path.find_last_of('/') + 1; 
    //     std::string name = path.substr(index, path.length() - index);
    //     if (node->data.attrib & (AM_DIR)) sprintf(strbuff, "/%.7s", name.c_str());
    //     else sprintf(strbuff, "%.8s", name.c_str());
    //     WriteString(display, strbuff, x, y, MAIN);
    //     y += CHAR_HEIGHT + 8;
    // }
    
    /**
     * Drawing Instruments
     */
    display->drawLine(320 - 80, 0, 320-80, 240, ACCENT2);
    display->drawFillRect(240, 0, 80, 21, ACCENT2);
    sprintf(strbuff, "LOADED");
    WriteString(display, strbuff, 320 - 76, CHAR_HEIGHT + 4, MAIN);
    int yOffset = CHAR_HEIGHT + 4 + CHAR_HEIGHT + 8;
    if (!instruments->empty()) {
        for (size_t i = 0; i < instruments->size(); i ++) {
            if (instruments->at(i) == nullptr) sprintf(strbuff, "NOT LOAD");
            else sprintf(strbuff, "%s", instruments->at(i)->GetName());
            WriteString(display, strbuff, 320 - 76, yOffset, ACCENT1);
            yOffset += CHAR_HEIGHT + 8;
        }
    }
    
}

void SampDisplay::WriteString(cLayer* display, char* strbuff, int x, int y, DadGFX::sColor color) {
    display->setCursor(x, y);
    display->setFont(MainFont);
    display->setTextFrontColor(color);
    display->drawText(strbuff);
}