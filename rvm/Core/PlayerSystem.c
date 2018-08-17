//
//  PlayerSystem.c
//  rvm
//

#include "PlayerSystem.h"

unsigned short delayLeft;
unsigned short delayRight;
unsigned short delayUp;
unsigned short delayDown;
unsigned short delayJumpPress;
unsigned short delayJumpHold;
unsigned char jumpWait;
unsigned char numActivePlayers;
unsigned char playerMenuNum;
struct PlayerObject playerList[2];
struct CollisionSensor player_cSensor[6];
int collisionLeft;
int collisionTop;
int collisionRight;
int collisionBottom;

void Init_PlayerSystem()
{
    numActivePlayers = 1;
    playerMenuNum = 0;
}

void PlayerSystem_FindFloorPosition(struct PlayerObject* playerO, struct CollisionSensor* cSensorRef, int prevCollisionPos)
{
    int num;
    int num1 = cSensorRef->angle;
    for (int i = 0; i < 48; i = i + 16)
    {
        if (cSensorRef->collided == 0)
        {
            int num2 = cSensorRef->xPos >> 16;
            int num3 = num2 >> 7;
            int num4 = (num2 & 127) >> 4;
            int num5 = (cSensorRef->yPos >> 16) - 16 + i;
            int num6 = num5 >> 7;
            int num7 = (num5 & 127) >> 4;
            if (num2 > -1 && num5 > -1)
            {
                int num8 = stageLayouts[0].tileMap[num3 + (num6 << 8)] << 6;
                num8 = num8 + num4 + (num7 << 3);
                int num9 = tile128x128.tile16x16[num8];
                if (tile128x128.collisionFlag[playerO->collisionPlane][num8] != 2 && tile128x128.collisionFlag[playerO->collisionPlane][num8] != 3)
                {
                    switch (tile128x128.direction[num8])
                    {
                        case 0:
                        {
                            num = (num2 & 15) + (num9 << 4);
                            if (tileCollisions[playerO->collisionPlane].floorMask[num] >= 64)
                            {
                                break;
                            }
                            cSensorRef->yPos = tileCollisions[playerO->collisionPlane].floorMask[num] + (num6 << 7) + (num7 << 4);
                            cSensorRef->collided = 1;
                            cSensorRef->angle = (int)(tileCollisions[playerO->collisionPlane].angle[num9] & 0xff);
                            break;
                        }
                        case 1:
                        {
                            num = 15 - (num2 & 15) + (num9 << 4);
                            if (tileCollisions[playerO->collisionPlane].floorMask[num] >= 64)
                            {
                                break;
                            }
                            cSensorRef->yPos = tileCollisions[playerO->collisionPlane].floorMask[num] + (num6 << 7) + (num7 << 4);
                            cSensorRef->collided = 1;
                            cSensorRef->angle = (int)(0x100 - (tileCollisions[playerO->collisionPlane].angle[num9] & 0xff));
                            break;
                        }
                        case 2:
                        {
                            num = (num2 & 15) + (num9 << 4);
                            if (tileCollisions[playerO->collisionPlane].roofMask[num] <= -64)
                            {
                                break;
                            }
                            cSensorRef->yPos = 15 - tileCollisions[playerO->collisionPlane].roofMask[num] + (num6 << 7) + (num7 << 4);
                            cSensorRef->collided = 1;
                            cSensorRef->angle = (int)(0x180 - ((tileCollisions[playerO->collisionPlane].angle[num9] & -16777216) >> 24) & 0xff);
                            break;
                        }
                        case 3:
                        {
                            num = 15 - (num2 & 15) + (num9 << 4);
                            if (tileCollisions[playerO->collisionPlane].roofMask[num] <= -64)
                            {
                                break;
                            }
                            cSensorRef->yPos = 15 - tileCollisions[playerO->collisionPlane].roofMask[num] + (num6 << 7) + (num7 << 4);
                            cSensorRef->collided = 1;
                            cSensorRef->angle = (int)(0x100 - (0x180 - ((tileCollisions[playerO->collisionPlane].angle[num9] & -16777216) >> 24) & 0xff));
                            break;
                        }
                    }
                }
                if (cSensorRef->collided == 1)
                {
                    if (cSensorRef->angle < 0)
                    {
                        struct CollisionSensor* collisionSensor = cSensorRef;
                        collisionSensor->angle = collisionSensor->angle + 0x100;
                    }
                    if (cSensorRef->angle > 0xff)
                    {
                        struct CollisionSensor* collisionSensor1 = cSensorRef;
                        collisionSensor1->angle = collisionSensor1->angle - 0x100;
                    }
                    if ((abs(cSensorRef->angle - num1) > 32) && (abs(cSensorRef->angle - 0x100 - num1) > 32) && (abs(cSensorRef->angle + 0x100 - num1) > 32))
                    {
                        cSensorRef->yPos = prevCollisionPos << 16;
                        cSensorRef->collided = 0;
                        cSensorRef->angle = num1;
                        i = 48;
                    }
                    else if (cSensorRef->yPos - prevCollisionPos > 14)
                    {
                        cSensorRef->yPos = prevCollisionPos << 16;
                        cSensorRef->collided = 0;
                    }
                    else if (cSensorRef->yPos - prevCollisionPos < -14)
                    {
                        cSensorRef->yPos = prevCollisionPos << 16;
                        cSensorRef->collided = 0;
                    }
                }
            }
        }
    }
}

void PlayerSystem_FindLWallPosition(struct PlayerObject* playerO, struct CollisionSensor* cSensorRef, int prevCollisionPos)
{
    int num;
    int num1 = cSensorRef->angle;
    for (int i = 0; i < 48; i = i + 16)
    {
        if (cSensorRef->collided == 0)
        {
            int num2 = (cSensorRef->xPos >> 16) - 16 + i;
            int num3 = num2 >> 7;
            int num4 = (num2 & 127) >> 4;
            int num5 = cSensorRef->yPos >> 16;
            int num6 = num5 >> 7;
            int num7 = (num5 & 127) >> 4;
            if (num2 > -1 && num5 > -1)
            {
                int num8 = stageLayouts[0].tileMap[num3 + (num6 << 8)] << 6;
                num8 = num8 + num4 + (num7 << 3);
                int num9 = tile128x128.tile16x16[num8];
                if (tile128x128.collisionFlag[playerO->collisionPlane][num8] < 3)
                {
                    switch (tile128x128.direction[num8])
                    {
                        case 0:
                        {
                            num = (num5 & 15) + (num9 << 4);
                            if (tileCollisions[playerO->collisionPlane].leftWallMask[num] >= 64)
                            {
                                break;
                            }
                            cSensorRef->xPos = tileCollisions[playerO->collisionPlane].leftWallMask[num] + (num3 << 7) + (num4 << 4);
                            cSensorRef->collided = 1;
                            cSensorRef->angle = (int)((tileCollisions[playerO->collisionPlane].angle[num9] & 0xff00) >> 8);
                            break;
                        }
                        case 1:
                        {
                            num = (num5 & 15) + (num9 << 4);
                            if (tileCollisions[playerO->collisionPlane].rightWallMask[num] <= -64)
                            {
                                break;
                            }
                            cSensorRef->xPos = 15 - tileCollisions[playerO->collisionPlane].rightWallMask[num] + (num3 << 7) + (num4 << 4);
                            cSensorRef->collided = 1;
                            cSensorRef->angle = (int)(0x100 - ((tileCollisions[playerO->collisionPlane].angle[num9] & 0xff0000) >> 16));
                            break;
                        }
                        case 2:
                        {
                            num = 15 - (num5 & 15) + (num9 << 4);
                            if (tileCollisions[playerO->collisionPlane].leftWallMask[num] >= 64)
                            {
                                break;
                            }
                            cSensorRef->xPos = tileCollisions[playerO->collisionPlane].leftWallMask[num] + (num3 << 7) + (num4 << 4);
                            cSensorRef->collided = 1;
                            cSensorRef->angle = (int)(0x180 - ((tileCollisions[playerO->collisionPlane].angle[num9] & 0xff00) >> 8) & 0xff);
                            break;
                        }
                        case 3:
                        {
                            num = 15 - (num5 & 15) + (num9 << 4);
                            if (tileCollisions[playerO->collisionPlane].rightWallMask[num] <= -64)
                            {
                                break;
                            }
                            cSensorRef->xPos = 15 - tileCollisions[playerO->collisionPlane].rightWallMask[num] + (num3 << 7) + (num4 << 4);
                            cSensorRef->collided = 1;
                            cSensorRef->angle = (int)(0x100 - (0x180 - ((tileCollisions[playerO->collisionPlane].angle[num9] & 0xff0000) >> 16) & 0xff));
                            break;
                        }
                    }
                }
                if (cSensorRef->collided == 1)
                {
                    if (cSensorRef->angle < 0)
                    {
                        struct CollisionSensor* collisionSensor = cSensorRef;
                        collisionSensor->angle = collisionSensor->angle + 0x100;
                    }
                    if (cSensorRef->angle > 0xff)
                    {
                        struct CollisionSensor* collisionSensor1 = cSensorRef;
                        collisionSensor1->angle = collisionSensor1->angle - 0x100;
                    }
                    if (abs(num1 - cSensorRef->angle) > 32)
                    {
                        cSensorRef->xPos = prevCollisionPos << 16;
                        cSensorRef->collided = 0;
                        cSensorRef->angle = num1;
                        i = 48;
                    }
                    else if (cSensorRef->xPos - prevCollisionPos > 14)
                    {
                        cSensorRef->xPos = prevCollisionPos << 16;
                        cSensorRef->collided = 0;
                    }
                    else if (cSensorRef->xPos - prevCollisionPos < -14)
                    {
                        cSensorRef->xPos = prevCollisionPos << 16;
                        cSensorRef->collided = 0;
                    }
                }
            }
        }
    }
}

void PlayerSystem_FindRoofPosition(struct PlayerObject* playerO, struct CollisionSensor* cSensorRef, int prevCollisionPos)
{
    int num;
    int num1 = cSensorRef->angle;
    for (int i = 0; i < 48; i = i + 16)
    {
        if (cSensorRef->collided == 0)
        {
            int num2 = cSensorRef->xPos >> 16;
            int num3 = num2 >> 7;
            int num4 = (num2 & 127) >> 4;
            int num5 = (cSensorRef->yPos >> 16) + 16 - i;
            int num6 = num5 >> 7;
            int num7 = (num5 & 127) >> 4;
            if (num2 > -1 && num5 > -1)
            {
                int num8 = stageLayouts[0].tileMap[num3 + (num6 << 8)] << 6;
                num8 = num8 + num4 + (num7 << 3);
                int num9 = tile128x128.tile16x16[num8];
                if (tile128x128.collisionFlag[playerO->collisionPlane][num8] < 3)
                {
                    switch (tile128x128.direction[num8])
                    {
                        case 0:
                        {
                            num = (num2 & 15) + (num9 << 4);
                            if (tileCollisions[playerO->collisionPlane].roofMask[num] <= -64)
                            {
                                break;
                            }
                            cSensorRef->yPos = tileCollisions[playerO->collisionPlane].roofMask[num] + (num6 << 7) + (num7 << 4);
                            cSensorRef->collided = 1;
                            cSensorRef->angle = (int)((tileCollisions[playerO->collisionPlane].angle[num9] & -16777216) >> 24);
                            break;
                        }
                        case 1:
                        {
                            num = 15 - (num2 & 15) + (num9 << 4);
                            if (tileCollisions[playerO->collisionPlane].roofMask[num] <= -64)
                            {
                                break;
                            }
                            cSensorRef->yPos = tileCollisions[playerO->collisionPlane].roofMask[num] + (num6 << 7) + (num7 << 4);
                            cSensorRef->collided = 1;
                            cSensorRef->angle = (int)(0x100 - ((tileCollisions[playerO->collisionPlane].angle[num9] & -16777216) >> 24));
                            break;
                        }
                        case 2:
                        {
                            num = (num2 & 15) + (num9 << 4);
                            if (tileCollisions[playerO->collisionPlane].floorMask[num] >= 64)
                            {
                                break;
                            }
                            cSensorRef->yPos = 15 - tileCollisions[playerO->collisionPlane].floorMask[num] + (num6 << 7) + (num7 << 4);
                            cSensorRef->collided = 1;
                            cSensorRef->angle = (int)(0x180 - (tileCollisions[playerO->collisionPlane].angle[num9] & 0xff) & 0xff);
                            break;
                        }
                        case 3:
                        {
                            num = 15 - (num2 & 15) + (num9 << 4);
                            if (tileCollisions[playerO->collisionPlane].floorMask[num] >= 64)
                            {
                                break;
                            }
                            cSensorRef->yPos = 15 - tileCollisions[playerO->collisionPlane].floorMask[num] + (num6 << 7) + (num7 << 4);
                            cSensorRef->collided = 1;
                            cSensorRef->angle = (int)(0x100 - (0x180 - (tileCollisions[playerO->collisionPlane].angle[num9] & 0xff) & 0xff));
                            break;
                        }
                    }
                }
                if (cSensorRef->collided == 1)
                {
                    if (cSensorRef->angle < 0)
                    {
                        struct CollisionSensor* collisionSensor = cSensorRef;
                        collisionSensor->angle = collisionSensor->angle + 0x100;
                    }
                    if (cSensorRef->angle > 0xff)
                    {
                        struct CollisionSensor* collisionSensor1 = cSensorRef;
                        collisionSensor1->angle = collisionSensor1->angle - 0x100;
                    }
                    if (abs(cSensorRef->angle - num1) <= 32)
                    {
                        if (cSensorRef->yPos - prevCollisionPos > 15)
                        {
                            cSensorRef->yPos = prevCollisionPos << 16;
                            cSensorRef->collided = 0;
                        }
                        if (cSensorRef->yPos - prevCollisionPos < -15)
                        {
                            cSensorRef->yPos = prevCollisionPos << 16;
                            cSensorRef->collided = 0;
                        }
                    }
                    else
                    {
                        cSensorRef->yPos = prevCollisionPos << 16;
                        cSensorRef->collided = 0;
                        cSensorRef->angle = num1;
                        i = 48;
                    }
                }
            }
        }
    }
}

