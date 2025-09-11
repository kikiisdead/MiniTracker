#include "daisy.h"
#include <cmath>

const double EulerConstant = std::exp(1.0);

extern const int16_t ulawDecodeTable[256];

#define A_LAW_CONST 87.6 // const used in ALaw decoding

inline int16_t MuLaw2Lin(uint8_t u_val) {
    return ulawDecodeTable[u_val];
}

inline int16_t ALaw2Lin(uint8_t a_val) {
    double scale = (static_cast<double>(a_val) / static_cast<double>(0x8F)) - 1.0f; // scaling between -1 and 1
    double val = 0;
    if (abs(scale) < 1.0f / (1.0f + log(A_LAW_CONST))) {
        val = (scale * (1.0f + log(A_LAW_CONST)))/A_LAW_CONST;
    } else if (1 >= scale) {
        val = pow(EulerConstant, (-1.0f + abs(scale) * (1.0f + log(A_LAW_CONST)))) / A_LAW_CONST;
    } else if (-1 <= scale) {
        val = -1 * pow(EulerConstant, (-1.0f + abs(scale) * (1.0f + log(A_LAW_CONST)))) / A_LAW_CONST;
    }
    return static_cast<int16_t>(val * (double) 0x8FFF);
}

