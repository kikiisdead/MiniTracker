#include "sampDisplay.h"

void SampDisplay::Init(WaveFileLoader* fileLoader, std::vector<Instrument*>* instruments, cFont *MainFont, size_t* bufferIndex, Node<File>* rootNode) {
    this->fileLoader = fileLoader;
    this->instruments = instruments;
    this->MainFont = MainFont;
    this->bufferIndex = bufferIndex;
    currentNode = rootNode;
    row = 0;
    scrRow = 0;
    col = 0;
    lev = 0;
}

void SampDisplay::AButton() {

    if (currentNode->children.at(row)->data.attrib & (AM_DIR)) {
        currentNode = currentNode->children.at(row);
        lev += 1;
        if (lev > 2) {
            col = 2;
        } else {
            col = lev;
        }
        row = 0;
        scrRow = 0;
    } else {
        Instrument* inst = fileLoader->CreateInstrument(currentNode->children.at(row));
        if (inst != nullptr) instruments->push_back(inst);
    }
}

void SampDisplay::BButton(){
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
    if (row >= (int) currentNode->children.size()) {
        row = currentNode->children.size() - 1;
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
    display->drawFillRect(0, 0, 320, 240, BACKGROUND);

    double sdramUsage = (double) *bufferIndex / (double) MAX_BUFFER_SIZE;

    display->drawLine(0, 20, 240, 20, ACCENT2);

    display->drawFillRect(0, 0, sdramUsage * 240.0f, 20, ACCENT2);

    sprintf(strbuff, "MEM USAGE");
    WriteString(display, strbuff, 4, CHAR_HEIGHT + 3, MAIN);


    display->drawLine(80, 21, 80, 240, MAIN);
    display->drawLine(160, 21, 160, 240, MAIN);

    /**
     * Drawing selector
     */
    display->drawFillRect(col * 80 + 1, scrRow * (CHAR_HEIGHT + 8) + 20, 79, CHAR_HEIGHT + 8, ACCENT2);

    /**
     * Drawing parent dir
     */
    int y = CHAR_HEIGHT + 4 + 18;
    int x = 4;
    if (currentNode->parent != nullptr) {

        if (currentNode->parent->parent != nullptr) {
            for (Node<File>* node : currentNode->parent->parent->children) {
                if (node->data.attrib & (AM_DIR)) sprintf(strbuff, "/%.7s", *node->data.name);
                else sprintf(strbuff, "%.8s", *node->data.name);

                if (node == currentNode->parent) {
                    display->drawFillRect(x - 3, y - (CHAR_HEIGHT + 2), 79, CHAR_HEIGHT + 8, ACCENT2);
                }

                WriteString(display, strbuff, x, y, MAIN);
                y += CHAR_HEIGHT + 8;
            }
            x += 80;
        }

        y = CHAR_HEIGHT + 4 + 18;
        for (Node<File>* node : currentNode->parent->children) {
            if (node->data.attrib & (AM_DIR)) sprintf(strbuff, "/%.7s", *node->data.name);
            else sprintf(strbuff, "%.8s", *node->data.name);

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

        if (pos >= (int) currentNode->children.size()) continue;
        else if (pos < 0) continue;

        Node<File>* node = currentNode->children.at(pos);

        if (node->data.attrib & (AM_DIR)) sprintf(strbuff, "/%.7s", *node->data.name);
        else sprintf(strbuff, "%.8s", *node->data.name);
        
        WriteString(display, strbuff, x, y, MAIN);
        y += CHAR_HEIGHT + 8; 
    }
    
    /**
     * Drawing Instruments
     */
    display->drawLine(320 - 80, 0, 320-80, 240, ACCENT2);
    display->drawFillRect(240, 0, 80, 21, ACCENT2);
    sprintf(strbuff, "LOADED");
    WriteString(display, strbuff, 320 - 76, CHAR_HEIGHT + 3, MAIN);
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
