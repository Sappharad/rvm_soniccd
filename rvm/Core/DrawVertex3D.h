//
//  DrawVertex3D.h
//  rvm
//

#ifndef DrawVertex3D_h
#define DrawVertex3D_h

#include "DrawVertex.h"

struct Vector3 {
    float X;
    float Y;
    float Z;
};

struct DrawVertex3D {
    struct Vector3 position;
    struct Vector2 texCoord;
    struct Color color;
};

#endif /* DrawVertex3D_h */
