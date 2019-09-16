//
//  StageSystem.h
//  rvm
//

#ifndef StageSystem_h
#define StageSystem_h

#include <stdio.h>
#include "GlobalAppDefinitions.h"
#include "InputResult.h"
#include "Mappings128x128.h"
#include "LayoutMap.h"
#include "CollisionMask16x16.h"
#include "LineScrollParallax.h"
#include "TextMenu.h"
#include "GraphicsSystem.h"
#include "ObjectSystem.h"
#include "PlayerSystem.h"
#ifdef DREAMCAST
#include "AudioPlayback_dc.h"
#else
#include "AudioPlayback.h"
#endif
#include "AnimationSystem.h"
#include "TextSystem.h"
#ifdef DREAMCAST
#include "RenderDevice_dc.h"
#else
#include "RenderDevice.h"
#endif

#include "FileIO.h"
#include "Scene3D.h"
#ifdef DREAMCAST
#include "InputSystem_dc.h"
#else
#include "InputSystem.h"
#endif

extern struct InputResult gKeyDown;
extern struct InputResult gKeyPress;
extern uint8_t stageMode;
extern uint8_t pauseEnabled;
extern int stageListPosition;
extern struct Mappings128x128 tile128x128;
extern struct LayoutMap stageLayouts[9];
extern uint8_t tLayerMidPoint;
extern uint8_t activeTileLayers[4];
extern struct CollisionMask16x16 tileCollisions[2];
extern struct LineScrollParallax hParallax;
extern struct LineScrollParallax vParallax;
extern int lastXSize;
extern int lastYSize;
extern int bgDeformationData0[0x240];
extern int bgDeformationData1[0x240];
extern int bgDeformationData2[0x240];
extern int bgDeformationData3[0x240];
extern int xBoundary1;
extern int xBoundary2;
extern int yBoundary1;
extern int yBoundary2;
extern int newXBoundary1;
extern int newXBoundary2;
extern int newYBoundary1;
extern int newYBoundary2;
extern uint8_t cameraEnabled;
extern signed char cameraTarget;
extern uint8_t cameraShift;
extern uint8_t cameraStyle;
extern int cameraAdjustY;
extern int xScrollOffset;
extern int yScrollOffset;
extern int yScrollA;
extern int yScrollB;
extern int xScrollA;
extern int xScrollB;
extern int xScrollMove;
extern int yScrollMove;
extern int screenShakeX;
extern int screenShakeY;
extern int waterLevel;
extern char titleCardText[24];
extern char titleCardWord2;
extern struct TextMenu gameMenu[2];
extern uint8_t timeEnabled;
extern uint8_t milliSeconds;
extern uint8_t seconds;
extern uint8_t minutes;
extern uint8_t debugMode;

void Init_StageSystem(void);
void StageSystem_Draw3DFloorLayer(uint8_t layerNum);
void StageSystem_DrawHLineScrollLayer8(uint8_t layerNum);
void StageSystem_DrawStageGfx(void);
void StageSystem_InitErrorMessage(void);
void StageSystem_InitFirstStage(void);
void StageSystem_InitStageSelectMenu(void);
void StageSystem_Load128x128Mappings(void);
void StageSystem_LoadActLayout(void);
void StageSystem_LoadStageBackground(void);
void StageSystem_LoadStageCollisions(void);
void StageSystem_LoadStageFiles(void);
void StageSystem_ProcessStage(void);
void StageSystem_ProcessStageSelectMenu(void);
void StageSystem_ResetBackgroundSettings(void);
void StageSystem_SetLayerDeformation(int selectedDef, int waveLength, int waveWidth, int wType, int yPos, int wSize);


#endif /* StageSystem_h */
