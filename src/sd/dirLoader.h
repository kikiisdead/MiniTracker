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
 * WAVEFILELOADER
 * loads wave files from SD Card into SDRAM
 * will return an instrument object with WavFile struct (file info + file name + void pointer to start index in SDRAM)
 */

struct File {
    char (*name)[256];
    BYTE attrib;
};

class DirLoader {
public:
    DirLoader(){}
    ~DirLoader(){}

    /**
     * Initializes the wave file object
     */
    void Init(void* (*search_buffer_allocate)(size_t size), const char* path);

    /**
     * Get Root Node of tree
     * @return the root node
     */
    Node<File>* GetRootNode() { return &rootNode; }

    /**
     * Search for node by its name
     * @return corresponding node
     */
    Node<File>* GetNode(std::string name, Node<File>* dir);

private:
    Node<File> rootNode;
    bool safe = false;
    void* (*search_buffer_allocate)(size_t size);

    /**
     * Recursively searches through the SDCard to extract all file info and put into TREE
     * @param path the path to the directory being opened
     * @param node the current node being modified and added to
     */
    void OpenDir(const char* path, Node<File>* node);
};

#endif