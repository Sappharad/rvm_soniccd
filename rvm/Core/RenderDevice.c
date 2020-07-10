//
//  RenderDevice.c
//  rvm
//

#include "RenderDevice.h"
#if WINDOWS
#include <Windows.h>
#include <gl/GL.h>
#include <gl/glext.h>
#elif LINUX
#include <gl/GL.h>
#include <gl/glext.h>
#else
#include <OpenGL/gl.h>
#endif

#define NUM_TEXTURES 6
const int TEXTURE_SIZE = 1024*1024*2;
int orthWidth;
int viewWidth;
int viewHeight;
float viewAspect;
int bufferWidth;
int bufferHeight;
int highResMode;
int virtualX;
int virtualY;
int virtualWidth;
int virtualHeight;
GLuint gfxTextureID[NUM_TEXTURES];
GLuint framebufferId;
GLuint fbTextureId;
short screenVerts[] = {
    0, 0,
    6400, 0,
    0, 3844,
    6400, 0,
    0, 3844,
    6400, 3844
};
float fbTexVerts[] = {
    -1024, 1024,
    0, 1024,
    -1024, 0,
    0, 1024,
    -1024, 0,
    0, 0,
};
float pureLight[] = {
    1.0, 1.0, 1.0, 1.0,
    1.0, 1.0, 1.0, 1.0
};

void HandleGlError(){
    GLenum boo = glGetError();
    if(boo != GL_NO_ERROR){
        if (boo == GL_INVALID_OPERATION){
            printf("Invalid operation\n");
        }
#if !defined(WINDOWS) && !defined(LINUX)
        else if(boo == GL_INVALID_FRAMEBUFFER_OPERATION){
            boo = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if(boo == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT){
                printf("Framebuffer attachment missing!");
            }
            else{
                printf("Invalid framebuffer operation\n");
            }
        }
#endif
        else{
            printf("An error!\n");
        }
    }
}