void PlayerSystem_FindRWallPosition(struct PlayerObject* playerO, struct CollisionSensor* cSensorRef, int prevCollisionPos)
{
    int num;
    int num1 = cSensorRef->angle;
    for (int i = 0; i < 48; i = i + 16)
    {
        if (cSensorRef->collided == 0)
        {
            int num2 = (cSensorRef->xPos >> 16) + 16 - i;
            int num3 = num2 >> 7;
            int num4 = (num2 & 127) >> 4;
            int num5 = cSensorRef->yPos >> 16;
            int num6 = num5 >> 7;
            int num7 = (num5 & 127) >> 4;
            if (num2 > -1 && num5 > -1)
            {
                int num8 = stageLayouts[0].tileMap[num3 + (num6 << 8)] << 6;
                num8 = num8 + num4 + (num7 << 3);
                int num9 = tile128x128.tile16x16[num8];
                if (tile128x128.collisionFlag[playerO->collisionPlane][num8] < 3)
                {
                    switch (tile128x128.direction[num8])
                    {
                        case 0:
                        {
                            num = (num5 & 15) + (num9 << 4);
                            if (tileCollisions[playerO->collisionPlane].rightWallMask[num] <= -64)
                            {
                                break;
                            }
                            cSensorRef->xPos = tileCollisions[playerO->collisionPlane].rightWallMask[num] + (num3 << 7) + (num4 << 4);
                            cSensorRef->collided = 1;
                            cSensorRef->angle = (int)((tileCollisions[playerO->collisionPlane].angle[num9] & 0xff0000) >> 16);
                            break;
                        }
                        case 1:
                        {
                            num = (num5 & 15) + (num9 << 4);
                            if (tileCollisions[playerO->collisionPlane].leftWallMask[num] >= 64)
                            {
                                break;
                            }
                            cSensorRef->xPos = 15 - tileCollisions[playerO->collisionPlane].leftWallMask[num] + (num3 << 7) + (num4 << 4);
                            cSensorRef->collided = 1;
                            cSensorRef->angle = (int)(0x100 - ((tileCollisions[playerO->collisionPlane].angle[num9] & 0xff00) >> 8));
                            break;
                        }
                        case 2:
                        {
                            num = 15 - (num5 & 15) + (num9 << 4);
                            if (tileCollisions[playerO->collisionPlane].rightWallMask[num] <= -64)
                            {
                                break;
                            }
                            cSensorRef->xPos = tileCollisions[playerO->collisionPlane].rightWallMask[num] + (num3 << 7) + (num4 << 4);
                            cSensorRef->collided = 1;
                            cSensorRef->angle = (int)(0x180 - ((tileCollisions[playerO->collisionPlane].angle[num9] & 0xff0000) >> 16) & 0xff);
                            break;
                        }
                        case 3:
                        {
                            num = 15 - (num5 & 15) + (num9 << 4);
                            if (tileCollisions[playerO->collisionPlane].leftWallMask[num] >= 64)
                            {
                                break;
                            }
                            cSensorRef->xPos = 15 - tileCollisions[playerO->collisionPlane].leftWallMask[num] + (num3 << 7) + (num4 << 4);
                            cSensorRef->collided = 1;
                            cSensorRef->angle = (int)(0x100 - (0x180 - ((tileCollisions[playerO->collisionPlane].angle[num9] & 0xff00) >> 8) & 0xff));
                            break;
                        }
                    }
                }
                if (cSensorRef->collided == 1)
                {
                    if (cSensorRef->angle < 0)
                    {
                        struct CollisionSensor* collisionSensor = cSensorRef;
                        collisionSensor->angle = collisionSensor->angle + 0x100;
                    }
                    if (cSensorRef->angle > 0xff)
                    {
                        struct CollisionSensor* collisionSensor1 = cSensorRef;
                        collisionSensor1->angle = collisionSensor1->angle - 0x100;
                    }
                    if (abs(cSensorRef->angle - num1) > 32)
                    {
                        cSensorRef->xPos = prevCollisionPos << 16;
                        cSensorRef->collided = 0;
                        cSensorRef->angle = num1;
                        i = 48;
                    }
                    else if (cSensorRef->xPos - prevCollisionPos > 14)
                    {
                        cSensorRef->xPos = prevCollisionPos >> 16;
                        cSensorRef->collided = 0;
                    }
                    else if (cSensorRef->xPos - prevCollisionPos < -14)
                    {
                        cSensorRef->xPos = prevCollisionPos << 16;
                        cSensorRef->collided = 0;
                    }
                }
            }
        }
    }
}

void PlayerSystem_FloorCollision(struct PlayerObject* playerO, struct CollisionSensor* cSensorRef)
{
    int num;
    int num1 = cSensorRef->yPos >> 16;
    for (int i = 0; i < 48; i = i + 16)
    {
        if (cSensorRef->collided == 0)
        {
            int num2 = cSensorRef->xPos >> 16;
            int num3 = num2 >> 7;
            int num4 = (num2 & 127) >> 4;
            int num5 = (cSensorRef->yPos >> 16) - 16 + i;
            int num6 = num5 >> 7;
            int num7 = (num5 & 127) >> 4;
            if (num2 > -1 && num5 > -1)
            {
                int num8 = stageLayouts[0].tileMap[num3 + (num6 << 8)] << 6;
                num8 = num8 + num4 + (num7 << 3);
                int num9 = tile128x128.tile16x16[num8];
                if (tile128x128.collisionFlag[playerO->collisionPlane][num8] != 2 && tile128x128.collisionFlag[playerO->collisionPlane][num8] != 3)
                {
                    switch (tile128x128.direction[num8])
                    {
                        case 0:
                        {
                            num = (num2 & 15) + (num9 << 4);
                            if ((num5 & 15) <= tileCollisions[playerO->collisionPlane].floorMask[num] - 16 + i || tileCollisions[playerO->collisionPlane].floorMask[num] >= 15)
                            {
                                break;
                            }
                            cSensorRef->yPos = tileCollisions[playerO->collisionPlane].floorMask[num] + (num6 << 7) + (num7 << 4);
                            cSensorRef->collided = 1;
                            cSensorRef->angle = (int)(tileCollisions[playerO->collisionPlane].angle[num9] & 0xff);
                            break;
                        }
                        case 1:
                        {
                            num = 15 - (num2 & 15) + (num9 << 4);
                            if ((num5 & 15) <= tileCollisions[playerO->collisionPlane].floorMask[num] - 16 + i || tileCollisions[playerO->collisionPlane].floorMask[num] >= 15)
                            {
                                break;
                            }
                            cSensorRef->yPos = tileCollisions[playerO->collisionPlane].floorMask[num] + (num6 << 7) + (num7 << 4);
                            cSensorRef->collided = 1;
                            cSensorRef->angle = (int)(0x100 - (tileCollisions[playerO->collisionPlane].angle[num9] & 0xff));
                            break;
                        }
                        case 2:
                        {
                            num = (num2 & 15) + (num9 << 4);
                            if ((num5 & 15) <= 15 - tileCollisions[playerO->collisionPlane].roofMask[num] - 16 + i)
                            {
                                break;
                            }
                            cSensorRef->yPos = 15 - tileCollisions[playerO->collisionPlane].roofMask[num] + (num6 << 7) + (num7 << 4);
                            cSensorRef->collided = 1;
                            cSensorRef->angle = (int)(0x180 - ((tileCollisions[playerO->collisionPlane].angle[num9] & -16777216) >> 24) & 0xff);
                            break;
                        }
                        case 3:
                        {
                            num = 15 - (num2 & 15) + (num9 << 4);
                            if ((num5 & 15) <= 15 - tileCollisions[playerO->collisionPlane].roofMask[num] - 16 + i)
                            {
                                break;
                            }
                            cSensorRef->yPos = 15 - tileCollisions[playerO->collisionPlane].roofMask[num] + (num6 << 7) + (num7 << 4);
                            cSensorRef->collided = 1;
                            cSensorRef->angle = (int)(0x100 - (0x180 - ((tileCollisions[playerO->collisionPlane].angle[num9] & -16777216) >> 24) & 0xff));
                            break;
                        }
                    }
                }
                if (cSensorRef->collided == 1)
                {
                    if (cSensorRef->angle < 0)
                    {
                        struct CollisionSensor* collisionSensor = cSensorRef;
                        collisionSensor->angle = collisionSensor->angle + 0x100;
                    }
                    if (cSensorRef->angle > 0xff)
                    {
                        struct CollisionSensor* collisionSensor1 = cSensorRef;
                        collisionSensor1->angle = collisionSensor1->angle - 0x100;
                    }
                    if (cSensorRef->yPos - num1 > 14)
                    {
                        cSensorRef->yPos = num1 << 16;
                        cSensorRef->collided = 0;
                    }
                    else if (cSensorRef->yPos - num1 < -17)
                    {
                        cSensorRef->yPos = num1 << 16;
                        cSensorRef->collided = 0;
                    }
                }
            }
        }
    }
}

void PlayerSystem_LWallCollision(struct PlayerObject* playerO, struct CollisionSensor* cSensorRef)
{
    int num;
    int num1 = cSensorRef->xPos >> 16;
    for (int i = 0; i < 48; i = i + 16)
    {
        if (cSensorRef->collided == 0)
        {
            int num2 = (cSensorRef->xPos >> 16) - 16 + i;
            int num3 = num2 >> 7;
            int num4 = (num2 & 127) >> 4;
            int num5 = cSensorRef->yPos >> 16;
            int num6 = num5 >> 7;
            int num7 = (num5 & 127) >> 4;
            if (num2 > -1 && num5 > -1)
            {
                int num8 = stageLayouts[0].tileMap[num3 + (num6 << 8)] << 6;
                num8 = num8 + num4 + (num7 << 3);
                int num9 = tile128x128.tile16x16[num8];
                if (tile128x128.collisionFlag[playerO->collisionPlane][num8] != 1 && tile128x128.collisionFlag[playerO->collisionPlane][num8] < 3)
                {
                    switch (tile128x128.direction[num8])
                    {
                        case 0:
                        {
                            num = (num5 & 15) + (num9 << 4);
                            if ((num2 & 15) <= tileCollisions[playerO->collisionPlane].leftWallMask[num] - 16 + i)
                            {
                                break;
                            }
                            cSensorRef->xPos = tileCollisions[playerO->collisionPlane].leftWallMask[num] + (num3 << 7) + (num4 << 4);
                            cSensorRef->collided = 1;
                            break;
                        }
                        case 1:
                        {
                            num = (num5 & 15) + (num9 << 4);
                            if ((num2 & 15) <= 15 - tileCollisions[playerO->collisionPlane].rightWallMask[num] - 16 + i)
                            {
                                break;
                            }
                            cSensorRef->xPos = 15 - tileCollisions[playerO->collisionPlane].rightWallMask[num] + (num3 << 7) + (num4 << 4);
                            cSensorRef->collided = 1;
                            break;
                        }
                        case 2:
                        {
                            num = 15 - (num5 & 15) + (num9 << 4);
                            if ((num2 & 15) <= tileCollisions[playerO->collisionPlane].leftWallMask[num] - 16 + i)
                            {
                                break;
                            }
                            cSensorRef->xPos = tileCollisions[playerO->collisionPlane].leftWallMask[num] + (num3 << 7) + (num4 << 4);
                            cSensorRef->collided = 1;
                            break;
                        }
                        case 3:
                        {
                            num = 15 - (num5 & 15) + (num9 << 4);
                            if ((num2 & 15) <= 15 - tileCollisions[playerO->collisionPlane].rightWallMask[num] - 16 + i)
                            {
                                break;
                            }
                            cSensorRef->xPos = 15 - tileCollisions[playerO->collisionPlane].rightWallMask[num] + (num3 << 7) + (num4 << 4);
                            cSensorRef->collided = 1;
                            break;
                        }
                    }
                }
                if (cSensorRef->collided == 1)
                {
                    if (cSensorRef->xPos - num1 > 15)
                    {
                        cSensorRef->xPos = num1 << 16;
                        cSensorRef->collided = 0;
                    }
                    else if (cSensorRef->xPos - num1 < -15)
                    {
                        cSensorRef->xPos = num1 << 16;
                        cSensorRef->collided = 0;
                    }
                }
            }
        }
    }
}

