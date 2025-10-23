#pragma once

#ifndef SONG_DISPLAY_H
#define SONG_DISPLAY_H

#include "buttonInterface.h"
#include "sequencer.h"
#include "../sd/dirLoader.h"
#include "../sd/projSaverLoader.h"
#include "../globals.h"

/**
 * A UI screen that displays the song settings and project saving / loading
 * Implements the buttonInterface to allow for user input
 * 
 * @author Kiyoko Iuchi-Fung
 * @version 0.1.0
 */
class SongDisplay : public buttonInterface {

    /**
     * Internal class to create key board letters
     * 
     * @author Kiyoko Iuchi-Fung
     * @version 0.1.0
     */
    class Letter {
    public:
        Letter(const char* dataLow, const char* dataHigh, int size) {
            this->dataLow = dataLow;
            this->dataHigh = dataHigh;
            selected = false;
            capital = false;
            this->size = size;
        }

        const char* GetVal() {
            if (capital) return dataHigh;
            else return dataLow;
        }

        void Display(cLayer* display, cFont* MainFont, int x, int y) {

            int width = 24 * size;
            if (selected) display->drawFillRect(x, y, width + 1, 25, ACCENT2);
            display->drawRect(x, y, width + 1, 25, 1, MAIN);

            int len = strlen(dataLow);
            int xOffset = (width / 2) - ((len * CHAR_WIDTH) / 2);
            int yOffset = CHAR_HEIGHT + ((25 / 2) - (CHAR_HEIGHT / 2));

            display->setCursor(x + xOffset, y + yOffset);
            display->setFont(MainFont);
        display->setTextFrontColor(MAIN);
        if (capital) display->drawText(dataHigh);
        else display->drawText(dataLow);
        }

        const char* dataLow;
        const char* dataHigh;
        bool selected;
        bool capital;
        int size;
    };

public:
    SongDisplay(){}
    ~SongDisplay(){}

    /**
     * Possible parameters that can be edited based on user input
     */
    enum PARAM { SAVE, SAVEAS, LOAD, VOL, TEMPO };

    /**
     * Initialize the Song Display
     */
    void Init(Sequencer *sequencer, Node<File>* node, ProjSaverLoader* projSaverLoader, cFont* MainFont, void* (*search_buffer_allocate)(size_t size));

    /**
     * Process to apply gain
     */
    void Process(float& left, float& right);

    /**
     * INHERITED FROM BUTTON INTERFACE
     */

    /**
     * Initiates selected action either
     * Save, Save As, or Load
     * 
     * @note in SAVE AS, adds selected character to file name
     * @note in LOAD, opens selected project
     */
    void AButton() override;

    /**
     * Initiates selected deselect action
     * 
     * @note in SAVE AS, closes the save as window
     * @note in LOAD, moves selected DIR down a level
     */
    void BButton() override;

    /**
     * Increments selected param
     * 
     * @note in BPM, increments BPM
     * @note in VOL, increments VOL
     */
    void UpButton() override;

    /**
     * Decrements selected param
     * 
     * @note in BPM, decrements BPM
     * @note in VOL, decrements VOL
     */
    void DownButton() override;

    /**
     * Moves parameter select left
     */
    void LeftButton() override;

    /**
     * Moves parameter select right
     */
    void RightButton() override;

    /**
     * Updates display
     * 
     * Displays SAVE, SAVE AS, LOAD, BPM, and VOL along bottom  
     * @note if in SAVE AS, displays a keyboard to write in projName  
     * @note if in LOAD, displays save path file directory  
     * @note displays sliders for BPM and VOL along right
     * 
     * @param display the display to be written to
     */
    void UpdateDisplay(cLayer* display);

private:
    Sequencer          *sequencer;          /**< Pointer to the sequencer object (should only be one) */
    float               vol;                /**< The output volume */
    char                strbuff[256];       /**< Used to write strings to be displayed */
    PARAM               param;              /**< The parameter selected to be edited / modified */
    ProjSaverLoader    *projSaverLoader;    /**< Pointer to the saving and loading object */
    std::string         projName;           /**< A string that contains the current project name */
    Node<File>*         currentNode;        /**< The current node in the file directory that is being observed */
    Node<File>*         rootNode;           /**< The root node of the file directory that this object has access to */
    Letter*             keyboard[4][9];     /**< an array for the keyboard object */
    std::string         tempName;           /**< Temporary name used to display name to be saved */

    /**
     * A function callback to allocate space for a new file name in the file buffer
     * @param size the size in bytes to be allocated for the name
     * @return a void pointer to the start of the allocated memory
     */
    void* (*search_buffer_allocate)(size_t size);

    int     keyV;   /**< vertical position of cursor in keyboard */
    int     keyH;   /**< horizontal position of cursor in keyboard */
    bool    load;   /**< Load toggle */
    bool    saveas; /**< Save as toggle */
    int     row;    /**< Row of current selected node */
    int     col;    /**< Column of current selected node */
    int     lev;    /**< Selected column relative to screen */
    int     scrRow; /**< Selected row relative to screen */

    /**
     * Increments the parameter to be edited
     * Called by user input
     */
    void NextParam();

    /**
     * Decrements the parameter to be edited
     * Called by user input
     */
    void PrevParam();

    /**
     * Increments the selected parameter
     * Called by user input
     */
    void Increment();

    /**
     * Decrements the selected parameter
     * Called by user input
     */
    void Decrement();  
};


#endif