//
//  ObjectSystem.c
//  rvm
//

#include "ObjectSystem.h"

const int SUB_MAIN = 0;
const int SUB_PLAYER = 1;
const int SUB_DRAW = 2;
const int SUB_STARTUP = 3;
const int NUM_ARITHMETIC_TOKENS = 13;
const int NUM_EVALUATION_TOKENS = 6;
const int NUM_OPCODES = 135;
const int NUM_VARIABLE_NAMES = 229;
const int NUM_CONSTANTS = 31;
const int SCRIPT_DATA_SIZE = 0x40000;
const int JUMP_TABLE_SIZE = 0x4000;
int scriptData[0x40000];
int scriptDataPos;
int scriptDataOffset;
int scriptLineNumber;
int jumpTableData[0x4000];
int jumpTableDataPos;
int jumpTableOffset;
int jumpTableStack[0x400];
int jumpTableStackPos;
int NUM_FUNCTIONS;
int functionStack[0x400];
int functionStackPos;
struct SpriteFrame scriptFrames[0x1000];
int scriptFramesNo;
uint8_t NO_GLOBALVARIABLES;
char globalVariableNames[0x100][32];
int globalVariables[0x100];
int objectLoop;
struct ScriptEngine scriptEng;
char scriptText[0x100];
struct ObjectScript objectScriptList[0x100];
struct FunctionScript functionScriptList[0x200];
struct ObjectEntity objectEntityList[0x4A0];
struct ObjectDrawList objectDrawOrderList[7];
int playerNum;
struct CollisionSensor object_cSensor[6];

char functionNames[0x200][32];
char typeNames[0x100][32];
signed char scriptOpcodeSizes[] = { 0, 2, 2, 2, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 0, 0, 3, 3, 3, 3, 3, 3, 0, 2, 0, 0, 2, 2, 2, 2,
    2, 5, 5, 3, 4, 7, 1, 1, 1, 3, 3, 4, 7, 7, 3, 6, 6, 5, 3, 4, 3, 7, 2, 1, 4, 4, 1,
    4, 3, 4, 0, 8, 5, 5, 4, 2, 0, 0, 0, 0, 0, 3, 1, 0, 2, 1, 3, 4, 4, 1, 0, 2, 1, 1,
    0, 1, 2, 4, 4, 2, 2, 2, 4, 3, 1, 0, 6, 4, 4, 4, 3, 3, 0, 0, 1, 2, 3, 3, 4, 2, 4,
    2, 0, 0, 1, 3, 7, 5, 2, 2, 2, 1, 1, 4 };

void Init_ObjectSystem()
{
    scriptDataPos = 0;
    jumpTableDataPos = 0;
    jumpTableStackPos = 0;
    functionStackPos = 0;
    scriptFramesNo = 0;
    time_t t;
    srand((unsigned) time(&t));
}

void ObjectSystem_BasicCollision(int cLeft, int cTop, int cRight, int cBottom)
{
    struct PlayerObject* playerObject = &playerList[playerNum];
    struct CollisionBox* collisionBox = &collisionBoxList[playerObject->animationFile->cbListOffset + animationFrames[animationList[playerObject->animationFile->aniListOffset + playerObject->objectPtr->animation].frameListOffset + playerObject->objectPtr->frame].collisionBox];
    collisionLeft = playerObject->xPos >> 16;
    collisionTop = playerObject->yPos >> 16;
    collisionRight = collisionLeft;
    collisionBottom = collisionTop;
    collisionLeft = collisionLeft + collisionBox->left[0];
    collisionTop = collisionTop + collisionBox->top[0];
    collisionRight = collisionRight + collisionBox->right[0];
    collisionBottom = collisionBottom + collisionBox->bottom[0];
    if (collisionRight > cLeft && collisionLeft < cRight && collisionBottom > cTop && collisionTop < cBottom)
    {
        scriptEng.checkResult = 1;
        return;
    }
    scriptEng.checkResult = 0;
}

void ObjectSystem_BoxCollision(int cLeft, int cTop, int cRight, int cBottom)
{
    int i = 0;
    struct PlayerObject* playerObject = &playerList[playerNum];
    struct CollisionBox* collisionBox = &collisionBoxList[playerObject->animationFile->cbListOffset + animationFrames[animationList[playerObject->animationFile->aniListOffset + playerObject->objectPtr->animation].frameListOffset + playerObject->objectPtr->frame].collisionBox];
    collisionLeft = collisionBox->left[0];
    collisionTop = collisionBox->top[0];
    collisionRight = collisionBox->right[0];
    collisionBottom = collisionBox->bottom[0];
    scriptEng.checkResult = 0;
    switch (playerObject->collisionMode)
    {
        case 0:
        case 2:
        {
            if (playerObject->xVelocity == 0)
            {
                i = abs(playerObject->speed);
                break;
            }
            else
            {
                i = abs(playerObject->xVelocity);
                break;
            }
        }
        case 1:
        case 3:
        {
            i = abs(playerObject->xVelocity);
            break;
        }
    }
    if (i <= abs(playerObject->yVelocity))
    {
        object_cSensor[0].collided = 0;
        object_cSensor[1].collided = 0;
        object_cSensor[2].collided = 0;
        object_cSensor[0].xPos = playerObject->xPos + ((collisionLeft + 2) << 16);
        object_cSensor[1].xPos = playerObject->xPos;
        object_cSensor[2].xPos = playerObject->xPos + ((collisionRight - 2) << 16);
        object_cSensor[0].yPos = playerObject->yPos + (collisionBottom << 16);
        object_cSensor[1].yPos = object_cSensor[0].yPos;
        object_cSensor[2].yPos = object_cSensor[0].yPos;
        if (playerObject->yVelocity > -1)
        {
            for (i = 0; i < 3; i++)
            {
                if (object_cSensor[i].xPos > cLeft && object_cSensor[i].xPos < cRight && object_cSensor[i].yPos >= cTop && playerObject->yPos - playerObject->yVelocity < cTop)
                {
                    object_cSensor[i].collided = 1;
                    playerObject->flailing[i] = 1;
                }
            }
        }
        if (object_cSensor[0].collided == 1 || object_cSensor[1].collided == 1 || object_cSensor[2].collided == 1)
        {
            if (playerObject->gravity == 0 && (playerObject->collisionMode == 1 || playerObject->collisionMode == 3))
            {
                playerObject->xVelocity = 0;
                playerObject->speed = 0;
            }
            playerObject->yPos = cTop - (collisionBottom << 16);
            playerObject->gravity = 0;
            playerObject->yVelocity = 0;
            playerObject->angle = 0;
            playerObject->objectPtr->rotation = 0;
            playerObject->controlLock = 0;
            scriptEng.checkResult = 1;
            return;
        }
        object_cSensor[0].collided = 0;
        object_cSensor[1].collided = 0;
        object_cSensor[0].xPos = playerObject->xPos + ((collisionLeft + 2) << 16);
        object_cSensor[1].xPos = playerObject->xPos + ((collisionRight - 2) << 16);
        object_cSensor[0].yPos = playerObject->yPos + (collisionTop << 16);
        object_cSensor[1].yPos = object_cSensor[0].yPos;
        for (i = 0; i < 2; i++)
        {
            if (object_cSensor[i].xPos > cLeft && object_cSensor[i].xPos < cRight && object_cSensor[i].yPos <= cBottom && playerObject->yPos - playerObject->yVelocity > cBottom)
            {
                object_cSensor[i].collided = 1;
            }
        }
        if (object_cSensor[0].collided == 1 || object_cSensor[1].collided == 1)
        {
            if (playerObject->gravity == 1)
            {
                playerObject->yPos = cBottom - (collisionTop << 16);
            }
            if (playerObject->yVelocity < 1)
            {
                playerObject->yVelocity = 0;
            }
            scriptEng.checkResult = 4;
            return;
        }
        object_cSensor[0].collided = 0;
        object_cSensor[1].collided = 0;
        object_cSensor[0].xPos = playerObject->xPos + (collisionRight << 16);
        object_cSensor[1].xPos = object_cSensor[0].xPos;
        object_cSensor[0].yPos = playerObject->yPos - 0x20000;
        object_cSensor[1].yPos = playerObject->yPos + 0x80000;
        for (i = 0; i < 2; i++)
        {
            if (object_cSensor[i].xPos >= cLeft && playerObject->xPos - playerObject->xVelocity < cLeft && object_cSensor[1].yPos > cTop && object_cSensor[0].yPos < cBottom)
            {
                object_cSensor[i].collided = 1;
            }
        }
        if (object_cSensor[0].collided == 1 || object_cSensor[1].collided == 1)
        {
            playerObject->xPos = cLeft - (collisionRight << 16);
            if (playerObject->xVelocity > 0)
            {
                if (playerObject->objectPtr->direction == 0)
                {
                    playerObject->pushing = 2;
                }
                playerObject->xVelocity = 0;
                playerObject->speed = 0;
            }
            scriptEng.checkResult = 2;
            return;
        }
        object_cSensor[0].collided = 0;
        object_cSensor[1].collided = 0;
        object_cSensor[0].xPos = playerObject->xPos + (collisionLeft << 16);
        object_cSensor[1].xPos = object_cSensor[0].xPos;
        object_cSensor[0].yPos = playerObject->yPos - 0x20000;
        object_cSensor[1].yPos = playerObject->yPos + 0x80000;
        for (i = 0; i < 2; i++)
        {
            if (object_cSensor[i].xPos <= cRight && playerObject->xPos - playerObject->xVelocity > cRight && object_cSensor[1].yPos > cTop && object_cSensor[0].yPos < cBottom)
            {
                object_cSensor[i].collided = 1;
            }
        }
        if (object_cSensor[0].collided == 1 || object_cSensor[1].collided == 1)
        {
            playerObject->xPos = cRight - (collisionLeft << 16);
            if (playerObject->xVelocity < 0)
            {
                if (playerObject->objectPtr->direction == 1)
                {
                    playerObject->pushing = 2;
                }
                playerObject->xVelocity = 0;
                playerObject->speed = 0;
            }
            scriptEng.checkResult = 3;
        }
    }
    else
    {
        object_cSensor[0].collided = 0;
        object_cSensor[1].collided = 0;
        object_cSensor[0].xPos = playerObject->xPos + (collisionRight << 16);
        object_cSensor[1].xPos = object_cSensor[0].xPos;
        object_cSensor[0].yPos = playerObject->yPos - 0x20000;
        object_cSensor[1].yPos = playerObject->yPos + 0x80000;
        for (i = 0; i < 2; i++)
        {
            if (object_cSensor[i].xPos >= cLeft && playerObject->xPos - playerObject->xVelocity < cLeft && object_cSensor[1].yPos > cTop && object_cSensor[0].yPos < cBottom)
            {
                object_cSensor[i].collided = 1;
            }
        }
        if (object_cSensor[0].collided == 1 || object_cSensor[1].collided == 1)
        {
            playerObject->xPos = cLeft - (collisionRight << 16);
            if (playerObject->xVelocity > 0)
            {
                if (playerObject->objectPtr->direction == 0)
                {
                    playerObject->pushing = 2;
                }
                playerObject->xVelocity = 0;
                playerObject->speed = 0;
            }
            scriptEng.checkResult = 2;
            return;
        }
        object_cSensor[0].collided = 0;
        object_cSensor[1].collided = 0;
        object_cSensor[0].xPos = playerObject->xPos + (collisionLeft << 16);
        object_cSensor[1].xPos = object_cSensor[0].xPos;
        object_cSensor[0].yPos = playerObject->yPos - 0x20000;
        object_cSensor[1].yPos = playerObject->yPos + 0x80000;
        for (i = 0; i < 2; i++)
        {
            if (object_cSensor[i].xPos <= cRight && playerObject->xPos - playerObject->xVelocity > cRight && object_cSensor[1].yPos > cTop && object_cSensor[0].yPos < cBottom)
            {
                object_cSensor[i].collided = 1;
            }
        }
        if (object_cSensor[0].collided == 1 || object_cSensor[1].collided == 1)
        {
            playerObject->xPos = cRight - (collisionLeft << 16);
            if (playerObject->xVelocity < 0)
            {
                if (playerObject->objectPtr->direction == 1)
                {
                    playerObject->pushing = 2;
                }
                playerObject->xVelocity = 0;
                playerObject->speed = 0;
            }
            scriptEng.checkResult = 3;
            return;
        }
        object_cSensor[0].collided = 0;
        object_cSensor[1].collided = 0;
        object_cSensor[2].collided = 0;
        object_cSensor[0].xPos = playerObject->xPos + ((collisionLeft + 2) << 16);
        object_cSensor[1].xPos = playerObject->xPos;
        object_cSensor[2].xPos = playerObject->xPos + ((collisionRight - 2) << 16);
        object_cSensor[0].yPos = playerObject->yPos + (collisionBottom << 16);
        object_cSensor[1].yPos = object_cSensor[0].yPos;
        object_cSensor[2].yPos = object_cSensor[0].yPos;
        if (playerObject->yVelocity > -1)
        {
            for (i = 0; i < 3; i++)
            {
                if (object_cSensor[i].xPos > cLeft && object_cSensor[i].xPos < cRight && object_cSensor[i].yPos >= cTop && playerObject->yPos - playerObject->yVelocity < cTop)
                {
                    object_cSensor[i].collided = 1;
                    playerObject->flailing[i] = 1;
                }
            }
        }
        if (object_cSensor[0].collided == 1 || object_cSensor[1].collided == 1 || object_cSensor[2].collided == 1)
        {
            if (playerObject->gravity == 0 && (playerObject->collisionMode == 1 || playerObject->collisionMode == 3))
            {
                playerObject->xVelocity = 0;
                playerObject->speed = 0;
            }
            playerObject->yPos = cTop - (collisionBottom << 16);
            playerObject->gravity = 0;
            playerObject->yVelocity = 0;
            playerObject->angle = 0;
            playerObject->objectPtr->rotation = 0;
            playerObject->controlLock = 0;
            scriptEng.checkResult = 1;
            return;
        }
        object_cSensor[0].collided = 0;
        object_cSensor[1].collided = 0;
        object_cSensor[0].xPos = playerObject->xPos + ((collisionLeft + 2) << 16);
        object_cSensor[1].xPos = playerObject->xPos + ((collisionRight - 2) << 16);
        object_cSensor[0].yPos = playerObject->yPos + (collisionTop << 16);
        object_cSensor[1].yPos = object_cSensor[0].yPos;
        for (i = 0; i < 2; i++)
        {
            if (object_cSensor[i].xPos > cLeft && object_cSensor[i].xPos < cRight && object_cSensor[i].yPos <= cBottom && playerObject->yPos - playerObject->yVelocity > cBottom)
            {
                object_cSensor[i].collided = 1;
            }
        }
        if (object_cSensor[0].collided == 1 || object_cSensor[1].collided == 1)
        {
            if (playerObject->gravity == 1)
            {
                playerObject->yPos = cBottom - (collisionTop << 16);
            }
            if (playerObject->yVelocity < 1)
            {
                playerObject->yVelocity = 0;
            }
            scriptEng.checkResult = 4;
            return;
        }
    }
}

void ObjectSystem_ClearScriptData()
{
    int i;
    char charArray[] = "BlankObject";
    for (i = 0; i < 0x40000; i++)
    {
        scriptData[i] = 0;
    }
    for (i = 0; i < 0x4000; i++)
    {
        jumpTableData[i] = 0;
    }
    scriptDataPos = 0;
    jumpTableDataPos = 0;
    scriptFramesNo = 0;
    NUM_FUNCTIONS = 0;
    AnimationSystem_ClearAnimationData();
    for (i = 0; i < 2; i++)
    {
        playerList[i].animationFile = AnimationSystem_GetDefaultAnimationRef();
        playerList[i].objectPtr = &objectEntityList[0];
    }
    for (i = 0; i < 0x100; i++)
    {
        objectScriptList[i].mainScript = 0x3ffff;
        objectScriptList[i].mainJumpTable = 0x3fff;
        objectScriptList[i].playerScript = 0x3ffff;
        objectScriptList[i].playerJumpTable = 0x3fff;
        objectScriptList[i].drawScript = 0x3ffff;
        objectScriptList[i].drawJumpTable = 0x3fff;
        objectScriptList[i].startupScript = 0x3ffff;
        objectScriptList[i].startupJumpTable = 0x3fff;
        objectScriptList[i].frameListOffset = 0;
        objectScriptList[i].numFrames = 0;
        objectScriptList[i].surfaceNum = 0;
        objectScriptList[i].animationFile = AnimationSystem_GetDefaultAnimationRef();
        functionScriptList[i].mainScript = 0x3ffff;
        functionScriptList[i].mainJumpTable = 0x3fff;
        typeNames[i][0] = '\0';
    }
    ObjectSystem_SetObjectTypeName(charArray, 0);
}

void ObjectSystem_DrawObjectList(int DrawListNo)
{
    int num = objectDrawOrderList[DrawListNo].listSize;
    for (int i = 0; i < num; i++)
    {
        objectLoop = objectDrawOrderList[DrawListNo].entityRef[i];
        if (objectEntityList[objectLoop].type > 0)
        {
            playerNum = 0;
            if (scriptData[objectScriptList[objectEntityList[objectLoop].type].drawScript] > 0)
            {
                ObjectSystem_ProcessScript(objectScriptList[objectEntityList[objectLoop].type].drawScript, objectScriptList[objectEntityList[objectLoop].type].drawJumpTable, 2);
            }
        }
    }
}

void ObjectSystem_LoadByteCodeFile(int fileType, int scriptNum)
{
    int num;
    int num1;
    int i;
    uint8_t num2;
    struct FileData fileDatum;
    char charArray[] = "Data/Scripts/ByteCode/";
    char chrArray[] = ".bin";
    FileIO_StrCopy(scriptText, sizeof(scriptText), charArray, sizeof(charArray));
    switch (fileType)
    {
        case PRESENTATION_STAGE:
        {
            if(useOldSdkLayout){
                sprintf(scriptText,"%sPS%03d.bin",scriptText,stageListPosition);
            }
            else{
                FileIO_StrAdd(scriptText, sizeof(scriptText), pStageList[stageListPosition].stageFolderName, sizeof(pStageList[stageListPosition].stageFolderName));
                FileIO_StrAdd(scriptText, sizeof(scriptText), chrArray, sizeof(chrArray));
            }
            break;
        }
        case ZONE_STAGE:
        {
            if(useOldSdkLayout){
                sprintf(scriptText,"%sRS%03d.bin",scriptText,stageListPosition);
            }
            else{
                FileIO_StrAdd(scriptText, sizeof(scriptText), zStageList[stageListPosition].stageFolderName, sizeof(zStageList[stageListPosition].stageFolderName));
                FileIO_StrAdd(scriptText, sizeof(scriptText), chrArray, sizeof(chrArray));
            }
            break;
        }
        case BONUS_STAGE:
        {
            FileIO_StrAdd(scriptText, sizeof(scriptText), bStageList[stageListPosition].stageFolderName, sizeof(bStageList[stageListPosition].stageFolderName));
            FileIO_StrAdd(scriptText, sizeof(scriptText), chrArray, sizeof(chrArray));
            break;
        }
        case SPECIAL_STAGE:
        {
            if(useOldSdkLayout){
                sprintf(scriptText,"%sSS%03d.bin",scriptText,stageListPosition);
            }
            else{
                FileIO_StrAdd(scriptText, sizeof(scriptText), sStageList[stageListPosition].stageFolderName, sizeof(sStageList[stageListPosition].stageFolderName));
                FileIO_StrAdd(scriptText, sizeof(scriptText), chrArray, sizeof(chrArray));
            }
            break;
        }
        case 4:
        {
            if(useOldSdkLayout){
                sprintf(scriptText,"%sGS000.bin", scriptText);
            }
            else{
                sprintf(scriptText,"%sGlobalCode.bin", scriptText);
            }
            break;
        }
    }
    if (FileIO_LoadFile(scriptText, &fileDatum))
    {
        int num3 = scriptDataPos;
        uint8_t num4 = FileIO_ReadByte();
        int num5 = num4;
        num4 = FileIO_ReadByte();
        num5 = num5 + (num4 << 8);
        num4 = FileIO_ReadByte();
        num5 = num5 + (num4 << 16);
        num4 = FileIO_ReadByte();
        num5 = num5 + (num4 << 24);
        while (num5 > 0)
        {
            num4 = FileIO_ReadByte();
            num2 = (uint8_t)(num4 & 127);
            if (num4 >= 128)
            {
                while (num2 > 0)
                {
                    num4 = FileIO_ReadByte();
                    num = num4;
                    num4 = FileIO_ReadByte();
                    num = num + (num4 << 8);
                    num4 = FileIO_ReadByte();
                    num = num + (num4 << 16);
                    num4 = FileIO_ReadByte();
                    num = num + (num4 << 24);
                    scriptData[num3] = num;
                    num3++;
                    scriptDataPos = scriptDataPos + 1;
                    num5--;
                    num2 = (uint8_t)(num2 - 1);
                }
            }
            else
            {
                while (num2 > 0)
                {
                    num4 = FileIO_ReadByte();
                    scriptData[num3] = num4;
                    num3++;
                    scriptDataPos = scriptDataPos + 1;
                    num5--;
                    num2 = (uint8_t)(num2 - 1);
                }
            }
        }
        num3 = jumpTableDataPos;
        num4 = FileIO_ReadByte();
        num5 = num4;
        num4 = FileIO_ReadByte();
        num5 = num5 + (num4 << 8);
        num4 = FileIO_ReadByte();
        num5 = num5 + (num4 << 16);
        num4 = FileIO_ReadByte();
        num5 = num5 + (num4 << 24);
        while (num5 > 0)
        {
            num4 = FileIO_ReadByte();
            num2 = (uint8_t)(num4 & 127);
            if (num4 >= 128)
            {
                while (num2 > 0)
                {
                    num4 = FileIO_ReadByte();
                    num = num4;
                    num4 = FileIO_ReadByte();
                    num = num + (num4 << 8);
                    num4 = FileIO_ReadByte();
                    num = num + (num4 << 16);
                    num4 = FileIO_ReadByte();
                    num = num + (num4 << 24);
                    jumpTableData[num3] = num;
                    num3++;
                    jumpTableDataPos = jumpTableDataPos + 1;
                    num5--;
                    num2 = (uint8_t)(num2 - 1);
                }
            }
            else
            {
                while (num2 > 0)
                {
                    num4 = FileIO_ReadByte();
                    jumpTableData[num3] = num4;
                    num3++;
                    jumpTableDataPos = jumpTableDataPos + 1;
                    num5--;
                    num2 = (uint8_t)(num2 - 1);
                }
            }
        }
        num4 = FileIO_ReadByte();
        num5 = num4;
        num4 = FileIO_ReadByte();
        num5 = num5 + (num4 << 8);
        num = scriptNum;
        for (i = num5; i > 0; i--)
        {
            num4 = FileIO_ReadByte();
            num1 = num4;
            num4 = FileIO_ReadByte();
            num1 = num1 + (num4 << 8);
            num4 = FileIO_ReadByte();
            num1 = num1 + (num4 << 16);
            num4 = FileIO_ReadByte();
            num1 = num1 + (num4 << 24);
            objectScriptList[num].mainScript = num1;
            num4 = FileIO_ReadByte();
            num1 = num4;
            num4 = FileIO_ReadByte();
            num1 = num1 + (num4 << 8);
            num4 = FileIO_ReadByte();
            num1 = num1 + (num4 << 16);
            num4 = FileIO_ReadByte();
            num1 = num1 + (num4 << 24);
            objectScriptList[num].playerScript = num1;
            num4 = FileIO_ReadByte();
            num1 = num4;
            num4 = FileIO_ReadByte();
            num1 = num1 + (num4 << 8);
            num4 = FileIO_ReadByte();
            num1 = num1 + (num4 << 16);
            num4 = FileIO_ReadByte();
            num1 = num1 + (num4 << 24);
            objectScriptList[num].drawScript = num1;
            num4 = FileIO_ReadByte();
            num1 = num4;
            num4 = FileIO_ReadByte();
            num1 = num1 + (num4 << 8);
            num4 = FileIO_ReadByte();
            num1 = num1 + (num4 << 16);
            num4 = FileIO_ReadByte();
            num1 = num1 + (num4 << 24);
            objectScriptList[num].startupScript = num1;
            num++;
        }
        num = scriptNum;
        for (i = num5; i > 0; i--)
        {
            num4 = FileIO_ReadByte();
            num1 = num4;
            num4 = FileIO_ReadByte();
            num1 = num1 + (num4 << 8);
            num4 = FileIO_ReadByte();
            num1 = num1 + (num4 << 16);
            num4 = FileIO_ReadByte();
            num1 = num1 + (num4 << 24);
            objectScriptList[num].mainJumpTable = num1;
            num4 = FileIO_ReadByte();
            num1 = num4;
            num4 = FileIO_ReadByte();
            num1 = num1 + (num4 << 8);
            num4 = FileIO_ReadByte();
            num1 = num1 + (num4 << 16);
            num4 = FileIO_ReadByte();
            num1 = num1 + (num4 << 24);
            objectScriptList[num].playerJumpTable = num1;
            num4 = FileIO_ReadByte();
            num1 = num4;
            num4 = FileIO_ReadByte();
            num1 = num1 + (num4 << 8);
            num4 = FileIO_ReadByte();
            num1 = num1 + (num4 << 16);
            num4 = FileIO_ReadByte();
            num1 = num1 + (num4 << 24);
            objectScriptList[num].drawJumpTable = num1;
            num4 = FileIO_ReadByte();
            num1 = num4;
            num4 = FileIO_ReadByte();
            num1 = num1 + (num4 << 8);
            num4 = FileIO_ReadByte();
            num1 = num1 + (num4 << 16);
            num4 = FileIO_ReadByte();
            num1 = num1 + (num4 << 24);
            objectScriptList[num].startupJumpTable = num1;
            num++;
        }
        num4 = FileIO_ReadByte();
        num5 = num4;
        num4 = FileIO_ReadByte();
        num5 = num5 + (num4 << 8);
        num = 0;
        for (i = num5; i > 0; i--)
        {
            num4 = FileIO_ReadByte();
            num1 = num4;
            num4 = FileIO_ReadByte();
            num1 = num1 + (num4 << 8);
            num4 = FileIO_ReadByte();
            num1 = num1 + (num4 << 16);
            num4 = FileIO_ReadByte();
            num1 = num1 + (num4 << 24);
            functionScriptList[num].mainScript = num1;
            num++;
        }
        num = 0;
        for (i = num5; i > 0; i--)
        {
            num4 = FileIO_ReadByte();
            num1 = num4;
            num4 = FileIO_ReadByte();
            num1 = num1 + (num4 << 8);
            num4 = FileIO_ReadByte();
            num1 = num1 + (num4 << 16);
            num4 = FileIO_ReadByte();
            num1 = num1 + (num4 << 24);
            functionScriptList[num].mainJumpTable = num1;
            num++;
        }
        FileIO_CloseFile();
    }
}