void PlayerSystem_ProcessAirCollision(struct PlayerObject* playerO)
{
    int num;
    int i;
    uint8_t num1;
    uint8_t num2;
    uint8_t num3;
    struct CollisionBox* collisionBox = &collisionBoxList[playerO->animationFile->cbListOffset + animationFrames[animationList[playerO->animationFile->aniListOffset + playerO->objectPtr->animation].frameListOffset + playerO->objectPtr->frame].collisionBox];
    collisionLeft = collisionBox->left[0];
    collisionTop = collisionBox->top[0];
    collisionRight = collisionBox->right[0];
    collisionBottom = collisionBox->bottom[0];
    if (playerO->xVelocity < 0)
    {
        num1 = 0;
    }
    else
    {
        num1 = 1;
        player_cSensor[0].yPos = playerO->yPos + 0x20000;
        player_cSensor[0].collided = 0;
        player_cSensor[0].xPos = playerO->xPos + (collisionRight << 16);
    }
    if (playerO->xVelocity > 0)
    {
        num2 = 0;
    }
    else
    {
        num2 = 1;
        player_cSensor[1].yPos = playerO->yPos + 0x20000;
        player_cSensor[1].collided = 0;
        player_cSensor[1].xPos = playerO->xPos + ((collisionLeft - 1) << 16);
    }
    player_cSensor[2].xPos = playerO->xPos + (collisionBox->left[1] << 16);
    player_cSensor[3].xPos = playerO->xPos + (collisionBox->right[1] << 16);
    player_cSensor[2].collided = 0;
    player_cSensor[3].collided = 0;
    player_cSensor[4].xPos = player_cSensor[2].xPos;
    player_cSensor[5].xPos = player_cSensor[3].xPos;
    player_cSensor[4].collided = 0;
    player_cSensor[5].collided = 0;
    if (playerO->yVelocity < 0)
    {
        num3 = 0;
    }
    else
    {
        num3 = 1;
        player_cSensor[2].yPos = playerO->yPos + (collisionBottom << 16);
        player_cSensor[3].yPos = playerO->yPos + (collisionBottom << 16);
    }
    uint8_t num4 = 1;
    player_cSensor[4].yPos = playerO->yPos + ((collisionTop - 1) << 16);
    player_cSensor[5].yPos = playerO->yPos + ((collisionTop - 1) << 16);
    num = (abs(playerO->xVelocity) <= abs(playerO->yVelocity) ? (abs(playerO->yVelocity) >> 19) + 1 : (abs(playerO->xVelocity) >> 19) + 1);
    int num5 = playerO->xVelocity / num;
    int num6 = playerO->yVelocity / num;
    int num7 = playerO->xVelocity - num5 * (num - 1);
    int num8 = playerO->yVelocity - num6 * (num - 1);
    while (num > 0)
    {
        if (num < 2)
        {
            num5 = num7;
            num6 = num8;
        }
        num--;
        if (num1 == 1)
        {
            struct CollisionSensor* collisionSensor = &player_cSensor[0];
            collisionSensor->xPos = collisionSensor->xPos + num5 + 0x10000;
            struct CollisionSensor*  collisionSensor1 = &player_cSensor[0];
            collisionSensor1->yPos = collisionSensor1->yPos + num6;
            PlayerSystem_LWallCollision(playerO, &player_cSensor[0]);
            if (player_cSensor[0].collided == 1)
            {
                num1 = 2;
            }
        }
        if (num2 == 1)
        {
            struct CollisionSensor* collisionSensor2 = &player_cSensor[1];
            collisionSensor2->xPos = collisionSensor2->xPos + (num5 - 0x10000);
            struct CollisionSensor* collisionSensor3 = &player_cSensor[1];
            collisionSensor3->yPos = collisionSensor3->yPos + num6;
            PlayerSystem_RWallCollision(playerO, &player_cSensor[1]);
            if (player_cSensor[1].collided == 1)
            {
                num2 = 2;
            }
        }
        if (num1 == 2)
        {
            playerO->xVelocity = 0;
            playerO->speed = 0;
            playerO->xPos = (player_cSensor[0].xPos - collisionRight) << 16;
            player_cSensor[2].xPos = playerO->xPos + ((collisionLeft + 1) << 16);
            player_cSensor[3].xPos = playerO->xPos + ((collisionRight - 2) << 16);
            player_cSensor[4].xPos = player_cSensor[2].xPos;
            player_cSensor[5].xPos = player_cSensor[3].xPos;
            num5 = 0;
            num7 = 0;
            num1 = 3;
        }
        if (num2 == 2)
        {
            playerO->xVelocity = 0;
            playerO->speed = 0;
            playerO->xPos = (player_cSensor[1].xPos - collisionLeft + 1) << 16;
            player_cSensor[2].xPos = playerO->xPos + ((collisionLeft + 1) << 16);
            player_cSensor[3].xPos = playerO->xPos + ((collisionRight - 2) << 16);
            player_cSensor[4].xPos = player_cSensor[2].xPos;
            player_cSensor[5].xPos = player_cSensor[3].xPos;
            num5 = 0;
            num7 = 0;
            num2 = 3;
        }
        if (num3 == 1)
        {
            for (i = 2; i < 4; i++)
            {
                if (player_cSensor[i].collided == 0)
                {
                    struct CollisionSensor* collisionSensor4 = &player_cSensor[i];
                    collisionSensor4->xPos = collisionSensor4->xPos + num5;
                    struct CollisionSensor* collisionSensor5 = &player_cSensor[i];
                    collisionSensor5->yPos = collisionSensor5->yPos + num6;
                    PlayerSystem_FloorCollision(playerO, &player_cSensor[i]);
                }
            }
            if (player_cSensor[2].collided == 1 | player_cSensor[3].collided == 1)
            {
                num3 = 2;
                num = 0;
            }
        }
        if (num4 != 1)
        {
            continue;
        }
        for (i = 4; i < 6; i++)
        {
            if (player_cSensor[i].collided == 0)
            {
                struct CollisionSensor* collisionSensor6 = &player_cSensor[i];
                collisionSensor6->xPos = collisionSensor6->xPos + num5;
                struct CollisionSensor* collisionSensor7 = &player_cSensor[i];
                collisionSensor7->yPos = collisionSensor7->yPos + num6;
                PlayerSystem_RoofCollision(playerO, &player_cSensor[i]);
            }
        }
        if (!(player_cSensor[4].collided == 1 | player_cSensor[5].collided == 1))
        {
            continue;
        }
        num4 = 2;
        num = 0;
    }
    if (num1 < 2 && num2 < 2)
    {
        playerO->xPos = playerO->xPos + playerO->xVelocity;
    }
    if (num4 < 2 && num3 < 2)
    {
        playerO->yPos = playerO->yPos + playerO->yVelocity;
        return;
    }
    if (num3 == 2)
    {
        playerO->gravity = 0;
        if (player_cSensor[2].collided == 1 && player_cSensor[3].collided == 1)
        {
            if (player_cSensor[2].yPos >= player_cSensor[3].yPos)
            {
                playerO->yPos = (player_cSensor[3].yPos - collisionBottom) << 16;
                playerO->angle = player_cSensor[3].angle;
            }
            else
            {
                playerO->yPos = (player_cSensor[2].yPos - collisionBottom) << 16;
                playerO->angle = player_cSensor[2].angle;
            }
        }
        else if (player_cSensor[2].collided == 1)
        {
            playerO->yPos = (player_cSensor[2].yPos - collisionBottom) << 16;
            playerO->angle = player_cSensor[2].angle;
        }
        else if (player_cSensor[3].collided == 1)
        {
            playerO->yPos = (player_cSensor[3].yPos - collisionBottom) << 16;
            playerO->angle = player_cSensor[3].angle;
        }
        if (playerO->angle > 160 && playerO->angle < 224 && playerO->collisionMode != 1)
        {
            playerO->collisionMode = 1;
            playerO->xPos = playerO->xPos - 0x40000;
        }
        if (playerO->angle > 32 && playerO->angle < 96 && playerO->collisionMode != 3)
        {
            playerO->collisionMode = 3;
            playerO->xPos = playerO->xPos + 0x40000;
        }
        if (playerO->angle < 32 | playerO->angle > 224)
        {
            playerO->controlLock = 0;
        }
        playerO->objectPtr->rotation = playerO->angle << 1;
        if (playerO->down == 1)
        {
            if (playerO->angle < 128)
            {
                if (playerO->angle < 16)
                {
                    i = playerO->xVelocity;
                }
                else if (playerO->angle >= 32)
                {
                    i = (abs(playerO->xVelocity) <= abs(playerO->yVelocity) ? playerO->yVelocity + playerO->yVelocity / 12 : playerO->xVelocity);
                }
                else
                {
                    i = (abs(playerO->xVelocity) <= abs(playerO->yVelocity >> 1) ? (playerO->yVelocity + playerO->yVelocity / 12) >> 1 : playerO->xVelocity);
                }
            }
            else if (playerO->angle > 240)
            {
                i = playerO->xVelocity;
            }
            else if (playerO->angle <= 224)
            {
                i = (abs(playerO->xVelocity) <= abs(playerO->yVelocity) ? -(playerO->yVelocity + playerO->yVelocity / 12) : playerO->xVelocity);
            }
            else
            {
                i = (abs(playerO->xVelocity) <= abs(playerO->yVelocity >> 1) ? -((playerO->yVelocity + playerO->yVelocity / 12) >> 1) : playerO->xVelocity);
            }
        }
        else if (playerO->angle < 128)
        {
            if (playerO->angle < 16)
            {
                i = playerO->xVelocity;
            }
            else if (playerO->angle >= 32)
            {
                i = (abs(playerO->xVelocity) <= abs(playerO->yVelocity) ? playerO->yVelocity : playerO->xVelocity);
            }
            else
            {
                i = (abs(playerO->xVelocity) <= abs(playerO->yVelocity >> 1) ? playerO->yVelocity >> 1 : playerO->xVelocity);
            }
        }
        else if (playerO->angle > 240)
        {
            i = playerO->xVelocity;
        }
        else if (playerO->angle <= 224)
        {
            i = (abs(playerO->xVelocity) <= abs(playerO->yVelocity) ? -playerO->yVelocity : playerO->xVelocity);
        }
        else
        {
            i = (abs(playerO->xVelocity) <= abs(playerO->yVelocity >> 1) ? -(playerO->yVelocity >> 1) : playerO->xVelocity);
        }
        if (i < -1572864)
        {
            i = -1572864;
        }
        if (i > 0x180000)
        {
            i = 0x180000;
        }
        playerO->speed = i;
        playerO->yVelocity = 0;
        scriptEng.checkResult = 1;
    }
    if (num4 == 2)
    {
        if (player_cSensor[4].collided == 1 && player_cSensor[5].collided == 1)
        {
            if (player_cSensor[4].yPos <= player_cSensor[5].yPos)
            {
                playerO->yPos = (player_cSensor[5].yPos - collisionTop + 1) << 16;
                num = player_cSensor[5].angle;
            }
            else
            {
                playerO->yPos = (player_cSensor[4].yPos - collisionTop + 1) << 16;
                num = player_cSensor[4].angle;
            }
        }
        else if (player_cSensor[4].collided == 1)
        {
            playerO->yPos = (player_cSensor[4].yPos - collisionTop + 1) << 16;
            num = player_cSensor[4].angle;
        }
        else if (player_cSensor[5].collided == 1)
        {
            playerO->yPos = (player_cSensor[5].yPos - collisionTop + 1) << 16;
            num = player_cSensor[5].angle;
        }
        num = num & 0xff;
        i = GlobalAppDefinitions_ArcTanLookup(playerO->xVelocity, playerO->yVelocity);
        if (num > 64 && num < 98 && i > 160 && i < 194)
        {
            playerO->gravity = 0;
            playerO->angle = num;
            playerO->objectPtr->rotation = playerO->angle << 1;
            playerO->collisionMode = 3;
            playerO->xPos = playerO->xPos + 0x40000;
            playerO->yPos = playerO->yPos - 0x20000;
            if (playerO->angle <= 96)
            {
                playerO->speed = playerO->yVelocity;
            }
            else
            {
                playerO->speed = playerO->yVelocity >> 1;
            }
        }
        if (num > 158 && num < 192 && i > 190 && i < 224)
        {
            playerO->gravity = 0;
            playerO->angle = num;
            playerO->objectPtr->rotation = playerO->angle << 1;
            playerO->collisionMode = 1;
            playerO->xPos = playerO->xPos - 0x40000;
            playerO->yPos = playerO->yPos - 0x20000;
            if (playerO->angle >= 160)
            {
                playerO->speed = -playerO->yVelocity;
            }
            else
            {
                playerO->speed = -playerO->yVelocity >> 1;
            }
        }
        if (playerO->yVelocity < 0)
        {
            playerO->yVelocity = 0;
        }
        scriptEng.checkResult = 2;
    }
}

