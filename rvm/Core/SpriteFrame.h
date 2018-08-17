//
//  SpriteFrame.h
//  rvm
//

#ifndef SpriteFrame_h
#define SpriteFrame_h

struct SpriteFrame {
    int left;
    int top;
    int xSize;
    int ySize;
    int xPivot;
    int yPivot;
    unsigned char surfaceNum;
    unsigned char collisionBox;
};

#endif /* SpriteFrame_h */
