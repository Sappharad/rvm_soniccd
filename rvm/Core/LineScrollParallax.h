//
//  LineScrollParallax.h
//  rvm
//

#ifndef LineScrollParallax_h
#define LineScrollParallax_h

struct LineScrollParallax {
    int parallaxFactor[256];
    int scrollSpeed[256];
    int scrollPosition[256];
    int linePos[256];
    uint8_t deformationEnabled[256];
    uint8_t numEntries;
};

#endif /* LineScrollParallax_h */