void PlayerSystem_ProcessPathGrip(struct PlayerObject* playerO)
{
    int cosValue256;
    int sinValue256;
    int i;
    int num = -1;
    player_cSensor[4].xPos = playerO->xPos;
    player_cSensor[4].yPos = playerO->yPos;
    player_cSensor[3].collided = 0;
    player_cSensor[0].angle = playerO->angle;
    player_cSensor[1].angle = playerO->angle;
    player_cSensor[2].angle = playerO->angle;
    PlayerSystem_SetPathGripSensors(playerO);
    int num1 = abs(playerO->speed);
    int num2 = num1 >> 18;
    num1 = num1 & 0x3ffff;
    while (num2 > -1)
    {
        if (num2 >= 1)
        {
            cosValue256 = CosValue256[playerO->angle] << 10;
            sinValue256 = SinValue256[playerO->angle] << 10;
            num2--;
        }
        else
        {
            cosValue256 = CosValue256[playerO->angle] * num1 >> 8;
            sinValue256 = SinValue256[playerO->angle] * num1 >> 8;
            num2 = -1;
        }
        if (playerO->speed < 0)
        {
            cosValue256 = -cosValue256;
            sinValue256 = -sinValue256;
        }
        player_cSensor[0].collided = 0;
        player_cSensor[1].collided = 0;
        player_cSensor[2].collided = 0;
        struct CollisionSensor* collisionSensor = &player_cSensor[4];
        collisionSensor->xPos = collisionSensor->xPos + cosValue256;
        struct CollisionSensor* collisionSensor1 = &player_cSensor[4];
        collisionSensor1->yPos = collisionSensor1->yPos + sinValue256;
        switch (playerO->collisionMode)
        {
            case 0:
            {
                struct CollisionSensor* collisionSensor2 = &player_cSensor[3];
                collisionSensor2->xPos = collisionSensor2->xPos + cosValue256;
                struct CollisionSensor* collisionSensor3 = &player_cSensor[3];
                collisionSensor3->yPos = collisionSensor3->yPos + sinValue256;
                if (playerO->speed > 0)
                {
                    PlayerSystem_LWallCollision(playerO, &player_cSensor[3]);
                }
                if (playerO->speed < 0)
                {
                    PlayerSystem_RWallCollision(playerO, &player_cSensor[3]);
                }
                if (player_cSensor[3].collided == 1)
                {
                    cosValue256 = 0;
                    num2 = -1;
                }
                for (i = 0; i < 3; i++)
                {
                    struct CollisionSensor* collisionSensor4 = &player_cSensor[i];
                    collisionSensor4->xPos = collisionSensor4->xPos + cosValue256;
                    struct CollisionSensor* collisionSensor5 = &player_cSensor[i];
                    collisionSensor5->yPos = collisionSensor5->yPos + sinValue256;
                    PlayerSystem_FindFloorPosition(playerO, &player_cSensor[i], player_cSensor[i].yPos >> 16);
                }
                num = -1;
                for (i = 0; i < 3; i++)
                {
                    if (num > -1)
                    {
                        if (player_cSensor[i].collided == 1)
                        {
                            if (player_cSensor[i].yPos < player_cSensor[num].yPos)
                            {
                                num = i;
                            }
                            if (player_cSensor[i].yPos == player_cSensor[num].yPos && player_cSensor[i].angle < 8 | player_cSensor[i].angle > 248)
                            {
                                num = i;
                            }
                        }
                    }
                    else if (player_cSensor[i].collided == 1)
                    {
                        num = i;
                    }
                }
                if (num <= -1)
                {
                    num2 = -1;
                }
                else
                {
                    player_cSensor[0].yPos = player_cSensor[num].yPos << 16;
                    player_cSensor[0].angle = player_cSensor[num].angle;
                    player_cSensor[1].yPos = player_cSensor[0].yPos;
                    player_cSensor[1].angle = player_cSensor[0].angle;
                    player_cSensor[2].yPos = player_cSensor[0].yPos;
                    player_cSensor[2].angle = player_cSensor[0].angle;
                    player_cSensor[3].yPos = player_cSensor[0].yPos - 0x40000;
                    player_cSensor[3].angle = player_cSensor[0].angle;
                    player_cSensor[4].xPos = player_cSensor[1].xPos;
                    player_cSensor[4].yPos = player_cSensor[1].yPos - (collisionBottom << 16);
                }
                if (player_cSensor[0].angle < 222 && player_cSensor[0].angle > 128)
                {
                    playerO->collisionMode = 1;
                }
                if (player_cSensor[0].angle <= 34 || player_cSensor[0].angle >= 128)
                {
                    break;
                }
                playerO->collisionMode = 3;
                break;
            }
            case 1:
            {
                struct CollisionSensor* collisionSensor6 = &player_cSensor[3];
                collisionSensor6->xPos = collisionSensor6->xPos + cosValue256;
                struct CollisionSensor* collisionSensor7 = &player_cSensor[3];
                collisionSensor7->yPos = collisionSensor7->yPos + sinValue256;
                if (playerO->speed > 0)
                {
                    PlayerSystem_RoofCollision(playerO, &player_cSensor[3]);
                }
                if (playerO->speed < 0)
                {
                    PlayerSystem_FloorCollision(playerO, &player_cSensor[3]);
                }
                if (player_cSensor[3].collided == 1)
                {
                    sinValue256 = 0;
                    num2 = -1;
                }
                for (i = 0; i < 3; i++)
                {
                    struct CollisionSensor* collisionSensor8 = &player_cSensor[i];
                    collisionSensor8->xPos = collisionSensor8->xPos + cosValue256;
                    struct CollisionSensor* collisionSensor9 = &player_cSensor[i];
                    collisionSensor9->yPos = collisionSensor9->yPos + sinValue256;
                    PlayerSystem_FindLWallPosition(playerO, &player_cSensor[i], player_cSensor[i].xPos >> 16);
                }
                num = -1;
                for (i = 0; i < 3; i++)
                {
                    if (num > -1)
                    {
                        if (player_cSensor[i].xPos < player_cSensor[num].xPos && player_cSensor[i].collided == 1)
                        {
                            num = i;
                        }
                    }
                    else if (player_cSensor[i].collided == 1)
                    {
                        num = i;
                    }
                }
                if (num <= -1)
                {
                    num2 = -1;
                }
                else
                {
                    player_cSensor[0].xPos = player_cSensor[num].xPos << 16;
                    player_cSensor[0].angle = player_cSensor[num].angle;
                    player_cSensor[1].xPos = player_cSensor[0].xPos;
                    player_cSensor[1].angle = player_cSensor[0].angle;
                    player_cSensor[2].xPos = player_cSensor[0].xPos;
                    player_cSensor[2].angle = player_cSensor[0].angle;
                    player_cSensor[4].yPos = player_cSensor[1].yPos;
                    player_cSensor[4].xPos = player_cSensor[1].xPos - (collisionRight << 16);
                }
                if (player_cSensor[0].angle > 226)
                {
                    playerO->collisionMode = 0;
                }
                if (player_cSensor[0].angle >= 158)
                {
                    break;
                }
                playerO->collisionMode = 2;
                break;
            }
            case 2:
            {
                struct CollisionSensor* collisionSensor10 = &player_cSensor[3];
                collisionSensor10->xPos = collisionSensor10->xPos + cosValue256;
                struct CollisionSensor* collisionSensor11 = &player_cSensor[3];
                collisionSensor11->yPos = collisionSensor11->yPos + sinValue256;
                if (playerO->speed > 0)
                {
                    PlayerSystem_RWallCollision(playerO, &player_cSensor[3]);
                }
                if (playerO->speed < 0)
                {
                    PlayerSystem_LWallCollision(playerO, &player_cSensor[3]);
                }
                if (player_cSensor[3].collided == 1)
                {
                    cosValue256 = 0;
                    num2 = -1;
                }
                for (i = 0; i < 3; i++)
                {
                    struct CollisionSensor* collisionSensor12 = &player_cSensor[i];
                    collisionSensor12->xPos = collisionSensor12->xPos + cosValue256;
                    struct CollisionSensor* collisionSensor13 = &player_cSensor[i];
                    collisionSensor13->yPos = collisionSensor13->yPos + sinValue256;
                    PlayerSystem_FindRoofPosition(playerO, &player_cSensor[i], player_cSensor[i].yPos >> 16);
                }
                num = -1;
                for (i = 0; i < 3; i++)
                {
                    if (num > -1)
                    {
                        if (player_cSensor[i].yPos > player_cSensor[num].yPos && player_cSensor[i].collided == 1)
                        {
                            num = i;
                        }
                    }
                    else if (player_cSensor[i].collided == 1)
                    {
                        num = i;
                    }
                }
                if (num <= -1)
                {
                    num2 = -1;
                }
                else
                {
                    player_cSensor[0].yPos = player_cSensor[num].yPos << 16;
                    player_cSensor[0].angle = player_cSensor[num].angle;
                    player_cSensor[1].yPos = player_cSensor[0].yPos;
                    player_cSensor[1].angle = player_cSensor[0].angle;
                    player_cSensor[2].yPos = player_cSensor[0].yPos;
                    player_cSensor[2].angle = player_cSensor[0].angle;
                    player_cSensor[3].yPos = player_cSensor[0].yPos + 0x40000;
                    player_cSensor[3].angle = player_cSensor[0].angle;
                    player_cSensor[4].xPos = player_cSensor[1].xPos;
                    player_cSensor[4].yPos = player_cSensor[1].yPos - ((collisionTop - 1) << 16);
                }
                if (player_cSensor[0].angle > 162)
                {
                    playerO->collisionMode = 1;
                }
                if (player_cSensor[0].angle >= 94)
                {
                    break;
                }
                playerO->collisionMode = 3;
                break;
            }
            case 3:
            {
                struct CollisionSensor* collisionSensor14 = &player_cSensor[3];
                collisionSensor14->xPos = collisionSensor14->xPos + cosValue256;
                struct CollisionSensor* collisionSensor15 = &player_cSensor[3];
                collisionSensor15->yPos = collisionSensor15->yPos + sinValue256;
                if (playerO->speed > 0)
                {
                    PlayerSystem_FloorCollision(playerO, &player_cSensor[3]);
                }
                if (playerO->speed < 0)
                {
                    PlayerSystem_RoofCollision(playerO, &player_cSensor[3]);
                }
                if (player_cSensor[3].collided == 1)
                {
                    sinValue256 = 0;
                    num2 = -1;
                }
                for (i = 0; i < 3; i++)
                {
                    struct CollisionSensor* collisionSensor16 = &player_cSensor[i];
                    collisionSensor16->xPos = collisionSensor16->xPos + cosValue256;
                    struct CollisionSensor* collisionSensor17 = &player_cSensor[i];
                    collisionSensor17->yPos = collisionSensor17->yPos + sinValue256;
                    PlayerSystem_FindRWallPosition(playerO, &player_cSensor[i], player_cSensor[i].xPos >> 16);
                }
                num = -1;
                for (i = 0; i < 3; i++)
                {
                    if (num > -1)
                    {
                        if (player_cSensor[i].xPos > player_cSensor[num].xPos && player_cSensor[i].collided == 1)
                        {
                            num = i;
                        }
                    }
                    else if (player_cSensor[i].collided == 1)
                    {
                        num = i;
                    }
                }
                if (num <= -1)
                {
                    num2 = -1;
                }
                else
                {
                    player_cSensor[0].xPos = player_cSensor[num].xPos << 16;
                    player_cSensor[0].angle = player_cSensor[num].angle;
                    player_cSensor[1].xPos = player_cSensor[0].xPos;
                    player_cSensor[1].angle = player_cSensor[0].angle;
                    player_cSensor[2].xPos = player_cSensor[0].xPos;
                    player_cSensor[2].angle = player_cSensor[0].angle;
                    player_cSensor[4].yPos = player_cSensor[1].yPos;
                    player_cSensor[4].xPos = player_cSensor[1].xPos - ((collisionLeft - 1) << 16);
                }
                if (player_cSensor[0].angle < 30)
                {
                    playerO->collisionMode = 0;
                }
                if (player_cSensor[0].angle <= 98)
                {
                    break;
                }
                playerO->collisionMode = 2;
                break;
            }
        }
        if (num > -1)
        {
            playerO->angle = player_cSensor[0].angle;
        }
        if (player_cSensor[3].collided != 1)
        {
            PlayerSystem_SetPathGripSensors(playerO);
        }
        else
        {
            num2 = -2;
        }
    }
    switch (playerO->collisionMode)
    {
        case 0:
        {
            if (player_cSensor[0].collided != 0 || player_cSensor[1].collided != 0 || player_cSensor[2].collided != 0)
            {
                playerO->angle = player_cSensor[0].angle;
                playerO->objectPtr->rotation = playerO->angle << 1;
                playerO->flailing[0] = player_cSensor[0].collided;
                playerO->flailing[1] = player_cSensor[1].collided;
                playerO->flailing[2] = player_cSensor[2].collided;
                if (player_cSensor[3].collided != 1)
                {
                    playerO->pushing = 0;
                    playerO->xPos = player_cSensor[4].xPos;
                }
                else
                {
                    if (playerO->speed > 0)
                    {
                        playerO->xPos = (player_cSensor[3].xPos - collisionRight) << 16;
                    }
                    if (playerO->speed < 0)
                    {
                        playerO->xPos = (player_cSensor[3].xPos - collisionLeft + 1) << 16;
                    }
                    playerO->speed = 0;
                    if (playerO->left == 1 | playerO->right == 1 && playerO->pushing < 2)
                    {
                        struct PlayerObject* playerObject = playerO;
                        playerObject->pushing = (uint8_t)(playerObject->pushing + 1);
                    }
                }
                playerO->yPos = player_cSensor[4].yPos;
                return;
            }
            playerO->gravity = 1;
            playerO->collisionMode = 0;
            playerO->xVelocity = CosValue256[playerO->angle] * playerO->speed >> 8;
            playerO->yVelocity = SinValue256[playerO->angle] * playerO->speed >> 8;
            if (playerO->yVelocity < -1048576)
            {
                playerO->yVelocity = -1048576;
            }
            if (playerO->yVelocity > 0x100000)
            {
                playerO->yVelocity = 0x100000;
            }
            playerO->speed = playerO->xVelocity;
            playerO->angle = 0;
            if (player_cSensor[3].collided != 1)
            {
                playerO->pushing = 0;
                playerO->xPos = playerO->xPos + playerO->xVelocity;
            }
            else
            {
                if (playerO->speed > 0)
                {
                    playerO->xPos = (player_cSensor[3].xPos - collisionRight) << 16;
                }
                if (playerO->speed < 0)
                {
                    playerO->xPos = (player_cSensor[3].xPos - collisionLeft + 1) << 16;
                }
                playerO->speed = 0;
                if (playerO->left == 1 | playerO->right == 1 && playerO->pushing < 2)
                {
                    playerO->pushing = (uint8_t)(playerO->pushing + 1);
                }
            }
            playerO->yPos = playerO->yPos + playerO->yVelocity;
            return;
        }
        case 1:
        {
            if (player_cSensor[0].collided == 0 && player_cSensor[1].collided == 0 && player_cSensor[2].collided == 0)
            {
                playerO->gravity = 1;
                playerO->collisionMode = 0;
                playerO->xVelocity = CosValue256[playerO->angle] * playerO->speed >> 8;
                playerO->yVelocity = SinValue256[playerO->angle] * playerO->speed >> 8;
                if (playerO->yVelocity < -1048576)
                {
                    playerO->yVelocity = -1048576;
                }
                if (playerO->yVelocity > 0x100000)
                {
                    playerO->yVelocity = 0x100000;
                }
                playerO->speed = playerO->xVelocity;
                playerO->angle = 0;
            }
            else if (playerO->speed >= 0x28000 || playerO->speed <= -163840 || playerO->controlLock != 0)
            {
                playerO->angle = player_cSensor[0].angle;
                playerO->objectPtr->rotation = playerO->angle << 1;
            }
            else
            {
                playerO->gravity = 1;
                playerO->angle = 0;
                playerO->collisionMode = 0;
                playerO->speed = playerO->xVelocity;
                playerO->controlLock = 30;
            }
            if (player_cSensor[3].collided != 1)
            {
                playerO->yPos = player_cSensor[4].yPos;
            }
            else
            {
                if (playerO->speed > 0)
                {
                    playerO->yPos = (player_cSensor[3].yPos - collisionTop) << 16;
                }
                if (playerO->speed < 0)
                {
                    playerO->yPos = (player_cSensor[3].yPos - collisionBottom) << 16;
                }
                playerO->speed = 0;
            }
            playerO->xPos = player_cSensor[4].xPos;
            return;
        }
        case 2:
        {
            if (player_cSensor[0].collided == 0 && player_cSensor[1].collided == 0 && player_cSensor[2].collided == 0)
            {
                playerO->gravity = 1;
                playerO->collisionMode = 0;
                playerO->xVelocity = CosValue256[playerO->angle] * playerO->speed >> 8;
                playerO->yVelocity = SinValue256[playerO->angle] * playerO->speed >> 8;
                playerO->flailing[0] = 0;
                playerO->flailing[1] = 0;
                playerO->flailing[2] = 0;
                if (playerO->yVelocity < -1048576)
                {
                    playerO->yVelocity = -1048576;
                }
                if (playerO->yVelocity > 0x100000)
                {
                    playerO->yVelocity = 0x100000;
                }
                playerO->angle = 0;
                playerO->speed = playerO->xVelocity;
                if (player_cSensor[3].collided != 1)
                {
                    playerO->xPos = playerO->xPos + playerO->xVelocity;
                }
                else
                {
                    if (playerO->speed > 0)
                    {
                        playerO->xPos = (player_cSensor[3].xPos - collisionRight) << 16;
                    }
                    if (playerO->speed < 0)
                    {
                        playerO->xPos = (player_cSensor[3].xPos - collisionLeft + 1) << 16;
                    }
                    playerO->speed = 0;
                }
            }
            else if (playerO->speed <= -163840 || playerO->speed >= 0x28000)
            {
                playerO->angle = player_cSensor[0].angle;
                playerO->objectPtr->rotation = playerO->angle << 1;
                if (player_cSensor[3].collided != 1)
                {
                    playerO->xPos = player_cSensor[4].xPos;
                }
                else
                {
                    if (playerO->speed < 0)
                    {
                        playerO->xPos = (player_cSensor[3].xPos - collisionRight) << 16;
                    }
                    if (playerO->speed > 0)
                    {
                        playerO->xPos = (player_cSensor[3].xPos - collisionLeft + 1) << 16;
                    }
                    playerO->speed = 0;
                }
            }
            else
            {
                playerO->gravity = 1;
                playerO->angle = 0;
                playerO->collisionMode = 0;
                playerO->speed = playerO->xVelocity;
                playerO->flailing[0] = 0;
                playerO->flailing[1] = 0;
                playerO->flailing[2] = 0;
                if (player_cSensor[3].collided != 1)
                {
                    playerO->xPos = playerO->xPos + playerO->xVelocity;
                }
                else
                {
                    if (playerO->speed > 0)
                    {
                        playerO->xPos = (player_cSensor[3].xPos - collisionRight) << 16;
                    }
                    if (playerO->speed < 0)
                    {
                        playerO->xPos = (player_cSensor[3].xPos - collisionLeft + 1) << 16;
                    }
                    playerO->speed = 0;
                }
            }
            playerO->yPos = player_cSensor[4].yPos;
            return;
        }
        case 3:
        {
            if (player_cSensor[0].collided == 0 && player_cSensor[1].collided == 0 && player_cSensor[2].collided == 0)
            {
                playerO->gravity = 1;
                playerO->collisionMode = 0;
                playerO->xVelocity = CosValue256[playerO->angle] * playerO->speed >> 8;
                playerO->yVelocity = SinValue256[playerO->angle] * playerO->speed >> 8;
                if (playerO->yVelocity < -1048576)
                {
                    playerO->yVelocity = -1048576;
                }
                if (playerO->yVelocity > 0x100000)
                {
                    playerO->yVelocity = 0x100000;
                }
                playerO->speed = playerO->xVelocity;
                playerO->angle = 0;
            }
            else if (playerO->speed <= -163840 || playerO->speed >= 0x28000 || playerO->controlLock != 0)
            {
                playerO->angle = player_cSensor[0].angle;
                playerO->objectPtr->rotation = playerO->angle << 1;
            }
            else
            {
                playerO->gravity = 1;
                playerO->angle = 0;
                playerO->collisionMode = 0;
                playerO->speed = playerO->xVelocity;
                playerO->controlLock = 30;
            }
            if (player_cSensor[3].collided != 1)
            {
                playerO->yPos = player_cSensor[4].yPos;
            }
            else
            {
                if (playerO->speed > 0)
                {
                    playerO->yPos = (player_cSensor[3].yPos - collisionBottom) << 16;
                }
                if (playerO->speed < 0)
                {
                    playerO->yPos = (player_cSensor[3].yPos - collisionTop + 1) << 16;
                }
                playerO->speed = 0;
            }
            playerO->xPos = player_cSensor[4].xPos;
            return;
        }
        default:
        {
            return;
        }
    }
}

