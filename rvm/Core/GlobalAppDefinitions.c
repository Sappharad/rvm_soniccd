//
//  GlobalAppDefinitions.c
//  rvm
//

#include "GlobalAppDefinitions.h"

const int RETRO_EN = 0;
const int RETRO_FR = 1;
const int RETRO_IT = 2;
const int RETRO_DE = 3;
const int RETRO_ES = 4;
const int RETRO_JP = 5;
const int DEFAULTSCREEN = 0;
const int MAINGAME = 1;
const int RESETGAME = 2;
const int EXITGAME = 3;
const int SCRIPTERROR = 4;
const int ENTER_HIRESMODE = 5;
const int EXIT_HIRESMODE = 6;
const int PAUSE_ENGINE = 7;
const int PAUSE_WAIT = 8;
const int VIDEO_WAIT = 9;
const int RETRO_WIN = 0;
const int RETRO_OSX = 1;
const int RETRO_XBOX_360 = 2;
const int RETRO_PS3 = 3;
const int RETRO_iOS = 4;
const int RETRO_ANDROID = 5;
const int RETRO_WP7 = 6;
const int MAX_PLAYERS = 2;
const int FACING_LEFT = 1;
const int FACING_RIGHT = 0;
const int GAME_FULL = 0;
const int GAME_TRIAL = 1;
const int OBJECT_BORDER_Y1 = 256;
const int OBJECT_BORDER_Y2 = 496;
const double Pi = 3.141592654;
char gameWindowText[] = "Retro-Engine";
char gameVersion[] = "2.0.1";
char gameDescriptionText[256];
char gamePlatform[] = "Mobile";
char gameRenderType[] = "HW_Rendering";
char gameHapticsSetting[] = "No_Haptics";
uint8_t gameMode;
uint8_t gameLanguage;
int gameMessage;
uint8_t gameOnlineActive;
uint8_t gameHapticsEnabled;
uint8_t frameCounter;
int frameSkipTimer;
int frameSkipSetting;
int gameSFXVolume;
int gameBGMVolume;
uint8_t gameTrialMode;
int gamePlatformID;
bool HQ3DFloorEnabled;
int SCREEN_XSIZE;
int SCREEN_CENTER;
int SCREEN_SCROLL_LEFT;
int SCREEN_SCROLL_RIGHT;
int OBJECT_BORDER_X1;
int OBJECT_BORDER_X2;
int SinValue256[256];
int CosValue256[256];
int SinValue512[512];
int CosValue512[512];
int SinValueM7[512];
int CosValueM7[512];
uint8_t ATanValue256[256][256];

