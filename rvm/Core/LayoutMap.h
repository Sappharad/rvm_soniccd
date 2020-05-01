//
//  LayoutMap.h
//  rvm
//

#ifndef LayoutMap_h
#define LayoutMap_h
#ifdef WINDOWS
#define ushort unsigned short
#endif

struct LayoutMap {
    uint16_t tileMap[0x10000];
    uint8_t lineScrollRef[0x8000];
    int parallaxFactor;
    int scrollSpeed;
    int scrollPosition;
    int angle;
    int xPos;
    int yPos;
    int zPos;
    int deformationPos;
    int deformationPosW;
    uint8_t type;
    uint8_t xSize;
    uint8_t ySize;
};

#endif /* LayoutMap_h */
