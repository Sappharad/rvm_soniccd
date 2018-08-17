//
//  AnimationFileList.h
//  rvm
//

#ifndef AnimationFileList_h
#define AnimationFileList_h

struct AnimationFileList {
    char fileName[32];
    int numAnimations;
    int aniListOffset;
    int cbListOffset;
};

#endif /* AnimationFileList_h */
