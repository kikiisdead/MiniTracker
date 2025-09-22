#pragma once

#ifndef NODE_H
#define NODE_H

/**
 * Simple Tree made to store all the sdcard info into multiple levels
 * DOES NOT NEED TO BE A BALANCED OR SORTED TREE AS THERE IS NO SEARCHING ONLY ACCESSING
 */

template <typename T> class Node {
public:
    Node(){}
    ~Node(){}

    std::vector<Node*> down; // Children
    Node *up; // Parent
    T data;

};

#endif