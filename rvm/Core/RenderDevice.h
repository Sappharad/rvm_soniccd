//
//  RenderDevice.h
//  rvm
//

#ifndef RenderDevice_h
#define RenderDevice_h

#include <stdio.h>
#include <stdbool.h>
#include "GraphicsSystem.h"
#include "InputSystem.h"

extern int orthWidth;
extern int viewWidth;
extern int viewHeight;
extern float viewAspect;
extern int bufferWidth;
extern int bufferHeight;
extern int highResMode;
extern bool useFBTexture;

void InitRenderDevice(void);
void RenderDevice_UpdateHardwareTextures(void);
void RenderDevice_SetScreenDimensions(int width, int height);
void RenderDevice_ScaleViewport(int width, int height);
void RenderDevice_FlipScreen(void);
void RenderDevice_FlipScreenHRes(void);
void drawGLTest(void);

#endif /* RenderDevice_h */