void InitRenderDevice()
{
    Init_GraphicsSystem();
    highResMode = 0;
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glDisable(GL_LIGHTING);
    glDisable(GL_DITHER);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);
    
    glLoadIdentity();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GraphicsSystem_SetupPolygonLists();
    
    for (int i = 0; i < NUM_TEXTURES; i++)
    {
        glGenTextures(1, &gfxTextureID[i]);
        glBindTexture(GL_TEXTURE_2D, gfxTextureID[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, texBuffer);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(0.0009765625f, 0.0009765625f, 1.0f); //1.0 / 1024.0. Allows for texture locations in pixels instead of from 0.0 to 1.0
    glMatrixMode(GL_PROJECTION);
    
    glClear(GL_COLOR_BUFFER_BIT);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glClear(GL_COLOR_BUFFER_BIT);
    
    framebufferId = 0;
    fbTextureId = 0;
}
void RenderDevice_UpdateHardwareTextures()
{
    GraphicsSystem_SetActivePalette(0, 0, 240);
    GraphicsSystem_UpdateTextureBufferWithTiles();
    GraphicsSystem_UpdateTextureBufferWithSortedSprites();
    
    glBindTexture(GL_TEXTURE_2D, gfxTextureID[0]);
    HandleGlError();
    
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1024, 1024, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, texBuffer);
    HandleGlError();
    
    for (uint8_t b = 1; b < NUM_TEXTURES; b += 1)
    {
        GraphicsSystem_SetActivePalette(b, 0, 240);
        GraphicsSystem_UpdateTextureBufferWithTiles();
        GraphicsSystem_UpdateTextureBufferWithSprites();

        glBindTexture(GL_TEXTURE_2D, gfxTextureID[b]);
        HandleGlError();
        
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1024, 1024, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, texBuffer);
        HandleGlError();
    }
    GraphicsSystem_SetActivePalette(0, 0, 240);
}
void RenderDevice_SetScreenDimensions(int width, int height)
{
    touchWidth = width;
    touchHeight = height;
    viewWidth = touchWidth;
    viewHeight = touchHeight;
    float num = (float)viewWidth / (float)viewHeight;
    num *= 240.0f;
    bufferWidth = (int)num;
    bufferWidth += 8;
    bufferWidth = bufferWidth >> 4 << 4;
    /*if (bufferWidth > 400)
    {
        bufferWidth = 400;
    }*/
    viewAspect = 0.75f;
    if (viewHeight >= 480.0)
    {
        HQ3DFloorEnabled = true;
    }
    else
    {
        HQ3DFloorEnabled = false;
    }
    GraphicsSystem_SetScreenRenderSize(bufferWidth, bufferWidth);
    if (viewHeight >= 480)
    {
        bufferWidth *= 2;
        bufferHeight = 480;
    }
    else
    {
        bufferHeight = 240;
    }
    orthWidth = SCREEN_XSIZE * 16;
    
    //You should never change screen dimensions, so we should not need to do this, but I'll do it anyway.
    if(framebufferId > 0){
        glDeleteFramebuffers(1, &framebufferId);
    }
    if(fbTextureId > 0){
        glDeleteTextures(1, &fbTextureId);
    }
    //Setup framebuffer texture
    glGenFramebuffers(1, &framebufferId);
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
    glGenTextures(1, &fbTextureId);
    glBindTexture(GL_TEXTURE_2D, fbTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, viewWidth, viewHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbTextureId, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    int newWidth = width * 8;
    int newHeight = (height * 8)+4;
    
    screenVerts[2] = newWidth;
    screenVerts[6] = newWidth;
    screenVerts[10] = newWidth;
    screenVerts[5] = newHeight;
    screenVerts[9] = newHeight;
    screenVerts[11] = newHeight;
    RenderDevice_ScaleViewport(width, height);
}

void RenderDevice_ScaleViewport(int width, int height){
    virtualWidth = width;
    virtualHeight = height;
    virtualX = 0;
    virtualY = 0;
    
    float virtualAspect = (float)width / height;
    float realAspect = (float)viewWidth / viewHeight;
    if(virtualAspect < realAspect){
        virtualHeight = viewHeight * ((float)width/viewWidth);
        virtualY = (height - virtualHeight) >> 1;
    }
    else{
        virtualWidth = viewWidth * ((float)height/viewHeight);
        virtualX = (width - virtualWidth) >> 1;
    }
}

void CalcPerspective(float fov, float aspectRatio, float nearPlane, float farPlane){
    GLfloat matrix[16];
    float w = 1.0 / tanf(fov * 0.5f);
    float h = 1.0 / (w*aspectRatio);
    float q = (nearPlane+farPlane)/(farPlane - nearPlane);
    
    matrix[0] = w;
    matrix[1] = 0;
    matrix[2] = 0;
    matrix[3] = 0;
    
    matrix[4] = 0;
    matrix[5] = h/2;
    matrix[6] = 0;
    matrix[7] = 0;
    
    matrix[8] = 0;
    matrix[9] = 0;
    matrix[10] = q;
    matrix[11] = 1.0;
    
    matrix[12] = 0;
    matrix[13] = 0;
    matrix[14] = (((farPlane*-2.0f)*nearPlane)/(farPlane-nearPlane));
    matrix[15] = 0;
    
    glMultMatrixf(matrix);
}

void RenderDevice_FlipScreen()
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
    
    glLoadIdentity();
    HandleGlError();
    
    glOrtho(0, orthWidth, 3844.0f, 0.0, 0.0f, 100.0f);
    if(texPaletteNum >= NUM_TEXTURES){
        //This is a stage that requires the software renderer for correct water palettes.
        //Only happens if using the PC / Console Data.rsdk file
        glBindTexture(GL_TEXTURE_2D, gfxTextureID[texPaletteNum % NUM_TEXTURES]);
    }
    else{
        glBindTexture(GL_TEXTURE_2D, gfxTextureID[texPaletteNum]);
    }
    glEnableClientState(GL_COLOR_ARRAY);
    HandleGlError();
    if(render3DEnabled){
        glVertexPointer(2, GL_SHORT, 12, &gfxPolyList[0].position);
        glTexCoordPointer(2, GL_SHORT, 12, &gfxPolyList[0].texCoord);
        glColorPointer(4, GL_UNSIGNED_BYTE, 12, &gfxPolyList[0].color);
        glDrawElements(GL_TRIANGLES, gfxIndexSizeOpaque, GL_UNSIGNED_SHORT, gfxPolyListIndex);
        glEnable(GL_BLEND);
        HandleGlError();
        
        glViewport(0, 0, viewWidth, viewHeight);
        glPushMatrix();
        glLoadIdentity();
        CalcPerspective(1.8326f, viewAspect, 0.1f, 1000.0f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
       
        glScalef(1.0f, -1.0f, -1.0f);
        glRotatef(180.0f + floor3DAngle, 0, 1.0f, 0);
        glTranslatef(floor3DPos.X, floor3DPos.Y, floor3DPos.Z);
        glVertexPointer(3, GL_FLOAT, 20, &polyList3D[0].position);
        glTexCoordPointer(2, GL_SHORT, 20, &polyList3D[0].texCoord);
        glColorPointer(4, GL_UNSIGNED_BYTE, 20, &polyList3D[0].color);
        glDrawElements(GL_TRIANGLES, indexSize3D, GL_UNSIGNED_SHORT, gfxPolyListIndex);
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        
        glViewport(0, 0, bufferWidth, bufferHeight);
        glPopMatrix();
        HandleGlError();
        
        int numBlendedGfx = (int)(gfxIndexSize - gfxIndexSizeOpaque);
        glVertexPointer(2, GL_SHORT, 12, &gfxPolyList[0].position);
        glTexCoordPointer(2, GL_SHORT, 12, &gfxPolyList[0].texCoord);
        glColorPointer(4, GL_UNSIGNED_BYTE, 12, &gfxPolyList[0].color);
        glDrawElements(GL_TRIANGLES, numBlendedGfx, GL_UNSIGNED_SHORT, &gfxPolyListIndex[gfxIndexSizeOpaque]);
        HandleGlError();
    }
    else{
        glVertexPointer(2, GL_SHORT, 12, &gfxPolyList[0].position);
        glTexCoordPointer(2, GL_SHORT, 12, &gfxPolyList[0].texCoord);
        glColorPointer(4, GL_UNSIGNED_BYTE, 12, &gfxPolyList[0].color);
        glDrawElements(GL_TRIANGLES, gfxIndexSizeOpaque, GL_UNSIGNED_SHORT, gfxPolyListIndex);
        HandleGlError();
        
        int numBlendedGfx = (int)(gfxIndexSize - gfxIndexSizeOpaque);
        
        glEnable(GL_BLEND);
        glEnable(GL_TEXTURE_2D);
        glVertexPointer(2, GL_SHORT, 12, &gfxPolyList[0].position);
        glTexCoordPointer(2, GL_SHORT, 12, &gfxPolyList[0].texCoord);
        glColorPointer(4, GL_UNSIGNED_BYTE, 12, &gfxPolyList[0].color);
        glDrawElements(GL_TRIANGLES, numBlendedGfx, GL_UNSIGNED_SHORT, &gfxPolyListIndex[gfxIndexSizeOpaque]);
        HandleGlError();
    }
    glDisableClientState(GL_COLOR_ARRAY);
    
    //Render the framebuffer now
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glViewport(virtualX, virtualY, virtualWidth, virtualHeight);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fbTextureId);
    glVertexPointer(2, GL_SHORT, 0, &screenVerts);
    glTexCoordPointer(2, GL_FLOAT, 0, &fbTexVerts);
    glColorPointer(4, GL_FLOAT, 0, &pureLight);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glViewport(0, 0, bufferWidth, bufferHeight);
}

