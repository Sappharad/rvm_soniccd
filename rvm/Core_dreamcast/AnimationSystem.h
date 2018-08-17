//
//  AnimationSystem.h
//

#ifndef AnimationSystem_h
#define AnimationSystem_h

#include <stdio.h>
#include <string.h>
#include "AnimationFileList.h"
#include "SpriteFrame.h"
#include "SpriteAnimation.h"
#include "CollisionBox.h"
#include "ObjectEntity.h"
#include "GraphicsSystem.h"
#include "FileData.h"
#include "FileIO.h"

extern struct SpriteFrame animationFrames[4096];
extern int animationFramesNo;
extern struct SpriteAnimation animationList[1024];
extern int animationListNo;
extern struct AnimationFileList animationFile[256];
extern int animationFileNo;
extern struct CollisionBox collisionBoxList[32];
extern int collisionBoxNo;

void Init_AnimationSystem();
void AnimationSystem_LoadAnimationFile(char* filePath);
struct AnimationFileList* AnimationSystem_AddAnimationFile(char* fileName);
struct AnimationFileList* AnimationSystem_GetDefaultAnimationRef();
void AnimationSystem_ClearAnimationData();
void AnimationSystem_ProcessObjectAnimation(struct SpriteAnimation *animationRef, struct ObjectEntity *currentObject);
void AnimationSystem_DrawObjectAnimation(struct SpriteAnimation *animationRef, struct ObjectEntity *currentObject, int xPos, int yPos);

#endif /* AnimationSystem_h */
