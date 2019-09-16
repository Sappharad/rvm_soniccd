//
//  RenderDevice.h
//  rvm
//

#ifndef RenderDevice_h
#define RenderDevice_h

#include <stdio.h>
#include <stdbool.h>
#include "GraphicsSystem.h"
#include "InputSystem_dc.h"

extern int orthWidth;
extern int viewWidth;
extern int viewHeight;
extern float viewAspect;
extern int bufferWidth;
extern int bufferHeight;
extern int highResMode;
extern bool useFBTexture;

void InitRenderDevice();
void RenderDevice_UpdateHardwareTextures();
void RenderDevice_SetScreenDimensions(int width, int height);
void RenderDevice_FlipScreen();
void RenderDevice_FlipScreenHRes();
void drawGLTest();

#endif /* RenderDevice_h */