void PlayerSystem_ProcessPlayerControl(struct PlayerObject* playerO)
{
    switch (playerO->controlMode)
    {
        case -1:
        {
            delayUp = (ushort)(delayUp << 1);
            delayUp = (ushort)(delayUp | playerO->up);
            delayDown = (ushort)(delayDown << 1);
            delayDown = (ushort)(delayDown | playerO->down);
            delayLeft = (ushort)(delayLeft << 1);
            delayLeft = (ushort)(delayLeft | playerO->left);
            delayRight = (ushort)(delayRight << 1);
            delayRight = (ushort)(delayRight | playerO->right);
            delayJumpPress = (ushort)(delayJumpPress << 1);
            delayJumpPress = (ushort)(delayJumpPress | playerO->jumpPress);
            delayJumpHold = (ushort)(delayJumpHold << 1);
            delayJumpHold = (ushort)(delayJumpHold | playerO->jumpHold);
            return;
        }
        case 0:
        {
            playerO->up = gKeyDown.up;
            playerO->down = gKeyDown.down;
            if (gKeyDown.left != 1 || gKeyDown.right != 1)
            {
                playerO->left = gKeyDown.left;
                playerO->right = gKeyDown.right;
            }
            else
            {
                playerO->left = 0;
                playerO->right = 0;
            }
            playerO->jumpHold = (uint8_t)(gKeyDown.buttonA | gKeyDown.buttonB | gKeyDown.buttonC);
            playerO->jumpPress = (uint8_t)(gKeyPress.buttonA | gKeyPress.buttonB | gKeyPress.buttonC);
            delayUp = (ushort)(delayUp << 1);
            delayUp = (ushort)(delayUp | playerO->up);
            delayDown = (ushort)(delayDown << 1);
            delayDown = (ushort)(delayDown | playerO->down);
            delayLeft = (ushort)(delayLeft << 1);
            delayLeft = (ushort)(delayLeft | playerO->left);
            delayRight = (ushort)(delayRight << 1);
            delayRight = (ushort)(delayRight | playerO->right);
            delayJumpPress = (ushort)(delayJumpPress << 1);
            delayJumpPress = (ushort)(delayJumpPress | playerO->jumpPress);
            delayJumpHold = (ushort)(delayJumpHold << 1);
            delayJumpHold = (ushort)(delayJumpHold | playerO->jumpHold);
            return;
        }
        case 1:
        {
            playerO->up = (uint8_t)(delayUp >> 15);
            playerO->down = (uint8_t)(delayDown >> 15);
            playerO->left = (uint8_t)(delayLeft >> 15);
            playerO->right = (uint8_t)(delayRight >> 15);
            playerO->jumpPress = (uint8_t)(delayJumpPress >> 15);
            playerO->jumpHold = (uint8_t)(delayJumpHold >> 15);
            return;
        }
        default:
        {
            return;
        }
    }
}

