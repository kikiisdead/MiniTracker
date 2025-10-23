/**
 * This file contains global variables that need to be accessed throughout the program. 
 * Created here to avoid adding yet another thing to my bloated constructors
 */

#pragma once
#ifndef GLOBALS_H
#define GLOBALS_H

/**
 * A static global variable so that I don't need to keep passing a
 * reference to the project BPM. Will exist through out program
 */
static float BPM = 178.0f;

#endif