void ObjectSystem_ObjectFloorCollision(int xOffset, int yOffset, int cPlane)
{
    int num;
    scriptEng.checkResult = 0;
    int num1 = (objectEntityList[objectLoop].xPos >> 16) + xOffset;
    int num2 = (objectEntityList[objectLoop].yPos >> 16) + yOffset;
    if (num1 > 0 && num1 < stageLayouts[0].xSize << 7 && num2 > 0 && num2 < stageLayouts[0].ySize << 7)
    {
        int num3 = num1 >> 7;
        int num4 = (num1 & 127) >> 4;
        int num5 = num2 >> 7;
        int num6 = (num2 & 127) >> 4;
        int num7 = stageLayouts[0].tileMap[num3 + (num5 << 8)] << 6;
        num7 = num7 + num4 + (num6 << 3);
        int num8 = tile128x128.tile16x16[num7];
        if (tile128x128.collisionFlag[cPlane][num7] != 2 && tile128x128.collisionFlag[cPlane][num7] != 3)
        {
            switch (tile128x128.direction[num7])
            {
                case 0:
                {
                    num = (num1 & 15) + (num8 << 4);
                    if ((num2 & 15) <= tileCollisions[cPlane].floorMask[num])
                    {
                        break;
                    }
                    num2 = tileCollisions[cPlane].floorMask[num] + (num5 << 7) + (num6 << 4);
                    scriptEng.checkResult = 1;
                    break;
                }
                case 1:
                {
                    num = 15 - (num1 & 15) + (num8 << 4);
                    if ((num2 & 15) <= tileCollisions[cPlane].floorMask[num])
                    {
                        break;
                    }
                    num2 = tileCollisions[cPlane].floorMask[num] + (num5 << 7) + (num6 << 4);
                    scriptEng.checkResult = 1;
                    break;
                }
                case 2:
                {
                    num = (num1 & 15) + (num8 << 4);
                    if ((num2 & 15) <= 15 - tileCollisions[cPlane].roofMask[num])
                    {
                        break;
                    }
                    num2 = 15 - tileCollisions[cPlane].roofMask[num] + (num5 << 7) + (num6 << 4);
                    scriptEng.checkResult = 1;
                    break;
                }
                case 3:
                {
                    num = 15 - (num1 & 15) + (num8 << 4);
                    if ((num2 & 15) <= 15 - tileCollisions[cPlane].roofMask[num])
                    {
                        break;
                    }
                    num2 = 15 - tileCollisions[cPlane].roofMask[num] + (num5 << 7) + (num6 << 4);
                    scriptEng.checkResult = 1;
                    break;
                }
            }
        }
        if (scriptEng.checkResult == 1)
        {
            objectEntityList[objectLoop].yPos = (num2 - yOffset) << 16;
        }
    }
}

void ObjectSystem_ObjectFloorGrip(int xOffset, int yOffset, int cPlane)
{
    int num;
    scriptEng.checkResult = 0;
    int num1 = (objectEntityList[objectLoop].xPos >> 16) + xOffset;
    int num2 = (objectEntityList[objectLoop].yPos >> 16) + yOffset;
    int num3 = num2;
    num2 = num2 - 16;
    for (int i = 3; i > 0; i--)
    {
        if (num1 > 0 && num1 < stageLayouts[0].xSize << 7 && num2 > 0 && num2 < stageLayouts[0].ySize << 7 && scriptEng.checkResult == 0)
        {
            int num4 = num1 >> 7;
            int num5 = (num1 & 127) >> 4;
            int num6 = num2 >> 7;
            int num7 = (num2 & 127) >> 4;
            int num8 = stageLayouts[0].tileMap[num4 + (num6 << 8)] << 6;
            num8 = num8 + num5 + (num7 << 3);
            int num9 = tile128x128.tile16x16[num8];
            if (tile128x128.collisionFlag[cPlane][num8] != 2 && tile128x128.collisionFlag[cPlane][num8] != 3)
            {
                switch (tile128x128.direction[num8])
                {
                    case 0:
                    {
                        num = (num1 & 15) + (num9 << 4);
                        if (tileCollisions[cPlane].floorMask[num] >= 64)
                        {
                            break;
                        }
                        objectEntityList[objectLoop].yPos = tileCollisions[cPlane].floorMask[num] + (num6 << 7) + (num7 << 4);
                        scriptEng.checkResult = 1;
                        break;
                    }
                    case 1:
                    {
                        num = 15 - (num1 & 15) + (num9 << 4);
                        if (tileCollisions[cPlane].floorMask[num] >= 64)
                        {
                            break;
                        }
                        objectEntityList[objectLoop].yPos = tileCollisions[cPlane].floorMask[num] + (num6 << 7) + (num7 << 4);
                        scriptEng.checkResult = 1;
                        break;
                    }
                    case 2:
                    {
                        num = (num1 & 15) + (num9 << 4);
                        if (tileCollisions[cPlane].roofMask[num] <= -64)
                        {
                            break;
                        }
                        objectEntityList[objectLoop].yPos = 15 - tileCollisions[cPlane].roofMask[num] + (num6 << 7) + (num7 << 4);
                        scriptEng.checkResult = 1;
                        break;
                    }
                    case 3:
                    {
                        num = 15 - (num1 & 15) + (num9 << 4);
                        if (tileCollisions[cPlane].roofMask[num] <= -64)
                        {
                            break;
                        }
                        objectEntityList[objectLoop].yPos = 15 - tileCollisions[cPlane].roofMask[num] + (num6 << 7) + (num7 << 4);
                        scriptEng.checkResult = 1;
                        break;
                    }
                }
            }
        }
        num2 = num2 + 16;
    }
    if (scriptEng.checkResult == 1)
    {
        if (abs(objectEntityList[objectLoop].yPos - num3) < 16)
        {
            objectEntityList[objectLoop].yPos = (objectEntityList[objectLoop].yPos - yOffset) << 16;
            return;
        }
        objectEntityList[objectLoop].yPos = (num3 - yOffset) << 16;
        scriptEng.checkResult = 0;
    }
}

void ObjectSystem_ObjectLWallCollision(int xOffset, int yOffset, int cPlane)
{
    int num;
    scriptEng.checkResult = 0;
    int num1 = (objectEntityList[objectLoop].xPos >> 16) + xOffset;
    int num2 = (objectEntityList[objectLoop].yPos >> 16) + yOffset;
    if (num1 > 0 && num1 < stageLayouts[0].xSize << 7 && num2 > 0 && num2 < stageLayouts[0].ySize << 7)
    {
        int num3 = num1 >> 7;
        int num4 = (num1 & 127) >> 4;
        int num5 = num2 >> 7;
        int num6 = (num2 & 127) >> 4;
        int num7 = stageLayouts[0].tileMap[num3 + (num5 << 8)] << 6;
        num7 = num7 + num4 + (num6 << 3);
        int num8 = tile128x128.tile16x16[num7];
        if (tile128x128.collisionFlag[cPlane][num7] != 1 && tile128x128.collisionFlag[cPlane][num7] < 3)
        {
            switch (tile128x128.direction[num7])
            {
                case 0:
                {
                    num = (num2 & 15) + (num8 << 4);
                    if ((num1 & 15) <= tileCollisions[cPlane].leftWallMask[num])
                    {
                        break;
                    }
                    num1 = tileCollisions[cPlane].leftWallMask[num] + (num3 << 7) + (num4 << 4);
                    scriptEng.checkResult = 1;
                    break;
                }
                case 1:
                {
                    num = (num2 & 15) + (num8 << 4);
                    if ((num1 & 15) <= 15 - tileCollisions[cPlane].rightWallMask[num])
                    {
                        break;
                    }
                    num1 = 15 - tileCollisions[cPlane].rightWallMask[num] + (num3 << 7) + (num4 << 4);
                    scriptEng.checkResult = 1;
                    break;
                }
                case 2:
                {
                    num = 15 - (num2 & 15) + (num8 << 4);
                    if ((num1 & 15) <= tileCollisions[cPlane].leftWallMask[num])
                    {
                        break;
                    }
                    num1 = tileCollisions[cPlane].leftWallMask[num] + (num3 << 7) + (num4 << 4);
                    scriptEng.checkResult = 1;
                    break;
                }
                case 3:
                {
                    num = 15 - (num2 & 15) + (num8 << 4);
                    if ((num1 & 15) <= 15 - tileCollisions[cPlane].rightWallMask[num])
                    {
                        break;
                    }
                    num1 = 15 - tileCollisions[cPlane].rightWallMask[num] + (num3 << 7) + (num4 << 4);
                    scriptEng.checkResult = 1;
                    break;
                }
            }
        }
        if (scriptEng.checkResult == 1)
        {
            objectEntityList[objectLoop].xPos = (num1 - xOffset) << 16;
        }
    }
}

void ObjectSystem_ObjectLWallGrip(int xOffset, int yOffset, int cPlane)
{
    int num;
    scriptEng.checkResult = 0;
    int num1 = (objectEntityList[objectLoop].xPos >> 16) + xOffset;
    int num2 = (objectEntityList[objectLoop].yPos >> 16) + yOffset;
    int num3 = num1;
    num1 = num1 - 16;
    for (int i = 3; i > 0; i--)
    {
        if (num1 > 0 && num1 < stageLayouts[0].xSize << 7 && num2 > 0 && num2 < stageLayouts[0].ySize << 7 && scriptEng.checkResult == 0)
        {
            int num4 = num1 >> 7;
            int num5 = (num1 & 127) >> 4;
            int num6 = num2 >> 7;
            int num7 = (num2 & 127) >> 4;
            int num8 = stageLayouts[0].tileMap[num4 + (num6 << 8)] << 6;
            num8 = num8 + num5 + (num7 << 3);
            int num9 = tile128x128.tile16x16[num8];
            if (tile128x128.collisionFlag[cPlane][num8] < 3)
            {
                switch (tile128x128.direction[num8])
                {
                    case 0:
                    {
                        num = (num2 & 15) + (num9 << 4);
                        if (tileCollisions[cPlane].leftWallMask[num] >= 64)
                        {
                            break;
                        }
                        objectEntityList[objectLoop].xPos = tileCollisions[cPlane].leftWallMask[num] + (num4 << 7) + (num5 << 4);
                        scriptEng.checkResult = 1;
                        break;
                    }
                    case 1:
                    {
                        num = (num2 & 15) + (num9 << 4);
                        if (tileCollisions[cPlane].rightWallMask[num] <= -64)
                        {
                            break;
                        }
                        objectEntityList[objectLoop].xPos = 15 - tileCollisions[cPlane].rightWallMask[num] + (num4 << 7) + (num5 << 4);
                        scriptEng.checkResult = 1;
                        break;
                    }
                    case 2:
                    {
                        num = 15 - (num2 & 15) + (num9 << 4);
                        if (tileCollisions[cPlane].leftWallMask[num] >= 64)
                        {
                            break;
                        }
                        objectEntityList[objectLoop].xPos = tileCollisions[cPlane].leftWallMask[num] + (num4 << 7) + (num5 << 4);
                        scriptEng.checkResult = 1;
                        break;
                    }
                    case 3:
                    {
                        num = 15 - (num2 & 15) + (num9 << 4);
                        if (tileCollisions[cPlane].rightWallMask[num] <= -64)
                        {
                            break;
                        }
                        objectEntityList[objectLoop].xPos = 15 - tileCollisions[cPlane].rightWallMask[num] + (num4 << 7) + (num5 << 4);
                        scriptEng.checkResult = 1;
                        break;
                    }
                }
            }
        }
        num1 = num1 + 16;
    }
    if (scriptEng.checkResult == 1)
    {
        if (abs(objectEntityList[objectLoop].xPos - num3) < 16)
        {
            objectEntityList[objectLoop].xPos = (objectEntityList[objectLoop].xPos - xOffset) << 16;
            return;
        }
        objectEntityList[objectLoop].xPos = (num3 - xOffset) << 16;
        scriptEng.checkResult = 0;
    }
}

void ObjectSystem_ObjectRoofCollision(int xOffset, int yOffset, int cPlane)
{
    int num;
    scriptEng.checkResult = 0;
    int num1 = (objectEntityList[objectLoop].xPos >> 16) + xOffset;
    int num2 = (objectEntityList[objectLoop].yPos >> 16) + yOffset;
    if (num1 > 0 && num1 < stageLayouts[0].xSize << 7 && num2 > 0 && num2 < stageLayouts[0].ySize << 7)
    {
        int num3 = num1 >> 7;
        int num4 = (num1 & 127) >> 4;
        int num5 = num2 >> 7;
        int num6 = (num2 & 127) >> 4;
        int num7 = stageLayouts[0].tileMap[num3 + (num5 << 8)] << 6;
        num7 = num7 + num4 + (num6 << 3);
        int num8 = tile128x128.tile16x16[num7];
        if (tile128x128.collisionFlag[cPlane][num7] != 1 && tile128x128.collisionFlag[cPlane][num7] < 3)
        {
            switch (tile128x128.direction[num7])
            {
                case 0:
                {
                    num = (num1 & 15) + (num8 << 4);
                    if ((num2 & 15) >= tileCollisions[cPlane].roofMask[num])
                    {
                        break;
                    }
                    num2 = tileCollisions[cPlane].roofMask[num] + (num5 << 7) + (num6 << 4);
                    scriptEng.checkResult = 1;
                    break;
                }
                case 1:
                {
                    num = 15 - (num1 & 15) + (num8 << 4);
                    if ((num2 & 15) >= tileCollisions[cPlane].roofMask[num])
                    {
                        break;
                    }
                    num2 = tileCollisions[cPlane].roofMask[num] + (num5 << 7) + (num6 << 4);
                    scriptEng.checkResult = 1;
                    break;
                }
                case 2:
                {
                    num = (num1 & 15) + (num8 << 4);
                    if ((num2 & 15) >= 15 - tileCollisions[cPlane].floorMask[num])
                    {
                        break;
                    }
                    num2 = 15 - tileCollisions[cPlane].floorMask[num] + (num5 << 7) + (num6 << 4);
                    scriptEng.checkResult = 1;
                    break;
                }
                case 3:
                {
                    num = 15 - (num1 & 15) + (num8 << 4);
                    if ((num2 & 15) >= 15 - tileCollisions[cPlane].floorMask[num])
                    {
                        break;
                    }
                    num2 = 15 - tileCollisions[cPlane].floorMask[num] + (num5 << 7) + (num6 << 4);
                    scriptEng.checkResult = 1;
                    break;
                }
            }
        }
        if (scriptEng.checkResult == 1)
        {
            objectEntityList[objectLoop].yPos = (num2 - yOffset) << 16;
        }
    }
}

void ObjectSystem_ObjectRoofGrip(int xOffset, int yOffset, int cPlane)
{
    int num;
    scriptEng.checkResult = 0;
    int num1 = (objectEntityList[objectLoop].xPos >> 16) + xOffset;
    int num2 = (objectEntityList[objectLoop].yPos >> 16) + yOffset;
    int num3 = num2;
    num2 = num2 + 16;
    for (int i = 3; i > 0; i--)
    {
        if (num1 > 0 && num1 < stageLayouts[0].xSize << 7 && num2 > 0 && num2 < stageLayouts[0].ySize << 7 && scriptEng.checkResult == 0)
        {
            int num4 = num1 >> 7;
            int num5 = (num1 & 127) >> 4;
            int num6 = num2 >> 7;
            int num7 = (num2 & 127) >> 4;
            int num8 = stageLayouts[0].tileMap[num4 + (num6 << 8)] << 6;
            num8 = num8 + num5 + (num7 << 3);
            int num9 = tile128x128.tile16x16[num8];
            if (tile128x128.collisionFlag[cPlane][num8] < 3)
            {
                switch (tile128x128.direction[num8])
                {
                    case 0:
                    {
                        num = (num1 & 15) + (num9 << 4);
                        if (tileCollisions[cPlane].roofMask[num] <= -64)
                        {
                            break;
                        }
                        objectEntityList[objectLoop].yPos = tileCollisions[cPlane].roofMask[num] + (num6 << 7) + (num7 << 4);
                        scriptEng.checkResult = 1;
                        break;
                    }
                    case 1:
                    {
                        num = 15 - (num1 & 15) + (num9 << 4);
                        if (tileCollisions[cPlane].roofMask[num] <= -64)
                        {
                            break;
                        }
                        objectEntityList[objectLoop].yPos = tileCollisions[cPlane].roofMask[num] + (num6 << 7) + (num7 << 4);
                        scriptEng.checkResult = 1;
                        break;
                    }
                    case 2:
                    {
                        num = (num1 & 15) + (num9 << 4);
                        if (tileCollisions[cPlane].floorMask[num] >= 64)
                        {
                            break;
                        }
                        objectEntityList[objectLoop].yPos = 15 - tileCollisions[cPlane].floorMask[num] + (num6 << 7) + (num7 << 4);
                        scriptEng.checkResult = 1;
                        break;
                    }
                    case 3:
                    {
                        num = 15 - (num1 & 15) + (num9 << 4);
                        if (tileCollisions[cPlane].floorMask[num] >= 64)
                        {
                            break;
                        }
                        objectEntityList[objectLoop].yPos = 15 - tileCollisions[cPlane].floorMask[num] + (num6 << 7) + (num7 << 4);
                        scriptEng.checkResult = 1;
                        break;
                    }
                }
            }
        }
        num2 = num2 - 16;
    }
    if (scriptEng.checkResult == 1)
    {
        if (abs(objectEntityList[objectLoop].yPos - num3) < 16)
        {
            objectEntityList[objectLoop].yPos = (objectEntityList[objectLoop].yPos - yOffset) << 16;
            return;
        }
        objectEntityList[objectLoop].yPos = (num3 - yOffset) << 16;
        scriptEng.checkResult = 0;
    }
}

void ObjectSystem_ObjectRWallCollision(int xOffset, int yOffset, int cPlane)
{
    int num;
    scriptEng.checkResult = 0;
    int num1 = (objectEntityList[objectLoop].xPos >> 16) + xOffset;
    int num2 = (objectEntityList[objectLoop].yPos >> 16) + yOffset;
    if (num1 > 0 && num1 < stageLayouts[0].xSize << 7 && num2 > 0 && num2 < stageLayouts[0].ySize << 7)
    {
        int num3 = num1 >> 7;
        int num4 = (num1 & 127) >> 4;
        int num5 = num2 >> 7;
        int num6 = (num2 & 127) >> 4;
        int num7 = stageLayouts[0].tileMap[num3 + (num5 << 8)] << 6;
        num7 = num7 + num4 + (num6 << 3);
        int num8 = tile128x128.tile16x16[num7];
        if (tile128x128.collisionFlag[cPlane][num7] != 1 && tile128x128.collisionFlag[cPlane][num7] < 3)
        {
            switch (tile128x128.direction[num7])
            {
                case 0:
                {
                    num = (num2 & 15) + (num8 << 4);
                    if ((num1 & 15) >= tileCollisions[cPlane].rightWallMask[num])
                    {
                        break;
                    }
                    num1 = tileCollisions[cPlane].rightWallMask[num] + (num3 << 7) + (num4 << 4);
                    scriptEng.checkResult = 1;
                    break;
                }
                case 1:
                {
                    num = (num2 & 15) + (num8 << 4);
                    if ((num1 & 15) >= 15 - tileCollisions[cPlane].leftWallMask[num])
                    {
                        break;
                    }
                    num1 = 15 - tileCollisions[cPlane].leftWallMask[num] + (num3 << 7) + (num4 << 4);
                    scriptEng.checkResult = 1;
                    break;
                }
                case 2:
                {
                    num = 15 - (num2 & 15) + (num8 << 4);
                    if ((num1 & 15) >= tileCollisions[cPlane].rightWallMask[num])
                    {
                        break;
                    }
                    num1 = tileCollisions[cPlane].rightWallMask[num] + (num3 << 7) + (num4 << 4);
                    scriptEng.checkResult = 1;
                    break;
                }
                case 3:
                {
                    num = 15 - (num2 & 15) + (num8 << 4);
                    if ((num1 & 15) >= 15 - tileCollisions[cPlane].leftWallMask[num])
                    {
                        break;
                    }
                    num1 = 15 - tileCollisions[cPlane].leftWallMask[num] + (num3 << 7) + (num4 << 4);
                    scriptEng.checkResult = 1;
                    break;
                }
            }
        }
        if (scriptEng.checkResult == 1)
        {
            objectEntityList[objectLoop].xPos = (num1 - xOffset) << 16;
        }
    }
}

