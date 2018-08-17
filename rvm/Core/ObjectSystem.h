//
//  ObjectSystem.h
//  rvm
//

#ifndef ObjectSystem_h
#define ObjectSystem_h

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "SpriteFrame.h"
#include "ScriptEngine.h"
#include "ObjectScript.h"
#include "FunctionScript.h"
#include "ObjectEntity.h"
#include "ObjectDrawList.h"
#include "CollisionSensor.h"
#include "AnimationSystem.h"
#include "PlayerObject.h"
#include "PlayerSystem.h"
#include "FileData.h"
#include "FileIO.h"
#include "StageSystem.h"
#include "GlobalAppDefinitions.h"
#include "AudioPlayback.h"
#include "Scene3D.h"
#include "GraphicsSystem.h"
#include "TextSystem.h"
#include "EngineCallbacks.h"

extern int scriptData[0x40000];
extern int scriptDataPos;
extern int scriptDataOffset;
extern int scriptLineNumber;
extern int jumpTableData[0x4000];
extern int jumpTableDataPos;
extern int jumpTableOffset;
extern int jumpTableStack[0x400];
extern int jumpTableStackPos;
extern int NUM_FUNCTIONS;
extern int functionStack[0x400];
extern int functionStackPos;
extern struct SpriteFrame scriptFrames[0x1000];
extern int scriptFramesNo;
extern uint8_t NO_GLOBALVARIABLES;
extern char globalVariableNames[0x100][32];
extern int globalVariables[0x100];
extern int objectLoop;
extern struct ScriptEngine scriptEng;
extern char scriptText[0x100];
extern struct ObjectScript objectScriptList[0x100];
extern struct FunctionScript functionScriptList[0x200];
extern struct ObjectEntity objectEntityList[0x4A0];
extern struct ObjectDrawList objectDrawOrderList[7];
extern int playerNum;

void Init_ObjectSystem(void);
void ObjectSystem_BasicCollision(int cLeft, int cTop, int cRight, int cBottom);
void ObjectSystem_BoxCollision(int cLeft, int cTop, int cRight, int cBottom);
void ObjectSystem_ClearScriptData(void);
void ObjectSystem_DrawObjectList(int DrawListNo);
void ObjectSystem_LoadByteCodeFile(int fileType, int scriptNum);
void ObjectSystem_ObjectFloorCollision(int xOffset, int yOffset, int cPlane);
void ObjectSystem_ObjectFloorGrip(int xOffset, int yOffset, int cPlane);
void ObjectSystem_ObjectLWallCollision(int xOffset, int yOffset, int cPlane);
void ObjectSystem_ObjectLWallGrip(int xOffset, int yOffset, int cPlane);
void ObjectSystem_ObjectRoofCollision(int xOffset, int yOffset, int cPlane);
void ObjectSystem_ObjectRoofGrip(int xOffset, int yOffset, int cPlane);
void ObjectSystem_ObjectRWallCollision(int xOffset, int yOffset, int cPlane);
void ObjectSystem_ObjectRWallGrip(int xOffset, int yOffset, int cPlane);
void ObjectSystem_PlatformCollision(int cLeft, int cTop, int cRight, int cBottom);
void ObjectSystem_ProcessObjects(void);
void ObjectSystem_ProcessPausedObjects(void);
void ObjectSystem_ProcessScript(int scriptCodePtr, int jumpTablePtr, int scriptSub);
void ObjectSystem_ProcessStartupScripts(void);
void ObjectSystem_SetObjectTypeName(char* typeName, int scriptNum);

#endif /* ObjectSystem_h */
