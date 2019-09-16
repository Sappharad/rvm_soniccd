//
//  GraphicsSystem.h
//  rvm
//

#ifndef GraphicsSystem_h
#define GraphicsSystem_h

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "GfxSurfaceDesc.h"
#include "PaletteEntry.h"
#include "DrawVertex.h"
#include "DrawVertex3D.h"
#include "Quad2D.h"
#include "FileIO.h"
#include "GifLoader.h"
#include "GlobalAppDefinitions.h"
#include "Quad2D.h"

#define NUM_SPRITESHEETS 24
#define GRAPHIC_DATASIZE 0x100000 //0x200000
#define VERTEX_LIMIT 0x2000
#define INDEX_LIMIT 0xC000

extern bool render3DEnabled;
extern unsigned char fadeMode;
extern unsigned char fadeR;
extern unsigned char fadeG;
extern unsigned char fadeB;
extern unsigned char fadeA;
extern unsigned char paletteMode;
extern unsigned char colourMode;
extern unsigned char texBuffer2[0x200000];
extern unsigned char texBufferMode;
extern unsigned char tileGfx[0x40000];
extern unsigned char graphicData[GRAPHIC_DATASIZE];
extern struct GfxSurfaceDesc gfxSurface[NUM_SPRITESHEETS];
extern unsigned int gfxDataPosition;
extern struct DrawVertex gfxPolyList[VERTEX_LIMIT];
//extern struct DrawVertex3D polyList3D[6404];
extern unsigned short gfxPolyListIndex[INDEX_LIMIT];
extern unsigned short gfxVertexSize;
extern unsigned short gfxVertexSizeOpaque;
extern unsigned short gfxIndexSize;
extern unsigned short gfxIndexSizeOpaque;
extern unsigned short vertexSize3D;
extern unsigned short indexSize3D;
extern float tileUVArray[0x1000];
extern struct Vector3 floor3DPos;
extern float floor3DAngle;
extern unsigned short blendLookupTable[0x2000];
extern unsigned short subtractiveLookupTable[0x2000];
extern struct PaletteEntry tilePalette[256];
extern unsigned short tilePalette16_Data[8][256];
extern int texPaletteNum;
extern int waterDrawPos;
extern bool videoPlaying;
extern int currentVideoFrame;

void DumpTexBuffer(void); //TODO: Test function to be deleted later

void Init_GraphicsSystem(void);
void GraphicsSystem_SetScreenRenderSize(int gfxWidth, int gfxPitch);
unsigned short GraphicsSystem_RGB_16BIT5551(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
void GraphicsSystem_LoadPalette(char* fileName, int paletteNum, int destPoint, int startPoint, int endPoint);
unsigned char GraphicsSystem_AddGraphicsFile(char* fileName);
void GraphicsSystem_RemoveGraphicsFile(char* fileName, int surfaceNum);
void GraphicsSystem_ClearGraphicsData(void);
bool GraphicsSystem_CheckSurfaceSize(int size);
void GraphicsSystem_SetupPolygonLists(void);
void GraphicsSystem_UpdateTextureBufferWithTiles(void);
void GraphicsSystem_UpdateTextureBufferWithSortedSprites(void);
void GraphicsSystem_UpdateTextureBufferWithSprites(void);
void GraphicsSystem_LoadBMPFile(char* fileName, int surfaceNum);
void GraphicsSystem_LoadGIFFile(char* fileName, int surfaceNum);
void GraphicsSystem_LoadStageGIFFile(int zNumber);
void GraphicsSystem_Copy16x16Tile(int tDest, int tSource);
void GraphicsSystem_ClearScreen(uint8_t clearColour);
void GraphicsSystem_DrawSprite(int xPos, int yPos, int xSize, int ySize, int xBegin, int yBegin, int surfaceNum);
void GraphicsSystem_DrawSpriteFlipped(int xPos, int yPos, int xSize, int ySize, int xBegin, int yBegin, int direction, int surfaceNum);
void GraphicsSystem_DrawBlendedSprite(int xPos, int yPos, int xSize, int ySize, int xBegin, int yBegin, int surfaceNum);
void GraphicsSystem_DrawAlphaBlendedSprite(int xPos, int yPos, int xSize, int ySize, int xBegin, int yBegin, int alpha, int surfaceNum);
void GraphicsSystem_DrawAdditiveBlendedSprite(int xPos, int yPos, int xSize, int ySize, int xBegin, int yBegin, int alpha, int surfaceNum);
void GraphicsSystem_DrawSubtractiveBlendedSprite(int xPos, int yPos, int xSize, int ySize, int xBegin, int yBegin, int alpha, int surfaceNum);
void GraphicsSystem_DrawRectangle(int xPos, int yPos, int xSize, int ySize, int r, int g, int b, int alpha);
void GraphicsSystem_DrawTintRectangle(int xPos, int yPos, int xSize, int ySize);
void GraphicsSystem_DrawTintSpriteMask(int xPos, int yPos, int xSize, int ySize, int xBegin, int yBegin, int tableNo, int surfaceNum);
void GraphicsSystem_DrawScaledTintMask(uint8_t direction, int xPos, int yPos, int xPivot, int yPivot, int xScale, int yScale, int xSize, int ySize, int xBegin, int yBegin, int surfaceNum);
void GraphicsSystem_DrawScaledSprite(uint8_t direction, int xPos, int yPos, int xPivot, int yPivot, int xScale, int yScale, int xSize, int ySize, int xBegin, int yBegin, int surfaceNum);
void GraphicsSystem_DrawScaledChar(uint8_t direction, int xPos, int yPos, int xPivot, int yPivot, int xScale, int yScale, int xSize, int ySize, int xBegin, int yBegin, int surfaceNum);
void GraphicsSystem_DrawRotatedSprite(uint8_t direction, int xPos, int yPos, int xPivot, int yPivot, int xBegin, int yBegin, int xSize, int ySize, int rotAngle, int surfaceNum);
void GraphicsSystem_DrawRotoZoomSprite(uint8_t direction, int xPos, int yPos, int xPivot, int yPivot, int xBegin, int yBegin, int xSize, int ySize, int rotAngle, int scale, int surfaceNum);
void GraphicsSystem_DrawQuad(struct Quad2D* face, int rgbVal);
void GraphicsSystem_DrawTexturedQuad(struct Quad2D* face, int surfaceNum);
void GraphicsSystem_SetPaletteEntry(uint8_t entryPos, uint8_t cR, uint8_t cG, uint8_t cB);
void GraphicsSystem_SetFade(uint8_t clrR, uint8_t clrG, uint8_t clrB, unsigned short clrA);
void GraphicsSystem_SetLimitedFade(uint8_t paletteNum, uint8_t clrR, uint8_t clrG, uint8_t clrB, unsigned short clrA, int fStart, int fEnd);
void GraphicsSystem_SetActivePalette(uint8_t paletteNum, int minY, int maxY);
void GraphicsSystem_CopyPalette(uint8_t paletteSource, uint8_t paletteDest);
void GraphicsSystem_RotatePalette(uint8_t pStart, uint8_t pEnd, uint8_t pDirection);
void GraphicsSystem_GenerateBlendLookupTable(void);

#endif /* GraphicsSystem_h */