void PlayerSystem_ProcessPlayerTileCollisions(struct PlayerObject* playerO)
{
    playerO->flailing[0] = 0;
    playerO->flailing[1] = 0;
    playerO->flailing[2] = 0;
    scriptEng.checkResult = 0;
    if (playerO->gravity == 1)
    {
        PlayerSystem_ProcessAirCollision(playerO);
    }
    else
    {
        PlayerSystem_ProcessPathGrip(playerO);
    }
}

void PlayerSystem_RoofCollision(struct PlayerObject* playerO, struct CollisionSensor* cSensorRef)
{
    int num;
    int num1 = cSensorRef->yPos >> 16;
    for (int i = 0; i < 48; i = i + 16)
    {
        if (cSensorRef->collided == 0)
        {
            int num2 = cSensorRef->xPos >> 16;
            int num3 = num2 >> 7;
            int num4 = (num2 & 127) >> 4;
            int num5 = (cSensorRef->yPos >> 16) + 16 - i;
            int num6 = num5 >> 7;
            int num7 = (num5 & 127) >> 4;
            if (num2 > -1 && num5 > -1)
            {
                int num8 = stageLayouts[0].tileMap[num3 + (num6 << 8)] << 6;
                num8 = num8 + num4 + (num7 << 3);
                int num9 = tile128x128.tile16x16[num8];
                if (tile128x128.collisionFlag[playerO->collisionPlane][num8] != 1 && tile128x128.collisionFlag[playerO->collisionPlane][num8] < 3)
                {
                    switch (tile128x128.direction[num8])
                    {
                        case 0:
                        {
                            num = (num2 & 15) + (num9 << 4);
                            if ((num5 & 15) >= tileCollisions[playerO->collisionPlane].roofMask[num] + 16 - i)
                            {
                                break;
                            }
                            cSensorRef->yPos = tileCollisions[playerO->collisionPlane].roofMask[num] + (num6 << 7) + (num7 << 4);
                            cSensorRef->collided = 1;
                            cSensorRef->angle = (int)((tileCollisions[playerO->collisionPlane].angle[num9] & -16777216) >> 24);
                            break;
                        }
                        case 1:
                        {
                            num = 15 - (num2 & 15) + (num9 << 4);
                            if ((num5 & 15) >= tileCollisions[playerO->collisionPlane].roofMask[num] + 16 - i)
                            {
                                break;
                            }
                            cSensorRef->yPos = tileCollisions[playerO->collisionPlane].roofMask[num] + (num6 << 7) + (num7 << 4);
                            cSensorRef->collided = 1;
                            cSensorRef->angle = (int)(0x100 - ((tileCollisions[playerO->collisionPlane].angle[num9] & -16777216) >> 24));
                            break;
                        }
                        case 2:
                        {
                            num = (num2 & 15) + (num9 << 4);
                            if ((num5 & 15) >= 15 - tileCollisions[playerO->collisionPlane].floorMask[num] + 16 - i)
                            {
                                break;
                            }
                            cSensorRef->yPos = 15 - tileCollisions[playerO->collisionPlane].floorMask[num] + (num6 << 7) + (num7 << 4);
                            cSensorRef->collided = 1;
                            cSensorRef->angle = (int)(0x180 - (tileCollisions[playerO->collisionPlane].angle[num9] & 0xff) & 0xff);
                            break;
                        }
                        case 3:
                        {
                            num = 15 - (num2 & 15) + (num9 << 4);
                            if ((num5 & 15) >= 15 - tileCollisions[playerO->collisionPlane].floorMask[num] + 16 - i)
                            {
                                break;
                            }
                            cSensorRef->yPos = 15 - tileCollisions[playerO->collisionPlane].floorMask[num] + (num6 << 7) + (num7 << 4);
                            cSensorRef->collided = 1;
                            cSensorRef->angle = (int)(0x100 - (0x180 - (tileCollisions[playerO->collisionPlane].angle[num9] & 0xff) & 0xff));
                            break;
                        }
                    }
                }
                if (cSensorRef->collided == 1)
                {
                    if (cSensorRef->angle < 0)
                    {
                        struct CollisionSensor* collisionSensor = cSensorRef;
                        collisionSensor->angle = collisionSensor->angle + 0x100;
                    }
                    if (cSensorRef->angle > 0xff)
                    {
                        struct CollisionSensor* collisionSensor1 = cSensorRef;
                        collisionSensor1->angle = collisionSensor1->angle - 0x100;
                    }
                    if (cSensorRef->yPos - num1 > 14)
                    {
                        cSensorRef->yPos = num1 << 16;
                        cSensorRef->collided = 0;
                    }
                    else if (cSensorRef->yPos - num1 < -14)
                    {
                        cSensorRef->yPos = num1 << 16;
                        cSensorRef->collided = 0;
                    }
                }
            }
        }
    }
}

void PlayerSystem_RWallCollision(struct PlayerObject* playerO, struct CollisionSensor* cSensorRef)
{
    int num;
    int num1 = cSensorRef->xPos >> 16;
    for (int i = 0; i < 48; i = i + 16)
    {
        if (cSensorRef->collided == 0)
        {
            int num2 = (cSensorRef->xPos >> 16) + 16 - i;
            int num3 = num2 >> 7;
            int num4 = (num2 & 127) >> 4;
            int num5 = cSensorRef->yPos >> 16;
            int num6 = num5 >> 7;
            int num7 = (num5 & 127) >> 4;
            if (num2 > -1 && num5 > -1)
            {
                int num8 = stageLayouts[0].tileMap[num3 + (num6 << 8)] << 6;
                num8 = num8 + num4 + (num7 << 3);
                int num9 = tile128x128.tile16x16[num8];
                if (tile128x128.collisionFlag[playerO->collisionPlane][num8] != 1 && tile128x128.collisionFlag[playerO->collisionPlane][num8] < 3)
                {
                    switch (tile128x128.direction[num8])
                    {
                        case 0:
                        {
                            num = (num5 & 15) + (num9 << 4);
                            if ((num2 & 15) >= tileCollisions[playerO->collisionPlane].rightWallMask[num] + 16 - i)
                            {
                                break;
                            }
                            cSensorRef->xPos = tileCollisions[playerO->collisionPlane].rightWallMask[num] + (num3 << 7) + (num4 << 4);
                            cSensorRef->collided = 1;
                            break;
                        }
                        case 1:
                        {
                            num = (num5 & 15) + (num9 << 4);
                            if ((num2 & 15) >= 15 - tileCollisions[playerO->collisionPlane].leftWallMask[num] + 16 - i)
                            {
                                break;
                            }
                            cSensorRef->xPos = 15 - tileCollisions[playerO->collisionPlane].leftWallMask[num] + (num3 << 7) + (num4 << 4);
                            cSensorRef->collided = 1;
                            break;
                        }
                        case 2:
                        {
                            num = 15 - (num5 & 15) + (num9 << 4);
                            if ((num2 & 15) >= tileCollisions[playerO->collisionPlane].rightWallMask[num] + 16 - i)
                            {
                                break;
                            }
                            cSensorRef->xPos = tileCollisions[playerO->collisionPlane].rightWallMask[num] + (num3 << 7) + (num4 << 4);
                            cSensorRef->collided = 1;
                            break;
                        }
                        case 3:
                        {
                            num = 15 - (num5 & 15) + (num9 << 4);
                            if ((num2 & 15) >= 15 - tileCollisions[playerO->collisionPlane].leftWallMask[num] + 16 - i)
                            {
                                break;
                            }
                            cSensorRef->xPos = 15 - tileCollisions[playerO->collisionPlane].leftWallMask[num] + (num3 << 7) + (num4 << 4);
                            cSensorRef->collided = 1;
                            break;
                        }
                    }
                }
                if (cSensorRef->collided == 1)
                {
                    if (cSensorRef->xPos - num1 > 15)
                    {
                        cSensorRef->xPos = num1 << 16;
                        cSensorRef->collided = 0;
                    }
                    else if (cSensorRef->xPos - num1 < -15)
                    {
                        cSensorRef->xPos = num1 << 16;
                        cSensorRef->collided = 0;
                    }
                }
            }
        }
    }
}

void PlayerSystem_SetPathGripSensors(struct PlayerObject* playerO)
{
    struct CollisionBox* collisionBox = &collisionBoxList[playerO->animationFile->cbListOffset + animationFrames[animationList[playerO->animationFile->aniListOffset + playerO->objectPtr->animation].frameListOffset + playerO->objectPtr->frame].collisionBox];
    switch (playerO->collisionMode)
    {
        case 0:
        {
            collisionLeft = collisionBox->left[0];
            collisionTop = collisionBox->top[0];
            collisionRight = collisionBox->right[0];
            collisionBottom = collisionBox->bottom[0];
            player_cSensor[0].yPos = player_cSensor[4].yPos + (collisionBottom << 16);
            player_cSensor[1].yPos = player_cSensor[0].yPos;
            player_cSensor[2].yPos = player_cSensor[0].yPos;
            player_cSensor[3].yPos = player_cSensor[4].yPos + 0x40000;
            player_cSensor[0].xPos = player_cSensor[4].xPos + ((collisionBox->left[1] - 1) << 16);
            player_cSensor[1].xPos = player_cSensor[4].xPos;
            player_cSensor[2].xPos = player_cSensor[4].xPos + (collisionBox->right[1] << 16);
            if (playerO->speed > 0)
            {
                player_cSensor[3].xPos = player_cSensor[4].xPos + ((collisionRight + 1) << 16);
                return;
            }
            player_cSensor[3].xPos = player_cSensor[4].xPos + ((collisionLeft - 1) << 16);
            return;
        }
        case 1:
        {
            collisionLeft = collisionBox->left[2];
            collisionTop = collisionBox->top[2];
            collisionRight = collisionBox->right[2];
            collisionBottom = collisionBox->bottom[2];
            player_cSensor[0].xPos = player_cSensor[4].xPos + (collisionRight << 16);
            player_cSensor[1].xPos = player_cSensor[0].xPos;
            player_cSensor[2].xPos = player_cSensor[0].xPos;
            player_cSensor[3].xPos = player_cSensor[4].xPos + 0x40000;
            player_cSensor[0].yPos = player_cSensor[4].yPos + ((collisionBox->top[3] - 1) << 16);
            player_cSensor[1].yPos = player_cSensor[4].yPos;
            player_cSensor[2].yPos = player_cSensor[4].yPos + (collisionBox->bottom[3] << 16);
            if (playerO->speed > 0)
            {
                player_cSensor[3].yPos = player_cSensor[4].yPos + (collisionTop << 16);
                return;
            }
            player_cSensor[3].yPos = player_cSensor[4].yPos + ((collisionBottom - 1) << 16);
            return;
        }
        case 2:
        {
            collisionLeft = collisionBox->left[4];
            collisionTop = collisionBox->top[4];
            collisionRight = collisionBox->right[4];
            collisionBottom = collisionBox->bottom[4];
            player_cSensor[0].yPos = player_cSensor[4].yPos + ((collisionTop - 1) << 16);
            player_cSensor[1].yPos = player_cSensor[0].yPos;
            player_cSensor[2].yPos = player_cSensor[0].yPos;
            player_cSensor[3].yPos = player_cSensor[4].yPos - 0x40000;
            player_cSensor[0].xPos = player_cSensor[4].xPos + ((collisionBox->left[5] - 1) << 16);
            player_cSensor[1].xPos = player_cSensor[4].xPos;
            player_cSensor[2].xPos = player_cSensor[4].xPos + (collisionBox->right[5] << 16);
            if (playerO->speed < 0)
            {
                player_cSensor[3].xPos = player_cSensor[4].xPos + ((collisionRight + 1) << 16);
                return;
            }
            player_cSensor[3].xPos = player_cSensor[4].xPos + ((collisionLeft - 1) << 16);
            return;
        }
        case 3:
        {
            collisionLeft = collisionBox->left[6];
            collisionTop = collisionBox->top[6];
            collisionRight = collisionBox->right[6];
            collisionBottom = collisionBox->bottom[6];
            player_cSensor[0].xPos = player_cSensor[4].xPos + ((collisionLeft - 1) << 16);
            player_cSensor[1].xPos = player_cSensor[0].xPos;
            player_cSensor[2].xPos = player_cSensor[0].xPos;
            player_cSensor[3].xPos = player_cSensor[4].xPos - 0x40000;
            player_cSensor[0].yPos = player_cSensor[4].yPos + ((collisionBox->top[7] - 1) << 16);
            player_cSensor[1].yPos = player_cSensor[4].yPos;
            player_cSensor[2].yPos = player_cSensor[4].yPos + (collisionBox->bottom[7] << 16);
            if (playerO->speed > 0)
            {
                player_cSensor[3].yPos = player_cSensor[4].yPos + (collisionBottom << 16);
                return;
            }
            player_cSensor[3].yPos = player_cSensor[4].yPos + ((collisionTop - 1) << 16);
            return;
        }
        default:
        {
            return;
        }
    }
}

