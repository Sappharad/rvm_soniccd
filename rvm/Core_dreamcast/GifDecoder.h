//
//  GifDecoder.h
//  rvm
//

#ifndef GifDecoder_h
#define GifDecoder_h

#include <stdint.h>

struct GifDecoder {
    int depth;
    int clearCode;
    int eofCode;
    int runningCode;
    int runningBits;
    int prevCode;
    int currentCode;
    int maxCodePlusOne;
    int stackPtr;
    int shiftState;
    int fileState;
    int position;
    int bufferSize;
    unsigned int shiftData;
    unsigned int pixelCount;
    uint8_t buffer[256];
    uint8_t stack[4096];
    uint8_t suffix[4096];
    unsigned int prefix[4096];
};

#endif /* GifDecoder_h */