void ObjectSystem_ObjectRWallGrip(int xOffset, int yOffset, int cPlane)
{
    int num;
    scriptEng.checkResult = 0;
    int num1 = (objectEntityList[objectLoop].xPos >> 16) + xOffset;
    int num2 = (objectEntityList[objectLoop].yPos >> 16) + yOffset;
    int num3 = num1;
    num1 = num1 + 16;
    for (int i = 3; i > 0; i--)
    {
        if (num1 > 0 && num1 < stageLayouts[0].xSize << 7 && num2 > 0 && num2 < stageLayouts[0].ySize << 7 && scriptEng.checkResult == 0)
        {
            int num4 = num1 >> 7;
            int num5 = (num1 & 127) >> 4;
            int num6 = num2 >> 7;
            int num7 = (num2 & 127) >> 4;
            int num8 = stageLayouts[0].tileMap[num4 + (num6 << 8)] << 6;
            num8 = num8 + num5 + (num7 << 3);
            int num9 = tile128x128.tile16x16[num8];
            if (tile128x128.collisionFlag[cPlane][num8] < 3)
            {
                switch (tile128x128.direction[num8])
                {
                    case 0:
                    {
                        num = (num2 & 15) + (num9 << 4);
                        if (tileCollisions[cPlane].rightWallMask[num] <= -64)
                        {
                            break;
                        }
                        objectEntityList[objectLoop].xPos = tileCollisions[cPlane].rightWallMask[num] + (num4 << 7) + (num5 << 4);
                        scriptEng.checkResult = 1;
                        break;
                    }
                    case 1:
                    {
                        num = (num2 & 15) + (num9 << 4);
                        if (tileCollisions[cPlane].leftWallMask[num] >= 64)
                        {
                            break;
                        }
                        objectEntityList[objectLoop].xPos = 15 - tileCollisions[cPlane].leftWallMask[num] + (num4 << 7) + (num5 << 4);
                        scriptEng.checkResult = 1;
                        break;
                    }
                    case 2:
                    {
                        num = 15 - (num2 & 15) + (num9 << 4);
                        if (tileCollisions[cPlane].rightWallMask[num] <= -64)
                        {
                            break;
                        }
                        objectEntityList[objectLoop].xPos = tileCollisions[cPlane].rightWallMask[num] + (num4 << 7) + (num5 << 4);
                        scriptEng.checkResult = 1;
                        break;
                    }
                    case 3:
                    {
                        num = 15 - (num2 & 15) + (num9 << 4);
                        if (tileCollisions[cPlane].leftWallMask[num] >= 64)
                        {
                            break;
                        }
                        objectEntityList[objectLoop].xPos = 15 - tileCollisions[cPlane].leftWallMask[num] + (num4 << 7) + (num5 << 4);
                        scriptEng.checkResult = 1;
                        break;
                    }
                }
            }
        }
        num1 = num1 - 16;
    }
    if (scriptEng.checkResult == 1)
    {
        if (abs(objectEntityList[objectLoop].xPos - num3) < 16)
        {
            objectEntityList[objectLoop].xPos = (objectEntityList[objectLoop].xPos - xOffset) << 16;
            return;
        }
        objectEntityList[objectLoop].xPos = (num3 - xOffset) << 16;
        scriptEng.checkResult = 0;
    }
}

void ObjectSystem_PlatformCollision(int cLeft, int cTop, int cRight, int cBottom)
{
    struct PlayerObject* playerObject = &playerList[playerNum];
    struct CollisionBox* collisionBox = &collisionBoxList[playerObject->animationFile->cbListOffset + animationFrames[animationList[playerObject->animationFile->aniListOffset + playerObject->objectPtr->animation].frameListOffset + playerObject->objectPtr->frame].collisionBox];
    collisionLeft = collisionBox->left[0];
    collisionTop = collisionBox->top[0];
    collisionRight = collisionBox->right[0];
    collisionBottom = collisionBox->bottom[0];
    object_cSensor[0].collided = 0;
    object_cSensor[1].collided = 0;
    object_cSensor[2].collided = 0;
    object_cSensor[0].xPos = playerObject->xPos + ((collisionLeft + 1) << 16);
    object_cSensor[1].xPos = playerObject->xPos;
    object_cSensor[2].xPos = playerObject->xPos + (collisionRight << 16);
    object_cSensor[0].yPos = playerObject->yPos + (collisionBottom << 16);
    object_cSensor[1].yPos = object_cSensor[0].yPos;
    object_cSensor[2].yPos = object_cSensor[0].yPos;
    scriptEng.checkResult = 0;
    for (int i = 0; i < 3; i++)
    {
        if (object_cSensor[i].xPos > cLeft && object_cSensor[i].xPos < cRight && object_cSensor[i].yPos > cTop - 2 && object_cSensor[i].yPos < cBottom && playerObject->yVelocity >= 0)
        {
            object_cSensor[i].collided = 1;
            playerObject->flailing[i] = 1;
        }
    }
    if (object_cSensor[0].collided == 1 || object_cSensor[1].collided == 1 || object_cSensor[2].collided == 1)
    {
        if (playerObject->gravity == 0 && (playerObject->collisionMode == 1 || playerObject->collisionMode == 3))
        {
            playerObject->xVelocity = 0;
            playerObject->speed = 0;
        }
        playerObject->yPos = cTop - (collisionBottom << 16);
        playerObject->gravity = 0;
        playerObject->yVelocity = 0;
        playerObject->angle = 0;
        playerObject->objectPtr->rotation = 0;
        playerObject->controlLock = 0;
        scriptEng.checkResult = 1;
    }
}

void ObjectSystem_ProcessObjects()
{
    int num;
    int num1;
    bool flag = false;
    objectDrawOrderList[0].listSize = 0;
    objectDrawOrderList[1].listSize = 0;
    objectDrawOrderList[2].listSize = 0;
    objectDrawOrderList[3].listSize = 0;
    objectDrawOrderList[4].listSize = 0;
    objectDrawOrderList[5].listSize = 0;
    objectDrawOrderList[6].listSize = 0;
    objectLoop = 0;
    while (objectLoop < 0x4a0)
    {
        switch (objectEntityList[objectLoop].priority)
        {
            case 0:
            {
                num = objectEntityList[objectLoop].xPos >> 16;
                num1 = objectEntityList[objectLoop].yPos >> 16;
                if (num <= xScrollOffset - OBJECT_BORDER_X1 || num >= xScrollOffset + OBJECT_BORDER_X2 || num1 <= yScrollOffset - 0x100 || num1 >= yScrollOffset + 0x1f0)
                {
                    flag = false;
                    break;
                }
                else
                {
                    flag = true;
                    break;
                }
            }
            case 1:
            {
                flag = true;
                break;
            }
            case 2:
            {
                flag = true;
                break;
            }
            case 3:
            {
                num = objectEntityList[objectLoop].xPos >> 16;
                if (num <= xScrollOffset - OBJECT_BORDER_X1 || num >= xScrollOffset + OBJECT_BORDER_X2)
                {
                    flag = false;
                    break;
                }
                else
                {
                    flag = true;
                    break;
                }
            }
            case 4:
            {
                num = objectEntityList[objectLoop].xPos >> 16;
                num1 = objectEntityList[objectLoop].yPos >> 16;
                if (num <= xScrollOffset - OBJECT_BORDER_X1 || num >= xScrollOffset + OBJECT_BORDER_X2 || num1 <= yScrollOffset - 0x100 || num1 >= yScrollOffset + 0x1f0)
                {
                    flag = false;
                    objectEntityList[objectLoop].type = 0;
                    break;
                }
                else
                {
                    flag = true;
                    break;
                }
            }
            case 5:
            {
                flag = false;
                break;
            }
        }
        if (flag && objectEntityList[objectLoop].type > 0)
        {
            int num2 = objectEntityList[objectLoop].type;
            playerNum = 0;
            if (scriptData[objectScriptList[num2].mainScript] > 0)
            {
                ObjectSystem_ProcessScript(objectScriptList[num2].mainScript, objectScriptList[num2].mainJumpTable, 0);
            }
            if (scriptData[objectScriptList[num2].playerScript] > 0)
            {
                while (playerNum < numActivePlayers)
                {
                    if (playerList[playerNum].objectInteraction == 1)
                    {
                        ObjectSystem_ProcessScript(objectScriptList[num2].playerScript, objectScriptList[num2].playerJumpTable, 1);
                    }
                    playerNum = playerNum + 1;
                }
            }
            num2 = objectEntityList[objectLoop].drawOrder;
            if (num2 < 7)
            {
                objectDrawOrderList[num2].entityRef[objectDrawOrderList[num2].listSize] = objectLoop;
                struct ObjectDrawList* objectDrawList = &objectDrawOrderList[num2];
                objectDrawList->listSize = objectDrawList->listSize + 1;
            }
        }
        objectLoop = objectLoop + 1;
    }
}

void ObjectSystem_ProcessPausedObjects()
{
    objectDrawOrderList[0].listSize = 0;
    objectDrawOrderList[1].listSize = 0;
    objectDrawOrderList[2].listSize = 0;
    objectDrawOrderList[3].listSize = 0;
    objectDrawOrderList[4].listSize = 0;
    objectDrawOrderList[5].listSize = 0;
    objectDrawOrderList[6].listSize = 0;
    objectLoop = 0;
    while (objectLoop < 0x4a0)
    {
        if (objectEntityList[objectLoop].priority == 2 && objectEntityList[objectLoop].type > 0)
        {
            int num = objectEntityList[objectLoop].type;
            playerNum = 0;
            if (scriptData[objectScriptList[num].mainScript] > 0)
            {
                ObjectSystem_ProcessScript(objectScriptList[num].mainScript, objectScriptList[num].mainJumpTable, 0);
            }
            if (scriptData[objectScriptList[num].playerScript] > 0)
            {
                while (playerNum < numActivePlayers)
                {
                    if (playerList[playerNum].objectInteraction == 1)
                    {
                        ObjectSystem_ProcessScript(objectScriptList[num].playerScript, objectScriptList[num].playerJumpTable, 1);
                    }
                    playerNum = playerNum + 1;
                }
            }
            num = objectEntityList[objectLoop].drawOrder;
            if (num < 7)
            {
                objectDrawOrderList[num].entityRef[objectDrawOrderList[num].listSize] = objectLoop;
                struct ObjectDrawList* objectDrawList = &objectDrawOrderList[num];
                objectDrawList->listSize = objectDrawList->listSize + 1;
            }
        }
        objectLoop = objectLoop + 1;
    }
}