void PlayerSystem_SetPlayerHLockedScreenPosition(struct PlayerObject* playerO)
{
    int num = playerO->xPos >> 16;
    int num1 = playerO->yPos >> 16;
    if (newYBoundary1 > yBoundary1)
    {
        if (yScrollOffset <= newYBoundary1)
        {
            yBoundary1 = yScrollOffset;
        }
        else
        {
            yBoundary1 = newYBoundary1;
        }
    }
    if (newYBoundary1 < yBoundary1)
    {
        if (yScrollOffset <= yBoundary1)
        {
            yBoundary1 = yBoundary1 - 1;
        }
        else
        {
            yBoundary1 = newYBoundary1;
        }
    }
    if (newYBoundary2 < yBoundary2)
    {
        if (yScrollOffset + 240 >= yBoundary2 || yScrollOffset + 240 <= newYBoundary2)
        {
            yBoundary2 = yBoundary2 - 1;
        }
        else
        {
            yBoundary2 = yScrollOffset + 240;
        }
    }
    if (newYBoundary2 > yBoundary2)
    {
        if (yScrollOffset + 240 >= yBoundary2)
        {
            yBoundary2 = yBoundary2 + 1;
        }
        else
        {
            yBoundary2 = newYBoundary2;
        }
    }
    int num2 = xScrollA;
    int num3 = xScrollB;
    if (num <= num2 + SCREEN_CENTER)
    {
        playerO->screenXPos = num - num2 + screenShakeX;
        xScrollOffset = num2 - screenShakeX;
    }
    else
    {
        xScrollOffset = num - SCREEN_CENTER + screenShakeX;
        playerO->screenXPos = SCREEN_CENTER - screenShakeX;
        if (num > num3 - SCREEN_CENTER)
        {
            playerO->screenXPos = SCREEN_CENTER + (num - (num3 - SCREEN_CENTER)) + screenShakeX;
            xScrollOffset = num3 - SCREEN_XSIZE - screenShakeX;
        }
    }
    num2 = yScrollA;
    num3 = yScrollB;
    num1 = num1 + cameraAdjustY;
    int num4 = num1 + playerO->lookPos - (num2 + 104);
    if (playerO->trackScroll != 1)
    {
        if (yScrollMove == 32)
        {
            yScrollMove = (104 - playerO->screenYPos - playerO->lookPos) >> 1 << 1;
            if (yScrollMove > 32)
            {
                yScrollMove = 32;
            }
            if (yScrollMove < -32)
            {
                yScrollMove = -32;
            }
        }
        if (yScrollMove > 0)
        {
            yScrollMove = yScrollMove - 6;
            if (yScrollMove < 0)
            {
                yScrollMove = yScrollMove;
            }
        }
        if (yScrollMove < 0)
        {
            yScrollMove = yScrollMove + 6;
            if (yScrollMove > 0)
            {
                yScrollMove = yScrollMove;
            }
        }
    }
    else
    {
        yScrollMove = 32;
    }
    if (abs(num4) >= abs(yScrollMove) + 17)
    {
        num2 = (num4 <= 0 ? num2 - 16 : num2 + 16);
        num3 = num2 + 240;
    }
    else if (yScrollMove != 32)
    {
        num2 = num1 - 104 + yScrollMove + playerO->lookPos;
        num3 = num2 + 240;
    }
    else
    {
        if (num1 + playerO->lookPos > num2 + 104 + yScrollMove)
        {
            num2 = num2 + (num1 + playerO->lookPos - (num2 + 104 + yScrollMove));
            num3 = num2 + 240;
        }
        if (num1 + playerO->lookPos < num2 + 104 - yScrollMove)
        {
            num2 = num2 - (num2 + 104 - yScrollMove - (num1 + playerO->lookPos));
            num3 = num2 + 240;
        }
    }
    if (num2 < yBoundary1)
    {
        num2 = yBoundary1;
        num3 = yBoundary1 + 240;
    }
    if (num3 > yBoundary2)
    {
        num3 = yBoundary2;
        num2 = yBoundary2 - 240;
    }
    yScrollA = num2;
    yScrollB = num3;
    if (num1 + playerO->lookPos <= num2 + 104)
    {
        playerO->screenYPos = num1 - num2 - screenShakeY;
        yScrollOffset = num2 + screenShakeY;
    }
    else
    {
        yScrollOffset = num1 - 104 + playerO->lookPos + screenShakeY;
        playerO->screenYPos = 104 - playerO->lookPos - screenShakeY;
        if (num1 + playerO->lookPos > num3 - 136)
        {
            playerO->screenYPos = 104 + (num1 - (num3 - 136)) + screenShakeY;
            yScrollOffset = num3 - 240 - screenShakeY;
        }
    }
    playerO->screenYPos = playerO->screenYPos - cameraAdjustY;
    if (screenShakeX != 0)
    {
        if (screenShakeX <= 0)
        {
            screenShakeX = -screenShakeX;
            screenShakeX = screenShakeX - 1;
        }
        else
        {
            screenShakeX = -screenShakeX;
        }
    }
    if (screenShakeY != 0)
    {
        if (screenShakeY > 0)
        {
            screenShakeY = -screenShakeY;
            return;
        }
        screenShakeY = -screenShakeY;
        screenShakeY = screenShakeY - 1;
    }
}

void PlayerSystem_SetPlayerLockedScreenPosition(struct PlayerObject* playerO)
{
    int num = playerO->xPos >> 16;
    int num1 = playerO->yPos >> 16;
    int num2 = xScrollA;
    int num3 = xScrollB;
    if (num <= num2 + SCREEN_CENTER)
    {
        playerO->screenXPos = num - num2 + screenShakeX;
        xScrollOffset = num2 - screenShakeX;
    }
    else
    {
        xScrollOffset = num - SCREEN_CENTER + screenShakeX;
        playerO->screenXPos = SCREEN_CENTER - screenShakeX;
        if (num > num3 - SCREEN_CENTER)
        {
            playerO->screenXPos = SCREEN_CENTER + (num - (num3 - SCREEN_CENTER)) + screenShakeX;
            xScrollOffset = num3 - SCREEN_XSIZE - screenShakeX;
        }
    }
    num2 = yScrollA;
    num3 = yScrollB;
    num1 = num1 + cameraAdjustY;
    //int num4 = playerO->lookPos;
    yScrollA = num2;
    yScrollB = num3;
    if (num1 + playerO->lookPos <= num2 + 104)
    {
        playerO->screenYPos = num1 - num2 - screenShakeY;
        yScrollOffset = num2 + screenShakeY;
    }
    else
    {
        yScrollOffset = num1 - 104 + playerO->lookPos + screenShakeY;
        playerO->screenYPos = 104 - playerO->lookPos - screenShakeY;
        if (num1 + playerO->lookPos > num3 - 136)
        {
            playerO->screenYPos = 104 + (num1 - (num3 - 136)) + screenShakeY;
            yScrollOffset = num3 - 240 - screenShakeY;
        }
    }
    playerO->screenYPos = playerO->screenYPos - cameraAdjustY;
    if (screenShakeX != 0)
    {
        if (screenShakeX <= 0)
        {
            screenShakeX = -screenShakeX;
            screenShakeX = screenShakeX - 1;
        }
        else
        {
            screenShakeX = -screenShakeX;
        }
    }
    if (screenShakeY != 0)
    {
        if (screenShakeY > 0)
        {
            screenShakeY = -screenShakeY;
            return;
        }
        screenShakeY = -screenShakeY;
        screenShakeY = screenShakeY - 1;
    }
}

