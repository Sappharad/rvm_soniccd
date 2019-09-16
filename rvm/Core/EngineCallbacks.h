//
//  EngineCallbacks.h
//  rvm
//

#ifndef EngineCallbacks_h
#define EngineCallbacks_h

#include <stdio.h>
#include "FileIO.h"
#if DREAMCAST
#include "AudioPlayback_dc.h"
#else
#include "AudioPlayback.h"
#endif
#include "GlobalAppDefinitions.h"
#include "GraphicsSystem.h"
#include "StageSystem.h"
#include "ObjectSystem.h"
#if DREAMCAST
#include "RenderDevice_dc.h"
#else
#include "RenderDevice.h"
#endif

void EngineCallbacks_PlayVideoFile(char* fileName);
void EngineCallbacks_OnlineSetAchievement(int achievementID, int achievementDone);
void EngineCallbacks_OnlineSetLeaderboard(int leaderboardID, int result);
void EngineCallbacks_OnlineLoadAchievementsMenu(void);
void EngineCallbacks_OnlineLoadLeaderboardsMenu(void);
void EngineCallbacks_RetroEngineCallback(int callbackID);
void EngineCallbacks_UpsellScreen(void);
void EngineCallbacks_ConfirmationScreen(void);
void EngineCallbacks_ExitConfirmation(void);
void EngineCallbacks_StartupRetroEngine(void);
void EngineCallbacks_ProcessMainLoop(void);

#endif /* EngineCallbacks_h */
