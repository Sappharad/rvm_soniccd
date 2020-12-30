//
//  AnimationSystem.c
//  rvm
//

#include "AnimationSystem.h"

struct SpriteFrame animationFrames[4096];
int animationFramesNo;
struct SpriteAnimation animationList[1024];
int animationListNo;
struct AnimationFileList animationFile[256];
int animationFileNo;
struct CollisionBox collisionBoxList[32];
int collisionBoxNo;

void Init_AnimationSystem()
{
    animationFramesNo = 0;
    animationListNo = 0;
    animationFileNo = 0;
    collisionBoxNo = 0;
}
void AnimationSystem_LoadAnimationFile(char* filePath)
{
    char array[32];
    uint8_t array2[24];
    struct FileData fileData;
    if (FileIO_LoadFile(filePath, &fileData))
    {
        uint8_t b = FileIO_ReadByte();
        for (int i = 0; i < 24; i++)
        {
            array2[i] = 0;
        }
        uint8_t b2;
        for (int i = 0; i < (int)b; i++)
        {
            b2 = FileIO_ReadByte();
            int j = 0;
            if (b2 > 0)
            {
                while ((int)b2 > j)
                {
                    array[j] = (char)FileIO_ReadByte();
                    j++;
                }
                array[j] = '\0';
                FileIO_GetFileInfo(&fileData);
                FileIO_CloseFile();
                array2[i] = GraphicsSystem_AddGraphicsFile(array);
                FileIO_SetFileInfo(&fileData);
            }
        }
        b2 = FileIO_ReadByte();
        animationFile[animationFileNo].numAnimations = (int)b2;
        animationFile[animationFileNo].aniListOffset = animationListNo;
        for (int i = 0; i < animationFile[animationFileNo].numAnimations; i++)
        {
            b = FileIO_ReadByte();
            int j;
            for (j = 0; j < (int)b; j++)
            {
                animationList[animationListNo].name[j] = (char)FileIO_ReadByte();
            }
            animationList[animationListNo].name[j] = '\0';
            b2 = FileIO_ReadByte();
            animationList[animationListNo].numFrames = b2;
            b2 = FileIO_ReadByte();
            animationList[animationListNo].animationSpeed = b2;
            b2 = FileIO_ReadByte();
            animationList[animationListNo].loopPosition = b2;
            b2 = FileIO_ReadByte();
            animationList[animationListNo].rotationFlag = b2;
            animationList[animationListNo].frameListOffset = animationFramesNo;
            for (j = 0; j < (int)animationList[animationListNo].numFrames; j++)
            {
                b2 = FileIO_ReadByte();
                animationFrames[animationFramesNo].surfaceNum = array2[(int)b2];
                b2 = FileIO_ReadByte();
                animationFrames[animationFramesNo].collisionBox = b2;
                b2 = FileIO_ReadByte();
                animationFrames[animationFramesNo].left = (int)b2;
                b2 = FileIO_ReadByte();
                animationFrames[animationFramesNo].top = (int)b2;
                b2 = FileIO_ReadByte();
                animationFrames[animationFramesNo].xSize = (int)b2;
                b2 = FileIO_ReadByte();
                animationFrames[animationFramesNo].ySize = (int)b2;
                signed char b3 = (signed char)FileIO_ReadByte();
                animationFrames[animationFramesNo].xPivot = (int)b3;
                b3 = (char)FileIO_ReadByte();
                animationFrames[animationFramesNo].yPivot = (int)b3;
                animationFramesNo++;
            }
            if (animationList[animationListNo].rotationFlag == 3)
            {
                struct SpriteAnimation* animation = &animationList[animationListNo];
                animation->numFrames = (uint8_t)(animation->numFrames >> 1);
            }
            animationListNo++;
        }
        b = FileIO_ReadByte();
        animationFile[animationFileNo].cbListOffset = collisionBoxNo;
        for (int i = 0; i < (int)b; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                collisionBoxList[collisionBoxNo].left[j] = (char)FileIO_ReadByte();
                collisionBoxList[collisionBoxNo].top[j] = (char)FileIO_ReadByte();
                collisionBoxList[collisionBoxNo].right[j] = (char)FileIO_ReadByte();
                collisionBoxList[collisionBoxNo].bottom[j] = (char)FileIO_ReadByte();
            }
            collisionBoxNo++;
        }
        FileIO_CloseFile();
    }
}
struct AnimationFileList* AnimationSystem_AddAnimationFile(char* fileName)
{
    int i = 0;
    char array[] = "Data/Animations/";
    char filePath[64];
    FileIO_StrCopy(filePath, sizeof(filePath), array, sizeof(array));
    FileIO_StrAdd(filePath, sizeof(filePath), fileName, (int)strlen(fileName));
    while (i < 256)
    {
        if (strlen(animationFile[i].fileName) <= 0)
        {
            FileIO_StrCopy(animationFile[i].fileName, sizeof(animationFile[i].fileName), fileName, (int)strlen(fileName));
            AnimationSystem_LoadAnimationFile(filePath);
            animationFileNo++;
            return &animationFile[i];
        }
        if (FileIO_StringComp(animationFile[i].fileName, fileName))
        {
            return &animationFile[i];
        }
        i++;
    }
    return NULL;
}
struct AnimationFileList* AnimationSystem_GetDefaultAnimationRef()
{
    return &animationFile[0];
}
void AnimationSystem_ClearAnimationData()
{
    char array[] = "";
    for (int i = 0; i < 4096; i++)
    {
        animationFrames[i].left = 0;
        animationFrames[i].top = 0;
        animationFrames[i].xSize = 0;
        animationFrames[i].ySize = 0;
        animationFrames[i].xPivot = 0;
        animationFrames[i].yPivot = 0;
        animationFrames[i].surfaceNum = 0;
        animationFrames[i].collisionBox = 0;
    }
    for (int i = 0; i < 32; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            collisionBoxList[i].left[j] = 0;
            collisionBoxList[i].top[j] = 0;
            collisionBoxList[i].right[j] = 0;
            collisionBoxList[i].bottom[j] = 0;
        }
    }
    for (int i = 0; i < 256; i++)
    {
        FileIO_StrCopy(animationFile[i].fileName, sizeof(animationFile[i].fileName), array, sizeof(array));
    }
    animationFramesNo = 0;
    animationListNo = 0;
    animationFileNo = 0;
    collisionBoxNo = 0;
    animationFile[0].numAnimations = 0;
    animationList[0].frameListOffset = animationFramesNo;
    animationFile[0].aniListOffset = animationListNo;
    animationFile[0].cbListOffset = collisionBoxNo;
}
void AnimationSystem_ProcessObjectAnimation(struct SpriteAnimation *animationRef, struct ObjectEntity *currentObject)
{
    if (currentObject->animationSpeed > 0)
    {
        if (currentObject->animationSpeed > 240)
        {
            currentObject->animationSpeed = 240;
        }
        currentObject->animationTimer += currentObject->animationSpeed;
    }
    else
    {
        currentObject->animationTimer += (int)animationRef->animationSpeed;
    }
    if (currentObject->animation != currentObject->prevAnimation)
    {
        currentObject->prevAnimation = currentObject->animation;
        currentObject->frame = 0;
        currentObject->animationTimer = 0;
        currentObject->animationSpeed = 0;
    }
    if (currentObject->animationTimer > 239)
    {
        currentObject->animationTimer -= 240;
        currentObject->frame += 1;
    }
    if (currentObject->frame >= animationRef->numFrames)
    {
        currentObject->frame = animationRef->loopPosition;
    }
}
void AnimationSystem_DrawObjectAnimation(struct SpriteAnimation *animationRef, struct ObjectEntity *currentObject, int xPos, int yPos)
{
    switch (animationRef->rotationFlag)
    {
        case 0:
        {
            struct SpriteFrame* spriteFrame = &animationFrames[animationRef->frameListOffset + (int)currentObject->frame];
            switch (currentObject->direction)
            {
                case 0:
                    GraphicsSystem_DrawSpriteFlipped(xPos + spriteFrame->xPivot, yPos + spriteFrame->yPivot, spriteFrame->xSize, spriteFrame->ySize, spriteFrame->left, spriteFrame->top, (int)currentObject->direction, (int)spriteFrame->surfaceNum);
                    return;
                case 1:
                    GraphicsSystem_DrawSpriteFlipped(xPos - spriteFrame->xSize - spriteFrame->xPivot, yPos + spriteFrame->yPivot, spriteFrame->xSize, spriteFrame->ySize, spriteFrame->left, spriteFrame->top, (int)currentObject->direction, (int)spriteFrame->surfaceNum);
                    return;
                case 2:
                    GraphicsSystem_DrawSpriteFlipped(xPos + spriteFrame->xPivot, yPos - spriteFrame->ySize - spriteFrame->yPivot, spriteFrame->xSize, spriteFrame->ySize, spriteFrame->left, spriteFrame->top, (int)currentObject->direction, (int)spriteFrame->surfaceNum);
                    return;
                case 3:
                    GraphicsSystem_DrawSpriteFlipped(xPos - spriteFrame->xSize - spriteFrame->xPivot, yPos - spriteFrame->ySize - spriteFrame->yPivot, spriteFrame->xSize, spriteFrame->ySize, spriteFrame->left, spriteFrame->top, (int)currentObject->direction, (int)spriteFrame->surfaceNum);
                    return;
                default:
                    return;
            }
            break;
        }
        case 1:
        {
            struct SpriteFrame* spriteFrame = &animationFrames[animationRef->frameListOffset + (int)currentObject->frame];
            GraphicsSystem_DrawRotatedSprite(currentObject->direction, xPos, yPos, -spriteFrame->xPivot, -spriteFrame->yPivot, spriteFrame->left, spriteFrame->top, spriteFrame->xSize, spriteFrame->ySize, currentObject->rotation, (int)spriteFrame->surfaceNum);
            return;
        }
        case 2:
        {
            struct SpriteFrame* spriteFrame = &animationFrames[animationRef->frameListOffset + (int)currentObject->frame];
            int rotAngle;
            if (currentObject->rotation < 256)
            {
                rotAngle = ((currentObject->rotation + 20) >> 6) << 6;
            }
            else
            {
                rotAngle = 512 - (((532 - currentObject->rotation) >> 6) << 6);
            }
            GraphicsSystem_DrawRotatedSprite(currentObject->direction, xPos, yPos, -spriteFrame->xPivot, -spriteFrame->yPivot, spriteFrame->left, spriteFrame->top, spriteFrame->xSize, spriteFrame->ySize, rotAngle, (int)spriteFrame->surfaceNum);
            return;
        }
        case 3:
        {
            int rotAngle;
            if (currentObject->rotation < 256)
            {
                rotAngle = (currentObject->rotation + 20) >> 6;
            }
            else
            {
                rotAngle = 8 - ((532 - currentObject->rotation) >> 6);
            }
            int num = (int)currentObject->frame;
            switch (rotAngle)
            {
                case 0:
                case 8:
                    rotAngle = 0;
                    break;
                case 1:
                    num += (int)animationRef->numFrames;
                    if (currentObject->direction == 0)
                    {
                        rotAngle = 128;
                    }
                    else
                    {
                        rotAngle = 0;
                    }
                    break;
                case 2:
                    rotAngle = 128;
                    break;
                case 3:
                    num += (int)animationRef->numFrames;
                    if (currentObject->direction == 0)
                    {
                        rotAngle = 256;
                    }
                    else
                    {
                        rotAngle = 128;
                    }
                    break;
                case 4:
                    rotAngle = 256;
                    break;
                case 5:
                    num += (int)animationRef->numFrames;
                    if (currentObject->direction == 0)
                    {
                        rotAngle = 384;
                    }
                    else
                    {
                        rotAngle = 256;
                    }
                    break;
                case 6:
                    rotAngle = 384;
                    break;
                case 7:
                    num += (int)animationRef->numFrames;
                    if (currentObject->direction == 0)
                    {
                        rotAngle = 0;
                    }
                    else
                    {
                        rotAngle = 384;
                    }
                    break;
            }
            struct SpriteFrame* spriteFrame = &animationFrames[animationRef->frameListOffset + num];
            GraphicsSystem_DrawRotatedSprite(currentObject->direction, xPos, yPos, -spriteFrame->xPivot, -spriteFrame->yPivot, spriteFrame->left, spriteFrame->top, spriteFrame->xSize, spriteFrame->ySize, rotAngle, (int)spriteFrame->surfaceNum);
            return;
        }
        default:
            return;
    }
}