void RenderDevice_FlipScreenHRes()
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferId);
    
    glLoadIdentity();
    
    glOrtho(0, orthWidth, 3844.0f, 0.0, 0.0f, 100.0f);
    glViewport(0, 0, bufferWidth, bufferHeight);
    glBindTexture(GL_TEXTURE_2D, gfxTextureID[texPaletteNum]);
    glDisable(GL_BLEND);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glEnableClientState(GL_COLOR_ARRAY);
    
    glVertexPointer(2, GL_SHORT, 12, &gfxPolyList[0].position);
    glTexCoordPointer(2, GL_SHORT, 12, &gfxPolyList[0].texCoord);
    glColorPointer(4, GL_UNSIGNED_BYTE, 12, &gfxPolyList[0].color);
    glDrawElements(GL_TRIANGLES, gfxIndexSizeOpaque, GL_UNSIGNED_SHORT, gfxPolyListIndex);
    
    HandleGlError();
    
    glEnable(GL_BLEND);
    int numBlendedGfx = (int)((gfxIndexSize) - (gfxIndexSizeOpaque));
    glVertexPointer(2, GL_SHORT, 12, &gfxPolyList[0].position);
    glTexCoordPointer(2, GL_SHORT, 12, &gfxPolyList[0].texCoord);
    glColorPointer(4, GL_UNSIGNED_BYTE, 12, &gfxPolyList[0].color);
    glDrawElements(GL_TRIANGLES, numBlendedGfx, GL_UNSIGNED_SHORT, &gfxPolyListIndex[gfxIndexSizeOpaque]);
    
    glDisableClientState(GL_COLOR_ARRAY);
    
    HandleGlError();
    
    //Render the framebuffer now
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glViewport(virtualX, virtualY, virtualWidth, virtualHeight);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, fbTextureId);
    glVertexPointer(2, GL_SHORT, 0, &screenVerts);
    glTexCoordPointer(2, GL_FLOAT, 0, &fbTexVerts);
    glColorPointer(4, GL_FLOAT, 0, &pureLight);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glViewport(0, 0, bufferWidth, bufferHeight);
}
