#include "daisy.h"

extern const int16_t ulawDecodeTable[256];

inline int16_t MuLaw2Lin(uint8_t u_val) {
    return ulawDecodeTable[u_val];
}

