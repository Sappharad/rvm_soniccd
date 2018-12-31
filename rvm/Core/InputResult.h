//
//  InputResult.h
//  rvm
//

#ifndef InputResult_h
#define InputResult_h
#include "SDL.h"

struct InputResult {
    uint8_t up;
    uint8_t down;
    uint8_t left;
    uint8_t right;
    uint8_t buttonA;
    uint8_t buttonB;
    uint8_t buttonC;
    uint8_t start;
    uint8_t touchDown[4];
    int touchX[4];
    int touchY[4];
    int touchID[4];
    int touches;
};

#endif /* InputResult_h */
