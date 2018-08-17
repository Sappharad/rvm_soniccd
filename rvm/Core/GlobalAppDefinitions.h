//
//  GlobalAppDefinitions.h
//  rvm
//

#ifndef GlobalAppDefinitions_h
#define GlobalAppDefinitions_h

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include "FileIO.h"
#include "ObjectSystem.h"

extern const int RETRO_EN;
extern const int RETRO_FR;
extern const int RETRO_IT;
extern const int RETRO_DE;
extern const int RETRO_ES;
extern const int RETRO_JP;
extern const int DEFAULTSCREEN;
extern const int MAINGAME;
extern const int RESETGAME;
extern const int EXITGAME;
extern const int SCRIPTERROR;
extern const int ENTER_HIRESMODE;
extern const int EXIT_HIRESMODE;
extern const int PAUSE_ENGINE;
extern const int PAUSE_WAIT;
extern const int VIDEO_WAIT;
extern const int RETRO_WIN;
extern const int RETRO_OSX;
extern const int RETRO_XBOX_360;
extern const int RETRO_PS3;
extern const int RETRO_iOS;
extern const int RETRO_ANDROID;
extern const int RETRO_WP7;
extern const int MAX_PLAYERS;
extern const int FACING_LEFT;
extern const int FACING_RIGHT;
extern const int GAME_FULL;
extern const int GAME_TRIAL;
extern const int OBJECT_BORDER_Y1;
extern const int OBJECT_BORDER_Y2;
extern const double Pi;
extern char gameWindowText[];
extern char gameVersion[];
extern char gameDescriptionText[256];
extern char gamePlatform[];
extern char gameRenderType[];
extern char gameHapticsSetting[];
extern uint8_t gameMode;
extern uint8_t gameLanguage;
extern int gameMessage;
extern uint8_t gameOnlineActive;
extern uint8_t gameHapticsEnabled;
extern uint8_t frameCounter;
extern int frameSkipTimer;
extern int frameSkipSetting;
extern int gameSFXVolume;
extern int gameBGMVolume;
extern uint8_t gameTrialMode;
extern int gamePlatformID;
extern bool HQ3DFloorEnabled;
extern int SCREEN_XSIZE;
extern int SCREEN_CENTER;
extern int SCREEN_SCROLL_LEFT;
extern int SCREEN_SCROLL_RIGHT;
extern int OBJECT_BORDER_X1;
extern int OBJECT_BORDER_X2;
extern int SinValue256[256];
extern int CosValue256[256];
extern int SinValue512[512];
extern int CosValue512[512];
extern int SinValueM7[512];
extern int CosValueM7[512];
extern uint8_t ATanValue256[256][256];

void Init_GlobalAppDefinitions(void);
void GlobalAppDefinitions_CalculateTrigAngles(void);
uint8_t GlobalAppDefinitions_ArcTanLookup(int x, int y);
void GlobalAppDefinitions_LoadGameConfig(char* filePath);

#endif /* GlobalAppDefinitions_h */
