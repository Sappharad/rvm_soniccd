//
//  DrawVertex.h
//  rvm
//

#ifndef DrawVertex_h
#define DrawVertex_h
#include "SDL.h"

struct Vector2 {
    short X;
    short Y;
};

struct Color {
    uint8_t R;
    uint8_t G;
    uint8_t B;
    uint8_t A;
};

struct DrawVertex {
    struct Vector2 position;
    struct Vector2 texCoord;
    struct Color color;
};

#endif /* DrawVertex_h */
