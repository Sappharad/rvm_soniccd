//
//  EngineCallbacks.c
//  rvm
//

#include "EngineCallbacks.h"

#define DISPLAY_LOGOS 0
#define TITLE_SCREEN_PRESS_START 1
#define ENTER_TIMEATTACK_NOTIFY 2
#define EXIT_TIMEATTACK_NOTIFY 3
#define FINISH_GAME_NOTIFY 4
#define RETURN_TO_STORE_SELECTED 5
#define RESTART_GAME_SELECTED 6
#define EXIT_GAME_SELECTED 7
#define UNLOCK_FULL_GAME_SELECTED 8
#define TERMS_SELECTED 9
#define PRIVACY_SELECTED 10
#define TRIAL_ENDED 11
#define SETTINGS_SELECTED 12
#define FULL_VERSION_ONLY 14

int prevMessage;
bool engineInit;
int waitValue;

void EngineCallbacks_PlayVideoFile(char* fileName)
{
    AudioPlayback_StopMusic();
    
    //Show video here
    
    waitValue = 0;
    gameMode = 9;
}
void EngineCallbacks_OnlineSetAchievement(int achievementID, int achievementDone)
{
    if (achievementDone > 99 && gameOnlineActive == 1 && gameTrialMode == 0)
    {
        switch (achievementID)
        {
            case 0:
                //AwardAchievement("88 Miles Per Hour");
                return;
            case 1:
                //AwardAchievement("Just One Hug is Enough");
                return;
            case 2:
                //AwardAchievement("Paradise Found");
                return;
            case 3:
                //AwardAchievement("Take the High Road");
                return;
            case 4:
                //AwardAchievement("King of the Rings");
                return;
            case 5:
                //AwardAchievement("Statue Saviour");
                return;
            case 6:
                //AwardAchievement("Heavy Metal");
                return;
            case 7:
                //AwardAchievement("All Stages Clear");
                return;
            case 8:
                //AwardAchievement("Treasure Hunter");
                return;
            case 9:
                //AwardAchievement("Dr Eggman Got Served");
                return;
            case 10:
                //AwardAchievement("Just In Time");
                return;
            case 11:
                //AwardAchievement("Saviour of the Planet");
                break;
            default:
                return;
        }
    }
}
void EngineCallbacks_OnlineSetLeaderboard(int leaderboardID, int result)
{
    //SetLeaderboard(leaderboardID, result);
    switch (leaderboardID)
    {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
            return;
    }
}
void EngineCallbacks_OnlineLoadAchievementsMenu()
{
    //Stub
}
void EngineCallbacks_OnlineLoadLeaderboardsMenu()
{
    //Stub
}

void EngineCallbacks_RetroEngineCallback(int callbackID)
{
    switch (callbackID)
    {
        case DISPLAY_LOGOS:
        case TITLE_SCREEN_PRESS_START:
        case ENTER_TIMEATTACK_NOTIFY:
        case EXIT_TIMEATTACK_NOTIFY:
        case FINISH_GAME_NOTIFY:
        case RETURN_TO_STORE_SELECTED:
        case TRIAL_ENDED:
        case SETTINGS_SELECTED:
        case 13:
            break;
        case RESTART_GAME_SELECTED:
            //Display restart prompt
            break;
        case EXIT_GAME_SELECTED:
            //Display exit prompt
            break;
        case UNLOCK_FULL_GAME_SELECTED:
        {
            //Open store to buy the game
            break;
        }
        case TERMS_SELECTED:
        {
            //Display mobile terms of service
            return;
        }
        case PRIVACY_SELECTED:
        {
            //Display mobile privacy policy
            return;
        }
        case FULL_VERSION_ONLY:
            //Upsell the full game
            break;
        default:
            return;
    }
}
void EngineCallbacks_UpsellScreen()
{
}
void EngineCallbacks_ConfirmationScreen()
{
}
void EngineCallbacks_ExitConfirmation()
{
}
void EngineCallbacks_StartupRetroEngine()
{
    if (!engineInit)
    {
        GlobalAppDefinitions_CalculateTrigAngles();
        GraphicsSystem_GenerateBlendLookupTable();
        if (FileIO_CheckRSDKFile())
        {
            GlobalAppDefinitions_LoadGameConfig("Data/Game/GameConfig.bin");
        }
        InitAudioPlayback();
        StageSystem_InitFirstStage();
        //StageSystem_InitStageSelectMenu(); //Init this instead of first stage for Retro Engine dev menu.
        ObjectSystem_ClearScriptData();
        engineInit = true;
        return;
    }
    RenderDevice_UpdateHardwareTextures();
}
void EngineCallbacks_ProcessMainLoop()
{
    switch (gameMode)
    {
        case 0:
            gfxIndexSize = 0;
            gfxVertexSize = 0;
            gfxIndexSizeOpaque = 0;
            gfxVertexSizeOpaque = 0;
            StageSystem_ProcessStageSelectMenu();
            return;
        case 1:
            gfxIndexSize = 0;
            gfxVertexSize = 0;
            gfxIndexSizeOpaque = 0;
            gfxVertexSizeOpaque = 0;
            vertexSize3D = 0;
            indexSize3D = 0;
            render3DEnabled = false;
            StageSystem_ProcessStage();
            if (prevMessage == gameMessage)
            {
                gameMessage = 0;
                prevMessage = 0;
                return;
            }
            prevMessage = gameMessage;
            return;
        case 2:
            GlobalAppDefinitions_LoadGameConfig("Data/Game/GameConfig.bin");
            StageSystem_InitFirstStage();
            FileIO_ResetCurrentStageFolder();
            return;
        case 3:
        case 7:
            break;
        case 4:
            GlobalAppDefinitions_LoadGameConfig("Data/Game/GameConfig.bin");
            StageSystem_InitErrorMessage();
            FileIO_ResetCurrentStageFolder();
            return;
        case 5:
            gameMode = 1;
            highResMode = 1;
            return;
        case 6:
            gameMode = 1;
            highResMode = 0;
            return;
        case 8:
            if (waitValue < 8)
            {
                waitValue++;
                return;
            }
            gameMode = 1;
            return;
        case 9:
            if (waitValue < 60)
            {
                waitValue++;
                return;
            }
            gameMode = 1;
            break;
        default:
            return;
    }
}