void PlayerSystem_SetPlayerScreenPosition(struct PlayerObject* playerO)
{
    int num = playerO->xPos >> 16;
    int num1 = playerO->yPos >> 16;
    if (newYBoundary1 > yBoundary1)
    {
        if (yScrollOffset <= newYBoundary1)
        {
            yBoundary1 = yScrollOffset;
        }
        else
        {
            yBoundary1 = newYBoundary1;
        }
    }
    if (newYBoundary1 < yBoundary1)
    {
        if (yScrollOffset <= yBoundary1)
        {
            yBoundary1 = yBoundary1 - 1;
        }
        else
        {
            yBoundary1 = newYBoundary1;
        }
    }
    if (newYBoundary2 < yBoundary2)
    {
        if (yScrollOffset + 240 >= yBoundary2 || yScrollOffset + 240 <= newYBoundary2)
        {
            yBoundary2 = yBoundary2 - 1;
        }
        else
        {
            yBoundary2 = yScrollOffset + 240;
        }
    }
    if (newYBoundary2 > yBoundary2)
    {
        if (yScrollOffset + 240 >= yBoundary2)
        {
            yBoundary2 = yBoundary2 + 1;
        }
        else
        {
            yBoundary2 = newYBoundary2;
        }
    }
    if (newXBoundary1 > xBoundary1)
    {
        if (xScrollOffset <= newXBoundary1)
        {
            xBoundary1 = xScrollOffset;
        }
        else
        {
            xBoundary1 = newXBoundary1;
        }
    }
    if (newXBoundary1 < xBoundary1)
    {
        if (xScrollOffset <= xBoundary1)
        {
            xBoundary1 = xBoundary1 - 1;
            if (playerO->xVelocity < 0)
            {
                xBoundary1 = xBoundary1 + (playerO->xVelocity >> 16);
                if (xBoundary1 < newXBoundary1)
                {
                    xBoundary1 = newXBoundary1;
                }
            }
        }
        else
        {
            xBoundary1 = newXBoundary1;
        }
    }
    if (newXBoundary2 < xBoundary2)
    {
        if (xScrollOffset + SCREEN_XSIZE >= xBoundary2)
        {
            xBoundary2 = xScrollOffset + SCREEN_XSIZE;
        }
        else
        {
            xBoundary2 = newXBoundary2;
        }
    }
    if (newXBoundary2 > xBoundary2)
    {
        if (xScrollOffset + SCREEN_XSIZE >= xBoundary2)
        {
            xBoundary2 = xBoundary2 + 1;
            if (playerO->xVelocity > 0)
            {
                xBoundary2 = xBoundary2 + (playerO->xVelocity >> 16);
                if (xBoundary2 > newXBoundary2)
                {
                    xBoundary2 = newXBoundary2;
                }
            }
        }
        else
        {
            xBoundary2 = newXBoundary2;
        }
    }
    int screenScrollRight = xScrollA;
    int screenXSize = xScrollB;
    int screenCenter = num - (screenScrollRight + SCREEN_CENTER);
    if (abs(screenCenter) >= 25)
    {
        screenScrollRight = (screenCenter <= 0 ? screenScrollRight - 16 : screenScrollRight + 16);
        screenXSize = screenScrollRight + SCREEN_XSIZE;
    }
    else
    {
        if (num > screenScrollRight + SCREEN_SCROLL_RIGHT)
        {
            screenScrollRight = screenScrollRight + (num - (screenScrollRight + SCREEN_SCROLL_RIGHT));
            screenXSize = screenScrollRight + SCREEN_XSIZE;
        }
        if (num < screenScrollRight + SCREEN_SCROLL_LEFT)
        {
            screenScrollRight = screenScrollRight - (screenScrollRight + SCREEN_SCROLL_LEFT - num);
            screenXSize = screenScrollRight + SCREEN_XSIZE;
        }
    }
    if (screenScrollRight < xBoundary1)
    {
        screenScrollRight = xBoundary1;
        screenXSize = xBoundary1 + SCREEN_XSIZE;
    }
    if (screenXSize > xBoundary2)
    {
        screenXSize = xBoundary2;
        screenScrollRight = xBoundary2 - SCREEN_XSIZE;
    }
    xScrollA = screenScrollRight;
    xScrollB = screenXSize;
    if (num <= screenScrollRight + SCREEN_CENTER)
    {
        playerO->screenXPos = num - screenScrollRight + screenShakeX;
        xScrollOffset = screenScrollRight - screenShakeX;
    }
    else
    {
        xScrollOffset = num - SCREEN_CENTER + screenShakeX;
        playerO->screenXPos = SCREEN_CENTER - screenShakeX;
        if (num > screenXSize - SCREEN_CENTER)
        {
            playerO->screenXPos = SCREEN_CENTER + (num - (screenXSize - SCREEN_CENTER)) + screenShakeX;
            xScrollOffset = screenXSize - SCREEN_XSIZE - screenShakeX;
        }
    }
    screenScrollRight = yScrollA;
    screenXSize = yScrollB;
    num1 = num1 + cameraAdjustY;
    screenCenter = num1 + playerO->lookPos - (screenScrollRight + 104);
    if (playerO->trackScroll != 1)
    {
        if (yScrollMove == 32)
        {
            yScrollMove = (104 - playerO->screenYPos - playerO->lookPos) >> 1 << 1;
            if (yScrollMove > 32)
            {
                yScrollMove = 32;
            }
            if (yScrollMove < -32)
            {
                yScrollMove = -32;
            }
        }
        if (yScrollMove > 0)
        {
            yScrollMove = yScrollMove - 6;
            if (yScrollMove < 0)
            {
                yScrollMove = yScrollMove;
            }
        }
        if (yScrollMove < 0)
        {
            yScrollMove = yScrollMove + 6;
            if (yScrollMove > 0)
            {
                yScrollMove = yScrollMove;
            }
        }
    }
    else
    {
        yScrollMove = 32;
    }
    if (abs(screenCenter) >= abs(yScrollMove) + 17)
    {
        screenScrollRight = (screenCenter <= 0 ? screenScrollRight - 16 : screenScrollRight + 16);
        screenXSize = screenScrollRight + 240;
    }
    else if (yScrollMove != 32)
    {
        screenScrollRight = num1 - 104 + yScrollMove + playerO->lookPos;
        screenXSize = screenScrollRight + 240;
    }
    else
    {
        if (num1 + playerO->lookPos > screenScrollRight + 104 + yScrollMove)
        {
            screenScrollRight = screenScrollRight + (num1 + playerO->lookPos - (screenScrollRight + 104 + yScrollMove));
            screenXSize = screenScrollRight + 240;
        }
        if (num1 + playerO->lookPos < screenScrollRight + 104 - yScrollMove)
        {
            screenScrollRight = screenScrollRight - (screenScrollRight + 104 - yScrollMove - (num1 + playerO->lookPos));
            screenXSize = screenScrollRight + 240;
        }
    }
    if (screenScrollRight < yBoundary1)
    {
        screenScrollRight = yBoundary1;
        screenXSize = yBoundary1 + 240;
    }
    if (screenXSize > yBoundary2)
    {
        screenXSize = yBoundary2;
        screenScrollRight = yBoundary2 - 240;
    }
    yScrollA = screenScrollRight;
    yScrollB = screenXSize;
    if (num1 + playerO->lookPos <= screenScrollRight + 104)
    {
        playerO->screenYPos = num1 - screenScrollRight - screenShakeY;
        yScrollOffset = screenScrollRight + screenShakeY;
    }
    else
    {
        yScrollOffset = num1 - 104 + playerO->lookPos + screenShakeY;
        playerO->screenYPos = 104 - playerO->lookPos - screenShakeY;
        if (num1 + playerO->lookPos > screenXSize - 136)
        {
            playerO->screenYPos = 104 + (num1 - (screenXSize - 136)) + screenShakeY;
            yScrollOffset = screenXSize - 240 - screenShakeY;
        }
    }
    playerO->screenYPos = playerO->screenYPos - cameraAdjustY;
    if (screenShakeX != 0)
    {
        if (screenShakeX <= 0)
        {
            screenShakeX = -screenShakeX;
            screenShakeX = screenShakeX - 1;
        }
        else
        {
            screenShakeX = -screenShakeX;
        }
    }
    if (screenShakeY != 0)
    {
        if (screenShakeY > 0)
        {
            screenShakeY = -screenShakeY;
            return;
        }
        screenShakeY = -screenShakeY;
        screenShakeY = screenShakeY - 1;
    }
}

void PlayerSystem_SetPlayerScreenPositionCDStyle(struct PlayerObject* playerO)
{
    int num = playerO->xPos >> 16;
    int num1 = playerO->yPos >> 16;
    if (newYBoundary1 > yBoundary1)
    {
        if (yScrollOffset <= newYBoundary1)
        {
            yBoundary1 = yScrollOffset;
        }
        else
        {
            yBoundary1 = newYBoundary1;
        }
    }
    if (newYBoundary1 < yBoundary1)
    {
        if (yScrollOffset <= yBoundary1)
        {
            yBoundary1 = yBoundary1 - 1;
        }
        else
        {
            yBoundary1 = newYBoundary1;
        }
    }
    if (newYBoundary2 < yBoundary2)
    {
        if (yScrollOffset + 240 >= yBoundary2 || yScrollOffset + 240 <= newYBoundary2)
        {
            yBoundary2 = yBoundary2 - 1;
        }
        else
        {
            yBoundary2 = yScrollOffset + 240;
        }
    }
    if (newYBoundary2 > yBoundary2)
    {
        if (yScrollOffset + 240 >= yBoundary2)
        {
            yBoundary2 = yBoundary2 + 1;
        }
        else
        {
            yBoundary2 = newYBoundary2;
        }
    }
    if (newXBoundary1 > xBoundary1)
    {
        if (xScrollOffset <= newXBoundary1)
        {
            xBoundary1 = xScrollOffset;
        }
        else
        {
            xBoundary1 = newXBoundary1;
        }
    }
    if (newXBoundary1 < xBoundary1)
    {
        if (xScrollOffset <= xBoundary1)
        {
            xBoundary1 = xBoundary1 - 1;
            if (playerO->xVelocity < 0)
            {
                xBoundary1 = xBoundary1 + (playerO->xVelocity >> 16);
                if (xBoundary1 < newXBoundary1)
                {
                    xBoundary1 = newXBoundary1;
                }
            }
        }
        else
        {
            xBoundary1 = newXBoundary1;
        }
    }
    if (newXBoundary2 < xBoundary2)
    {
        if (xScrollOffset + SCREEN_XSIZE >= xBoundary2)
        {
            xBoundary2 = xScrollOffset + SCREEN_XSIZE;
        }
        else
        {
            xBoundary2 = newXBoundary2;
        }
    }
    if (newXBoundary2 > xBoundary2)
    {
        if (xScrollOffset + SCREEN_XSIZE >= xBoundary2)
        {
            xBoundary2 = xBoundary2 + 1;
            if (playerO->xVelocity > 0)
            {
                xBoundary2 = xBoundary2 + (playerO->xVelocity >> 16);
                if (xBoundary2 > newXBoundary2)
                {
                    xBoundary2 = newXBoundary2;
                }
            }
        }
        else
        {
            xBoundary2 = newXBoundary2;
        }
    }
    if (playerO->gravity == 0)
    {
        if (playerO->objectPtr->direction == 0)
        {
            if (!(playerO->speed > 0x5f5c2 | cameraStyle == 2))
            {
                cameraShift = 0;
            }
            else
            {
                cameraShift = 1;
            }
        }
        else if (!(playerO->speed < -390594 | cameraStyle == 3))
        {
            cameraShift = 0;
        }
        else
        {
            cameraShift = 2;
        }
    }
    switch (cameraShift)
    {
        case 0:
        {
            if (xScrollMove < 0)
            {
                xScrollMove = xScrollMove + 2;
            }
            if (xScrollMove <= 0)
            {
                break;
            }
            xScrollMove = xScrollMove - 2;
            break;
        }
        case 1:
        {
            if (xScrollMove <= -64)
            {
                break;
            }
            xScrollMove = xScrollMove - 2;
            break;
        }
        case 2:
        {
            if (xScrollMove >= 64)
            {
                break;
            }
            xScrollMove = xScrollMove + 2;
            break;
        }
    }
    if (num <= xBoundary1 + SCREEN_CENTER + xScrollMove)
    {
        playerO->screenXPos = num - xBoundary1 + screenShakeX;
        xScrollOffset = xBoundary1 - screenShakeX;
    }
    else
    {
        xScrollOffset = num - SCREEN_CENTER + screenShakeX - xScrollMove;
        playerO->screenXPos = SCREEN_CENTER - screenShakeX + xScrollMove;
        if (num - xScrollMove > xBoundary2 - SCREEN_CENTER)
        {
            playerO->screenXPos = SCREEN_CENTER + (num - (xBoundary2 - SCREEN_CENTER)) + screenShakeX;
            xScrollOffset = xBoundary2 - SCREEN_XSIZE - screenShakeX;
        }
    }
    xScrollA = xScrollOffset;
    xScrollB = xScrollA + SCREEN_XSIZE;
    int num2 = yScrollA;
    int num3 = yScrollB;
    num1 = num1 + cameraAdjustY;
    int num4 = num1 + playerO->lookPos - (num2 + 104);
    if (playerO->trackScroll != 1)
    {
        if (yScrollMove == 32)
        {
            yScrollMove = (104 - playerO->screenYPos - playerO->lookPos) >> 1 << 1;
            if (yScrollMove > 32)
            {
                yScrollMove = 32;
            }
            if (yScrollMove < -32)
            {
                yScrollMove = -32;
            }
        }
        if (yScrollMove > 0)
        {
            yScrollMove = yScrollMove - 6;
            if (yScrollMove < 0)
            {
                yScrollMove = yScrollMove;
            }
        }
        if (yScrollMove < 0)
        {
            yScrollMove = yScrollMove + 6;
            if (yScrollMove > 0)
            {
                yScrollMove = yScrollMove;
            }
        }
    }
    else
    {
        yScrollMove = 32;
    }
    if (abs(num4) >= abs(yScrollMove) + 17)
    {
        num2 = (num4 <= 0 ? num2 - 16 : num2 + 16);
        num3 = num2 + 240;
    }
    else if (yScrollMove != 32)
    {
        num2 = num1 - 104 + yScrollMove + playerO->lookPos;
        num3 = num2 + 240;
    }
    else
    {
        if (num1 + playerO->lookPos > num2 + 104 + yScrollMove)
        {
            num2 = num2 + (num1 + playerO->lookPos - (num2 + 104 + yScrollMove));
            num3 = num2 + 240;
        }
        if (num1 + playerO->lookPos < num2 + 104 - yScrollMove)
        {
            num2 = num2 - (num2 + 104 - yScrollMove - (num1 + playerO->lookPos));
            num3 = num2 + 240;
        }
    }
    if (num2 < yBoundary1)
    {
        num2 = yBoundary1;
        num3 = yBoundary1 + 240;
    }
    if (num3 > yBoundary2)
    {
        num3 = yBoundary2;
        num2 = yBoundary2 - 240;
    }
    yScrollA = num2;
    yScrollB = num3;
    if (num1 + playerO->lookPos <= num2 + 104)
    {
        playerO->screenYPos = num1 - num2 - screenShakeY;
        yScrollOffset = num2 + screenShakeY;
    }
    else
    {
        yScrollOffset = num1 - 104 + playerO->lookPos + screenShakeY;
        playerO->screenYPos = 104 - playerO->lookPos - screenShakeY;
        if (num1 + playerO->lookPos > num3 - 136)
        {
            playerO->screenYPos = 104 + (num1 - (num3 - 136)) + screenShakeY;
            yScrollOffset = num3 - 240 - screenShakeY;
        }
    }
    playerO->screenYPos = playerO->screenYPos - cameraAdjustY;
    if (screenShakeX != 0)
    {
        if (screenShakeX <= 0)
        {
            screenShakeX = -screenShakeX;
            screenShakeX = screenShakeX - 1;
        }
        else
        {
            screenShakeX = -screenShakeX;
        }
    }
    if (screenShakeY != 0)
    {
        if (screenShakeY > 0)
        {
            screenShakeY = -screenShakeY;
            return;
        }
        screenShakeY = -screenShakeY;
        screenShakeY = screenShakeY - 1;
    }
}
