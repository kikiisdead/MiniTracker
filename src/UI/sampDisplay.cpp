#include "sampDisplay.h"

void SampDisplay::Init(WaveFileLoader* fileLoader, std::vector<Instrument*>* instruments, cFont *MainFont, size_t* bufferIndex, Node<File>* rootNode, void (*sample_buffer_deallocate)(void* start, size_t size)) {
    this->fileLoader = fileLoader;
    this->instruments = instruments;
    this->MainFont = MainFont;
    this->bufferIndex = bufferIndex;
    currentNode = rootNode;
    this->sample_buffer_deallocate = sample_buffer_deallocate;
    row = 0;
    scrRow = 0;
    col = 0;
    lev = 0;
    selectedInst = 0;
    instRow = 0;
}

void SampDisplay::Init(Config config) {
    this->fileLoader = config.fileLoader;
    this->instruments = config.instruments;
    this->MainFont = config.MainFont;
    this->bufferIndex = config.bufferIndex;
    currentNode = config.rootNode;
    this->sample_buffer_deallocate = config.sample_buffer_deallocate;
    row = 0;
    scrRow = 0;
    col = 0;
    lev = 0;
    instRow = 0;
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
        if (inst != nullptr) {
            instruments->push_back(inst);
            selectedInst = instruments->size() - 1;
            if (selectedInst >= 11) instRow = 10;
            else instRow = selectedInst;
        }

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

void SampDisplay::AltBButton() {
    if (instruments->size() == 0)
        return;
    
    DeleteInstrument(selectedInst);

    if (selectedInst >= instruments->size()) 
        selectedInst = instruments->size() - 1;
}

void SampDisplay::AltUpButton() {
    if (instruments->size() == 0)
        return;
    
    selectedInst -= 1;
    if (selectedInst < 0) {
        selectedInst = 0;
    }

    instRow -= 1;
    if (instRow < 0) {
        instRow = 0;
    }
}

void SampDisplay::AltDownButton() {
    if (instruments->size() == 0)
        return;

    selectedInst += 1;
    if (selectedInst >= instruments->size()) {
        selectedInst = instruments->size() - 1;
    }

    instRow += 1;
    if (instRow >= (int) instruments->size()) {
        instRow -= 1;
    } else if (instRow >= 11) {
        instRow = 11 - 1;
    }
    
}

void SampDisplay::UpdateDisplay(cLayer* display){
    display->eraseLayer(BACKGROUND);

    double sdramUsage = (double) *bufferIndex / (double) MAX_BUFFER_SIZE;

    display->drawLine(0, 20, 240, 20, ACCENT2);

    display->drawFillRect(0, 0, sdramUsage * 240.0f, 20, ACCENT2);

    WriteString(display, 4, CHAR_HEIGHT + 3, MAIN, "MEM USAGE");


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
                if (node == currentNode->parent) {
                    display->drawFillRect(x - 3, y - (CHAR_HEIGHT + 2), 79, CHAR_HEIGHT + 8, ACCENT2);
                }

                if (node->data.attrib & (AM_DIR)) WriteString(display, x, y, MAIN, "/%.7s", *node->data.name);
                else WriteString(display, x, y, MAIN, "%.8s", *node->data.name);

                y += CHAR_HEIGHT + 8;
            }
            x += 80;
        }

        y = CHAR_HEIGHT + 4 + 18;
        for (Node<File>* node : currentNode->parent->children) {
            if (node == currentNode) {
                display->drawFillRect(x - 3, y - (CHAR_HEIGHT + 2), 79, CHAR_HEIGHT + 8, ACCENT2);
            }

            if (node->data.attrib & (AM_DIR)) WriteString(display, x, y, MAIN, "/%.7s", *node->data.name);
            else WriteString(display, x, y, MAIN, "%.8s", *node->data.name);

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

        if (node->data.attrib & (AM_DIR)) WriteString(display, x, y, MAIN, "/%.7s", *node->data.name);
        else WriteString(display, x, y, MAIN, "%.8s", *node->data.name);
        
        y += CHAR_HEIGHT + 8; 
    }
    
    /**
     * Drawing Instruments
     */
    if ((int) instruments->size() < instRow) {
        instRow = instruments->size() - 1;
    }

    
    display->drawLine(320 - 80, 0, 320-80, 240, ACCENT2);
    display->drawFillRect(240, 0, 80, 21, ACCENT2);
    WriteString(display, 320 - 76, CHAR_HEIGHT + 3, MAIN, "LOADED");
    int yOffset = CHAR_HEIGHT + 4 + CHAR_HEIGHT + 8;
    if (!instruments->empty()) {
        for (int i = 0; i < 11; i++) {
            int pos = selectedInst + (i - instRow);
            if (pos >= (int) instruments->size() || pos < 0)
                continue;
            if (i == instRow) {
                WriteString(display, WAVEWIDTH + 4, yOffset, ACCENT1, "%d.%.8s", pos, instruments->at(pos)->GetName());
            } else {
                WriteString(display, WAVEWIDTH + 4, yOffset, MAIN, "%d.%.8s", pos, instruments->at(pos)->GetName());
            }
            yOffset += CHAR_HEIGHT + 8;
        }
    }
}

void SampDisplay::DeleteInstrument(int index) {
    Instrument* inst = instruments->at(index);

    size_t size = inst->GetSize() * sizeof(float);

    sample_buffer_deallocate(inst->GetStart(), size);

    instruments->erase(instruments->begin() + index);

    auto it = instruments->begin() + index;

    while (it != instruments->end()) {
        void* newStart = static_cast<uint8_t*>((*it)->GetStart()) - size;
        (*it)->SetStart(newStart); // update pointers to samples
        it++;
    }

    delete inst; // finally delete the old instrument object
}
