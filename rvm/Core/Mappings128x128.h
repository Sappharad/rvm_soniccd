//
//  Mappings128x128.h
//  rvm
//

#ifndef Mappings128x128_h
#define Mappings128x128_h

struct Mappings128x128 {
    int gfxDataPos[0x8000];
    unsigned short tile16x16[0x8000];
    uint8_t direction[0x8000];
    uint8_t visualPlane[0x8000];
    uint8_t collisionFlag[2][0x8000];
};

#endif /* Mappings128x128_h */
