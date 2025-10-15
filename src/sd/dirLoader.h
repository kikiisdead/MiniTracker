#pragma once

#ifndef DIR_LOADER_H
#define DIR_LOADER_H

#include "node.h"
#include "fatfs.h"
#include <string>
#include <cstring>

/**
 * Calls Open Dir at beginning of program to search through SD Card and build a tree
 * Maybe need to think of where it is being stored
 */

/**
 * The File struct is used to store a pointer to the name of the file in SDRAM
 * and the attribute of the file (whether dir, hid, fil, etc.)
 */
struct File {
    char (*name)[256];
    BYTE attrib;
};

/**
 * The DirLoader reads the directories of the SDCard at program start and
 * builds a tree based on the file directory
 * 
 * This allows for easy searching, displaying, saving, and loading to the SD Card
 * 
 * @author Kiyoko Iuchi-Fung
 * @version 0.1.0
 */
class DirLoader {
public:
    DirLoader(){}
    ~DirLoader(){}

    /**
     * Initializes the dir loader object
     * @param search_buffer_allocate a function callback that allocates space in SDRAM
     * @param path the initial root path to be explored in the SD card
     */
    void Init(void* (*search_buffer_allocate)(size_t size), const char* path);

    /**
     * Get Root Node of tree
     * @return the root node
     */
    Node<File>* GetRootNode() { return &rootNode; }

    /**
     * Search for node by its name
     * @param name the name of the node to be retrieved
     * @param dir node of whose children are being searched
     * @return corresponding node
     */
    Node<File>* GetNode(std::string name, Node<File>* dir);

private:
    Node<File>  rootNode;       /**< The root node of the file directory */
    bool        safe = false;   /**< Safe toggle */

    /**
     * A callback that allocates space within SDRAM for file names 
     * used because file names are big and were clogging SRAM
     * @param size the size in bytes of the file name to be loaded
     * @return a void pointer to the start of the memory allocated
     */
    void* (*search_buffer_allocate)(size_t size);

    /**
     * Recursively searches through the SDCard to extract all file info and put into TREE
     * @param path the path to the directory being opened
     * @param node the current node being modified and added to
     */
    void OpenDir(const char* path, Node<File>* node);
};

#endif