//
//  ObjectScript.h
//  rvm
//

#ifndef ObjectScript_h
#define ObjectScript_h

#include "AnimationFileList.h"

struct ObjectScript{
    int numFrames;
    uint8_t surfaceNum;
    int mainScript;
    int playerScript;
    int drawScript;
    int startupScript;
    int mainJumpTable;
    int playerJumpTable;
    int drawJumpTable;
    int startupJumpTable;
    int frameListOffset;
    struct AnimationFileList* animationFile;
};

#endif /* ObjectScript_h */
