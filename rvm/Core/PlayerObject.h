//
//  PlayerObject.h
//  rvm
//

#ifndef PlayerObject_h
#define PlayerObject_h

#include "SDL.h"
#include "PlayerStatistics.h"
#include "AnimationFileList.h"
#include "ObjectEntity.h"

struct PlayerObject {
    int objectNum;
    int xPos;
    int yPos;
    int xVelocity;
    int yVelocity;
    int speed;
    int screenXPos;
    int screenYPos;
    int angle;
    int timer;
    int lookPos;
    int value[8];
    uint8_t collisionMode;
    uint8_t skidding;
    uint8_t pushing;
    uint8_t collisionPlane;
    signed char controlMode;
    uint8_t controlLock;
    struct PlayerStatistics movementStats;
    uint8_t visible;
    uint8_t tileCollisions;
    uint8_t objectInteraction;
    uint8_t left;
    uint8_t right;
    uint8_t up;
    uint8_t down;
    uint8_t jumpPress;
    uint8_t jumpHold;
    uint8_t followPlayer1;
    uint8_t trackScroll;
    uint8_t gravity;
    uint8_t water;
    uint8_t flailing[3];
    struct AnimationFileList* animationFile;
    struct ObjectEntity* objectPtr;
};

#endif /* PlayerObject_h */
