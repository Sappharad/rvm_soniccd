//
//  GifLoader.h
//  rvm
//

#ifndef GifLoader_h
#define GifLoader_h

#include <stdio.h>
#include <stdbool.h>
#include "GifDecoder.h"
#include "FileIO.h"

void Init_GifDecoder();
void GifLoader_ReadGifPictureData(int width, int height, bool interlaced, uint8_t* gfxData, int offset);
void GifLoader_ReadGifLine(uint8_t* line, int length, int offset);

#endif /* GifLoader_h */
