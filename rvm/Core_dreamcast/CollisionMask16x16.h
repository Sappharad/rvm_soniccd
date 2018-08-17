//
//  CollisionMask16x16.h
//  rvm
//

#ifndef CollisionMask16x16_h
#define CollisionMask16x16_h

struct CollisionMask16x16 {
    signed char floorMask[0x4000];
    signed char leftWallMask[0x4000];
    signed char rightWallMask[0x4000];
    signed char roofMask[0x4000];
    unsigned int angle[0x400];
    unsigned char flags[0x400];
};

#endif /* CollisionMask16x16_h */
