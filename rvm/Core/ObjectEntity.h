//
//  ObjectEntity.h
//

#ifndef ObjectEntity_h
#define ObjectEntity_h

struct ObjectEntity {
    int xPos;
    int yPos;
    int value[8];
    int scale;
    int rotation;
    int animationTimer;
    int animationSpeed;
    unsigned char type;
    unsigned char propertyValue;
    unsigned char state;
    unsigned char priority;
    unsigned char drawOrder;
    unsigned char direction;
    unsigned char inkEffect;
    unsigned char alpha;
    unsigned char animation;
    unsigned char prevAnimation;
    unsigned char frame;
};

#endif /* ObjectEntity_h */