void Init_GlobalAppDefinitions()
{
    gameMode = MAINGAME;
    gameLanguage = RETRO_EN;
    gameMessage = 0;
    gameOnlineActive = 0;
    gameHapticsEnabled = 0;
    frameCounter = 0;
    frameSkipTimer = -1;
    frameSkipSetting = 0;
    gameSFXVolume = 100;
    gameBGMVolume = 100;
    gameTrialMode = GAME_FULL;
    gamePlatformID = RETRO_iOS;
    HQ3DFloorEnabled = false;
    SCREEN_XSIZE = 320;
}
void GlobalAppDefinitions_CalculateTrigAngles()
{
    for (int i = 0; i < 512; i++)
    {
        SinValueM7[i] = (int)(sin((double)i / 256.0 * 3.141592654) * 4096.0);
        CosValueM7[i] = (int)(cos((double)i / 256.0 * 3.141592654) * 4096.0);
    }
    SinValueM7[0] = 0;
    CosValueM7[0] = 4096;
    SinValueM7[128] = 4096;
    CosValueM7[128] = 0;
    SinValueM7[256] = 0;
    CosValueM7[256] = -4096;
    SinValueM7[384] = -4096;
    CosValueM7[384] = 0;
    for (int i = 0; i < 512; i++)
    {
        SinValue512[i] = (int)(sin((double)((float)i / 256.0f) * 3.141592654) * 512.0);
        CosValue512[i] = (int)(cos((double)((float)i / 256.0f) * 3.141592654) * 512.0);
    }
    SinValue512[0] = 0;
    CosValue512[0] = 512;
    SinValue512[128] = 512;
    CosValue512[128] = 0;
    SinValue512[256] = 0;
    CosValue512[256] = -512;
    SinValue512[384] = -512;
    CosValue512[384] = 0;
    for (int i = 0; i < 256; i++)
    {
        SinValue256[i] = SinValue512[i << 1] >> 1;
        CosValue256[i] = CosValue512[i << 1] >> 1;
    }
    for (int j = 0; j < 256; j++)
    {
        for (int i = 0; i < 256; i++)
        {
            ATanValue256[i][j] = (uint8_t)(atan2((double)((float)j), (double)((float)i)) * 40.74366542620519);
        }
    }
}
uint8_t GlobalAppDefinitions_ArcTanLookup(int x, int y)
{
    int i;
    if (x < 0)
    {
        i = -x;
    }
    else
    {
        i = x;
    }
    int j;
    if (y < 0)
    {
        j = -y;
    }
    else
    {
        j = y;
    }
    if (i > j)
    {
        while (i > 255)
        {
            i >>= 4;
            j >>= 4;
        }
    }
    else
    {
        while (j > 255)
        {
            i >>= 4;
            j >>= 4;
        }
    }
    uint8_t result;
    if (x > 0)
    {
        if (y > 0)
        {
            result = ATanValue256[i][j];
        }
        else
        {
            result = (uint8_t)(256 - (int)ATanValue256[i][j]);
        }
    }
    else
    {
        if (y > 0)
        {
            result = (uint8_t)(128 - ATanValue256[i][j]);
        }
        else
        {
            result = (uint8_t)(128 + ATanValue256[i][j]);
        }
    }
    return result;
}
void GlobalAppDefinitions_LoadGameConfig(char* filePath)
{
    struct FileData fData;
    char array[32];
    if (FileIO_LoadFile(filePath, &fData))
    {
        uint8_t b = FileIO_ReadByte();
        FileIO_ReadCharArray(gameWindowText, (int)b);
        gameWindowText[(int)b] = '\0';
        b = FileIO_ReadByte();
        for (int i = 0; i < (int)b; i++)
        {
            FileIO_ReadByte();
        }
        b = FileIO_ReadByte();
        FileIO_ReadCharArray(gameDescriptionText, (int)b);
        gameDescriptionText[(int)b] = '\0';
        uint8_t b3 = FileIO_ReadByte();
        for (int j = 0; j < (int)b3; j++)
        {
            b = FileIO_ReadByte();
            for (int i = 0; i < (int)b; i++)
            {
                FileIO_ReadByte();
            }
        }
        for (int j = 0; j < (int)b3; j++)
        {
            b = FileIO_ReadByte();
            for (int i = 0; i < (int)b; i++)
            {
                FileIO_ReadByte();
            }
        }
        b3 = FileIO_ReadByte();
        NO_GLOBALVARIABLES = 0;
        for (int j = 0; j < (int)b3; j++)
        {
            NO_GLOBALVARIABLES += 1;
            b = FileIO_ReadByte();
            int i;
            uint8_t b2;
            for (i = 0; i < (int)b; i++)
            {
                b2 = FileIO_ReadByte();
                array[i] = (char)b2;
            }
            array[i] = '\0';
            FileIO_StrCopy2D(globalVariableNames, 32, array, sizeof(array), j);
            b2 = FileIO_ReadByte();
            globalVariables[j] = (int)b2 << 24;
            b2 = FileIO_ReadByte();
            globalVariables[j] += (int)b2 << 16;
            b2 = FileIO_ReadByte();
            globalVariables[j] += (int)b2 << 8;
            b2 = FileIO_ReadByte();
            globalVariables[j] += (int)b2;
        }
        b3 = FileIO_ReadByte();
        for (int j = 0; j < (int)b3; j++)
        {
            b = FileIO_ReadByte();
            int i;
            for (i = 0; i < (int)b; i++)
            {
                uint8_t b2 = FileIO_ReadByte();
                array[i] = (char)b2;
            }
            array[i] = '\0';
        }
        b3 = FileIO_ReadByte();
        for (int j = 0; j < (int)b3; j++)
        {
            b = FileIO_ReadByte();
            for (int i = 0; i < (int)b; i++)
            {
                FileIO_ReadByte();
            }
        }
        noPresentationStages = FileIO_ReadByte();
        for (int j = 0; j < (int)noPresentationStages; j++)
        {
            b = FileIO_ReadByte();
            int i;
            for (i = 0; i < (int)b; i++)
            {
                uint8_t b2 = FileIO_ReadByte();
                pStageList[j].stageFolderName[i] = (char)b2;
            }
            pStageList[j].stageFolderName[i] = '\0';
            b = FileIO_ReadByte();
            for (i = 0; i < (int)b; i++)
            {
                uint8_t b2 = FileIO_ReadByte();
                pStageList[j].actNumber[i] = (char)b2;
            }
            pStageList[j].actNumber[i] = '\0';
            b = FileIO_ReadByte();
            for (i = 0; i < (int)b; i++)
            {
                FileIO_ReadByte();
            }
            b = FileIO_ReadByte();
        }
        noZoneStages = FileIO_ReadByte();
        for (int j = 0; j < (int)noZoneStages; j++)
        {
            b = FileIO_ReadByte();
            int i;
            for (i = 0; i < (int)b; i++)
            {
                uint8_t b2 = FileIO_ReadByte();
                zStageList[j].stageFolderName[i] = (char)b2;
            }
            zStageList[j].stageFolderName[i] = '\0';
            b = FileIO_ReadByte();
            for (i = 0; i < (int)b; i++)
            {
                uint8_t b2 = FileIO_ReadByte();
                zStageList[j].actNumber[i] = (char)b2;
            }
            zStageList[j].actNumber[i] = '\0';
            b = FileIO_ReadByte();
            for (i = 0; i < (int)b; i++)
            {
                FileIO_ReadByte();
            }
            b = FileIO_ReadByte();
        }
        noSpecialStages = FileIO_ReadByte();
        for (int j = 0; j < (int)noSpecialStages; j++)
        {
            b = FileIO_ReadByte();
            int i;
            for (i = 0; i < (int)b; i++)
            {
                uint8_t b2 = FileIO_ReadByte();
                sStageList[j].stageFolderName[i] = (char)b2;
            }
            sStageList[j].stageFolderName[i] = '\0';
            b = FileIO_ReadByte();
            for (i = 0; i < (int)b; i++)
            {
                uint8_t b2 = FileIO_ReadByte();
                sStageList[j].actNumber[i] = (char)b2;
            }
            sStageList[j].actNumber[i] = '\0';
            b = FileIO_ReadByte();
            for (i = 0; i < (int)b; i++)
            {
                FileIO_ReadByte();
            }
            b = FileIO_ReadByte();
        }
        noBonusStages = FileIO_ReadByte();
        for (int j = 0; j < (int)noBonusStages; j++)
        {
            b = FileIO_ReadByte();
            int i;
            for (i = 0; i < (int)b; i++)
            {
                uint8_t b2 = FileIO_ReadByte();
                bStageList[j].stageFolderName[i] = (char)b2;
            }
            bStageList[j].stageFolderName[i] = '\0';
            b = FileIO_ReadByte();
            for (i = 0; i < (int)b; i++)
            {
                uint8_t b2 = FileIO_ReadByte();
                bStageList[j].actNumber[i] = (char)b2;
            }
            bStageList[j].actNumber[i] = '\0';
            b = FileIO_ReadByte();
            for (i = 0; i < (int)b; i++)
            {
                FileIO_ReadByte();
            }
            b = FileIO_ReadByte();
        }
        FileIO_CloseFile();
    }
}
