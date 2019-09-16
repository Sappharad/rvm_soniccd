//
//  RenderDevice.c
//  rvm
//

#include "RenderDevice.h"
#include <OpenGL/gl.h>

const int NUM_TEXTURES = 1; //Was 6
const int TEXTURE_SIZE = 1024*1024*2;
int orthWidth;
int viewWidth;
int viewHeight;
float viewAspect;
int bufferWidth;
int bufferHeight;
int highResMode;
GLuint gfxTextureID[1]; //NUM_TEXTURES

void HandleGlError(){
    GLenum boo = glGetError();
    if(boo != GL_NO_ERROR){
        if (boo == GL_INVALID_OPERATION){
            printf("Invalid operation\n");
        }
        /*else if(boo == GL_INVALID_FRAMEBUFFER_OPERATION){
            boo = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if(boo == GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT){
                printf("Framebuffer attachment missing!");
            }
            else{
                printf("Invalid framebuffer operation\n");
            }
        }*/
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
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);
    
    glLoadIdentity();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GraphicsSystem_SetupPolygonLists();
    printf("Setup textures\n");
    for (int i = 0; i < NUM_TEXTURES; i++)
    {
        glGenTextures(1, &gfxTextureID[i]);
        glBindTexture(GL_TEXTURE_2D, gfxTextureID[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, texBuffer2);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 9728);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 9728);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 33071);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 33071);
    }
    printf("Done with textures\n");
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    //glScalef(0.0009765625f, 0.0009765625f, 1.0f); //1.0 / 1024.0. Allows for texture locations in pixels instead of from 0.0 to 1.0
    glMatrixMode(GL_PROJECTION);
    
    glClear(GL_COLOR_BUFFER_BIT);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glClear(GL_COLOR_BUFFER_BIT);
}
void RenderDevice_UpdateHardwareTextures()
{
    GraphicsSystem_SetActivePalette(0, 0, 240);
    GraphicsSystem_UpdateTextureBufferWithTiles();
    GraphicsSystem_UpdateTextureBufferWithSortedSprites();
    
    glBindTexture(GL_TEXTURE_2D, gfxTextureID[0]);
    HandleGlError();
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024, 1024, 0, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, texBuffer2);
    //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1024, 1024, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, texBuffer);
    HandleGlError();
    
    for (uint8_t b = 1; b < NUM_TEXTURES; b += 1)
    {
        GraphicsSystem_SetActivePalette(b, 0, 240);
        GraphicsSystem_UpdateTextureBufferWithTiles();
        GraphicsSystem_UpdateTextureBufferWithSprites();

        glBindTexture(GL_TEXTURE_2D, gfxTextureID[b]);
        HandleGlError();
        
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 1024, 1024, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, texBuffer2);
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
    if (bufferWidth > 400)
    {
        bufferWidth = 400;
    }
    viewAspect = 0.75f;
    if (viewHeight >= 480.0)
    {
        HQ3DFloorEnabled = true;
    }
    else
    {
        HQ3DFloorEnabled = false;
    }
    if (viewHeight >= 480)
    {
        GraphicsSystem_SetScreenRenderSize(bufferWidth, bufferWidth);
        bufferWidth *= 2;
        bufferHeight = 480;
    }
    else
    {
        bufferHeight = 240;
        GraphicsSystem_SetScreenRenderSize(bufferWidth, bufferWidth);
    }
    orthWidth = SCREEN_XSIZE * 16;
}

void CalcPerspective(float fov, float aspectRatio, float nearPlane, float farPlane){
    GLfloat matrix[16];
    float w = 1.0 / tanf(fov * 0.5f); //Probably correct
    float h = 1.0 / (w*aspectRatio); //Probably correct
    float q = (nearPlane+farPlane)/(farPlane - nearPlane); //Questionable, but probably right
    
    matrix[0] = w;
    matrix[1] = 0; //All 0s are correct
    matrix[2] = 0;
    matrix[3] = 0;
    
    matrix[4] = 0;
    matrix[5] = h;
    matrix[6] = 0;
    matrix[7] = 0;
    
    matrix[8] = 0;
    matrix[9] = 0;
    matrix[10] = q;
    matrix[11] = 1.0; //Correct
    
    matrix[12] = 0;
    matrix[13] = 0;
    matrix[14] = (((farPlane*-2.0f)*nearPlane)/(farPlane-nearPlane)); //Probably correct
    matrix[15] = 0;
    
    glMultMatrixf(matrix);
}

