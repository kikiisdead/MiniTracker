#include "dirLoader.h"

void DirLoader::Init(void* (*search_buffer_allocate)(size_t size), const char* path) {
    safe = true;
    rootNode.parent = nullptr;
    this->search_buffer_allocate = search_buffer_allocate;

    void* ptr = this->search_buffer_allocate(256);
    rootNode.data.name = (char (*)[256]) ptr;

    sprintf(*rootNode.data.name, path);
    OpenDir(*rootNode.data.name, &rootNode);
}

Node<File>* DirLoader::GetNode(std::string name, Node<File>* dir) {

    for (Node<File> *child : dir->children) {
        if (name.compare(std::string(*child->data.name)) == 0) {
            return child;
        } else {
            return dir;
        }
    }

    return dir;

}

void DirLoader::OpenDir(const char* loadpath, Node<File>* node) {
    if (!safe) return; // does nothing if f_mount failed

    DIR            dir; // exist locally for each recursive call as opposed to globally
    FILINFO        fno;

    if (f_opendir(&dir, loadpath) != FR_OK) {
        safe = false;
        return;
    }
    
    bool result;
    do
    {
        result = f_readdir(&dir, &fno);
        // Exit if bad read or NULL fname
        if(result != FR_OK || fno.fname[0] == 0)
            break;

        // Skip if its a hidden file.
        if(fno.fattrib & (AM_HID))
            continue;

        int pos = 0; 
        if (fno.fattrib & (AM_DIR)) {
            node->children.insert(node->children.begin(), new Node<File>); 
        } else {
            node->children.push_back(new Node<File>); 
            pos = node->children.size() - 1;
        }


        node->children.at(pos)->parent = node;

        void* ptr = search_buffer_allocate(256); // allocating space for file names in SDRAM
        node->children.at(pos)->data.name = (char (*)[256]) ptr; // casting to char array of size 256

        sprintf(*node->children.at(pos)->data.name, fno.fname);
        node->children.at(pos)->data.attrib = fno.fattrib;

        if (node->children.at(pos)->data.attrib & (AM_DIR)) {

            std::string path = std::string(*node->children.at(pos)->data.name);

            Node<File> *temp = node;

            while (temp != nullptr) {
                path = std::string(*temp->data.name) + std::string("/") + path;
                temp = temp->parent;
            }

            OpenDir(path.c_str(), node->children.at(pos));

        } else {
            node->children.at(pos)->children.push_back(nullptr);
        }

    } while(result == FR_OK);

    f_closedir(&dir);
}