void ObjectSystem_ProcessScript(int scriptCodePtr, int jumpTablePtr, int scriptSub)
{
    int i;
    int num;
    bool flag = false;
    int num1 = 0;
    int num2 = scriptCodePtr;
    jumpTableStackPos = 0;
    functionStackPos = 0;
    while (!flag)
    {
        int num3 = scriptData[scriptCodePtr];
        scriptCodePtr++;
        int num4 = 0;
        signed char num5 = scriptOpcodeSizes[num3];
        for (i = 0; i < num5; i++)
        {
            switch (scriptData[scriptCodePtr])
            {
                case 1:
                {
                    scriptCodePtr++;
                    num4++;
                    switch (scriptData[scriptCodePtr])
                    {
                        case 0:
                        {
                            num1 = objectLoop;
                            break;
                        }
                        case 1:
                        {
                            scriptCodePtr++;
                            if (scriptData[scriptCodePtr] != 1)
                            {
                                scriptCodePtr++;
                                num1 = scriptData[scriptCodePtr];
                            }
                            else
                            {
                                scriptCodePtr++;
                                num1 = scriptEng.arrayPosition[scriptData[scriptCodePtr]];
                            }
                            num4 = num4 + 2;
                            break;
                        }
                        case 2:
                        {
                            scriptCodePtr++;
                            if (scriptData[scriptCodePtr] != 1)
                            {
                                scriptCodePtr++;
                                num1 = objectLoop + scriptData[scriptCodePtr];
                            }
                            else
                            {
                                scriptCodePtr++;
                                num1 = objectLoop + scriptEng.arrayPosition[scriptData[scriptCodePtr]];
                            }
                            num4 = num4 + 2;
                            break;
                        }
                        case 3:
                        {
                            scriptCodePtr++;
                            if (scriptData[scriptCodePtr] != 1)
                            {
                                scriptCodePtr++;
                                num1 = objectLoop - scriptData[scriptCodePtr];
                            }
                            else
                            {
                                scriptCodePtr++;
                                num1 = objectLoop - scriptEng.arrayPosition[scriptData[scriptCodePtr]];
                            }
                            num4 = num4 + 2;
                            break;
                        }
                    }
                    scriptCodePtr++;
                    num4++;
                    switch (scriptData[scriptCodePtr])
                    {
                        case 0:
                        {
                            scriptEng.operands[i] = scriptEng.tempValue[0];
                            break;
                        }
                        case 1:
                        {
                            scriptEng.operands[i] = scriptEng.tempValue[1];
                            break;
                        }
                        case 2:
                        {
                            scriptEng.operands[i] = scriptEng.tempValue[2];
                            break;
                        }
                        case 3:
                        {
                            scriptEng.operands[i] = scriptEng.tempValue[3];
                            break;
                        }
                        case 4:
                        {
                            scriptEng.operands[i] = scriptEng.tempValue[4];
                            break;
                        }
                        case 5:
                        {
                            scriptEng.operands[i] = scriptEng.tempValue[5];
                            break;
                        }
                        case 6:
                        {
                            scriptEng.operands[i] = scriptEng.tempValue[6];
                            break;
                        }
                        case 7:
                        {
                            scriptEng.operands[i] = scriptEng.tempValue[7];
                            break;
                        }
                        case 8:
                        {
                            scriptEng.operands[i] = scriptEng.checkResult;
                            break;
                        }
                        case 9:
                        {
                            scriptEng.operands[i] = scriptEng.arrayPosition[0];
                            break;
                        }
                        case 10:
                        {
                            scriptEng.operands[i] = scriptEng.arrayPosition[1];
                            break;
                        }
                        case 11:
                        {
                            scriptEng.operands[i] = globalVariables[num1];
                            break;
                        }
                        case 12:
                        {
                            scriptEng.operands[i] = num1;
                            break;
                        }
                        case 13:
                        {
                            scriptEng.operands[i] = objectEntityList[num1].type;
                            break;
                        }
                        case 14:
                        {
                            scriptEng.operands[i] = objectEntityList[num1].propertyValue;
                            break;
                        }
                        case 15:
                        {
                            scriptEng.operands[i] = objectEntityList[num1].xPos;
                            break;
                        }
                        case 16:
                        {
                            scriptEng.operands[i] = objectEntityList[num1].yPos;
                            break;
                        }
                        case 17:
                        {
                            scriptEng.operands[i] = objectEntityList[num1].xPos >> 16;
                            break;
                        }
                        case 18:
                        {
                            scriptEng.operands[i] = objectEntityList[num1].yPos >> 16;
                            break;
                        }
                        case 19:
                        {
                            scriptEng.operands[i] = objectEntityList[num1].state;
                            break;
                        }
                        case 20:
                        {
                            scriptEng.operands[i] = objectEntityList[num1].rotation;
                            break;
                        }
                        case 21:
                        {
                            scriptEng.operands[i] = objectEntityList[num1].scale;
                            break;
                        }
                        case 22:
                        {
                            scriptEng.operands[i] = objectEntityList[num1].priority;
                            break;
                        }
                        case 23:
                        {
                            scriptEng.operands[i] = objectEntityList[num1].drawOrder;
                            break;
                        }
                        case 24:
                        {
                            scriptEng.operands[i] = objectEntityList[num1].direction;
                            break;
                        }
                        case 25:
                        {
                            scriptEng.operands[i] = objectEntityList[num1].inkEffect;
                            break;
                        }
                        case 26:
                        {
                            scriptEng.operands[i] = objectEntityList[num1].alpha;
                            break;
                        }
                        case 27:
                        {
                            scriptEng.operands[i] = objectEntityList[num1].frame;
                            break;
                        }
                        case 28:
                        {
                            scriptEng.operands[i] = objectEntityList[num1].animation;
                            break;
                        }
                        case 29:
                        {
                            scriptEng.operands[i] = objectEntityList[num1].prevAnimation;
                            break;
                        }
                        case 30:
                        {
                            scriptEng.operands[i] = objectEntityList[num1].animationSpeed;
                            break;
                        }
                        case 31:
                        {
                            scriptEng.operands[i] = objectEntityList[num1].animationTimer;
                            break;
                        }
                        case 32:
                        {
                            scriptEng.operands[i] = objectEntityList[num1].value[0];
                            break;
                        }
                        case 33:
                        {
                            scriptEng.operands[i] = objectEntityList[num1].value[1];
                            break;
                        }
                        case 34:
                        {
                            scriptEng.operands[i] = objectEntityList[num1].value[2];
                            break;
                        }
                        case 35:
                        {
                            scriptEng.operands[i] = objectEntityList[num1].value[3];
                            break;
                        }
                        case 36:
                        {
                            scriptEng.operands[i] = objectEntityList[num1].value[4];
                            break;
                        }
                        case 37:
                        {
                            scriptEng.operands[i] = objectEntityList[num1].value[5];
                            break;
                        }
                        case 38:
                        {
                            scriptEng.operands[i] = objectEntityList[num1].value[6];
                            break;
                        }
                        case 39:
                        {
                            scriptEng.operands[i] = objectEntityList[num1].value[7];
                            break;
                        }
                        case 40:
                        {
                            scriptEng.sRegister = objectEntityList[objectLoop].xPos >> 16;
                            if (scriptEng.sRegister <= xScrollOffset - OBJECT_BORDER_X1 || scriptEng.sRegister >= xScrollOffset + OBJECT_BORDER_X2)
                            {
                                scriptEng.operands[i] = 1;
                                break;
                            }
                            else
                            {
                                scriptEng.sRegister = objectEntityList[objectLoop].yPos >> 16;
                                if (scriptEng.sRegister <= yScrollOffset - 0x100 || scriptEng.sRegister >= yScrollOffset + 0x1f0)
                                {
                                    scriptEng.operands[i] = 1;
                                    break;
                                }
                                else
                                {
                                    scriptEng.operands[i] = 0;
                                    break;
                                }
                            }
                        }
                        case 41:
                        {
                            scriptEng.operands[i] = playerList[playerNum].objectPtr->state;
                            break;
                        }
                        case 42:
                        {
                            scriptEng.operands[i] = playerList[playerNum].controlMode;
                            break;
                        }
                        case 43:
                        {
                            scriptEng.operands[i] = playerList[playerNum].controlLock;
                            break;
                        }
                        case 44:
                        {
                            scriptEng.operands[i] = playerList[playerNum].collisionMode;
                            break;
                        }
                        case 45:
                        {
                            scriptEng.operands[i] = playerList[playerNum].collisionPlane;
                            break;
                        }
                        case 46:
                        {
                            scriptEng.operands[i] = playerList[playerNum].xPos;
                            break;
                        }
                        case 47:
                        {
                            scriptEng.operands[i] = playerList[playerNum].yPos;
                            break;
                        }
                        case 48:
                        {
                            scriptEng.operands[i] = playerList[playerNum].xPos >> 16;
                            break;
                        }
                        case 49:
                        {
                            scriptEng.operands[i] = playerList[playerNum].yPos >> 16;
                            break;
                        }
                        case 50:
                        {
                            scriptEng.operands[i] = playerList[playerNum].screenXPos;
                            break;
                        }
                        case 51:
                        {
                            scriptEng.operands[i] = playerList[playerNum].screenYPos;
                            break;
                        }
                        case 52:
                        {
                            scriptEng.operands[i] = playerList[playerNum].speed;
                            break;
                        }
                        case 53:
                        {
                            scriptEng.operands[i] = playerList[playerNum].xVelocity;
                            break;
                        }
                        case 54:
                        {
                            scriptEng.operands[i] = playerList[playerNum].yVelocity;
                            break;
                        }
                        case 55:
                        {
                            scriptEng.operands[i] = playerList[playerNum].gravity;
                            break;
                        }
                        case 56:
                        {
                            scriptEng.operands[i] = playerList[playerNum].angle;
                            break;
                        }
                        case 57:
                        {
                            scriptEng.operands[i] = playerList[playerNum].skidding;
                            break;
                        }
                        case 58:
                        {
                            scriptEng.operands[i] = playerList[playerNum].pushing;
                            break;
                        }
                        case 59:
                        {
                            scriptEng.operands[i] = playerList[playerNum].trackScroll;
                            break;
                        }
                        case 60:
                        {
                            scriptEng.operands[i] = playerList[playerNum].up;
                            break;
                        }
                        case 61:
                        {
                            scriptEng.operands[i] = playerList[playerNum].down;
                            break;
                        }
                        case 62:
                        {
                            scriptEng.operands[i] = playerList[playerNum].left;
                            break;
                        }
                        case 63:
                        {
                            scriptEng.operands[i] = playerList[playerNum].right;
                            break;
                        }
                        case 64:
                        {
                            scriptEng.operands[i] = playerList[playerNum].jumpPress;
                            break;
                        }
                        case 65:
                        {
                            scriptEng.operands[i] = playerList[playerNum].jumpHold;
                            break;
                        }
                        case 66:
                        {
                            scriptEng.operands[i] = playerList[playerNum].followPlayer1;
                            break;
                        }
                        case 67:
                        {
                            scriptEng.operands[i] = playerList[playerNum].lookPos;
                            break;
                        }
                        case 68:
                        {
                            scriptEng.operands[i] = playerList[playerNum].water;
                            break;
                        }
                        case 69:
                        {
                            scriptEng.operands[i] = playerList[playerNum].movementStats.topSpeed;
                            break;
                        }
                        case 70:
                        {
                            scriptEng.operands[i] = playerList[playerNum].movementStats.acceleration;
                            break;
                        }
                        case 71:
                        {
                            scriptEng.operands[i] = playerList[playerNum].movementStats.deceleration;
                            break;
                        }
                        case 72:
                        {
                            scriptEng.operands[i] = playerList[playerNum].movementStats.airAcceleration;
                            break;
                        }
                        case 73:
                        {
                            scriptEng.operands[i] = playerList[playerNum].movementStats.airDeceleration;
                            break;
                        }
                        case 74:
                        {
                            scriptEng.operands[i] = playerList[playerNum].movementStats.gravity;
                            break;
                        }
                        case 75:
                        {
                            scriptEng.operands[i] = playerList[playerNum].movementStats.jumpStrength;
                            break;
                        }
                        case 76:
                        {
                            scriptEng.operands[i] = playerList[playerNum].movementStats.jumpCap;
                            break;
                        }
                        case 77:
                        {
                            scriptEng.operands[i] = playerList[playerNum].movementStats.rollingAcceleration;
                            break;
                        }
                        case 78:
                        {
                            scriptEng.operands[i] = playerList[playerNum].movementStats.rollingDeceleration;
                            break;
                        }
                        case 79:
                        {
                            scriptEng.operands[i] = playerList[playerNum].objectNum;
                            break;
                        }
                        case 80:
                        {
                            scriptEng.operands[i] = collisionBoxList[playerList[playerNum].animationFile->cbListOffset + animationFrames[animationList[playerList[playerNum].animationFile->aniListOffset + playerList[playerNum].objectPtr->animation].frameListOffset + playerList[playerNum].objectPtr->frame].collisionBox].left[0];
                            break;
                        }
                        case 81:
                        {
                            scriptEng.operands[i] = collisionBoxList[playerList[playerNum].animationFile->cbListOffset + animationFrames[animationList[playerList[playerNum].animationFile->aniListOffset + playerList[playerNum].objectPtr->animation].frameListOffset + playerList[playerNum].objectPtr->frame].collisionBox].top[0];
                            break;
                        }
                        case 82:
                        {
                            scriptEng.operands[i] = collisionBoxList[playerList[playerNum].animationFile->cbListOffset + animationFrames[animationList[playerList[playerNum].animationFile->aniListOffset + playerList[playerNum].objectPtr->animation].frameListOffset + playerList[playerNum].objectPtr->frame].collisionBox].right[0];
                            break;
                        }
                        case 83:
                        {
                            scriptEng.operands[i] = collisionBoxList[playerList[playerNum].animationFile->cbListOffset + animationFrames[animationList[playerList[playerNum].animationFile->aniListOffset + playerList[playerNum].objectPtr->animation].frameListOffset + playerList[playerNum].objectPtr->frame].collisionBox].bottom[0];
                            break;
                        }
                        case 84:
                        {
                            scriptEng.operands[i] = playerList[playerNum].flailing[num1];
                            break;
                        }
                        case 85:
                        {
                            scriptEng.operands[i] = playerList[playerNum].timer;
                            break;
                        }
                        case 86:
                        {
                            scriptEng.operands[i] = playerList[playerNum].tileCollisions;
                            break;
                        }
                        case 87:
                        {
                            scriptEng.operands[i] = playerList[playerNum].objectInteraction;
                            break;
                        }
                        case 88:
                        {
                            scriptEng.operands[i] = playerList[playerNum].visible;
                            break;
                        }
                        case 89:
                        {
                            scriptEng.operands[i] = playerList[playerNum].objectPtr->rotation;
                            break;
                        }
                        case 90:
                        {
                            scriptEng.operands[i] = playerList[playerNum].objectPtr->scale;
                            break;
                        }
                        case 91:
                        {
                            scriptEng.operands[i] = playerList[playerNum].objectPtr->priority;
                            break;
                        }
                        case 92:
                        {
                            scriptEng.operands[i] = playerList[playerNum].objectPtr->drawOrder;
                            break;
                        }
                        case 93:
                        {
                            scriptEng.operands[i] = playerList[playerNum].objectPtr->direction;
                            break;
                        }
                        case 94:
                        {
                            scriptEng.operands[i] = playerList[playerNum].objectPtr->inkEffect;
                            break;
                        }
                        case 95:
                        {
                            scriptEng.operands[i] = playerList[playerNum].objectPtr->alpha;
                            break;
                        }
                        case 96:
                        {
                            scriptEng.operands[i] = playerList[playerNum].objectPtr->frame;
                            break;
                        }
                        case 97:
                        {
                            scriptEng.operands[i] = playerList[playerNum].objectPtr->animation;
                            break;
                        }
                        case 98:
                        {
                            scriptEng.operands[i] = playerList[playerNum].objectPtr->prevAnimation;
                            break;
                        }
                        case 99:
                        {
                            scriptEng.operands[i] = playerList[playerNum].objectPtr->animationSpeed;
                            break;
                        }
                        case 100:
                        {
                            scriptEng.operands[i] = playerList[playerNum].objectPtr->animationTimer;
                            break;
                        }
                        case 101:
                        {
                            scriptEng.operands[i] = playerList[playerNum].objectPtr->value[0];
                            break;
                        }
                        case 102:
                        {
                            scriptEng.operands[i] = playerList[playerNum].objectPtr->value[1];
                            break;
                        }
                        case 103:
                        {
                            scriptEng.operands[i] = playerList[playerNum].objectPtr->value[2];
                            break;
                        }
                        case 104:
                        {
                            scriptEng.operands[i] = playerList[playerNum].objectPtr->value[3];
                            break;
                        }
                        case 105:
                        {
                            scriptEng.operands[i] = playerList[playerNum].objectPtr->value[4];
                            break;
                        }
                        case 106:
                        {
                            scriptEng.operands[i] = playerList[playerNum].objectPtr->value[5];
                            break;
                        }
                        case 107:
                        {
                            scriptEng.operands[i] = playerList[playerNum].objectPtr->value[6];
                            break;
                        }
                        case 108:
                        {
                            scriptEng.operands[i] = playerList[playerNum].objectPtr->value[7];
                            break;
                        }
                        case 109:
                        {
                            scriptEng.operands[i] = playerList[playerNum].value[0];
                            break;
                        }
                        case 110:
                        {
                            scriptEng.operands[i] = playerList[playerNum].value[1];
                            break;
                        }
                        case 111:
                        {
                            scriptEng.operands[i] = playerList[playerNum].value[2];
                            break;
                        }
                        case 112:
                        {
                            scriptEng.operands[i] = playerList[playerNum].value[3];
                            break;
                        }
                        case 113:
                        {
                            scriptEng.operands[i] = playerList[playerNum].value[4];
                            break;
                        }
                        case 114:
                        {
                            scriptEng.operands[i] = playerList[playerNum].value[5];
                            break;
                        }
                        case 115:
                        {
                            scriptEng.operands[i] = playerList[playerNum].value[6];
                            break;
                        }
                        case 116:
                        {
                            scriptEng.operands[i] = playerList[playerNum].value[7];
                            break;
                        }
                        case 117:
                        {
                            scriptEng.sRegister = playerList[playerNum].objectPtr->xPos >> 16;
                            if (scriptEng.sRegister <= xScrollOffset - OBJECT_BORDER_X1 || scriptEng.sRegister >= xScrollOffset + OBJECT_BORDER_X2)
                            {
                                scriptEng.operands[i] = 1;
                                break;
                            }
                            else
                            {
                                scriptEng.sRegister = playerList[playerNum].objectPtr->yPos >> 16;
                                if (scriptEng.sRegister <= yScrollOffset - 0x100 || scriptEng.sRegister >= yScrollOffset + 0x1f0)
                                {
                                    scriptEng.operands[i] = 1;
                                    break;
                                }
                                else
                                {
                                    scriptEng.operands[i] = 0;
                                    break;
                                }
                            }
                        }
                        case 118:
                        {
                            scriptEng.operands[i] = stageMode;
                            break;
                        }
                        case 119:
                        {
                            scriptEng.operands[i] = activeStageList;
                            break;
                        }
                        case 120:
                        {
                            scriptEng.operands[i] = stageListPosition;
                            break;
                        }
                        case 121:
                        {
                            scriptEng.operands[i] = timeEnabled;
                            break;
                        }
                        case 122:
                        {
                            scriptEng.operands[i] = milliSeconds;
                            break;
                        }
                        case 123:
                        {
                            scriptEng.operands[i] = seconds;
                            break;
                        }
                        case 124:
                        {
                            scriptEng.operands[i] = minutes;
                            break;
                        }
                        case 125:
                        {
                            scriptEng.operands[i] = actNumber;
                            break;
                        }
                        case 126:
                        {
                            scriptEng.operands[i] = pauseEnabled;
                            break;
                        }
                        case 127:
                        {
                            switch (activeStageList)
                            {
                                case 0:
                                {
                                    scriptEng.operands[i] = noPresentationStages;
                                    break;
                                }
                                case 1:
                                {
                                    scriptEng.operands[i] = noZoneStages;
                                    break;
                                }
                                case 2:
                                {
                                    scriptEng.operands[i] = noBonusStages;
                                    break;
                                }
                                case 3:
                                {
                                    scriptEng.operands[i] = noSpecialStages;
                                    break;
                                }
                            }
                            break;
                        }
                        case 128:
                        {
                            scriptEng.operands[i] = newXBoundary1;
                            break;
                        }
                        case 129:
                        {
                            scriptEng.operands[i] = newXBoundary2;
                            break;
                        }
                        case 130:
                        {
                            scriptEng.operands[i] = newYBoundary1;
                            break;
                        }
                        case 131:
                        {
                            scriptEng.operands[i] = newYBoundary2;
                            break;
                        }
                        case 132:
                        {
                            scriptEng.operands[i] = xBoundary1;
                            break;
                        }
                        case 133:
                        {
                            scriptEng.operands[i] = xBoundary2;
                            break;
                        }
                        case 134:
                        {
                            scriptEng.operands[i] = yBoundary1;
                            break;
                        }
                        case 135:
                        {
                            scriptEng.operands[i] = yBoundary2;
                            break;
                        }
                        case 136:
                        {
                            scriptEng.operands[i] = bgDeformationData0[num1];
                            break;
                        }
                        case 137:
                        {
                            scriptEng.operands[i] = bgDeformationData1[num1];
                            break;
                        }
                        case 138:
                        {
                            scriptEng.operands[i] = bgDeformationData2[num1];
                            break;
                        }
                        case 139:
                        {
                            scriptEng.operands[i] = bgDeformationData3[num1];
                            break;
                        }
                        case 140:
                        {
                            scriptEng.operands[i] = waterLevel;
                            break;
                        }
                        case 141:
                        {
                            scriptEng.operands[i] = activeTileLayers[num1];
                            break;
                        }
                        case 142:
                        {
                            scriptEng.operands[i] = tLayerMidPoint;
                            break;
                        }
                        case 143:
                        {
                            scriptEng.operands[i] = playerMenuNum;
                            break;
                        }
                        case 144:
                        {
                            scriptEng.operands[i] = playerNum;
                            break;
                        }
                        case 145:
                        {
                            scriptEng.operands[i] = cameraEnabled;
                            break;
                        }
                        case 146:
                        {
                            scriptEng.operands[i] = cameraTarget;
                            break;
                        }
                        case 147:
                        {
                            scriptEng.operands[i] = cameraStyle;
                            break;
                        }
                        case 148:
                        {
                            scriptEng.operands[i] = objectDrawOrderList[num1].listSize;
                            break;
                        }
                        case 149:
                        {
                            scriptEng.operands[i] = SCREEN_CENTER;
                            break;
                        }
                        case 150:
                        {
                            scriptEng.operands[i] = 120;
                            break;
                        }
                        case 151:
                        {
                            scriptEng.operands[i] = SCREEN_XSIZE;
                            break;
                        }
                        case 152:
                        {
                            scriptEng.operands[i] = 240;
                            break;
                        }
                        case 153:
                        {
                            scriptEng.operands[i] = xScrollOffset;
                            break;
                        }
                        case 154:
                        {
                            scriptEng.operands[i] = yScrollOffset;
                            break;
                        }
                        case 155:
                        {
                            scriptEng.operands[i] = screenShakeX;
                            break;
                        }
                        case 156:
                        {
                            scriptEng.operands[i] = screenShakeY;
                            break;
                        }
                        case 157:
                        {
                            scriptEng.operands[i] = cameraAdjustY;
                            break;
                        }
                        case 158:
                        {
                            scriptEng.operands[i] = gKeyDown.touchDown[num1];
                            break;
                        }
                        case 159:
                        {
                            scriptEng.operands[i] = gKeyDown.touchX[num1];
                            break;
                        }
                        case 160:
                        {
                            scriptEng.operands[i] = gKeyDown.touchY[num1];
                            break;
                        }
                        case 161:
                        {
                            scriptEng.operands[i] = musicVolume;
                            break;
                        }
                        case 162:
                        {
                            scriptEng.operands[i] = currentMusicTrack;
                            break;
                        }
                        case 163:
                        {
                            scriptEng.operands[i] = gKeyDown.up;
                            break;
                        }
                        case 164:
                        {
                            scriptEng.operands[i] = gKeyDown.down;
                            break;
                        }
                        case 165:
                        {
                            scriptEng.operands[i] = gKeyDown.left;
                            break;
                        }
                        case 166:
                        {
                            scriptEng.operands[i] = gKeyDown.right;
                            break;
                        }
                        case 167:
                        {
                            scriptEng.operands[i] = gKeyDown.buttonA;
                            break;
                        }
                        case 168:
                        {
                            scriptEng.operands[i] = gKeyDown.buttonB;
                            break;
                        }
                        case 169:
                        {
                            scriptEng.operands[i] = gKeyDown.buttonC;
                            break;
                        }
                        case 170:
                        {
                            scriptEng.operands[i] = gKeyDown.start;
                            break;
                        }
                        case 171:
                        {
                            scriptEng.operands[i] = gKeyPress.up;
                            break;
                        }
                        case 172:
                        {
                            scriptEng.operands[i] = gKeyPress.down;
                            break;
                        }
                        case 173:
                        {
                            scriptEng.operands[i] = gKeyPress.left;
                            break;
                        }
                        case 174:
                        {
                            scriptEng.operands[i] = gKeyPress.right;
                            break;
                        }
                        case 175:
                        {
                            scriptEng.operands[i] = gKeyPress.buttonA;
                            break;
                        }
                        case 176:
                        {
                            scriptEng.operands[i] = gKeyPress.buttonB;
                            break;
                        }
                        case 177:
                        {
                            scriptEng.operands[i] = gKeyPress.buttonC;
                            break;
                        }
                        case 178:
                        {
                            scriptEng.operands[i] = gKeyPress.start;
                            break;
                        }
                        case 179:
                        {
                            scriptEng.operands[i] = gameMenu[0].selection1;
                            break;
                        }
                        case 180:
                        {
                            scriptEng.operands[i] = gameMenu[1].selection1;
                            break;
                        }
                        case 181:
                        {
                            scriptEng.operands[i] = stageLayouts[num1].xSize;
                            break;
                        }
                        case 182:
                        {
                            scriptEng.operands[i] = stageLayouts[num1].ySize;
                            break;
                        }
                        case 183:
                        {
                            scriptEng.operands[i] = stageLayouts[num1].type;
                            break;
                        }
                        case 184:
                        {
                            scriptEng.operands[i] = stageLayouts[num1].angle;
                            break;
                        }
                        case 185:
                        {
                            scriptEng.operands[i] = stageLayouts[num1].xPos;
                            break;
                        }
                        case 186:
                        {
                            scriptEng.operands[i] = stageLayouts[num1].yPos;
                            break;
                        }
                        case 187:
                        {
                            scriptEng.operands[i] = stageLayouts[num1].zPos;
                            break;
                        }
                        case 188:
                        {
                            scriptEng.operands[i] = stageLayouts[num1].parallaxFactor;
                            break;
                        }
                        case 189:
                        {
                            scriptEng.operands[i] = stageLayouts[num1].scrollSpeed;
                            break;
                        }
                        case 190:
                        {
                            scriptEng.operands[i] = stageLayouts[num1].scrollPosition;
                            break;
                        }
                        case 191:
                        {
                            scriptEng.operands[i] = stageLayouts[num1].deformationPos;
                            break;
                        }
                        case 192:
                        {
                            scriptEng.operands[i] = stageLayouts[num1].deformationPosW;
                            break;
                        }
                        case 193:
                        {
                            scriptEng.operands[i] = hParallax.parallaxFactor[num1];
                            break;
                        }
                        case 194:
                        {
                            scriptEng.operands[i] = hParallax.scrollSpeed[num1];
                            break;
                        }
                        case 195:
                        {
                            scriptEng.operands[i] = hParallax.scrollPosition[num1];
                            break;
                        }
                        case 196:
                        {
                            scriptEng.operands[i] = vParallax.parallaxFactor[num1];
                            break;
                        }
                        case 197:
                        {
                            scriptEng.operands[i] = vParallax.scrollSpeed[num1];
                            break;
                        }
                        case 198:
                        {
                            scriptEng.operands[i] = vParallax.scrollPosition[num1];
                            break;
                        }
                        case 199:
                        {
                            scriptEng.operands[i] = numVertices;
                            break;
                        }
                        case 200:
                        {
                            scriptEng.operands[i] = numFaces;
                            break;
                        }
                        case 201:
                        {
                            scriptEng.operands[i] = vertexBuffer[num1].x;
                            break;
                        }
                        case 202:
                        {
                            scriptEng.operands[i] = vertexBuffer[num1].y;
                            break;
                        }
                        case 203:
                        {
                            scriptEng.operands[i] = vertexBuffer[num1].z;
                            break;
                        }
                        case 204:
                        {
                            scriptEng.operands[i] = vertexBuffer[num1].u;
                            break;
                        }
                        case 205:
                        {
                            scriptEng.operands[i] = vertexBuffer[num1].v;
                            break;
                        }
                        case 206:
                        {
                            scriptEng.operands[i] = indexBuffer[num1].a;
                            break;
                        }
                        case 207:
                        {
                            scriptEng.operands[i] = indexBuffer[num1].b;
                            break;
                        }
                        case 208:
                        {
                            scriptEng.operands[i] = indexBuffer[num1].c;
                            break;
                        }
                        case 209:
                        {
                            scriptEng.operands[i] = indexBuffer[num1].d;
                            break;
                        }
                        case 210:
                        {
                            scriptEng.operands[i] = indexBuffer[num1].flag;
                            break;
                        }
                        case 211:
                        {
                            scriptEng.operands[i] = indexBuffer[num1].color;
                            break;
                        }
                        case 212:
                        {
                            scriptEng.operands[i] = projectionX;
                            break;
                        }
                        case 213:
                        {
                            scriptEng.operands[i] = projectionY;
                            break;
                        }
                        case 214:
                        {
                            scriptEng.operands[i] = gameMode;
                            break;
                        }
                        case 215:
                        {
                            scriptEng.operands[i] = debugMode;
                            break;
                        }
                        case 216:
                        {
                            scriptEng.operands[i] = gameMessage;
                            break;
                        }
                        case 217:
                        {
                            scriptEng.operands[i] = saveRAM[num1];
                            break;
                        }
                        case 218:
                        {
                            scriptEng.operands[i] = gameLanguage;
                            break;
                        }
                        case 219:
                        {
                            scriptEng.operands[i] = objectScriptList[objectEntityList[objectLoop].type].surfaceNum;
                            break;
                        }
                        case 220:
                        {
                            scriptEng.operands[i] = gameOnlineActive;
                            break;
                        }
                        case 221:
                        {
                            scriptEng.operands[i] = frameSkipTimer;
                            break;
                        }
                        case 222:
                        {
                            scriptEng.operands[i] = frameSkipSetting;
                            break;
                        }
                        case 223:
                        {
                            scriptEng.operands[i] = gameSFXVolume;
                            break;
                        }
                        case 224:
                        {
                            scriptEng.operands[i] = gameBGMVolume;
                            break;
                        }
                        case 225:
                        {
                            scriptEng.operands[i] = gamePlatformID;
                            break;
                        }
                        case 226:
                        {
                            scriptEng.operands[i] = gameTrialMode;
                            break;
                        }
                        case 227:
                        {
                            scriptEng.operands[i] = gKeyPress.start;
                            break;
                        }
                        case 228:
                        {
                            scriptEng.operands[i] = gameHapticsEnabled;
                            break;
                        }
                    }
                    scriptCodePtr++;
                    num4++;
                    break;
                }
                case 2:
                {
                    scriptCodePtr++;
                    scriptEng.operands[i] = scriptData[scriptCodePtr];
                    scriptCodePtr++;
                    num4 = num4 + 2;
                    break;
                }
                case 3:
                {
                    scriptCodePtr++;
                    num4++;
                    num = 0;
                    num1 = 0;
                    scriptEng.sRegister = scriptData[scriptCodePtr];
                    scriptText[scriptEng.sRegister] = '\0';
                    while (num < scriptEng.sRegister)
                    {
                        switch (num1)
                        {
                            case 0:
                            {
                                scriptCodePtr++;
                                num4++;
                                scriptText[num] = (char)(scriptData[scriptCodePtr] >> 24);
                                num1++;
                                break;
                            }
                            case 1:
                            {
                                scriptText[num] = (char)((scriptData[scriptCodePtr] & 0xffffff) >> 16);
                                num1++;
                                break;
                            }
                            case 2:
                            {
                                scriptText[num] = (char)((scriptData[scriptCodePtr] & 0xffff) >> 8);
                                num1++;
                                break;
                            }
                            case 3:
                            {
                                scriptText[num] = (char)(scriptData[scriptCodePtr] & 0xff);
                                num1 = 0;
                                break;
                            }
                        }
                        num++;
                    }
                    if (num1 != 0)
                    {
                        scriptCodePtr++;
                        num4++;
                        break;
                    }
                    else
                    {
                        scriptCodePtr = scriptCodePtr + 2;
                        num4 = num4 + 2;
                        break;
                    }
                }
            }
        }
        switch (num3)
        {
            case 0:
            {
                flag = true;
                break;
            }
            case 1:
            {
                scriptEng.operands[0] = scriptEng.operands[1];
                break;
            }
            case 2:
            {
                scriptEng.operands[0] = scriptEng.operands[0] + scriptEng.operands[1];
                break;
            }
            case 3:
            {
                scriptEng.operands[0] = scriptEng.operands[0] - scriptEng.operands[1];
                break;
            }
            case 4:
            {
                scriptEng.operands[0] = scriptEng.operands[0] + 1;
                break;
            }
            case 5:
            {
                scriptEng.operands[0] = scriptEng.operands[0] - 1;
                break;
            }
            case 6:
            {
                scriptEng.operands[0] = scriptEng.operands[0] * scriptEng.operands[1];
                break;
            }
            case 7:
            {
                scriptEng.operands[0] = scriptEng.operands[0] / scriptEng.operands[1];
                break;
            }
            case 8:
            {
                scriptEng.operands[0] = scriptEng.operands[0] >> (scriptEng.operands[1] & 31);
                break;
            }
            case 9:
            {
                scriptEng.operands[0] = scriptEng.operands[0] << (scriptEng.operands[1] & 31);
                break;
            }
            case 10:
            {
                scriptEng.operands[0] = scriptEng.operands[0] & scriptEng.operands[1];
                break;
            }
            case 11:
            {
                scriptEng.operands[0] = scriptEng.operands[0] | scriptEng.operands[1];
                break;
            }
            case 12:
            {
                scriptEng.operands[0] = scriptEng.operands[0] ^ scriptEng.operands[1];
                break;
            }
            case 13:
            {
                scriptEng.operands[0] = scriptEng.operands[0] % scriptEng.operands[1];
                break;
            }
            case 14:
            {
                scriptEng.operands[0] = -scriptEng.operands[0];
                break;
            }
            case 15:
            {
                if (scriptEng.operands[0] != scriptEng.operands[1])
                {
                    scriptEng.checkResult = 0;
                }
                else
                {
                    scriptEng.checkResult = 1;
                }
                num5 = 0;
                break;
            }
            case 16:
            {
                if (scriptEng.operands[0] <= scriptEng.operands[1])
                {
                    scriptEng.checkResult = 0;
                }
                else
                {
                    scriptEng.checkResult = 1;
                }
                num5 = 0;
                break;
            }
            case 17:
            {
                if (scriptEng.operands[0] >= scriptEng.operands[1])
                {
                    scriptEng.checkResult = 0;
                }
                else
                {
                    scriptEng.checkResult = 1;
                }
                num5 = 0;
                break;
            }
            case 18:
            {
                if (scriptEng.operands[0] == scriptEng.operands[1])
                {
                    scriptEng.checkResult = 0;
                }
                else
                {
                    scriptEng.checkResult = 1;
                }
                num5 = 0;
                break;
            }
            case 19:
            {
                if (scriptEng.operands[1] != scriptEng.operands[2])
                {
                    scriptCodePtr = num2;
                    scriptCodePtr = scriptCodePtr + jumpTableData[jumpTablePtr + scriptEng.operands[0]];
                    jumpTableStackPos = jumpTableStackPos + 1;
                    jumpTableStack[jumpTableStackPos] = scriptEng.operands[0];
                }
                else
                {
                    jumpTableStackPos = jumpTableStackPos + 1;
                    jumpTableStack[jumpTableStackPos] = scriptEng.operands[0];
                }
                num5 = 0;
                break;
            }
            case 20:
            {
                if (scriptEng.operands[1] <= scriptEng.operands[2])
                {
                    scriptCodePtr = num2;
                    scriptCodePtr = scriptCodePtr + jumpTableData[jumpTablePtr + scriptEng.operands[0]];
                    jumpTableStackPos = jumpTableStackPos + 1;
                    jumpTableStack[jumpTableStackPos] = scriptEng.operands[0];
                }
                else
                {
                    jumpTableStackPos = jumpTableStackPos + 1;
                    jumpTableStack[jumpTableStackPos] = scriptEng.operands[0];
                }
                num5 = 0;
                break;
            }
            case 21:
            {
                if (scriptEng.operands[1] < scriptEng.operands[2])
                {
                    scriptCodePtr = num2;
                    scriptCodePtr = scriptCodePtr + jumpTableData[jumpTablePtr + scriptEng.operands[0]];
                    jumpTableStackPos = jumpTableStackPos + 1;
                    jumpTableStack[jumpTableStackPos] = scriptEng.operands[0];
                }
                else
                {
                    jumpTableStackPos = jumpTableStackPos + 1;
                    jumpTableStack[jumpTableStackPos] = scriptEng.operands[0];
                }
                num5 = 0;
                break;
            }
            case 22:
            {
                if (scriptEng.operands[1] >= scriptEng.operands[2])
                {
                    scriptCodePtr = num2;
                    scriptCodePtr = scriptCodePtr + jumpTableData[jumpTablePtr + scriptEng.operands[0]];
                    jumpTableStackPos = jumpTableStackPos + 1;
                    jumpTableStack[jumpTableStackPos] = scriptEng.operands[0];
                }
                else
                {
                    jumpTableStackPos = jumpTableStackPos + 1;
                    jumpTableStack[jumpTableStackPos] = scriptEng.operands[0];
                }
                num5 = 0;
                break;
            }
            case 23:
            {
                if (scriptEng.operands[1] > scriptEng.operands[2])
                {
                    scriptCodePtr = num2;
                    scriptCodePtr = scriptCodePtr + jumpTableData[jumpTablePtr + scriptEng.operands[0]];
                    jumpTableStackPos = jumpTableStackPos + 1;
                    jumpTableStack[jumpTableStackPos] = scriptEng.operands[0];
                }
                else
                {
                    jumpTableStackPos = jumpTableStackPos + 1;
                    jumpTableStack[jumpTableStackPos] = scriptEng.operands[0];
                }
                num5 = 0;
                break;
            }
            case 24:
            {
                if (scriptEng.operands[1] == scriptEng.operands[2])
                {
                    scriptCodePtr = num2;
                    scriptCodePtr = scriptCodePtr + jumpTableData[jumpTablePtr + scriptEng.operands[0]];
                    jumpTableStackPos = jumpTableStackPos + 1;
                    jumpTableStack[jumpTableStackPos] = scriptEng.operands[0];
                }
                else
                {
                    jumpTableStackPos = jumpTableStackPos + 1;
                    jumpTableStack[jumpTableStackPos] = scriptEng.operands[0];
                }
                num5 = 0;
                break;
            }
            case 25:
            {
                num5 = 0;
                scriptCodePtr = num2;
                scriptCodePtr = scriptCodePtr + jumpTableData[jumpTablePtr + jumpTableStack[jumpTableStackPos] + 1];
                jumpTableStackPos = jumpTableStackPos - 1;
                break;
            }
            case 26:
            {
                num5 = 0;
                jumpTableStackPos = jumpTableStackPos - 1;
                break;
            }
            case 27:
            {
                if (scriptEng.operands[1] != scriptEng.operands[2])
                {
                    scriptCodePtr = num2;
                    scriptCodePtr = scriptCodePtr + jumpTableData[jumpTablePtr + scriptEng.operands[0] + 1];
                }
                else
                {
                    jumpTableStackPos = jumpTableStackPos + 1;
                    jumpTableStack[jumpTableStackPos] = scriptEng.operands[0];
                }
                num5 = 0;
                break;
            }
            case 28:
            {
                if (scriptEng.operands[1] <= scriptEng.operands[2])
                {
                    scriptCodePtr = num2;
                    scriptCodePtr = scriptCodePtr + jumpTableData[jumpTablePtr + scriptEng.operands[0] + 1];
                }
                else
                {
                    jumpTableStackPos = jumpTableStackPos + 1;
                    jumpTableStack[jumpTableStackPos] = scriptEng.operands[0];
                }
                num5 = 0;
                break;
            }
            case 29:
            {
                if (scriptEng.operands[1] < scriptEng.operands[2])
                {
                    scriptCodePtr = num2;
                    scriptCodePtr = scriptCodePtr + jumpTableData[jumpTablePtr + scriptEng.operands[0] + 1];
                }
                else
                {
                    jumpTableStackPos = jumpTableStackPos + 1;
                    jumpTableStack[jumpTableStackPos] = scriptEng.operands[0];
                }
                num5 = 0;
                break;
            }
            case 30:
            {
                if (scriptEng.operands[1] >= scriptEng.operands[2])
                {
                    scriptCodePtr = num2;
                    scriptCodePtr = scriptCodePtr + jumpTableData[jumpTablePtr + scriptEng.operands[0] + 1];
                }
                else
                {
                    jumpTableStackPos = jumpTableStackPos + 1;
                    jumpTableStack[jumpTableStackPos] = scriptEng.operands[0];
                }
                num5 = 0;
                break;
            }
            case 31:
            {
                if (scriptEng.operands[1] > scriptEng.operands[2])
                {
                    scriptCodePtr = num2;
                    scriptCodePtr = scriptCodePtr + jumpTableData[jumpTablePtr + scriptEng.operands[0] + 1];
                }
                else
                {
                    jumpTableStackPos = jumpTableStackPos + 1;
                    jumpTableStack[jumpTableStackPos] = scriptEng.operands[0];
                }
                num5 = 0;
                break;
            }
            case 32:
            {
                if (scriptEng.operands[1] == scriptEng.operands[2])
                {
                    scriptCodePtr = num2;
                    scriptCodePtr = scriptCodePtr + jumpTableData[jumpTablePtr + scriptEng.operands[0] + 1];
                }
                else
                {
                    jumpTableStackPos = jumpTableStackPos + 1;
                    jumpTableStack[jumpTableStackPos] = scriptEng.operands[0];
                }
                num5 = 0;
                break;
            }
            case 33:
            {
                num5 = 0;
                scriptCodePtr = num2;
                scriptCodePtr = scriptCodePtr + jumpTableData[jumpTablePtr + jumpTableStack[jumpTableStackPos]];
                jumpTableStackPos = jumpTableStackPos - 1;
                break;
            }
            case 34:
            {
                jumpTableStackPos = jumpTableStackPos + 1;
                jumpTableStack[jumpTableStackPos] = scriptEng.operands[0];
                if (scriptEng.operands[1] < jumpTableData[jumpTablePtr + scriptEng.operands[0]] || scriptEng.operands[1] > jumpTableData[jumpTablePtr + scriptEng.operands[0] + 1])
                {
                    scriptCodePtr = num2;
                    scriptCodePtr = scriptCodePtr + jumpTableData[jumpTablePtr + scriptEng.operands[0] + 2];
                }
                else
                {
                    scriptCodePtr = num2;
                    scriptCodePtr = scriptCodePtr + jumpTableData[jumpTablePtr + scriptEng.operands[0] + 4 + (scriptEng.operands[1] - jumpTableData[jumpTablePtr + scriptEng.operands[0]])];
                }
                num5 = 0;
                break;
            }
            case 35:
            {
                num5 = 0;
                scriptCodePtr = num2;
                scriptCodePtr = scriptCodePtr + jumpTableData[jumpTablePtr + jumpTableStack[jumpTableStackPos] + 3];
                jumpTableStackPos = jumpTableStackPos - 1;
                break;
            }
            case 36:
            {
                num5 = 0;
                jumpTableStackPos = jumpTableStackPos - 1;
                break;
            }
            case 37:
            {
                scriptEng.operands[0] = rand() % scriptEng.operands[1];
                break;
            }
            case 38:
            {
                scriptEng.sRegister = scriptEng.operands[1];
                if (scriptEng.sRegister < 0)
                {
                    scriptEng.sRegister = 0x200 - scriptEng.sRegister;
                }
                scriptEng.sRegister = scriptEng.sRegister & 0x1ff;
                scriptEng.operands[0] = SinValue512[scriptEng.sRegister];
                break;
            }
            case 39:
            {
                scriptEng.sRegister = scriptEng.operands[1];
                if (scriptEng.sRegister < 0)
                {
                    scriptEng.sRegister = 0x200 - scriptEng.sRegister;
                }
                scriptEng.sRegister = scriptEng.sRegister & 0x1ff;
                scriptEng.operands[0] = CosValue512[scriptEng.sRegister];
                break;
            }
            case 40:
            {
                scriptEng.sRegister = scriptEng.operands[1];
                if (scriptEng.sRegister < 0)
                {
                    scriptEng.sRegister = 0x100 - scriptEng.sRegister;
                }
                scriptEng.sRegister = scriptEng.sRegister & 0xff;
                scriptEng.operands[0] = SinValue256[scriptEng.sRegister];
                break;
            }
            case 41:
            {
                scriptEng.sRegister = scriptEng.operands[1];
                if (scriptEng.sRegister < 0)
                {
                    scriptEng.sRegister = 0x100 - scriptEng.sRegister;
                }
                scriptEng.sRegister = scriptEng.sRegister & 0xff;
                scriptEng.operands[0] = CosValue256[scriptEng.sRegister];
                break;
            }
            case 42:
            {
                scriptEng.sRegister = scriptEng.operands[1];
                if (scriptEng.sRegister < 0)
                {
                    scriptEng.sRegister = 0x200 - scriptEng.sRegister;
                }
                scriptEng.sRegister = scriptEng.sRegister & 0x1ff;
                scriptEng.operands[0] = (SinValue512[scriptEng.sRegister] >> (scriptEng.operands[2] & 31)) + scriptEng.operands[3] - scriptEng.operands[4];
                break;
            }
            case 43:
            {
                scriptEng.sRegister = scriptEng.operands[1];
                if (scriptEng.sRegister < 0)
                {
                    scriptEng.sRegister = 0x200 - scriptEng.sRegister;
                }
                scriptEng.sRegister = scriptEng.sRegister & 0x1ff;
                scriptEng.operands[0] = (CosValue512[scriptEng.sRegister] >> (scriptEng.operands[2] & 31)) + scriptEng.operands[3] - scriptEng.operands[4];
                break;
            }
            case 44:
            {
                scriptEng.operands[0] = GlobalAppDefinitions_ArcTanLookup(scriptEng.operands[1], scriptEng.operands[2]);
                break;
            }
            case 45:
            {
                scriptEng.operands[0] = (scriptEng.operands[1] * scriptEng.operands[3] + scriptEng.operands[2] * (0x100 - scriptEng.operands[3])) >> 8;
                break;
            }
            case 46:
            {
                scriptEng.operands[0] = (scriptEng.operands[2] * scriptEng.operands[6] >> 8) + (scriptEng.operands[3] * (0x100 - scriptEng.operands[6]) >> 8);
                scriptEng.operands[1] = (scriptEng.operands[4] * scriptEng.operands[6] >> 8) + (scriptEng.operands[5] * (0x100 - scriptEng.operands[6]) >> 8);
                break;
            }
            case 47:
            {
                num5 = 0;
                objectScriptList[objectEntityList[objectLoop].type].surfaceNum = GraphicsSystem_AddGraphicsFile(scriptText);
                break;
            }
            case 48:
            {
                num5 = 0;
                GraphicsSystem_RemoveGraphicsFile(scriptText, -1);
                break;
            }
            case 49:
            {
                num5 = 0;
                GraphicsSystem_DrawSprite((objectEntityList[objectLoop].xPos >> 16) - xScrollOffset + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xPivot, (objectEntityList[objectLoop].yPos >> 16) - yScrollOffset + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].yPivot, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].top, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                break;
            }
            case 50:
            {
                num5 = 0;
                GraphicsSystem_DrawSprite((scriptEng.operands[1] >> 16) - xScrollOffset + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xPivot, (scriptEng.operands[2] >> 16) - yScrollOffset + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].yPivot, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].top, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                break;
            }
            case 51:
            {
                num5 = 0;
                GraphicsSystem_DrawSprite(scriptEng.operands[1] + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xPivot, scriptEng.operands[2] + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].yPivot, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].top, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                break;
            }
            case 52:
            {
                num5 = 0;
                GraphicsSystem_DrawTintRectangle(scriptEng.operands[0], scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]);
                break;
            }
            case 53:
            {
                num5 = 0;
                scriptEng.operands[7] = 10;
                if (scriptEng.operands[6] != 0)
                {
                    while (scriptEng.operands[4] > 0)
                    {
                        scriptEng.sRegister = (scriptEng.operands[3] - scriptEng.operands[3] / scriptEng.operands[7] * scriptEng.operands[7]) / (scriptEng.operands[7] / 10);
                        scriptEng.sRegister = scriptEng.sRegister + scriptEng.operands[0];
                        GraphicsSystem_DrawSprite(scriptEng.operands[1] + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.sRegister].xPivot, scriptEng.operands[2] + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.sRegister].yPivot, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.sRegister].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.sRegister].ySize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.sRegister].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.sRegister].top, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                        scriptEng.operands[1] = scriptEng.operands[1] - scriptEng.operands[5];
                        scriptEng.operands[7] = scriptEng.operands[7] * 10;
                        scriptEng.operands[4] = scriptEng.operands[4] - 1;
                    }
                    break;
                }
                else
                {
                    if (scriptEng.operands[3] != 0)
                    {
                        scriptEng.operands[8] = scriptEng.operands[3] * 10;
                    }
                    else
                    {
                        scriptEng.operands[8] = 10;
                    }
                    while (scriptEng.operands[4] > 0)
                    {
                        if (scriptEng.operands[8] >= scriptEng.operands[7])
                        {
                            scriptEng.sRegister = (scriptEng.operands[3] - scriptEng.operands[3] / scriptEng.operands[7] * scriptEng.operands[7]) / (scriptEng.operands[7] / 10);
                            scriptEng.sRegister = scriptEng.sRegister + scriptEng.operands[0];
                            GraphicsSystem_DrawSprite(scriptEng.operands[1] + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.sRegister].xPivot, scriptEng.operands[2] + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.sRegister].yPivot, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.sRegister].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.sRegister].ySize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.sRegister].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.sRegister].top, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                        }
                        scriptEng.operands[1] = scriptEng.operands[1] - scriptEng.operands[5];
                        scriptEng.operands[7] = scriptEng.operands[7] * 10;
                        scriptEng.operands[4] = scriptEng.operands[4] - 1;
                    }
                    break;
                }
            }
            case 54:
            {
                num5 = 0;
                switch (scriptEng.operands[3])
                {
                    case 1:
                    {
                        scriptEng.sRegister = 0;
                        if (scriptEng.operands[4] == 1 && titleCardText[scriptEng.sRegister] != 0)
                        {
                            scriptEng.operands[7] = titleCardText[scriptEng.sRegister];
                            if (scriptEng.operands[7] == 32)
                            {
                                scriptEng.operands[7] = 0;
                            }
                            if (scriptEng.operands[7] == 45)
                            {
                                scriptEng.operands[7] = 0;
                            }
                            if (scriptEng.operands[7] > 47 && scriptEng.operands[7] < 58)
                            {
                                scriptEng.operands[7] = scriptEng.operands[7] - 22;
                            }
                            if (scriptEng.operands[7] > 57 && scriptEng.operands[7] < 102)
                            {
                                scriptEng.operands[7] = scriptEng.operands[7] - 65;
                            }
                            if (scriptEng.operands[7] <= -1)
                            {
                                scriptEng.operands[1] = scriptEng.operands[1] + scriptEng.operands[5] + scriptEng.operands[6];
                            }
                            else
                            {
                                scriptEng.operands[7] = scriptEng.operands[7] + scriptEng.operands[0];
                                GraphicsSystem_DrawSprite(scriptEng.operands[1] + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[7]].xPivot, scriptEng.operands[2] + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[7]].yPivot, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[7]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[7]].ySize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[7]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[7]].top, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                                scriptEng.operands[1] = scriptEng.operands[1] + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[7]].xSize + scriptEng.operands[6];
                            }
                            scriptEng.operands[0] = scriptEng.operands[0] + 26;
                            scriptEng.sRegister = scriptEng.sRegister + 1;
                        }
                        while (titleCardText[scriptEng.sRegister] != 0)
                        {
                            if (titleCardText[scriptEng.sRegister] != '-')
                            {
                                scriptEng.operands[7] = titleCardText[scriptEng.sRegister];
                                if (scriptEng.operands[7] == 32)
                                {
                                    scriptEng.operands[7] = 0;
                                }
                                if (scriptEng.operands[7] == 45)
                                {
                                    scriptEng.operands[7] = 0;
                                }
                                if (scriptEng.operands[7] > 47 && scriptEng.operands[7] < 58)
                                {
                                    scriptEng.operands[7] = scriptEng.operands[7] - 22;
                                }
                                if (scriptEng.operands[7] > 57 && scriptEng.operands[7] < 102)
                                {
                                    scriptEng.operands[7] = scriptEng.operands[7] - 65;
                                }
                                if (scriptEng.operands[7] <= -1)
                                {
                                    scriptEng.operands[1] = scriptEng.operands[1] + scriptEng.operands[5] + scriptEng.operands[6];
                                }
                                else
                                {
                                    scriptEng.operands[7] = scriptEng.operands[7] + scriptEng.operands[0];
                                    GraphicsSystem_DrawSprite(scriptEng.operands[1] + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[7]].xPivot, scriptEng.operands[2] + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[7]].yPivot, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[7]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[7]].ySize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[7]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[7]].top, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                                    scriptEng.operands[1] = scriptEng.operands[1] + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[7]].xSize + scriptEng.operands[6];
                                }
                                scriptEng.sRegister = scriptEng.sRegister + 1;
                            }
                        }
                        break;
                    }
                    case 2:
                    {
                        scriptEng.sRegister = titleCardWord2;
                        if (scriptEng.operands[4] == 1 && titleCardText[scriptEng.sRegister] != 0)
                        {
                            scriptEng.operands[7] = titleCardText[scriptEng.sRegister];
                            if (scriptEng.operands[7] == 32)
                            {
                                scriptEng.operands[7] = 0;
                            }
                            if (scriptEng.operands[7] == 45)
                            {
                                scriptEng.operands[7] = 0;
                            }
                            if (scriptEng.operands[7] > 47 && scriptEng.operands[7] < 58)
                            {
                                scriptEng.operands[7] = scriptEng.operands[7] - 22;
                            }
                            if (scriptEng.operands[7] > 57 && scriptEng.operands[7] < 102)
                            {
                                scriptEng.operands[7] = scriptEng.operands[7] - 65;
                            }
                            if (scriptEng.operands[7] <= -1)
                            {
                                scriptEng.operands[1] = scriptEng.operands[1] + scriptEng.operands[5] + scriptEng.operands[6];
                            }
                            else
                            {
                                scriptEng.operands[7] = scriptEng.operands[7] + scriptEng.operands[0];
                                GraphicsSystem_DrawSprite(scriptEng.operands[1] + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[7]].xPivot, scriptEng.operands[2] + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[7]].yPivot, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[7]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[7]].ySize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[7]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[7]].top, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                                scriptEng.operands[1] = scriptEng.operands[1] + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[7]].xSize + scriptEng.operands[6];
                            }
                            scriptEng.operands[0] = scriptEng.operands[0] + 26;
                            scriptEng.sRegister = scriptEng.sRegister + 1;
                        }
                        while (titleCardText[scriptEng.sRegister] != 0)
                        {
                            scriptEng.operands[7] = titleCardText[scriptEng.sRegister];
                            if (scriptEng.operands[7] == 32)
                            {
                                scriptEng.operands[7] = 0;
                            }
                            if (scriptEng.operands[7] == 45)
                            {
                                scriptEng.operands[7] = 0;
                            }
                            if (scriptEng.operands[7] > 47 && scriptEng.operands[7] < 58)
                            {
                                scriptEng.operands[7] = scriptEng.operands[7] - 22;
                            }
                            if (scriptEng.operands[7] > 57 && scriptEng.operands[7] < 102)
                            {
                                scriptEng.operands[7] = scriptEng.operands[7] - 65;
                            }
                            if (scriptEng.operands[7] <= -1)
                            {
                                scriptEng.operands[1] = scriptEng.operands[1] + scriptEng.operands[5] + scriptEng.operands[6];
                            }
                            else
                            {
                                scriptEng.operands[7] = scriptEng.operands[7] + scriptEng.operands[0];
                                GraphicsSystem_DrawSprite(scriptEng.operands[1] + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[7]].xPivot, scriptEng.operands[2] + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[7]].yPivot, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[7]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[7]].ySize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[7]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[7]].top, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                                scriptEng.operands[1] = scriptEng.operands[1] + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[7]].xSize + scriptEng.operands[6];
                            }
                            scriptEng.sRegister = scriptEng.sRegister + 1;
                        }
                        break;
                    }
                }
                break;
            }
            case 55:
            {
                num5 = 0;
                textMenuSurfaceNo = objectScriptList[objectEntityList[objectLoop].type].surfaceNum;
                TextSystem_DrawTextMenu(&gameMenu[scriptEng.operands[0]], scriptEng.operands[1], scriptEng.operands[2]);
                break;
            }
            case 56:
            {
                num5 = 0;
                if (scriptSub != 3 || scriptFramesNo >= 0x1000)
                {
                    break;
                }
                scriptFrames[scriptFramesNo].xPivot = scriptEng.operands[0];
                scriptFrames[scriptFramesNo].yPivot = scriptEng.operands[1];
                scriptFrames[scriptFramesNo].xSize = scriptEng.operands[2];
                scriptFrames[scriptFramesNo].ySize = scriptEng.operands[3];
                scriptFrames[scriptFramesNo].left = scriptEng.operands[4];
                scriptFrames[scriptFramesNo].top = scriptEng.operands[5];
                scriptFramesNo = scriptFramesNo + 1;
                break;
            }
            case 57:
            {
                num5 = 0;
                break;
            }
            case 58:
            {
                num5 = 0;
                GraphicsSystem_LoadPalette(scriptText, scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3], scriptEng.operands[4]);
                break;
            }
            case 59:
            {
                num5 = 0;
                GraphicsSystem_RotatePalette((uint8_t)scriptEng.operands[0], (uint8_t)scriptEng.operands[1], (uint8_t)scriptEng.operands[2]);
                break;
            }
            case 60:
            {
                num5 = 0;
                GraphicsSystem_SetFade((uint8_t)scriptEng.operands[0], (uint8_t)scriptEng.operands[1], (uint8_t)scriptEng.operands[2], (uint16_t)scriptEng.operands[3]);
                break;
            }
            case 61:
            {
                num5 = 0;
                GraphicsSystem_SetActivePalette((uint8_t)scriptEng.operands[0], scriptEng.operands[1], scriptEng.operands[2]);
                break;
            }
            case 62:
            {
                GraphicsSystem_SetLimitedFade((uint8_t)scriptEng.operands[0], (uint8_t)scriptEng.operands[1], (uint8_t)scriptEng.operands[2], (uint8_t)scriptEng.operands[3], (uint16_t)scriptEng.operands[4], scriptEng.operands[5], scriptEng.operands[6]);
                break;
            }
            case 63:
            {
                num5 = 0;
                GraphicsSystem_CopyPalette((uint8_t)scriptEng.operands[0], (uint8_t)scriptEng.operands[1]);
                break;
            }
            case 64:
            {
                num5 = 0;
                GraphicsSystem_ClearScreen((uint8_t)scriptEng.operands[0]);
                break;
            }
            case 65:
            {
                num5 = 0;
                switch (scriptEng.operands[1])
                {
                    case 0:
                    {
                        GraphicsSystem_DrawScaledSprite(objectEntityList[objectLoop].direction, (scriptEng.operands[2] >> 16) - xScrollOffset, (scriptEng.operands[3] >> 16) - yScrollOffset, -scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xPivot, -scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].yPivot, objectEntityList[objectLoop].scale, objectEntityList[objectLoop].scale, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].top, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                        break;
                    }
                    case 1:
                    {
                        GraphicsSystem_DrawRotatedSprite(objectEntityList[objectLoop].direction, (scriptEng.operands[2] >> 16) - xScrollOffset, (scriptEng.operands[3] >> 16) - yScrollOffset, -scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xPivot, -scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].yPivot, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].top, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize, objectEntityList[objectLoop].rotation, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                        break;
                    }
                    case 2:
                    {
                        GraphicsSystem_DrawRotoZoomSprite(objectEntityList[objectLoop].direction, (scriptEng.operands[2] >> 16) - xScrollOffset, (scriptEng.operands[3] >> 16) - yScrollOffset, -scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xPivot, -scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].yPivot, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].top, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize, objectEntityList[objectLoop].rotation, objectEntityList[objectLoop].scale, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                        break;
                    }
                    case 3:
                    {
                        switch (objectEntityList[objectLoop].inkEffect)
                        {
                            case 0:
                            {
                                GraphicsSystem_DrawSprite((scriptEng.operands[2] >> 16) - xScrollOffset + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xPivot, (scriptEng.operands[3] >> 16) - yScrollOffset + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].yPivot, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].top, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                                break;
                            }
                            case 1:
                            {
                                GraphicsSystem_DrawBlendedSprite((scriptEng.operands[2] >> 16) - xScrollOffset + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xPivot, (scriptEng.operands[3] >> 16) - yScrollOffset + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].yPivot, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].top, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                                break;
                            }
                            case 2:
                            {
                                GraphicsSystem_DrawAlphaBlendedSprite((scriptEng.operands[2] >> 16) - xScrollOffset + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xPivot, (scriptEng.operands[3] >> 16) - yScrollOffset + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].yPivot, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].top, (int)objectEntityList[objectLoop].alpha, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                                break;
                            }
                            case 3:
                            {
                                GraphicsSystem_DrawAdditiveBlendedSprite((scriptEng.operands[2] >> 16) - xScrollOffset + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xPivot, (scriptEng.operands[3] >> 16) - yScrollOffset + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].yPivot, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].top, (int)objectEntityList[objectLoop].alpha, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                                break;
                            }
                            case 4:
                            {
                                GraphicsSystem_DrawSubtractiveBlendedSprite((scriptEng.operands[2] >> 16) - xScrollOffset + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xPivot, (scriptEng.operands[3] >> 16) - yScrollOffset + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].yPivot, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].top, (int)objectEntityList[objectLoop].alpha, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                                break;
                            }
                        }
                        break;
                    }
                    case 4:
                    {
                        if (objectEntityList[objectLoop].inkEffect == 2)
                        {
                            GraphicsSystem_DrawScaledTintMask(objectEntityList[objectLoop].direction, (scriptEng.operands[2] >> 16) - xScrollOffset, (scriptEng.operands[3] >> 16) - yScrollOffset, -scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xPivot, -scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].yPivot, objectEntityList[objectLoop].scale, objectEntityList[objectLoop].scale, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].top, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                        }
                        else
                        {
                            GraphicsSystem_DrawScaledSprite(objectEntityList[objectLoop].direction, (scriptEng.operands[2] >> 16) - xScrollOffset, (scriptEng.operands[3] >> 16) - yScrollOffset, -scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xPivot, -scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].yPivot, objectEntityList[objectLoop].scale, objectEntityList[objectLoop].scale, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].top, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                        }
                        break;
                    }
                    case 5:
                    {
                        switch (objectEntityList[objectLoop].direction)
                        {
                            case 0:
                            {
                                GraphicsSystem_DrawSpriteFlipped((scriptEng.operands[2] >> 16) - xScrollOffset + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xPivot, (scriptEng.operands[3] >> 16) - yScrollOffset + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].yPivot, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].top, (int)objectEntityList[objectLoop].direction, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                                break;
                            }
                            case 1:
                            {
                                GraphicsSystem_DrawSpriteFlipped((scriptEng.operands[2] >> 16) - xScrollOffset - scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize - scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xPivot, (scriptEng.operands[3] >> 16) - yScrollOffset + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].yPivot, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].top, (int)objectEntityList[objectLoop].direction, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                                break;
                            }
                            case 2:
                            {
                                GraphicsSystem_DrawSpriteFlipped((scriptEng.operands[2] >> 16) - xScrollOffset + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xPivot, (scriptEng.operands[3] >> 16) - yScrollOffset - scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize - scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].yPivot, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].top, (int)objectEntityList[objectLoop].direction, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                                break;
                            }
                            case 3:
                            {
                                GraphicsSystem_DrawSpriteFlipped((scriptEng.operands[2] >> 16) - xScrollOffset - scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize - scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xPivot, (scriptEng.operands[3] >> 16) - yScrollOffset - scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize - scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].yPivot, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].top, (int)objectEntityList[objectLoop].direction, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                                break;
                            }
                        }
                        break;
                    }
                }
                break;
            }
            case 66:
            {
                num5 = 0;
                switch (scriptEng.operands[1])
                {
                    case 0:
                    {
                        GraphicsSystem_DrawScaledSprite(objectEntityList[objectLoop].direction, scriptEng.operands[2], scriptEng.operands[3], -scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xPivot, -scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].yPivot, objectEntityList[objectLoop].scale, objectEntityList[objectLoop].scale, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].top, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                        break;
                    }
                    case 1:
                    {
                        GraphicsSystem_DrawRotatedSprite(objectEntityList[objectLoop].direction, scriptEng.operands[2], scriptEng.operands[3], -scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xPivot, -scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].yPivot, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].top, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize, objectEntityList[objectLoop].rotation, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                        break;
                    }
                    case 2:
                    {
                        GraphicsSystem_DrawRotoZoomSprite(objectEntityList[objectLoop].direction, scriptEng.operands[2], scriptEng.operands[3], -scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xPivot, -scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].yPivot, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].top, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize, objectEntityList[objectLoop].rotation, objectEntityList[objectLoop].scale, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                        break;
                    }
                    case 3:
                    {
                        switch (objectEntityList[objectLoop].inkEffect)
                        {
                            case 0:
                            {
                                GraphicsSystem_DrawSprite(scriptEng.operands[2] + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xPivot, scriptEng.operands[3] + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].yPivot, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].top, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                                break;
                            }
                            case 1:
                            {
                                GraphicsSystem_DrawBlendedSprite(scriptEng.operands[2] + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xPivot, scriptEng.operands[3] + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].yPivot, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].top, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                                break;
                            }
                            case 2:
                            {
                                GraphicsSystem_DrawAlphaBlendedSprite(scriptEng.operands[2] + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xPivot, scriptEng.operands[3] + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].yPivot, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].top, (int)objectEntityList[objectLoop].alpha, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                                break;
                            }
                            case 3:
                            {
                                GraphicsSystem_DrawAdditiveBlendedSprite(scriptEng.operands[2] + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xPivot, scriptEng.operands[3] + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].yPivot, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].top, (int)objectEntityList[objectLoop].alpha, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                                break;
                            }
                            case 4:
                            {
                                GraphicsSystem_DrawSubtractiveBlendedSprite(scriptEng.operands[2] + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xPivot, scriptEng.operands[3] + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].yPivot, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].top, (int)objectEntityList[objectLoop].alpha, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                                break;
                            }
                        }
                        break;
                    }
                    case 4:
                    {
                        if (objectEntityList[objectLoop].inkEffect == 2)
                        {
                            GraphicsSystem_DrawScaledTintMask(objectEntityList[objectLoop].direction, scriptEng.operands[2], scriptEng.operands[3], -scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xPivot, -scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].yPivot, objectEntityList[objectLoop].scale, objectEntityList[objectLoop].scale, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].top, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                        }
                        else
                        {
                            GraphicsSystem_DrawScaledSprite(objectEntityList[objectLoop].direction, scriptEng.operands[2], scriptEng.operands[3], -scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xPivot, -scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].yPivot, objectEntityList[objectLoop].scale, objectEntityList[objectLoop].scale, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].top, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                        }
                        break;
                    }
                    case 5:
                    {
                        switch (objectEntityList[objectLoop].direction)
                        {
                            case 0:
                            {
                                GraphicsSystem_DrawSpriteFlipped(scriptEng.operands[2] + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xPivot, scriptEng.operands[3] + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].yPivot, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].top, (int)objectEntityList[objectLoop].direction, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                                break;
                            }
                            case 1:
                            {
                                GraphicsSystem_DrawSpriteFlipped(scriptEng.operands[2] - scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize - scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xPivot, scriptEng.operands[3] + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].yPivot, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].top, (int)objectEntityList[objectLoop].direction, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                                break;
                            }
                            case 2:
                            {
                                GraphicsSystem_DrawSpriteFlipped(scriptEng.operands[2] + scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xPivot, scriptEng.operands[3] - scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize - scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].yPivot, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].top, (int)objectEntityList[objectLoop].direction, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                                break;
                            }
                            case 3:
                            {
                                GraphicsSystem_DrawSpriteFlipped(scriptEng.operands[2] - scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize - scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xPivot, scriptEng.operands[3] - scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize - scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].yPivot, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].xSize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].ySize, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].left, scriptFrames[objectScriptList[objectEntityList[objectLoop].type].frameListOffset + scriptEng.operands[0]].top, (int)objectEntityList[objectLoop].direction, (int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                                break;
                            }
                        }
                        break;
                    }
                }
                break;
            }
            case 67:
            {
                num5 = 0;
                objectScriptList[objectEntityList[objectLoop].type].animationFile = AnimationSystem_AddAnimationFile(scriptText);
                break;
            }
            case 68:
            {
                num5 = 0;
                TextSystem_SetupTextMenu(&gameMenu[scriptEng.operands[0]], scriptEng.operands[1]);
                gameMenu[scriptEng.operands[0]].numSelections = (uint8_t)scriptEng.operands[2];
                gameMenu[scriptEng.operands[0]].alignment = (uint8_t)scriptEng.operands[3];
                break;
            }
            case 69:
            {
                num5 = 0;
                gameMenu[scriptEng.operands[0]].entryHighlight[gameMenu[scriptEng.operands[0]].numRows] = (uint8_t)scriptEng.operands[2];
                TextSystem_AddTextMenuEntry(&gameMenu[scriptEng.operands[0]], scriptText);
                break;
            }
            case 70:
            {
                num5 = 0;
                TextSystem_EditTextMenuEntry(&gameMenu[scriptEng.operands[0]], scriptText, scriptEng.operands[2]);
                gameMenu[scriptEng.operands[0]].entryHighlight[scriptEng.operands[2]] = (uint8_t)scriptEng.operands[3];
                break;
            }
            case 71:
            {
                num5 = 0;
                stageMode = 0;
                break;
            }
            case 72:
            {
                num5 = 0;
                GraphicsSystem_DrawRectangle(scriptEng.operands[0], scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3], scriptEng.operands[4], scriptEng.operands[5], scriptEng.operands[6], scriptEng.operands[7]);
                break;
            }
            case 73:
            {
                num5 = 0;
                objectEntityList[scriptEng.operands[0]].type = (uint8_t)scriptEng.operands[1];
                objectEntityList[scriptEng.operands[0]].propertyValue = (uint8_t)scriptEng.operands[2];
                objectEntityList[scriptEng.operands[0]].xPos = scriptEng.operands[3];
                objectEntityList[scriptEng.operands[0]].yPos = scriptEng.operands[4];
                objectEntityList[scriptEng.operands[0]].direction = 0;
                objectEntityList[scriptEng.operands[0]].frame = 0;
                objectEntityList[scriptEng.operands[0]].priority = 0;
                objectEntityList[scriptEng.operands[0]].rotation = 0;
                objectEntityList[scriptEng.operands[0]].state = 0;
                objectEntityList[scriptEng.operands[0]].drawOrder = 3;
                objectEntityList[scriptEng.operands[0]].scale = 0x200;
                objectEntityList[scriptEng.operands[0]].inkEffect = 0;
                objectEntityList[scriptEng.operands[0]].value[0] = 0;
                objectEntityList[scriptEng.operands[0]].value[1] = 0;
                objectEntityList[scriptEng.operands[0]].value[2] = 0;
                objectEntityList[scriptEng.operands[0]].value[3] = 0;
                objectEntityList[scriptEng.operands[0]].value[4] = 0;
                objectEntityList[scriptEng.operands[0]].value[5] = 0;
                objectEntityList[scriptEng.operands[0]].value[6] = 0;
                objectEntityList[scriptEng.operands[0]].value[7] = 0;
                break;
            }
            case 74:
            {
                num5 = 0;
                switch (scriptEng.operands[0])
                {
                    case 0:
                    {
                        scriptEng.operands[5] = objectEntityList[objectLoop].xPos >> 16;
                        scriptEng.operands[6] = objectEntityList[objectLoop].yPos >> 16;
                        ObjectSystem_BasicCollision(scriptEng.operands[1] + scriptEng.operands[5], scriptEng.operands[2] + scriptEng.operands[6], scriptEng.operands[3] + scriptEng.operands[5], scriptEng.operands[4] + scriptEng.operands[6]);
                        break;
                    }
                    case 1:
                    case 2:
                    {
                        ObjectSystem_BoxCollision((scriptEng.operands[1] << 16) + objectEntityList[objectLoop].xPos, (scriptEng.operands[2] << 16) + objectEntityList[objectLoop].yPos, (scriptEng.operands[3] << 16) + objectEntityList[objectLoop].xPos, (scriptEng.operands[4] << 16) + objectEntityList[objectLoop].yPos);
                        break;
                    }
                    case 3:
                    {
                        ObjectSystem_PlatformCollision((scriptEng.operands[1] << 16) + objectEntityList[objectLoop].xPos, (scriptEng.operands[2] << 16) + objectEntityList[objectLoop].yPos, (scriptEng.operands[3] << 16) + objectEntityList[objectLoop].xPos, (scriptEng.operands[4] << 16) + objectEntityList[objectLoop].yPos);
                        break;
                    }
                }
                break;
            }
            case 75:
            {
                num5 = 0;
                if (objectEntityList[scriptEng.arrayPosition[2]].type > 0)
                {
                    scriptEng.arrayPosition[2] = scriptEng.arrayPosition[2] + 1;
                    if (scriptEng.arrayPosition[2] == 0x4a0)
                    {
                        scriptEng.arrayPosition[2] = 0x420;
                    }
                }
                objectEntityList[scriptEng.arrayPosition[2]].type = (uint8_t)scriptEng.operands[0];
                objectEntityList[scriptEng.arrayPosition[2]].propertyValue = (uint8_t)scriptEng.operands[1];
                objectEntityList[scriptEng.arrayPosition[2]].xPos = scriptEng.operands[2];
                objectEntityList[scriptEng.arrayPosition[2]].yPos = scriptEng.operands[3];
                objectEntityList[scriptEng.arrayPosition[2]].direction = 0;
                objectEntityList[scriptEng.arrayPosition[2]].frame = 0;
                objectEntityList[scriptEng.arrayPosition[2]].priority = 1;
                objectEntityList[scriptEng.arrayPosition[2]].rotation = 0;
                objectEntityList[scriptEng.arrayPosition[2]].state = 0;
                objectEntityList[scriptEng.arrayPosition[2]].drawOrder = 3;
                objectEntityList[scriptEng.arrayPosition[2]].scale = 0x200;
                objectEntityList[scriptEng.arrayPosition[2]].inkEffect = 0;
                objectEntityList[scriptEng.arrayPosition[2]].alpha = 0;
                objectEntityList[scriptEng.arrayPosition[2]].animation = 0;
                objectEntityList[scriptEng.arrayPosition[2]].prevAnimation = 0;
                objectEntityList[scriptEng.arrayPosition[2]].animationSpeed = 0;
                objectEntityList[scriptEng.arrayPosition[2]].animationTimer = 0;
                objectEntityList[scriptEng.arrayPosition[2]].value[0] = 0;
                objectEntityList[scriptEng.arrayPosition[2]].value[1] = 0;
                objectEntityList[scriptEng.arrayPosition[2]].value[2] = 0;
                objectEntityList[scriptEng.arrayPosition[2]].value[3] = 0;
                objectEntityList[scriptEng.arrayPosition[2]].value[4] = 0;
                objectEntityList[scriptEng.arrayPosition[2]].value[5] = 0;
                objectEntityList[scriptEng.arrayPosition[2]].value[6] = 0;
                objectEntityList[scriptEng.arrayPosition[2]].value[7] = 0;
                break;
            }
            case 76:
            {
                num5 = 0;
                playerList[scriptEng.operands[0]].animationFile = objectScriptList[objectEntityList[scriptEng.operands[1]].type].animationFile;
                playerList[scriptEng.operands[0]].objectPtr = &objectEntityList[scriptEng.operands[1]];
                playerList[scriptEng.operands[0]].objectNum = scriptEng.operands[1];
                break;
            }
            case 77:
            {
                num5 = 0;
                if (playerList[playerNum].tileCollisions != 1)
                {
                    struct PlayerObject* playerObject = &playerList[playerNum];
                    playerObject->xPos = playerObject->xPos + playerList[playerNum].xVelocity;
                    struct PlayerObject* playerObject1 = &playerList[playerNum];
                    playerObject1->yPos = playerObject1->yPos + playerList[playerNum].yVelocity;
                    break;
                }
                else
                {
                    PlayerSystem_ProcessPlayerTileCollisions(&playerList[playerNum]);
                    break;
                }
            }
            case 78:
            {
                num5 = 0;
                PlayerSystem_ProcessPlayerControl(&playerList[playerNum]);
                break;
            }
            case 79:
            {
                AnimationSystem_ProcessObjectAnimation(&animationList[objectScriptList[objectEntityList[objectLoop].type].animationFile->aniListOffset + objectEntityList[objectLoop].animation], &objectEntityList[objectLoop]);
                num5 = 0;
                break;
            }
            case 80:
            {
                num5 = 0;
                AnimationSystem_DrawObjectAnimation(&animationList[objectScriptList[objectEntityList[objectLoop].type].animationFile->aniListOffset + objectEntityList[objectLoop].animation], &objectEntityList[objectLoop], (objectEntityList[objectLoop].xPos >> 16) - xScrollOffset, (objectEntityList[objectLoop].yPos >> 16) - yScrollOffset);
                break;
            }
            case 81:
            {
                num5 = 0;
                if (playerList[playerNum].visible != 1)
                {
                    break;
                }
                if (cameraEnabled != playerNum)
                {
                    AnimationSystem_DrawObjectAnimation(&animationList[objectScriptList[objectEntityList[objectLoop].type].animationFile->aniListOffset + objectEntityList[objectLoop].animation], &objectEntityList[objectLoop], (playerList[playerNum].xPos >> 16) - xScrollOffset, (playerList[playerNum].yPos >> 16) - yScrollOffset);
                    break;
                }
                else
                {
                    AnimationSystem_DrawObjectAnimation(&animationList[objectScriptList[objectEntityList[objectLoop].type].animationFile->aniListOffset + objectEntityList[objectLoop].animation], &objectEntityList[objectLoop], playerList[playerNum].screenXPos, playerList[playerNum].screenYPos);
                    break;
                }
            }
            case 82:
            {
                num5 = 0;
                if (scriptEng.operands[2] <= 1)
                {
                    AudioPlayback_SetMusicTrack(scriptText, scriptEng.operands[1], (uint8_t)scriptEng.operands[2], 0);
                    break;
                }
                else
                {
                    AudioPlayback_SetMusicTrack(scriptText, scriptEng.operands[1], 1, (uint32_t)scriptEng.operands[2]);
                    break;
                }
            }
            case 83:
            {
                num5 = 0;
                AudioPlayback_PlayMusic(scriptEng.operands[0]);
                break;
            }
            case 84:
            {
                num5 = 0;
                AudioPlayback_StopMusic();
                break;
            }
            case 85:
            {
                num5 = 0;
                AudioPlayback_PlaySfx(scriptEng.operands[0], (uint8_t)scriptEng.operands[1]);
                break;
            }
            case 86:
            {
                num5 = 0;
                AudioPlayback_StopSfx(scriptEng.operands[0]);
                break;
            }
            case 87:
            {
                num5 = 0;
                AudioPlayback_SetSfxAttributes(scriptEng.operands[0], scriptEng.operands[1], scriptEng.operands[2]);
                break;
            }
            case 88:
            {
                num5 = 0;
                switch (scriptEng.operands[0])
                {
                    case 0:
                    {
                        ObjectSystem_ObjectFloorCollision(scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]);
                        break;
                    }
                    case 1:
                    {
                        ObjectSystem_ObjectLWallCollision(scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]);
                        break;
                    }
                    case 2:
                    {
                        ObjectSystem_ObjectRWallCollision(scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]);
                        break;
                    }
                    case 3:
                    {
                        ObjectSystem_ObjectRoofCollision(scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]);
                        break;
                    }
                }
                break;
            }
            case 89:
            {
                num5 = 0;
                switch (scriptEng.operands[0])
                {
                    case 0:
                    {
                        ObjectSystem_ObjectFloorGrip(scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]);
                        break;
                    }
                    case 1:
                    {
                        ObjectSystem_ObjectLWallGrip(scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]);
                        break;
                    }
                    case 2:
                    {
                        ObjectSystem_ObjectRWallGrip(scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]);
                        break;
                    }
                    case 3:
                    {
                        ObjectSystem_ObjectRoofGrip(scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]);
                        break;
                    }
                }
                break;
            }
            case 90:
            {
                num5 = 0;
                AudioPlayback_PauseSound();
                EngineCallbacks_PlayVideoFile(scriptText);
                AudioPlayback_ResumeSound();
                break;
            }
            case 91:
            {
                num5 = 0;
                break;
            }
            case 92:
            {
                num5 = 0;
                AudioPlayback_PlaySfx(scriptEng.operands[0] + numGlobalSFX, (uint8_t)scriptEng.operands[1]);
                break;
            }
            case 93:
            {
                num5 = 0;
                AudioPlayback_StopSfx(scriptEng.operands[0] + numGlobalSFX);
                break;
            }
            case 94:
            {
                scriptEng.operands[0] = ~scriptEng.operands[0];
                break;
            }
            case 95:
            {
                num5 = 0;
                Scene3D_TransformVertexBuffer();
                Scene3D_Sort3DDrawList();
                Scene3D_Draw3DScene((int)objectScriptList[objectEntityList[objectLoop].type].surfaceNum);
                break;
            }
            case 96:
            {
                num5 = 0;
                switch (scriptEng.operands[0])
                {
                    case 0:
                    {
                        Scene3D_SetIdentityMatrix(matWorld);
                        break;
                    }
                    case 1:
                    {
                        Scene3D_SetIdentityMatrix(matView);
                        break;
                    }
                    case 2:
                    {
                        Scene3D_SetIdentityMatrix(matTemp);
                        break;
                    }
                }
                break;
            }
            case 97:
            {
                num5 = 0;
                switch (scriptEng.operands[0])
                {
                    case 0:
                    {
                        switch (scriptEng.operands[1])
                        {
                            case 0:
                            {
                                Scene3D_MatrixMultiply(matWorld, matWorld);
                                break;
                            }
                            case 1:
                            {
                                Scene3D_MatrixMultiply(matWorld, matView);
                                break;
                            }
                            case 2:
                            {
                                Scene3D_MatrixMultiply(matWorld, matTemp);
                                break;
                            }
                        }
                        break;
                    }
                    case 1:
                    {
                        switch (scriptEng.operands[1])
                        {
                            case 0:
                            {
                                Scene3D_MatrixMultiply(matView, matWorld);
                                break;
                            }
                            case 1:
                            {
                                Scene3D_MatrixMultiply(matView, matView);
                                break;
                            }
                            case 2:
                            {
                                Scene3D_MatrixMultiply(matView, matTemp);
                                break;
                            }
                        }
                        break;
                    }
                    case 2:
                    {
                        switch (scriptEng.operands[1])
                        {
                            case 0:
                            {
                                Scene3D_MatrixMultiply(matTemp, matWorld);
                                break;
                            }
                            case 1:
                            {
                                Scene3D_MatrixMultiply(matTemp, matView);
                                break;
                            }
                            case 2:
                            {
                                Scene3D_MatrixMultiply(matTemp, matTemp);
                                break;
                            }
                        }
                        break;
                    }
                }
                break;
            }
            case 98:
            {
                num5 = 0;
                switch (scriptEng.operands[0])
                {
                    case 0:
                    {
                        Scene3D_MatrixTranslateXYZ(matWorld, scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]);
                        break;
                    }
                    case 1:
                    {
                        Scene3D_MatrixTranslateXYZ(matView, scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]);
                        break;
                    }
                    case 2:
                    {
                        Scene3D_MatrixTranslateXYZ(matTemp, scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]);
                        break;
                    }
                }
                break;
            }
            case 99:
            {
                num5 = 0;
                switch (scriptEng.operands[0])
                {
                    case 0:
                    {
                        Scene3D_MatrixScaleXYZ(matWorld, scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]);
                        break;
                    }
                    case 1:
                    {
                        Scene3D_MatrixScaleXYZ(matView, scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]);
                        break;
                    }
                    case 2:
                    {
                        Scene3D_MatrixScaleXYZ(matTemp, scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]);
                        break;
                    }
                }
                break;
            }
            case 100:
            {
                num5 = 0;
                switch (scriptEng.operands[0])
                {
                    case 0:
                    {
                        Scene3D_MatrixRotateX(matWorld, scriptEng.operands[1]);
                        break;
                    }
                    case 1:
                    {
                        Scene3D_MatrixRotateX(matView, scriptEng.operands[1]);
                        break;
                    }
                    case 2:
                    {
                        Scene3D_MatrixRotateX(matTemp, scriptEng.operands[1]);
                        break;
                    }
                }
                break;
            }
            case 101:
            {
                num5 = 0;
                switch (scriptEng.operands[0])
                {
                    case 0:
                    {
                        Scene3D_MatrixRotateY(matWorld, scriptEng.operands[1]);
                        break;
                    }
                    case 1:
                    {
                        Scene3D_MatrixRotateY(matView, scriptEng.operands[1]);
                        break;
                    }
                    case 2:
                    {
                        Scene3D_MatrixRotateY(matTemp, scriptEng.operands[1]);
                        break;
                    }
                }
                break;
            }
            case 102:
            {
                num5 = 0;
                switch (scriptEng.operands[0])
                {
                    case 0:
                    {
                        Scene3D_MatrixRotateZ(matWorld, scriptEng.operands[1]);
                        break;
                    }
                    case 1:
                    {
                        Scene3D_MatrixRotateZ(matView, scriptEng.operands[1]);
                        break;
                    }
                    case 2:
                    {
                        Scene3D_MatrixRotateZ(matTemp, scriptEng.operands[1]);
                        break;
                    }
                }
                break;
            }
            case 103:
            {
                num5 = 0;
                switch (scriptEng.operands[0])
                {
                    case 0:
                    {
                        Scene3D_MatrixRotateXYZ(matWorld, scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]);
                        break;
                    }
                    case 1:
                    {
                        Scene3D_MatrixRotateXYZ(matView, scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]);
                        break;
                    }
                    case 2:
                    {
                        Scene3D_MatrixRotateXYZ(matTemp, scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3]);
                        break;
                    }
                }
                break;
            }
            case 104:
            {
                num5 = 0;
                switch (scriptEng.operands[0])
                {
                    case 0:
                    {
                        Scene3D_TransformVertices(matWorld, scriptEng.operands[1], scriptEng.operands[2]);
                        break;
                    }
                    case 1:
                    {
                        Scene3D_TransformVertices(matView, scriptEng.operands[1], scriptEng.operands[2]);
                        break;
                    }
                    case 2:
                    {
                        Scene3D_TransformVertices(matTemp, scriptEng.operands[1], scriptEng.operands[2]);
                        break;
                    }
                }
                break;
            }
            case 105:
            {
                num5 = 0;
                functionStack[functionStackPos] = scriptCodePtr;
                functionStackPos = functionStackPos + 1;
                functionStack[functionStackPos] = jumpTablePtr;
                functionStackPos = functionStackPos + 1;
                functionStack[functionStackPos] = num2;
                functionStackPos = functionStackPos + 1;
                scriptCodePtr = functionScriptList[scriptEng.operands[0]].mainScript;
                num2 = scriptCodePtr;
                jumpTablePtr = functionScriptList[scriptEng.operands[0]].mainJumpTable;
                break;
            }
            case 106:
            {
                num5 = 0;
                functionStackPos = functionStackPos - 1;
                num2 = functionStack[functionStackPos];
                functionStackPos = functionStackPos - 1;
                jumpTablePtr = functionStack[functionStackPos];
                functionStackPos = functionStackPos - 1;
                scriptCodePtr = functionStack[functionStackPos];
                break;
            }
            case 107:
            {
                num5 = 0;
                StageSystem_SetLayerDeformation(scriptEng.operands[0], scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3], scriptEng.operands[4], scriptEng.operands[5]);
                break;
            }
            case 108:
            {
                num5 = 0;
                scriptEng.checkResult = -1;
                while (num5 < gKeyDown.touches)
                {
                    if (gKeyDown.touchDown[num5] == 1 && gKeyDown.touchX[num5] > scriptEng.operands[0] && gKeyDown.touchX[num5] < scriptEng.operands[2] && gKeyDown.touchY[num5] > scriptEng.operands[1] && gKeyDown.touchY[num5] < scriptEng.operands[3])
                    {
                        scriptEng.checkResult = num5;
                    }
                    num5 = (signed char)(num5 + 1);
                }
                num5 = 0;
                break;
            }
            case 109:
            {
                if (scriptEng.operands[2] <= -1 || scriptEng.operands[3] <= -1)
                {
                    scriptEng.operands[0] = 0;
                    break;
                }
                else
                {
                    scriptEng.operands[0] = stageLayouts[scriptEng.operands[1]].tileMap[scriptEng.operands[2] + (scriptEng.operands[3] << 8)];
                    break;
                }
            }
            case 110:
            {
                if (scriptEng.operands[2] <= -1 || scriptEng.operands[3] <= -1)
                {
                    break;
                }
                stageLayouts[scriptEng.operands[1]].tileMap[scriptEng.operands[2] + (scriptEng.operands[3] << 8)] = (uint16_t)scriptEng.operands[0];
                break;
            }
            case 111:
            {
                scriptEng.operands[0] = (scriptEng.operands[1] & 1 << (scriptEng.operands[2] & 31)) >> (scriptEng.operands[2] & 31);
                break;
            }
            case 112:
            {
                if (scriptEng.operands[2] <= 0)
                {
                    scriptEng.operands[0] = scriptEng.operands[0] & ~(1 << (scriptEng.operands[1] & 31));
                    break;
                }
                else
                {
                    scriptEng.operands[0] = scriptEng.operands[0] | 1 << (scriptEng.operands[1] & 31);
                    break;
                }
            }
            case 113:
            {
                num5 = 0;
                AudioPlayback_PauseSound();
                break;
            }
            case 114:
            {
                num5 = 0;
                AudioPlayback_ResumeSound();
                break;
            }
            case 115:
            {
                num5 = 0;
                objectDrawOrderList[scriptEng.operands[0]].listSize = 0;
                break;
            }
            case 116:
            {
                num5 = 0;
                objectDrawOrderList[scriptEng.operands[0]].entityRef[objectDrawOrderList[scriptEng.operands[0]].listSize] = scriptEng.operands[1];
                struct ObjectDrawList* objectDrawList = &objectDrawOrderList[scriptEng.operands[0]];
                objectDrawList->listSize = objectDrawList->listSize + 1;
                break;
            }
            case 117:
            {
                scriptEng.operands[0] = objectDrawOrderList[scriptEng.operands[1]].entityRef[scriptEng.operands[2]];
                break;
            }
            case 118:
            {
                num5 = 0;
                objectDrawOrderList[scriptEng.operands[1]].entityRef[scriptEng.operands[2]] = scriptEng.operands[0];
                break;
            }
            case 119:
            {
                scriptEng.operands[4] = scriptEng.operands[1] >> 7;
                scriptEng.operands[5] = scriptEng.operands[2] >> 7;
                if (scriptEng.operands[4] <= -1 || scriptEng.operands[5] <= -1)
                {
                    scriptEng.operands[6] = 0;
                }
                else
                {
                    scriptEng.operands[6] = stageLayouts[0].tileMap[scriptEng.operands[4] + (scriptEng.operands[5] << 8)] << 6;
                }
                scriptEng.operands[6] = scriptEng.operands[6] + ((scriptEng.operands[1] & 127) >> 4) + ((scriptEng.operands[2] & 127) >> 4 << 3);
                switch (scriptEng.operands[3])
                {
                    case 0:
                    {
                        scriptEng.operands[0] = tile128x128.tile16x16[scriptEng.operands[6]];
                        break;
                    }
                    case 1:
                    {
                        scriptEng.operands[0] = tile128x128.direction[scriptEng.operands[6]];
                        break;
                    }
                    case 2:
                    {
                        scriptEng.operands[0] = tile128x128.visualPlane[scriptEng.operands[6]];
                        break;
                    }
                    case 3:
                    {
                        scriptEng.operands[0] = tile128x128.collisionFlag[0][scriptEng.operands[6]];
                        break;
                    }
                    case 4:
                    {
                        scriptEng.operands[0] = tile128x128.collisionFlag[1][scriptEng.operands[6]];
                        break;
                    }
                    case 5:
                    {
                        scriptEng.operands[0] = tileCollisions[0].flags[tile128x128.tile16x16[scriptEng.operands[6]]];
                        break;
                    }
                    case 6:
                    {
                        scriptEng.operands[0] = (int)tileCollisions[0].angle[tile128x128.tile16x16[scriptEng.operands[6]]];
                        break;
                    }
                    case 7:
                    {
                        scriptEng.operands[0] = tileCollisions[1].flags[tile128x128.tile16x16[scriptEng.operands[6]]];
                        break;
                    }
                    case 8:
                    {
                        scriptEng.operands[0] = (int)tileCollisions[1].angle[tile128x128.tile16x16[scriptEng.operands[6]]];
                        break;
                    }
                }
                break;
            }
            case 120:
            {
                num5 = 0;
                GraphicsSystem_Copy16x16Tile(scriptEng.operands[0], scriptEng.operands[1]);
                break;
            }
            case 121:
            {
                scriptEng.operands[4] = scriptEng.operands[1] >> 7;
                scriptEng.operands[5] = scriptEng.operands[2] >> 7;
                if (scriptEng.operands[4] <= -1 || scriptEng.operands[5] <= -1)
                {
                    scriptEng.operands[6] = 0;
                }
                else
                {
                    scriptEng.operands[6] = stageLayouts[0].tileMap[scriptEng.operands[4] + (scriptEng.operands[5] << 8)] << 6;
                }
                scriptEng.operands[6] = scriptEng.operands[6] + ((scriptEng.operands[1] & 127) >> 4) + ((scriptEng.operands[2] & 127) >> 4 << 3);
                switch (scriptEng.operands[3])
                {
                    case 0:
                    {
                        tile128x128.tile16x16[scriptEng.operands[6]] = (uint16_t)scriptEng.operands[0];
                        tile128x128.gfxDataPos[scriptEng.operands[6]] = tile128x128.tile16x16[scriptEng.operands[6]] << 2;
                        break;
                    }
                    case 1:
                    {
                        tile128x128.direction[scriptEng.operands[6]] = (uint8_t)scriptEng.operands[0];
                        break;
                    }
                    case 2:
                    {
                        tile128x128.visualPlane[scriptEng.operands[6]] = (uint8_t)scriptEng.operands[0];
                        break;
                    }
                    case 3:
                    {
                        tile128x128.collisionFlag[0][scriptEng.operands[6]] = (uint8_t)scriptEng.operands[0];
                        break;
                    }
                    case 4:
                    {
                        tile128x128.collisionFlag[1][scriptEng.operands[6]] = (uint8_t)scriptEng.operands[0];
                        break;
                    }
                    case 5:
                    {
                        tileCollisions[0].flags[tile128x128.tile16x16[scriptEng.operands[6]]] = (uint8_t)scriptEng.operands[0];
                        break;
                    }
                    case 6:
                    {
                        tileCollisions[0].angle[tile128x128.tile16x16[scriptEng.operands[6]]] = (uint8_t)scriptEng.operands[0];
                        break;
                    }
                }
                break;
            }
            case 122:
            {
                scriptEng.operands[0] = -1;
                scriptEng.sRegister = 0;
                while (scriptEng.operands[0] == -1)
                {
                    if (!FileIO_StringComp(scriptText, animationList[playerList[playerNum].animationFile->aniListOffset + scriptEng.sRegister].name))
                    {
                        scriptEng.sRegister = scriptEng.sRegister + 1;
                        if (scriptEng.sRegister != playerList[playerNum].animationFile->numAnimations)
                        {
                            continue;
                        }
                        scriptEng.operands[0] = 0;
                    }
                    else
                    {
                        scriptEng.operands[0] = scriptEng.sRegister;
                    }
                }
                break;
            }
            case 123:
            {
                num5 = 0;
                scriptEng.checkResult = FileIO_ReadSaveRAMData();
                break;
            }
            case 124:
            {
                num5 = 0;
                scriptEng.checkResult = FileIO_WriteSaveRAMData();
                break;
            }
            case 125:
            {
                num5 = 0;
                TextSystem_LoadFontFile(scriptText);
                break;
            }
            case 126:
            {
                num5 = 0;
                if (scriptEng.operands[2] != 0)
                {
                    TextSystem_LoadTextFile(&gameMenu[scriptEng.operands[0]], scriptText, 1);
                    break;
                }
                else
                {
                    TextSystem_LoadTextFile(&gameMenu[scriptEng.operands[0]], scriptText, 0);
                    break;
                }
            }
            case 127:
            {
                num5 = 0;
                textMenuSurfaceNo = objectScriptList[objectEntityList[objectLoop].type].surfaceNum;
                TextSystem_DrawBitmapText(&gameMenu[scriptEng.operands[0]], scriptEng.operands[1], scriptEng.operands[2], scriptEng.operands[3], scriptEng.operands[4], scriptEng.operands[5], scriptEng.operands[6]);
                break;
            }
            case 128:
            {
                switch (scriptEng.operands[2])
                {
                    case 0:
                    {
                        scriptEng.operands[0] = gameMenu[scriptEng.operands[1]].textData[gameMenu[scriptEng.operands[1]].entryStart[scriptEng.operands[3]] + scriptEng.operands[4]];
                        break;
                    }
                    case 1:
                    {
                        scriptEng.operands[0] = gameMenu[scriptEng.operands[1]].entrySize[scriptEng.operands[3]];
                        break;
                    }
                    case 2:
                    {
                        scriptEng.operands[0] = gameMenu[scriptEng.operands[1]].numRows;
                        break;
                    }
                }
                break;
            }
            case 129:
            {
                num5 = 0;
                gameMenu[scriptEng.operands[0]].entryHighlight[gameMenu[scriptEng.operands[0]].numRows] = (uint8_t)scriptEng.operands[1];
                TextSystem_AddTextMenuEntry(&gameMenu[scriptEng.operands[0]], gameVersion);
                break;
            }
            case 130:
            {
                num5 = 0;
                EngineCallbacks_OnlineSetAchievement(scriptEng.operands[0], scriptEng.operands[1]);
                break;
            }
            case 131:
            {
                num5 = 0;
                EngineCallbacks_OnlineSetLeaderboard(scriptEng.operands[0], scriptEng.operands[1]);
                break;
            }
            case 132:
            {
                num5 = 0;
                switch (scriptEng.operands[0])
                {
                    case 0:
                    {
                        EngineCallbacks_OnlineLoadAchievementsMenu();
                        break;
                    }
                    case 1:
                    {
                        EngineCallbacks_OnlineLoadLeaderboardsMenu();
                        break;
                    }
                }
                break;
            }
            case 133:
            {
                num5 = 0;
                EngineCallbacks_RetroEngineCallback(scriptEng.operands[0]);
                break;
            }
            case 134:
            {
                num5 = 0;
                break;
            }
        }
        if (num5 > 0)
        {
            scriptCodePtr = scriptCodePtr - num4;
        }
        for (i = 0; i < num5; i++)
        {
            switch (scriptData[scriptCodePtr])
            {
                case 1:
                {
                    scriptCodePtr++;
                    switch (scriptData[scriptCodePtr])
                    {
                        case 0:
                        {
                            num1 = objectLoop;
                            break;
                        }
                        case 1:
                        {
                            scriptCodePtr++;
                            if (scriptData[scriptCodePtr] != 1)
                            {
                                scriptCodePtr++;
                                num1 = scriptData[scriptCodePtr];
                                break;
                            }
                            else
                            {
                                scriptCodePtr++;
                                num1 = scriptEng.arrayPosition[scriptData[scriptCodePtr]];
                                break;
                            }
                        }
                        case 2:
                        {
                            scriptCodePtr++;
                            if (scriptData[scriptCodePtr] != 1)
                            {
                                scriptCodePtr++;
                                num1 = objectLoop + scriptData[scriptCodePtr];
                                break;
                            }
                            else
                            {
                                scriptCodePtr++;
                                num1 = objectLoop + scriptEng.arrayPosition[scriptData[scriptCodePtr]];
                                break;
                            }
                        }
                        case 3:
                        {
                            scriptCodePtr++;
                            if (scriptData[scriptCodePtr] != 1)
                            {
                                scriptCodePtr++;
                                num1 = objectLoop - scriptData[scriptCodePtr];
                                break;
                            }
                            else
                            {
                                scriptCodePtr++;
                                num1 = objectLoop - scriptEng.arrayPosition[scriptData[scriptCodePtr]];
                                break;
                            }
                        }
                    }
                    scriptCodePtr++;
                    switch (scriptData[scriptCodePtr])
                    {
                        case 0:
                        {
                            scriptEng.tempValue[0] = scriptEng.operands[i];
                            break;
                        }
                        case 1:
                        {
                            scriptEng.tempValue[1] = scriptEng.operands[i];
                            break;
                        }
                        case 2:
                        {
                            scriptEng.tempValue[2] = scriptEng.operands[i];
                            break;
                        }
                        case 3:
                        {
                            scriptEng.tempValue[3] = scriptEng.operands[i];
                            break;
                        }
                        case 4:
                        {
                            scriptEng.tempValue[4] = scriptEng.operands[i];
                            break;
                        }
                        case 5:
                        {
                            scriptEng.tempValue[5] = scriptEng.operands[i];
                            break;
                        }
                        case 6:
                        {
                            scriptEng.tempValue[6] = scriptEng.operands[i];
                            break;
                        }
                        case 7:
                        {
                            scriptEng.tempValue[7] = scriptEng.operands[i];
                            break;
                        }
                        case 8:
                        {
                            scriptEng.checkResult = scriptEng.operands[i];
                            break;
                        }
                        case 9:
                        {
                            scriptEng.arrayPosition[0] = scriptEng.operands[i];
                            break;
                        }
                        case 10:
                        {
                            scriptEng.arrayPosition[1] = scriptEng.operands[i];
                            break;
                        }
                        case 11:
                        {
                            globalVariables[num1] = scriptEng.operands[i];
                            break;
                        }
                        case 13:
                        {
                            objectEntityList[num1].type = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 14:
                        {
                            objectEntityList[num1].propertyValue = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 15:
                        {
                            objectEntityList[num1].xPos = scriptEng.operands[i];
                            break;
                        }
                        case 16:
                        {
                            objectEntityList[num1].yPos = scriptEng.operands[i];
                            break;
                        }
                        case 17:
                        {
                            objectEntityList[num1].xPos = scriptEng.operands[i] << 16;
                            break;
                        }
                        case 18:
                        {
                            objectEntityList[num1].yPos = scriptEng.operands[i] << 16;
                            break;
                        }
                        case 19:
                        {
                            objectEntityList[num1].state = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 20:
                        {
                            objectEntityList[num1].rotation = scriptEng.operands[i];
                            break;
                        }
                        case 21:
                        {
                            objectEntityList[num1].scale = scriptEng.operands[i];
                            break;
                        }
                        case 22:
                        {
                            objectEntityList[num1].priority = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 23:
                        {
                            objectEntityList[num1].drawOrder = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 24:
                        {
                            objectEntityList[num1].direction = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 25:
                        {
                            objectEntityList[num1].inkEffect = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 26:
                        {
                            objectEntityList[num1].alpha = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 27:
                        {
                            objectEntityList[num1].frame = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 28:
                        {
                            objectEntityList[num1].animation = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 29:
                        {
                            objectEntityList[num1].prevAnimation = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 30:
                        {
                            objectEntityList[num1].animationSpeed = scriptEng.operands[i];
                            break;
                        }
                        case 31:
                        {
                            objectEntityList[num1].animationTimer = scriptEng.operands[i];
                            break;
                        }
                        case 32:
                        {
                            objectEntityList[num1].value[0] = scriptEng.operands[i];
                            break;
                        }
                        case 33:
                        {
                            objectEntityList[num1].value[1] = scriptEng.operands[i];
                            break;
                        }
                        case 34:
                        {
                            objectEntityList[num1].value[2] = scriptEng.operands[i];
                            break;
                        }
                        case 35:
                        {
                            objectEntityList[num1].value[3] = scriptEng.operands[i];
                            break;
                        }
                        case 36:
                        {
                            objectEntityList[num1].value[4] = scriptEng.operands[i];
                            break;
                        }
                        case 37:
                        {
                            objectEntityList[num1].value[5] = scriptEng.operands[i];
                            break;
                        }
                        case 38:
                        {
                            objectEntityList[num1].value[6] = scriptEng.operands[i];
                            break;
                        }
                        case 39:
                        {
                            objectEntityList[num1].value[7] = scriptEng.operands[i];
                            break;
                        }
                        case 41:
                        {
                            playerList[playerNum].objectPtr->state = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 42:
                        {
                            playerList[playerNum].controlMode = (signed char)scriptEng.operands[i];
                            break;
                        }
                        case 43:
                        {
                            playerList[playerNum].controlLock = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 44:
                        {
                            playerList[playerNum].collisionMode = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 45:
                        {
                            playerList[playerNum].collisionPlane = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 46:
                        {
                            playerList[playerNum].xPos = scriptEng.operands[i];
                            break;
                        }
                        case 47:
                        {
                            playerList[playerNum].yPos = scriptEng.operands[i];
                            break;
                        }
                        case 48:
                        {
                            playerList[playerNum].xPos = scriptEng.operands[i] << 16;
                            break;
                        }
                        case 49:
                        {
                            playerList[playerNum].yPos = scriptEng.operands[i] << 16;
                            break;
                        }
                        case 50:
                        {
                            playerList[playerNum].screenXPos = scriptEng.operands[i];
                            break;
                        }
                        case 51:
                        {
                            playerList[playerNum].screenYPos = scriptEng.operands[i];
                            break;
                        }
                        case 52:
                        {
                            playerList[playerNum].speed = scriptEng.operands[i];
                            break;
                        }
                        case 53:
                        {
                            playerList[playerNum].xVelocity = scriptEng.operands[i];
                            break;
                        }
                        case 54:
                        {
                            playerList[playerNum].yVelocity = scriptEng.operands[i];
                            break;
                        }
                        case 55:
                        {
                            playerList[playerNum].gravity = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 56:
                        {
                            playerList[playerNum].angle = scriptEng.operands[i];
                            break;
                        }
                        case 57:
                        {
                            playerList[playerNum].skidding = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 58:
                        {
                            playerList[playerNum].pushing = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 59:
                        {
                            playerList[playerNum].trackScroll = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 60:
                        {
                            playerList[playerNum].up = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 61:
                        {
                            playerList[playerNum].down = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 62:
                        {
                            playerList[playerNum].left = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 63:
                        {
                            playerList[playerNum].right = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 64:
                        {
                            playerList[playerNum].jumpPress = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 65:
                        {
                            playerList[playerNum].jumpHold = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 66:
                        {
                            playerList[playerNum].followPlayer1 = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 67:
                        {
                            playerList[playerNum].lookPos = scriptEng.operands[i];
                            break;
                        }
                        case 68:
                        {
                            playerList[playerNum].water = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 69:
                        {
                            playerList[playerNum].movementStats.topSpeed = scriptEng.operands[i];
                            break;
                        }
                        case 70:
                        {
                            playerList[playerNum].movementStats.acceleration = scriptEng.operands[i];
                            break;
                        }
                        case 71:
                        {
                            playerList[playerNum].movementStats.deceleration = scriptEng.operands[i];
                            break;
                        }
                        case 72:
                        {
                            playerList[playerNum].movementStats.airAcceleration = scriptEng.operands[i];
                            break;
                        }
                        case 73:
                        {
                            playerList[playerNum].movementStats.airDeceleration = scriptEng.operands[i];
                            break;
                        }
                        case 74:
                        {
                            playerList[playerNum].movementStats.gravity = scriptEng.operands[i];
                            break;
                        }
                        case 75:
                        {
                            playerList[playerNum].movementStats.jumpStrength = scriptEng.operands[i];
                            break;
                        }
                        case 76:
                        {
                            playerList[playerNum].movementStats.jumpCap = scriptEng.operands[i];
                            break;
                        }
                        case 77:
                        {
                            playerList[playerNum].movementStats.rollingAcceleration = scriptEng.operands[i];
                            break;
                        }
                        case 78:
                        {
                            playerList[playerNum].movementStats.rollingDeceleration = scriptEng.operands[i];
                            break;
                        }
                        case 84:
                        {
                            playerList[playerNum].flailing[num1] = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 85:
                        {
                            playerList[playerNum].timer = scriptEng.operands[i];
                            break;
                        }
                        case 86:
                        {
                            playerList[playerNum].tileCollisions = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 87:
                        {
                            playerList[playerNum].objectInteraction = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 88:
                        {
                            playerList[playerNum].visible = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 89:
                        {
                            playerList[playerNum].objectPtr->rotation = scriptEng.operands[i];
                            break;
                        }
                        case 90:
                        {
                            playerList[playerNum].objectPtr->scale = scriptEng.operands[i];
                            break;
                        }
                        case 91:
                        {
                            playerList[playerNum].objectPtr->priority = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 92:
                        {
                            playerList[playerNum].objectPtr->drawOrder = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 93:
                        {
                            playerList[playerNum].objectPtr->direction = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 94:
                        {
                            playerList[playerNum].objectPtr->inkEffect = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 95:
                        {
                            playerList[playerNum].objectPtr->alpha = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 96:
                        {
                            playerList[playerNum].objectPtr->frame = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 97:
                        {
                            playerList[playerNum].objectPtr->animation = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 98:
                        {
                            playerList[playerNum].objectPtr->prevAnimation = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 99:
                        {
                            playerList[playerNum].objectPtr->animationSpeed = scriptEng.operands[i];
                            break;
                        }
                        case 100:
                        {
                            playerList[playerNum].objectPtr->animationTimer = scriptEng.operands[i];
                            break;
                        }
                        case 101:
                        {
                            playerList[playerNum].objectPtr->value[0] = scriptEng.operands[i];
                            break;
                        }
                        case 102:
                        {
                            playerList[playerNum].objectPtr->value[1] = scriptEng.operands[i];
                            break;
                        }
                        case 103:
                        {
                            playerList[playerNum].objectPtr->value[2] = scriptEng.operands[i];
                            break;
                        }
                        case 104:
                        {
                            playerList[playerNum].objectPtr->value[3] = scriptEng.operands[i];
                            break;
                        }
                        case 105:
                        {
                            playerList[playerNum].objectPtr->value[4] = scriptEng.operands[i];
                            break;
                        }
                        case 106:
                        {
                            playerList[playerNum].objectPtr->value[5] = scriptEng.operands[i];
                            break;
                        }
                        case 107:
                        {
                            playerList[playerNum].objectPtr->value[6] = scriptEng.operands[i];
                            break;
                        }
                        case 108:
                        {
                            playerList[playerNum].objectPtr->value[7] = scriptEng.operands[i];
                            break;
                        }
                        case 109:
                        {
                            playerList[playerNum].value[0] = scriptEng.operands[i];
                            break;
                        }
                        case 110:
                        {
                            playerList[playerNum].value[1] = scriptEng.operands[i];
                            break;
                        }
                        case 111:
                        {
                            playerList[playerNum].value[2] = scriptEng.operands[i];
                            break;
                        }
                        case 112:
                        {
                            playerList[playerNum].value[3] = scriptEng.operands[i];
                            break;
                        }
                        case 113:
                        {
                            playerList[playerNum].value[4] = scriptEng.operands[i];
                            break;
                        }
                        case 114:
                        {
                            playerList[playerNum].value[5] = scriptEng.operands[i];
                            break;
                        }
                        case 115:
                        {
                            playerList[playerNum].value[6] = scriptEng.operands[i];
                            break;
                        }
                        case 116:
                        {
                            playerList[playerNum].value[7] = scriptEng.operands[i];
                            break;
                        }
                        case 118:
                        {
                            stageMode = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 119:
                        {
                            activeStageList = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 120:
                        {
                            stageListPosition = scriptEng.operands[i];
                            break;
                        }
                        case 121:
                        {
                            timeEnabled = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 122:
                        {
                            milliSeconds = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 123:
                        {
                            seconds = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 124:
                        {
                            minutes = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 125:
                        {
                            actNumber = scriptEng.operands[i];
                            break;
                        }
                        case 126:
                        {
                            pauseEnabled = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 128:
                        {
                            newXBoundary1 = scriptEng.operands[i];
                            break;
                        }
                        case 129:
                        {
                            newXBoundary2 = scriptEng.operands[i];
                            break;
                        }
                        case 130:
                        {
                            newYBoundary1 = scriptEng.operands[i];
                            break;
                        }
                        case 131:
                        {
                            newYBoundary2 = scriptEng.operands[i];
                            break;
                        }
                        case 132:
                        {
                            if (xBoundary1 == scriptEng.operands[i])
                            {
                                break;
                            }
                            xBoundary1 = scriptEng.operands[i];
                            newXBoundary1 = scriptEng.operands[i];
                            break;
                        }
                        case 133:
                        {
                            if (xBoundary2 == scriptEng.operands[i])
                            {
                                break;
                            }
                            xBoundary2 = scriptEng.operands[i];
                            newXBoundary2 = scriptEng.operands[i];
                            break;
                        }
                        case 134:
                        {
                            if (yBoundary1 == scriptEng.operands[i])
                            {
                                break;
                            }
                            yBoundary1 = scriptEng.operands[i];
                            newYBoundary1 = scriptEng.operands[i];
                            break;
                        }
                        case 135:
                        {
                            if (yBoundary2 == scriptEng.operands[i])
                            {
                                break;
                            }
                            yBoundary2 = scriptEng.operands[i];
                            newYBoundary2 = scriptEng.operands[i];
                            break;
                        }
                        case 136:
                        {
                            bgDeformationData0[num1] = scriptEng.operands[i];
                            break;
                        }
                        case 137:
                        {
                            bgDeformationData1[num1] = scriptEng.operands[i];
                            break;
                        }
                        case 138:
                        {
                            bgDeformationData2[num1] = scriptEng.operands[i];
                            break;
                        }
                        case 139:
                        {
                            bgDeformationData3[num1] = scriptEng.operands[i];
                            break;
                        }
                        case 140:
                        {
                            waterLevel = scriptEng.operands[i];
                            break;
                        }
                        case 141:
                        {
                            activeTileLayers[num1] = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 142:
                        {
                            tLayerMidPoint = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 143:
                        {
                            playerMenuNum = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 144:
                        {
                            playerNum = scriptEng.operands[i];
                            break;
                        }
                        case 145:
                        {
                            cameraEnabled = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 146:
                        {
                            cameraTarget = (signed char)scriptEng.operands[i];
                            break;
                        }
                        case 147:
                        {
                            cameraStyle = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 148:
                        {
                            objectDrawOrderList[num1].listSize = scriptEng.operands[i];
                            break;
                        }
                        case 153:
                        {
                            xScrollOffset = scriptEng.operands[i];
                            xScrollA = xScrollOffset;
                            xScrollB = xScrollOffset + SCREEN_XSIZE;
                            break;
                        }
                        case 154:
                        {
                            yScrollOffset = scriptEng.operands[i];
                            yScrollA = yScrollOffset;
                            yScrollB = yScrollOffset + 240;
                            break;
                        }
                        case 155:
                        {
                            screenShakeX = scriptEng.operands[i];
                            break;
                        }
                        case 156:
                        {
                            screenShakeY = scriptEng.operands[i];
                            break;
                        }
                        case 157:
                        {
                            cameraAdjustY = scriptEng.operands[i];
                            break;
                        }
                        case 161:
                        {
                            AudioPlayback_SetMusicVolume(scriptEng.operands[i]);
                            break;
                        }
                        case 163:
                        {
                            gKeyDown.up = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 164:
                        {
                            gKeyDown.down = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 165:
                        {
                            gKeyDown.left = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 166:
                        {
                            gKeyDown.right = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 167:
                        {
                            gKeyDown.buttonA = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 168:
                        {
                            gKeyDown.buttonB = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 169:
                        {
                            gKeyDown.buttonC = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 170:
                        {
                            gKeyDown.start = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 171:
                        {
                            gKeyPress.up = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 172:
                        {
                            gKeyPress.down = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 173:
                        {
                            gKeyPress.left = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 174:
                        {
                            gKeyPress.right = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 175:
                        {
                            gKeyPress.buttonA = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 176:
                        {
                            gKeyPress.buttonB = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 177:
                        {
                            gKeyPress.buttonC = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 178:
                        {
                            gKeyPress.start = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 179:
                        {
                            gameMenu[0].selection1 = scriptEng.operands[i];
                            break;
                        }
                        case 180:
                        {
                            gameMenu[1].selection1 = scriptEng.operands[i];
                            break;
                        }
                        case 181:
                        {
                            stageLayouts[num1].xSize = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 182:
                        {
                            stageLayouts[num1].ySize = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 183:
                        {
                            stageLayouts[num1].type = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 184:
                        {
                            stageLayouts[num1].angle = scriptEng.operands[i];
                            if (stageLayouts[num1].angle < 0)
                            {
                                struct LayoutMap* layoutMap = &stageLayouts[num1];
                                layoutMap->angle = layoutMap->angle + 0x200;
                            }
                            struct LayoutMap* layoutMap1 = &stageLayouts[num1];
                            layoutMap1->angle = layoutMap1->angle & 0x1ff;
                            break;
                        }
                        case 185:
                        {
                            stageLayouts[num1].xPos = scriptEng.operands[i];
                            break;
                        }
                        case 186:
                        {
                            stageLayouts[num1].yPos = scriptEng.operands[i];
                            break;
                        }
                        case 187:
                        {
                            stageLayouts[num1].zPos = scriptEng.operands[i];
                            break;
                        }
                        case 188:
                        {
                            stageLayouts[num1].parallaxFactor = scriptEng.operands[i];
                            break;
                        }
                        case 189:
                        {
                            stageLayouts[num1].scrollSpeed = scriptEng.operands[i];
                            break;
                        }
                        case 190:
                        {
                            stageLayouts[num1].scrollPosition = scriptEng.operands[i];
                            break;
                        }
                        case 191:
                        {
                            stageLayouts[num1].deformationPos = scriptEng.operands[i];
                            struct LayoutMap* layoutMap2 = &stageLayouts[num1];
                            layoutMap2->deformationPos = layoutMap2->deformationPos & 0xff;
                            break;
                        }
                        case 192:
                        {
                            stageLayouts[num1].deformationPosW = scriptEng.operands[i];
                            struct LayoutMap* layoutMap3 = &stageLayouts[num1];
                            layoutMap3->deformationPosW = layoutMap3->deformationPosW & 0xff;
                            break;
                        }
                        case 193:
                        {
                            hParallax.parallaxFactor[num1] = scriptEng.operands[i];
                            break;
                        }
                        case 194:
                        {
                            hParallax.scrollSpeed[num1] = scriptEng.operands[i];
                            break;
                        }
                        case 195:
                        {
                            hParallax.scrollPosition[num1] = scriptEng.operands[i];
                            break;
                        }
                        case 196:
                        {
                            vParallax.parallaxFactor[num1] = scriptEng.operands[i];
                            break;
                        }
                        case 197:
                        {
                            vParallax.scrollSpeed[num1] = scriptEng.operands[i];
                            break;
                        }
                        case 198:
                        {
                            vParallax.scrollPosition[num1] = scriptEng.operands[i];
                            break;
                        }
                        case 199:
                        {
                            numVertices = scriptEng.operands[i];
                            break;
                        }
                        case 200:
                        {
                            numFaces = scriptEng.operands[i];
                            break;
                        }
                        case 201:
                        {
                            vertexBuffer[num1].x = scriptEng.operands[i];
                            break;
                        }
                        case 202:
                        {
                            vertexBuffer[num1].y = scriptEng.operands[i];
                            break;
                        }
                        case 203:
                        {
                            vertexBuffer[num1].z = scriptEng.operands[i];
                            break;
                        }
                        case 204:
                        {
                            vertexBuffer[num1].u = scriptEng.operands[i];
                            break;
                        }
                        case 205:
                        {
                            vertexBuffer[num1].v = scriptEng.operands[i];
                            break;
                        }
                        case 206:
                        {
                            indexBuffer[num1].a = scriptEng.operands[i];
                            break;
                        }
                        case 207:
                        {
                            indexBuffer[num1].b = scriptEng.operands[i];
                            break;
                        }
                        case 208:
                        {
                            indexBuffer[num1].c = scriptEng.operands[i];
                            break;
                        }
                        case 209:
                        {
                            indexBuffer[num1].d = scriptEng.operands[i];
                            break;
                        }
                        case 210:
                        {
                            indexBuffer[num1].flag = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 211:
                        {
                            indexBuffer[num1].color = scriptEng.operands[i];
                            break;
                        }
                        case 212:
                        {
                            projectionX = scriptEng.operands[i];
                            break;
                        }
                        case 213:
                        {
                            projectionY = scriptEng.operands[i];
                            break;
                        }
                        case 214:
                        {
                            gameMode = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 215:
                        {
                            debugMode = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 217:
                        {
                            saveRAM[num1] = scriptEng.operands[i];
                            break;
                        }
                        case 218:
                        {
                            gameLanguage = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 219:
                        {
                            objectScriptList[objectEntityList[objectLoop].type].surfaceNum = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 221:
                        {
                            frameSkipTimer = scriptEng.operands[i];
                            break;
                        }
                        case 222:
                        {
                            frameSkipSetting = scriptEng.operands[i];
                            break;
                        }
                        case 223:
                        {
                            gameSFXVolume = scriptEng.operands[i];
                            AudioPlayback_SetGameVolumes(gameBGMVolume, gameSFXVolume);
                            break;
                        }
                        case 224:
                        {
                            gameBGMVolume = scriptEng.operands[i];
                            AudioPlayback_SetGameVolumes(gameBGMVolume, gameSFXVolume);
                            break;
                        }
                        case 227:
                        {
                            gKeyPress.start = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 228:
                        {
                            gameHapticsEnabled = (uint8_t)scriptEng.operands[i];
                            break;
                        }
                        case 12:
                        case 40:
                        case 79:
                        case 80:
                        case 81:
                        case 82:
                        case 83:
                        case 117:
                        case 127:
                        case 149:
                        case 150:
                        case 151:
                        case 152:
                        case 158:
                        case 159:
                        case 160:
                        case 162:
                        case 216:
                        case 220:
                        case 225:
                        case 226:
                        default:
                        {
                            break;
                        }
                    }
                    scriptCodePtr++;
                    break;
                }
                case 2:
                {
                    scriptCodePtr = scriptCodePtr + 2;
                    break;
                }
                case 3:
                {
                    scriptCodePtr++;
                    num = 0;
                    num1 = 0;
                    scriptEng.sRegister = scriptData[scriptCodePtr];
                    while (num < scriptEng.sRegister)
                    {
                        switch (num1)
                        {
                            case 0:
                            {
                                scriptCodePtr++;
                                num1++;
                                break;
                            }
                            case 1:
                            {
                                num1++;
                                break;
                            }
                            case 2:
                            {
                                num1++;
                                break;
                            }
                            case 3:
                            {
                                num1 = 0;
                                break;
                            }
                        }
                        num++;
                    }
                    if (num1 != 0)
                    {
                        scriptCodePtr++;
                        break;
                    }
                    else
                    {
                        scriptCodePtr = scriptCodePtr + 2;
                        break;
                    }
                }
            }
        }
    }
}

void ObjectSystem_ProcessStartupScripts()
{
    objectEntityList[0x421].type = objectEntityList[0].type;
    scriptFramesNo = 0;
    playerNum = 0;
    scriptEng.arrayPosition[2] = 0x420;
    for (int i = 0; i < 0x100; i++)
    {
        objectLoop = 0x420;
        objectEntityList[0x420].type = (uint8_t)i;
        objectScriptList[i].numFrames = 0;
        objectScriptList[i].surfaceNum = 0;
        objectScriptList[i].frameListOffset = scriptFramesNo;
        objectScriptList[i].numFrames = scriptFramesNo;
        if (scriptData[objectScriptList[i].startupScript] > 0)
        {
            ObjectSystem_ProcessScript(objectScriptList[i].startupScript, objectScriptList[i].startupJumpTable, 3);
        }
        objectScriptList[i].numFrames = scriptFramesNo - objectScriptList[i].numFrames;
    }
    objectEntityList[0x420].type = objectEntityList[0x421].type;
    objectEntityList[0x420].type = 0;
}

void ObjectSystem_SetObjectTypeName(char* typeName, int scriptNum)
{
    int length = 0;
    int num = 0;
    while (length < (int)strlen(typeName))
    {
        if (typeName[length] == 0)
        {
            length = (int)strlen(typeName);
        }
        else
        {
            if (typeName[length] != ' ')
            {
                typeNames[scriptNum][num] = typeName[length];
                num++;
            }
            length++;
        }
    }
    if (num < sizeof(typeNames[1]))
    {
        typeNames[scriptNum][num] = '\0';
    }
}
