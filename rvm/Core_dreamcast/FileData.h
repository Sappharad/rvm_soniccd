//
//  FileData.h
//  rvm
//

#ifndef FileData_h
#define FileData_h
#include <stdbool.h>

struct FileData {
    char fileName[64];
    unsigned int fileSize;
    unsigned int position;
    unsigned int bufferPos;
    unsigned int virtualFileOffset;
    unsigned char eStringPosA;
    unsigned char eStringPosB;
    unsigned char eStringNo;
    bool eNybbleSwap;
    bool inRsdkFile;
};

#endif /* FileData_h */
