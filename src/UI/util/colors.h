#pragma once
#ifndef COLORS_H
#define COLORS_H

#define LIGHTMODE 1
// #define DARKMODE 1

#define CHAR_WIDTH 9
#define CHAR_HEIGHT 12

#ifdef DARKMODE

#define MAIN sColor(255, 255, 255, 255)
#define ALTMAIN sColor(130, 130, 130, 255)

#define ACCENT1 sColor(0, 255, 255, 255)
#define ALTACCENT1 sColor(103, 145, 191, 255)

#define ACCENT2 sColor(255, 0, 30, 255)
#define ALTACCENT2 sColor(176, 44, 75, 255)

#define BACKGROUND sColor(0, 0, 0, 255)
#define ALTBACKGROUND sColor(48, 48, 48, 255)

#endif

#ifdef LIGHTMODE

#define MAIN sColor(51, 50, 47, 255)
#define ALTMAIN sColor(51, 50, 47, 255)

#define ACCENT1 sColor(217, 66, 147, 255)
#define ALTACCENT1 sColor(103, 145, 191, 255)

#define ACCENT2 sColor(110, 171, 22, 255)
#define ALTACCENT2 sColor(221, 235, 215, 255)

#define BACKGROUND sColor(255, 251, 240, 255)
#define ALTBACKGROUND sColor(255, 251, 240, 255)

#endif

#endif