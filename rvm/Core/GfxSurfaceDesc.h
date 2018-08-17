//
//  GfxSurfaceDesc.h
//  rvm
//

#ifndef GfxSurfaceDesc_h
#define GfxSurfaceDesc_h

struct GfxSurfaceDesc{
    char fileName[64];
    int width;
    int height;
    int texStartX;
    int texStartY;
    unsigned int dataStart;
};

#endif /* GfxSurfaceDesc_h */
