//
//  PlayerSystem.h
//  rvm
//

#ifndef PlayerSystem_h
#define PlayerSystem_h

#include <stdio.h>
#include <stdlib.h>
#include "GlobalAppDefinitions.h"
#include "PlayerObject.h"
#include "CollisionSensor.h"
#include "StageSystem.h"
#include "AnimationSystem.h"
#include "ObjectSystem.h"

extern unsigned short delayLeft;
extern unsigned short delayRight;
extern unsigned short delayUp;
extern unsigned short delayDown;
extern unsigned short delayJumpPress;
extern unsigned short delayJumpHold;
extern unsigned char jumpWait;
extern unsigned char numActivePlayers;
extern unsigned char playerMenuNum;
extern struct PlayerObject playerList[2];
extern int collisionLeft;
extern int collisionTop;
extern int collisionRight;
extern int collisionBottom;

void Init_PlayerSystem();
void PlayerSystem_FindFloorPosition(struct PlayerObject* playerO, struct CollisionSensor* cSensorRef, int prevCollisionPos);
void PlayerSystem_FindLWallPosition(struct PlayerObject* playerO, struct CollisionSensor* cSensorRef, int prevCollisionPos);
void PlayerSystem_FindRoofPosition(struct PlayerObject* playerO, struct CollisionSensor* cSensorRef, int prevCollisionPos);
void PlayerSystem_FindRWallPosition(struct PlayerObject* playerO, struct CollisionSensor* cSensorRef, int prevCollisionPos);
void PlayerSystem_FloorCollision(struct PlayerObject* playerO, struct CollisionSensor* cSensorRef);
void PlayerSystem_LWallCollision(struct PlayerObject* playerO, struct CollisionSensor* cSensorRef);
void PlayerSystem_ProcessAirCollision(struct PlayerObject* playerO);
void PlayerSystem_ProcessPathGrip(struct PlayerObject* playerO);
void PlayerSystem_ProcessPlayerControl(struct PlayerObject* playerO);
void PlayerSystem_ProcessPlayerTileCollisions(struct PlayerObject* playerO);
void PlayerSystem_RoofCollision(struct PlayerObject* playerO, struct CollisionSensor* cSensorRef);
void PlayerSystem_RWallCollision(struct PlayerObject* playerO, struct CollisionSensor* cSensorRef);
void PlayerSystem_SetPathGripSensors(struct PlayerObject* playerO);
void PlayerSystem_SetPlayerHLockedScreenPosition(struct PlayerObject* playerO);
void PlayerSystem_SetPlayerLockedScreenPosition(struct PlayerObject* playerO);
void PlayerSystem_SetPlayerScreenPosition(struct PlayerObject* playerO);
void PlayerSystem_SetPlayerScreenPositionCDStyle(struct PlayerObject* playerO);

#endif /* PlayerSystem_h */
