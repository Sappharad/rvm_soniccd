//
//  TextMenu.h
//  rvm
//

#ifndef TextMenu_h
#define TextMenu_h

struct TextMenu {
    char textData[10240];
    int entryStart[512];
    int entrySize[512];
    unsigned char entryHighlight[512];
    int textDataPos;
    int selection1;
    int selection2;
    unsigned short numRows;
    unsigned short numVisibleRows;
    unsigned short visibleRowOffset;
    unsigned char alignment;
    unsigned char numSelections;
    signed char timer;
};

#endif /* TextMenu_h */
