//
//  SpriteAnimation.h
//  rvm
//

#ifndef SpriteAnimation_h
#define SpriteAnimation_h

struct SpriteAnimation {
    char name[16];
    unsigned char numFrames;
    unsigned char animationSpeed;
    unsigned char loopPosition;
    unsigned char rotationFlag;
    int frameListOffset;
};

#endif /* SpriteAnimation_h */
