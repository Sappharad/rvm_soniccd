//
//  EngineCallbacks.h
//  rvm
//

#ifndef EngineCallbacks_h
#define EngineCallbacks_h

#include <stdio.h>
#include "FileIO.h"
#include "AudioPlayback.h"
#include "GlobalAppDefinitions.h"
#include "GraphicsSystem.h"
#include "StageSystem.h"
#include "ObjectSystem.h"
#include "RenderDevice.h"
#ifdef ENABLE_MPV
#include "MpvPlayer.h"
#endif

void EngineCallbacks_PlayVideoFile(char* fileName);
void EngineCallbacks_OnlineSetAchievement(int achievementID, int achievementDone);
void EngineCallbacks_OnlineSetLeaderboard(int leaderboardID, int result);
void EngineCallbacks_OnlineLoadAchievementsMenu(void);
void EngineCallbacks_OnlineLoadLeaderboardsMenu(void);
void EngineCallbacks_RetroEngineCallback(int callbackID);
void EngineCallbacks_UpsellScreen(void);
void EngineCallbacks_ConfirmationScreen(bool yes);
void EngineCallbacks_ExitConfirmation(bool yes);
void EngineCallbacks_StartupRetroEngine(void);
void EngineCallbacks_ProcessMainLoop(void);

#endif /* EngineCallbacks_h */