void RenderDevice_FlipScreen()
{
    glLoadIdentity();
    HandleGlError();
    
    glOrtho(0, orthWidth, 3844.0f, 0.0, 0.0f, 100.0f);
    glBindTexture(GL_TEXTURE_2D, gfxTextureID[texPaletteNum]);
    glEnableClientState(GL_COLOR_ARRAY);
    HandleGlError();
    if(render3DEnabled){
        glVertexPointer(2, GL_FLOAT, 20, &gfxPolyList[0].position);
        glTexCoordPointer(2, GL_FLOAT, 20, &gfxPolyList[0].texCoord);
        glColorPointer(4, GL_UNSIGNED_BYTE, 20, &gfxPolyList[0].color);
        glDrawElements(GL_TRIANGLES, gfxIndexSizeOpaque, GL_UNSIGNED_SHORT, gfxPolyListIndex);
        glEnable(GL_BLEND);
        HandleGlError();
        
        //glViewport(viewWidth, 0, viewHeight, bufferWidth);
        glViewport(0, 0, viewWidth, viewHeight);
        glPushMatrix();
        /*glLoadIdentity();
        CalcPerspective();
        glRotatef(-90.0, 0, 0, 0);*/
        //RenderDevice.projection3D = Matrix.CreatePerspectiveFieldOfView(1.83259571f, RenderDevice.viewAspect, 0.1f, 2000f) * Matrix.CreateScale(1f, -1f, 1f) * Matrix.CreateTranslation(0f, -0.045f, 0f);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        CalcPerspective();
        /*glScalef(0.0f, 0.0f, -1.0f);
        glRotatef(floor3DAngle, 180.0, 0, 0);
        glTranslatef(floor3DPos.X, floor3DPos.Y, floor3DPos.Z);*/
        /*glScalef(1.0f, -1.0f, 1.0f);
        glRotatef(0, 3.14159274f * (180.0f + floor3DAngle) / 180.0f, 0, 0);
        glTranslatef(floor3DPos.X, floor3DPos.Y, floor3DPos.Z);
        glVertexPointer(3, GL_FLOAT, 24, &polyList3D[0].position);
        glTexCoordPointer(2, GL_FLOAT, 24, &polyList3D[0].texCoord);
        glColorPointer(4, GL_UNSIGNED_BYTE, 24, &polyList3D[0].color);
        glDrawElements(GL_TRIANGLES, indexSize3D, GL_UNSIGNED_SHORT, gfxPolyListIndex);*/
        glLoadIdentity();
        glMatrixMode(GL_PROJECTION);
        
        glViewport(0, 0, bufferWidth, bufferHeight);
        glPopMatrix();
        HandleGlError();
        
        int numBlendedGfx = (int)(gfxIndexSize - gfxIndexSizeOpaque);
        glVertexPointer(2, GL_FLOAT, 20, &gfxPolyList[0].position);
        glTexCoordPointer(2, GL_FLOAT, 20, &gfxPolyList[0].texCoord);
        glColorPointer(4, GL_UNSIGNED_BYTE, 20, &gfxPolyList[0].color);
        glDrawElements(GL_TRIANGLES, numBlendedGfx, GL_UNSIGNED_SHORT, &gfxPolyListIndex[gfxIndexSizeOpaque]);
        HandleGlError();
    }
    else{
        if(gfxIndexSizeOpaque > 0){
            glVertexPointer(2, GL_FLOAT, 20, &gfxPolyList[0].position);
            glTexCoordPointer(2, GL_FLOAT, 20, &gfxPolyList[0].texCoord);
            glColorPointer(4, GL_UNSIGNED_BYTE, 20, &gfxPolyList[0].color);
            glDrawElements(GL_TRIANGLES, gfxIndexSizeOpaque, GL_UNSIGNED_SHORT, gfxPolyListIndex);
            HandleGlError();
        }
        
        int numBlendedGfx = (int)(gfxIndexSize - gfxIndexSizeOpaque);
        if(numBlendedGfx > 0){
            glEnable(GL_BLEND);
            glEnable(GL_TEXTURE_2D);
            glVertexPointer(2, GL_FLOAT, 20, &gfxPolyList[0].position);
            glTexCoordPointer(2, GL_FLOAT, 20, &gfxPolyList[0].texCoord);
            glColorPointer(4, GL_UNSIGNED_BYTE, 20, &gfxPolyList[0].color);
            glDrawElements(GL_TRIANGLES, numBlendedGfx, GL_UNSIGNED_SHORT, &gfxPolyListIndex[gfxIndexSizeOpaque]);
            HandleGlError();
        }
    }
    
    glDisableClientState(GL_COLOR_ARRAY);
}

void RenderDevice_FlipScreenHRes()
{
    glLoadIdentity();
    
    texPaletteNum = 0;
    
    glOrtho(0, orthWidth, 3844.0f, 0.0, 0.0f, 100.0f);
    glViewport(0, 0, bufferWidth, bufferHeight);
    glBindTexture(GL_TEXTURE_2D, gfxTextureID[texPaletteNum]);
    glDisable(GL_BLEND);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glEnableClientState(GL_COLOR_ARRAY);
    
    glVertexPointer(2, GL_FLOAT, 20, &gfxPolyList[0].position);
    glTexCoordPointer(2, GL_FLOAT, 20, &gfxPolyList[0].texCoord);
    glColorPointer(4, GL_UNSIGNED_BYTE, 20, &gfxPolyList[0].color);
    glDrawElements(GL_TRIANGLES, gfxIndexSizeOpaque, GL_UNSIGNED_SHORT, gfxPolyListIndex);
    
    HandleGlError();
    
    glEnable(GL_BLEND);
    int numBlendedGfx = (int)((gfxIndexSize) - (gfxIndexSizeOpaque));
    glVertexPointer(2, GL_FLOAT, 20, &gfxPolyList[0].position);
    glTexCoordPointer(2, GL_FLOAT, 20, &gfxPolyList[0].texCoord);
    glColorPointer(4, GL_UNSIGNED_BYTE, 20, &gfxPolyList[0].color);
    glDrawElements(GL_TRIANGLES, numBlendedGfx, GL_UNSIGNED_SHORT, &gfxPolyListIndex[gfxIndexSizeOpaque]);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glDisableClientState(GL_COLOR_ARRAY);
    
    HandleGlError();
}
