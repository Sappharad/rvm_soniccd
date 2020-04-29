//
//  GraphicsSystem.c
//  rvm
//

#include "GraphicsSystem.h"
#include <stdio.h>

bool render3DEnabled;
unsigned char fadeMode;
unsigned char fadeR;
unsigned char fadeG;
unsigned char fadeB;
unsigned char fadeA;
unsigned char paletteMode;
unsigned char colourMode;
unsigned short texBuffer[0x100000];
unsigned char texBufferMode;
unsigned char tileGfx[0x40000];
unsigned char graphicData[GRAPHIC_DATASIZE];
struct GfxSurfaceDesc gfxSurface[NUM_SPRITESHEETS];
uint gfxDataPosition;
struct DrawVertex gfxPolyList[VERTEX_LIMIT];
struct DrawVertex3D polyList3D[6404];
unsigned short gfxPolyListIndex[INDEX_LIMIT];
unsigned short gfxVertexSize;
unsigned short gfxVertexSizeOpaque;
unsigned short gfxIndexSize;
unsigned short gfxIndexSizeOpaque;
unsigned short vertexSize3D;
unsigned short indexSize3D;
float tileUVArray[0x1000];
struct Vector3 floor3DPos;
float floor3DAngle;
unsigned short blendLookupTable[0x2000];
unsigned short subtractiveLookupTable[0x2000];
struct PaletteEntry tilePalette[256];
unsigned short tilePalette16_Data[8][256];
int texPaletteNum;
int waterDrawPos;
bool videoPlaying;
int currentVideoFrame;

//Dump the texture buffer in pallete 0 for testing purposes.
void DumpTexBuffer(){
    GraphicsSystem_SetActivePalette(0, 0, 240);
    GraphicsSystem_UpdateTextureBufferWithTiles();
    GraphicsSystem_UpdateTextureBufferWithSortedSprites();
    
    FILE* texFile = fopen("texDump.bin","w");
    if(texFile != NULL){
        for (int i=0; i<0x100000; i++) {
            fputc(texBuffer[i] & 0xFF, texFile);
            fputc(texBuffer[i] >> 8 & 0xFF, texFile);
        }
        
        fclose(texFile);
    }
}

void Init_GraphicsSystem()
{
    render3DEnabled = false;
    fadeMode = 0;
    fadeR = 0;
    fadeG = 0;
    fadeB = 0;
    fadeA = 0;
    paletteMode = 0;
    colourMode = 0;
    texBufferMode = 0;
    gfxVertexSize = 0;
    gfxVertexSizeOpaque = 0;
    gfxIndexSize = 0;
    gfxIndexSizeOpaque = 0;
    vertexSize3D = 0;
    indexSize3D = 0;
    texPaletteNum = 0;
    waterDrawPos = 320;
    videoPlaying = false;
}
void GraphicsSystem_SetScreenRenderSize(int gfxWidth, int gfxPitch)
{
    SCREEN_XSIZE = gfxWidth;
    SCREEN_CENTER = SCREEN_XSIZE / 2;
    SCREEN_SCROLL_LEFT = SCREEN_CENTER - 8;
    SCREEN_SCROLL_RIGHT = SCREEN_CENTER + 8;
    OBJECT_BORDER_X1 = 128;
    OBJECT_BORDER_X2 = SCREEN_XSIZE + 128;
}
unsigned short GraphicsSystem_RGB_16BIT5551(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return (unsigned short)((a & 1) + (r >> 3 << 11) + (g >> 3 << 6) + (b >> 3 << 1));
}
void GraphicsSystem_LoadPalette(char* fileName, int paletteNum, int destPoint, int startPoint, int endPoint)
{
    char filePath[64];
    char array[] = "Data/Palettes/";
    FileIO_StrCopy(filePath, sizeof(filePath), array, sizeof(array));
    FileIO_StrAdd(filePath, sizeof(filePath), fileName, (int)strlen(fileName));
    struct FileData fData;
    unsigned char array2[3];
    if (FileIO_LoadFile(filePath, &fData))
    {
        FileIO_SetFilePosition((uint)(startPoint * 3));
        if (paletteNum < 0 || paletteNum > 7)
        {
            paletteNum = 0;
        }
        if (paletteNum == 0)
        {
            for (int i = startPoint; i < endPoint; i++)
            {
                FileIO_ReadByteArray(array2, 3);
                tilePalette16_Data[0][destPoint] = GraphicsSystem_RGB_16BIT5551(array2[0], array2[1], array2[2], 1);
                tilePalette[destPoint].red = array2[0];
                tilePalette[destPoint].green = array2[1];
                tilePalette[destPoint].blue = array2[2];
                destPoint++;
            }
            tilePalette16_Data[0][0] = GraphicsSystem_RGB_16BIT5551(array2[0], array2[1], array2[2], 0);
        }
        else
        {
            for (int i = startPoint; i < endPoint; i++)
            {
                FileIO_ReadByteArray(array2, 3);
                tilePalette16_Data[paletteNum][destPoint] = GraphicsSystem_RGB_16BIT5551(array2[0], array2[1], array2[2], 1);
                destPoint++;
            }
            tilePalette16_Data[paletteNum][0] = GraphicsSystem_RGB_16BIT5551(array2[0], array2[1], array2[2], 0);
        }
        FileIO_CloseFile();
    }
}
unsigned char GraphicsSystem_AddGraphicsFile(char* fileName)
{
    unsigned char b = 0;
    char array[64];
    char array2[] = "Data/Sprites/";
    FileIO_StrCopy(array, sizeof(array), array2, sizeof(array2));
    FileIO_StrAdd(array, sizeof(array), fileName, (int)strlen(fileName));
#if DEBUG
    printf("Add graphics file: %s\n", fileName);
#endif
    while (b < 24)
    {
        if (FileIO_StringLength(gfxSurface[(int)b].fileName, sizeof(gfxSurface[(int)b].fileName)) <= 0)
        {
            int num = FileIO_StringLength(array, sizeof(array)) - 1;
            char c = array[num];
            if (c != 'f')
            {
                if (c == 'p')
                {
                    GraphicsSystem_LoadBMPFile(array, (int)b);
                }
            }
            else
            {
                GraphicsSystem_LoadGIFFile(array, (int)b);
            }
            return b;
        }
        if (FileIO_StringComp(gfxSurface[(int)b].fileName, array))
        {
            return b;
        }
        b += 1;
    }
    return 0;
}
void GraphicsSystem_RemoveGraphicsFile(char* fileName, int surfaceNum)
{
    unsigned int num;
    if (surfaceNum < 0)
    {
        for (num = 0u; num < 24u; num += 1u)
        {
            if (strlen(gfxSurface[num].fileName) > 0 && FileIO_StringComp(gfxSurface[num].fileName, fileName))
            {
                surfaceNum = (int)num;
            }
        }
    }
    if (surfaceNum < 0)
    {
        return;
    }
    if (strlen(gfxSurface[surfaceNum].fileName) == 0)
    {
        return;
    }
    FileIO_StrClear(gfxSurface[surfaceNum].fileName, (int)strlen(gfxSurface[surfaceNum].fileName));
    num = gfxSurface[surfaceNum].dataStart;
    uint num2 = (uint)((unsigned long)gfxSurface[surfaceNum].dataStart + (unsigned long)((long)(gfxSurface[surfaceNum].width * gfxSurface[surfaceNum].height)));
    for (unsigned int num3 = GRAPHIC_DATASIZE - num2; num3 > 0; num3 -= 1)
    {
        graphicData[num] = graphicData[num2];
        num += 1u;
        num2 += 1u;
    }
    gfxDataPosition -= (uint)(gfxSurface[surfaceNum].width * gfxSurface[surfaceNum].height);
    for (num = 0u; num < NUM_SPRITESHEETS; num += 1u)
    {
        if (gfxSurface[num].dataStart > gfxSurface[surfaceNum].dataStart)
        {
            gfxSurface[num].dataStart -= (uint)(gfxSurface[surfaceNum].width * gfxSurface[surfaceNum].height);
        }
    }
}
void GraphicsSystem_ClearGraphicsData()
{
    for (int i = 0; i < 24; i++)
    {
        FileIO_StrClear(gfxSurface[i].fileName, sizeof(gfxSurface[i].fileName));
    }
    gfxDataPosition = 0u;
}
bool GraphicsSystem_CheckSurfaceSize(int size)
{
    for (int i = 2; i < 2048; i <<= 1)
    {
        if (i == size)
        {
            return true;
        }
    }
    return false;
}
void GraphicsSystem_SetupPolygonLists()
{
    int num = 0;
    for (int i = 0; i < VERTEX_LIMIT; i++)
    {
        gfxPolyListIndex[num] = (i << 2);
        num++;
        gfxPolyListIndex[num] = ((i << 2) + 1);
        num++;
        gfxPolyListIndex[num] = ((i << 2) + 2);
        num++;
        gfxPolyListIndex[num] = ((i << 2) + 1);
        num++;
        gfxPolyListIndex[num] = ((i << 2) + 3);
        num++;
        gfxPolyListIndex[num] = ((i << 2) + 2);
        num++;
    }
    for (int i = 0; i < VERTEX_LIMIT; i++)
    {
        gfxPolyList[i].color.R = 0xFF;
        gfxPolyList[i].color.G = 0xFF;
        gfxPolyList[i].color.B = 0xFF;
        gfxPolyList[i].color.A = 0xFF;
    }
    for (int i = 0; i < 6404; i++)
    {
        polyList3D[i].color.R = 0xFF;
        polyList3D[i].color.G = 0xFF;
        polyList3D[i].color.B = 0xFF;
        polyList3D[i].color.A = 0xFF;
    }
}
void GraphicsSystem_UpdateTextureBufferWithTiles()
{
    int num = 0;
    int num3;
    if (texBufferMode == 0)
    {
        for (int i = 0; i < 512; i += 16)
        {
            for (int j = 0; j < 512; j += 16)
            {
                int num2 = num << 8;
                num++;
                num3 = j + (i << 10);
                for (int k = 0; k < 16; k++)
                {
                    for (int l = 0; l < 16; l++)
                    {
                        if (tileGfx[num2] > 0)
                        {
                            texBuffer[num3] = tilePalette16_Data[texPaletteNum][tileGfx[num2]];
                        }
                        else
                        {
                            texBuffer[num3] = 0;
                        }
                        num3++;
                        num2++;
                    }
                    num3 += 1008;
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < 504; i += 18)
        {
            for (int j = 0; j < 504; j += 18)
            {
                int num2 = num << 8;
                num++;
                if (num == 783)
                {
                    num = 1023;
                }
                num3 = j + (i << 10);
                if (tileGfx[num2] > 0)
                {
                    texBuffer[num3] = tilePalette16_Data[texPaletteNum][tileGfx[num2]];
                }
                else
                {
                    texBuffer[num3] = 0;
                }
                num3++;
                for (int l = 0; l < 15; l++)
                {
                    if (tileGfx[num2] > 0)
                    {
                        texBuffer[num3] = tilePalette16_Data[texPaletteNum][tileGfx[num2]];
                    }
                    else
                    {
                        texBuffer[num3] = 0;
                    }
                    num3++;
                    num2++;
                }
                if (tileGfx[num2] > 0)
                {
                    texBuffer[num3] = tilePalette16_Data[texPaletteNum][tileGfx[num2]];
                    num3++;
                    texBuffer[num3] = tilePalette16_Data[texPaletteNum][tileGfx[num2]];
                }
                else
                {
                    texBuffer[num3] = 0;
                    num3++;
                    texBuffer[num3] = 0;
                }
                num3++;
                num2 -= 15;
                num3 += 1006;
                for (int k = 0; k < 16; k++)
                {
                    if (tileGfx[num2] > 0)
                    {
                        texBuffer[num3] = tilePalette16_Data[texPaletteNum][tileGfx[num2]];
                    }
                    else
                    {
                        texBuffer[num3] = 0;
                    }
                    num3++;
                    for (int l = 0; l < 15; l++)
                    {
                        if (tileGfx[num2] > 0)
                        {
                            texBuffer[num3] = tilePalette16_Data[texPaletteNum][tileGfx[num2]];
                        }
                        else
                        {
                            texBuffer[num3] = 0;
                        }
                        num3++;
                        num2++;
                    }
                    if (tileGfx[num2] > 0)
                    {
                        texBuffer[num3] = tilePalette16_Data[texPaletteNum][tileGfx[num2]];
                        num3++;
                        texBuffer[num3] = tilePalette16_Data[texPaletteNum][tileGfx[num2]];
                    }
                    else
                    {
                        texBuffer[num3] = 0;
                        num3++;
                        texBuffer[num3] = 0;
                    }
                    num3++;
                    num2++;
                    num3 += 1006;
                }
                num2 -= 16;
                if (tileGfx[num2] > 0)
                {
                    texBuffer[num3] = tilePalette16_Data[texPaletteNum][tileGfx[num2]];
                }
                else
                {
                    texBuffer[num3] = 0;
                }
                num3++;
                for (int l = 0; l < 15; l++)
                {
                    if (tileGfx[num2] > 0)
                    {
                        texBuffer[num3] = tilePalette16_Data[texPaletteNum][tileGfx[num2]];
                    }
                    else
                    {
                        texBuffer[num3] = 0;
                    }
                    num3++;
                    num2++;
                }
                if (tileGfx[num2] > 0)
                {
                    texBuffer[num3] = tilePalette16_Data[texPaletteNum][tileGfx[num2]];
                    num3++;
                    texBuffer[num3] = tilePalette16_Data[texPaletteNum][tileGfx[num2]];
                }
                else
                {
                    texBuffer[num3] = 0;
                    num3++;
                    texBuffer[num3] = 0;
                }
                num3++;
                num3 += 1006;
            }
        }
    }
    num3 = 0;
    for (int k = 0; k < 16; k++)
    {
        for (int l = 0; l < 16; l++)
        {
            texBuffer[num3] = GraphicsSystem_RGB_16BIT5551(255, 255, 255, 1);
            num3++;
        }
        num3 += 1008;
    }
}
void GraphicsSystem_UpdateTextureBufferWithSortedSprites()
{
    uint8_t b = 0;
    uint8_t array[NUM_SPRITESHEETS];
    bool flag = true;
    for (int i = 0; i < NUM_SPRITESHEETS; i++)
    {
        gfxSurface[i].texStartX = -1;
    }
    for (int i = 0; i < 24; i++)
    {
        int j = 0;
        signed char b2 = -1;
        for (int k = 0; k < NUM_SPRITESHEETS; k++)
        {
            if (strlen(gfxSurface[k].fileName) > 0 && gfxSurface[k].texStartX == -1)
            {
                if (GraphicsSystem_CheckSurfaceSize(gfxSurface[k].width) && GraphicsSystem_CheckSurfaceSize(gfxSurface[k].height))
                {
                    if (gfxSurface[k].width + gfxSurface[k].height > j)
                    {
                        j = gfxSurface[k].width + gfxSurface[k].height;
                        b2 = (signed char)k;
                    }
                }
                else
                {
                    gfxSurface[k].texStartX = 0;
                }
            }
        }
        if (b2 == -1)
        {
            i = 24;
        }
        else
        {
            gfxSurface[(int)b2].texStartX = 0;
            array[(int)b] = (uint8_t)b2;
            b += 1;
        }
    }
    for (int i = 0; i < NUM_SPRITESHEETS; i++)
    {
        gfxSurface[i].texStartX = -1;
    }
    for (int i = 0; i < (int)b; i++)
    {
        signed char b2 = (signed char)array[i];
        gfxSurface[(int)b2].texStartX = 0;
        gfxSurface[(int)b2].texStartY = 0;
        int j = 0;
        while (j == 0)
        {
            j = 1;
            if (gfxSurface[(int)b2].height == 1024)
            {
                flag = false;
            }
            if (flag)
            {
                if (gfxSurface[(int)b2].texStartX < 512 && gfxSurface[(int)b2].texStartY < 512)
                {
                    j = 0;
                    gfxSurface[(int)b2].texStartX += gfxSurface[(int)b2].width;
                    if (gfxSurface[(int)b2].texStartX + gfxSurface[(int)b2].width > 1024)
                    {
                        gfxSurface[(int)b2].texStartX = 0;
                        gfxSurface[(int)b2].texStartY += gfxSurface[(int)b2].height;
                    }
                }
                else
                {
                    for (int k = 0; k < 24; k++)
                    {
                        if (gfxSurface[k].texStartX > -1 && k != (int)b2 && gfxSurface[(int)b2].texStartX < gfxSurface[k].texStartX + gfxSurface[k].width && gfxSurface[(int)b2].texStartX >= gfxSurface[k].texStartX && gfxSurface[(int)b2].texStartY < gfxSurface[k].texStartY + gfxSurface[k].height)
                        {
                            j = 0;
                            gfxSurface[(int)b2].texStartX += gfxSurface[(int)b2].width;
                            if (gfxSurface[(int)b2].texStartX + gfxSurface[(int)b2].width > 1024)
                            {
                                gfxSurface[(int)b2].texStartX = 0;
                                gfxSurface[(int)b2].texStartY += gfxSurface[(int)b2].height;
                            }
                            k = 24;
                        }
                    }
                }
            }
            else
            {
                if (gfxSurface[(int)b2].width < 1024)
                {
                    if (gfxSurface[(int)b2].texStartX < 16 && gfxSurface[(int)b2].texStartY < 16)
                    {
                        j = 0;
                        gfxSurface[(int)b2].texStartX += gfxSurface[(int)b2].width;
                        if (gfxSurface[(int)b2].texStartX + gfxSurface[(int)b2].width > 1024)
                        {
                            gfxSurface[(int)b2].texStartX = 0;
                            gfxSurface[(int)b2].texStartY += gfxSurface[(int)b2].height;
                        }
                    }
                    else
                    {
                        for (int k = 0; k < 24; k++)
                        {
                            if (gfxSurface[k].texStartX > -1 && k != (int)b2 && gfxSurface[(int)b2].texStartX < gfxSurface[k].texStartX + gfxSurface[k].width && gfxSurface[(int)b2].texStartX >= gfxSurface[k].texStartX && gfxSurface[(int)b2].texStartY < gfxSurface[k].texStartY + gfxSurface[k].height)
                            {
                                j = 0;
                                gfxSurface[(int)b2].texStartX += gfxSurface[(int)b2].width;
                                if (gfxSurface[(int)b2].texStartX + gfxSurface[(int)b2].width > 1024)
                                {
                                    gfxSurface[(int)b2].texStartX = 0;
                                    gfxSurface[(int)b2].texStartY += gfxSurface[(int)b2].height;
                                }
                                k = 24;
                            }
                        }
                    }
                }
            }
        }
        if (gfxSurface[(int)b2].texStartY + gfxSurface[(int)b2].height <= 1024)
        {
            int num = (int)gfxSurface[(int)b2].dataStart;
            int num2 = gfxSurface[(int)b2].texStartX + (gfxSurface[(int)b2].texStartY << 10);
            for (j = 0; j < gfxSurface[(int)b2].height; j++)
            {
                for (int k = 0; k < gfxSurface[(int)b2].width; k++)
                {
                    if (graphicData[num] > 0)
                    {
                        texBuffer[num2] = tilePalette16_Data[texPaletteNum][graphicData[num]];
                    }
                    else
                    {
                        texBuffer[num2] = 0;
                    }
                    num2++;
                    num++;
                }
                num2 += 1024 - gfxSurface[(int)b2].width;
            }
        }
    }
}
void GraphicsSystem_UpdateTextureBufferWithSprites()
{
    for (int i = 0; i < 24; i++)
    {
        if (gfxSurface[i].texStartY + gfxSurface[i].height <= 1024 && gfxSurface[i].texStartX > -1)
        {
            int num = (int)gfxSurface[i].dataStart;
            int num2 = gfxSurface[i].texStartX + (gfxSurface[i].texStartY << 10);
            for (int j = 0; j < gfxSurface[i].height; j++)
            {
                for (int k = 0; k < gfxSurface[i].width; k++)
                {
                    if (graphicData[num] > 0)
                    {
                        texBuffer[num2] = tilePalette16_Data[texPaletteNum][graphicData[num]];
                    }
                    else
                    {
                        texBuffer[num2] = 0;
                    }
                    num2++;
                    num++;
                }
                num2 += 1024 - gfxSurface[i].width;
            }
        }
    }
}
void GraphicsSystem_LoadBMPFile(char* fileName, int surfaceNum)
{
    struct FileData fileData;
    if (FileIO_LoadFile(fileName, &fileData))
    {
        FileIO_StrCopy(gfxSurface[surfaceNum].fileName, sizeof(gfxSurface[surfaceNum].fileName), fileName, (int)strlen(fileName));
        FileIO_SetFilePosition(18u);
        uint8_t b = FileIO_ReadByte();
        gfxSurface[surfaceNum].width = (int)b;
        b = FileIO_ReadByte();
        gfxSurface[surfaceNum].width += (int)b << 8;
        b = FileIO_ReadByte();
        gfxSurface[surfaceNum].width += (int)b << 16;
        b = FileIO_ReadByte();
        gfxSurface[surfaceNum].width += (int)b << 24;
        b = FileIO_ReadByte();
        gfxSurface[surfaceNum].height = (int)b;
        b = FileIO_ReadByte();
        gfxSurface[surfaceNum].height += (int)b << 8;
        b = FileIO_ReadByte();
        gfxSurface[surfaceNum].height += (int)b << 16;
        b = FileIO_ReadByte();
        gfxSurface[surfaceNum].height += (int)b << 24;
        FileIO_SetFilePosition((uint)(fileData.fileSize - (gfxSurface[surfaceNum].width * gfxSurface[surfaceNum].height)));
        gfxSurface[surfaceNum].dataStart = gfxDataPosition;
        int num = (int)gfxSurface[surfaceNum].dataStart;
        num += gfxSurface[surfaceNum].width * (gfxSurface[surfaceNum].height - 1);
        for (int i = 0; i < gfxSurface[surfaceNum].height; i++)
        {
            for (int j = 0; j < gfxSurface[surfaceNum].width; j++)
            {
                b = FileIO_ReadByte();
                graphicData[num] = b;
                num++;
            }
            num -= gfxSurface[surfaceNum].width << 1;
        }
        gfxDataPosition += (uint)(gfxSurface[surfaceNum].width * gfxSurface[surfaceNum].height);
        if (gfxDataPosition >= 0x400000)
        {
            gfxDataPosition = 0u;
        }
        FileIO_CloseFile();
    }
}
void GraphicsSystem_LoadGIFFile(char* fileName, int surfaceNum)
{
    struct FileData fData;
    uint8_t array[3];
    bool interlaced = false;
    if (FileIO_LoadFile(fileName, &fData))
    {
        FileIO_StrCopy(gfxSurface[surfaceNum].fileName, sizeof(gfxSurface[surfaceNum].fileName), fileName, (int)strlen(fileName));
        FileIO_SetFilePosition(6u);
        array[0] = FileIO_ReadByte();
        int num = (int)array[0];
        array[0] = FileIO_ReadByte();
        num += (int)array[0] << 8;
        array[0] = FileIO_ReadByte();
        int num2 = (int)array[0];
        array[0] = FileIO_ReadByte();
        num2 += (int)array[0] << 8;
        array[0] = FileIO_ReadByte();
        array[0] = FileIO_ReadByte();
        array[0] = FileIO_ReadByte();
        for (int i = 0; i < 256; i++)
        {
            FileIO_ReadByteArray(array, 3);
        }
        array[0] = FileIO_ReadByte();
        while (array[0] != 44)
        {
            array[0] = FileIO_ReadByte();
        }
        if (array[0] == 44)
        {
            FileIO_ReadByteArray(array, 2);
            FileIO_ReadByteArray(array, 2);
            FileIO_ReadByteArray(array, 2);
            FileIO_ReadByteArray(array, 2);
            array[0] = FileIO_ReadByte();
            if ((array[0] & 64) >> 6 == 1)
            {
                interlaced = true;
            }
            if ((array[0] & 128) >> 7 == 1)
            {
                for (int i = 128; i < 256; i++)
                {
                    FileIO_ReadByteArray(array, 3);
                }
            }
            gfxSurface[surfaceNum].width = num;
            gfxSurface[surfaceNum].height = num2;
            gfxSurface[surfaceNum].dataStart = gfxDataPosition;
            gfxDataPosition += (uint)(gfxSurface[surfaceNum].width * gfxSurface[surfaceNum].height);
            if (gfxDataPosition >= 0x400000)
            {
                gfxDataPosition = 0u;
            }
            else
            {
                GifLoader_ReadGifPictureData(num, num2, interlaced, graphicData, (int)gfxSurface[surfaceNum].dataStart);
            }
        }
        FileIO_CloseFile();
    }
}
void GraphicsSystem_LoadStageGIFFile(int zNumber)
{
    struct FileData fData;
    uint8_t array[3];
    bool interlaced = false;
    if (FileIO_LoadStageFile("16x16Tiles.gif", zNumber, &fData))
    {
        FileIO_SetFilePosition(6u);
        array[0] = FileIO_ReadByte();
        int num = (int)array[0];
        array[0] = FileIO_ReadByte();
        num += (int)array[0] << 8;
        array[0] = FileIO_ReadByte();
        int num2 = (int)array[0];
        array[0] = FileIO_ReadByte();
        num2 += (int)array[0] << 8;
        array[0] = FileIO_ReadByte();
        array[0] = FileIO_ReadByte();
        array[0] = FileIO_ReadByte();
        for (int i = 128; i < 256; i++)
        {
            FileIO_ReadByteArray(array, 3);
        }
        for (int i = 128; i < 256; i++)
        {
            FileIO_ReadByteArray(array, 3);
            tilePalette[i].red = array[0];
            tilePalette[i].green = array[1];
            tilePalette[i].blue = array[2];
            tilePalette16_Data[texPaletteNum][i] = GraphicsSystem_RGB_16BIT5551(array[0], array[1], array[2], 1);
        }
        array[0] = FileIO_ReadByte();
        if (array[0] == 44)
        {
            FileIO_ReadByteArray(array, 2);
            FileIO_ReadByteArray(array, 2);
            FileIO_ReadByteArray(array, 2);
            FileIO_ReadByteArray(array, 2);
            array[0] = FileIO_ReadByte();
            if ((array[0] & 64) >> 6 == 1)
            {
                interlaced = true;
            }
            if ((array[0] & 128) >> 7 == 1)
            {
                for (int i = 128; i < 256; i++)
                {
                    FileIO_ReadByteArray(array, 3);
                }
            }
            GifLoader_ReadGifPictureData(num, num2, interlaced, tileGfx, 0);
            array[0] = tileGfx[0];
            for (int i = 0; i < 0x40000; i++)
            {
                if (tileGfx[i] == array[0])
                {
                    tileGfx[i] = 0;
                }
            }
        }
        FileIO_CloseFile();
    }
}
void GraphicsSystem_Copy16x16Tile(int tDest, int tSource)
{
    tSource <<= 2;
    tDest <<= 2;
    tileUVArray[tDest] = tileUVArray[tSource];
    tileUVArray[tDest + 1] = tileUVArray[tSource + 1];
    tileUVArray[tDest + 2] = tileUVArray[tSource + 2];
    tileUVArray[tDest + 3] = tileUVArray[tSource + 3];
}
void GraphicsSystem_ClearScreen(uint8_t clearColour)
{
    gfxPolyList[(int)gfxVertexSize].position.X = 0.0f;
    gfxPolyList[(int)gfxVertexSize].position.Y = 0.0f;
    gfxPolyList[(int)gfxVertexSize].color.R = tilePalette[(int)clearColour].red;
    gfxPolyList[(int)gfxVertexSize].color.G = tilePalette[(int)clearColour].green;
    gfxPolyList[(int)gfxVertexSize].color.B = tilePalette[(int)clearColour].blue;
    gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
    gfxPolyList[(int)gfxVertexSize].texCoord.X = 0.0f;
    gfxPolyList[(int)gfxVertexSize].texCoord.Y = 0.0f;
    gfxVertexSize += 1;
    gfxPolyList[(int)gfxVertexSize].position.X = (float)(SCREEN_XSIZE << 4);
    gfxPolyList[(int)gfxVertexSize].position.Y = 0.0f;
    gfxPolyList[(int)gfxVertexSize].color.R = tilePalette[(int)clearColour].red;
    gfxPolyList[(int)gfxVertexSize].color.G = tilePalette[(int)clearColour].green;
    gfxPolyList[(int)gfxVertexSize].color.B = tilePalette[(int)clearColour].blue;
    gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
    gfxPolyList[(int)gfxVertexSize].texCoord.X = 0.0f;
    gfxPolyList[(int)gfxVertexSize].texCoord.Y = 0.0f;
    gfxVertexSize += 1;
    gfxPolyList[(int)gfxVertexSize].position.X = 0.0f;
    gfxPolyList[(int)gfxVertexSize].position.Y = 3840.0f;
    gfxPolyList[(int)gfxVertexSize].color.R = tilePalette[(int)clearColour].red;
    gfxPolyList[(int)gfxVertexSize].color.G = tilePalette[(int)clearColour].green;
    gfxPolyList[(int)gfxVertexSize].color.B = tilePalette[(int)clearColour].blue;
    gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
    gfxPolyList[(int)gfxVertexSize].texCoord.X = 0.0f;
    gfxPolyList[(int)gfxVertexSize].texCoord.Y = 0.0f;
    gfxVertexSize += 1;
    gfxPolyList[(int)gfxVertexSize].position.X = (float)(SCREEN_XSIZE << 4);
    gfxPolyList[(int)gfxVertexSize].position.Y = 3840.0f;
    gfxPolyList[(int)gfxVertexSize].color.R = tilePalette[(int)clearColour].red;
    gfxPolyList[(int)gfxVertexSize].color.G = tilePalette[(int)clearColour].green;
    gfxPolyList[(int)gfxVertexSize].color.B = tilePalette[(int)clearColour].blue;
    gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
    gfxPolyList[(int)gfxVertexSize].texCoord.X = 0.0f;
    gfxPolyList[(int)gfxVertexSize].texCoord.Y = 0.0f;
    gfxVertexSize += 1;
    gfxIndexSize += 6;
}
void GraphicsSystem_DrawSprite(int xPos, int yPos, int xSize, int ySize, int xBegin, int yBegin, int surfaceNum)
{
    if (gfxSurface[surfaceNum].texStartX > -1 && gfxVertexSize < VERTEX_LIMIT && xPos > -512 && xPos < 872 && yPos > -512 && yPos < 752)
    {
        gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos << 4);
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos << 4);
        gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + xBegin) * 0.0009765625f;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + yBegin) * 0.0009765625f;
        gfxVertexSize += 1;
        gfxPolyList[(int)gfxVertexSize].position.X = (float)((xPos + xSize) << 4);
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos << 4);
        gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + xBegin + xSize) * 0.0009765625f;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = gfxPolyList[(int)(gfxVertexSize - 1)].texCoord.Y;
        gfxVertexSize += 1;
        gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos << 4);
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)((yPos + ySize) << 4);
        gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = gfxPolyList[(int)(gfxVertexSize - 2)].texCoord.X;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + yBegin + ySize) * 0.0009765625f;
        gfxVertexSize += 1;
        gfxPolyList[(int)gfxVertexSize].position.X = gfxPolyList[(int)(gfxVertexSize - 2)].position.X;
        gfxPolyList[(int)gfxVertexSize].position.Y = gfxPolyList[(int)(gfxVertexSize - 1)].position.Y;
        gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = gfxPolyList[(int)(gfxVertexSize - 2)].texCoord.X;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = gfxPolyList[(int)(gfxVertexSize - 1)].texCoord.Y;
        gfxVertexSize += 1;
        gfxIndexSize += 6;
    }
}
void GraphicsSystem_DrawSpriteFlipped(int xPos, int yPos, int xSize, int ySize, int xBegin, int yBegin, int direction, int surfaceNum)
{
    if (gfxSurface[surfaceNum].texStartX > -1 && gfxVertexSize < VERTEX_LIMIT && xPos > -512 && xPos < 872 && yPos > -512 && yPos < 752)
    {
        switch (direction)
        {
            case 0:
                gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos << 4);
                gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos << 4);
                gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
                gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + xBegin) * 0.0009765625f;
                gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + yBegin) * 0.0009765625f;
                gfxVertexSize += 1;
                gfxPolyList[(int)gfxVertexSize].position.X = (float)((xPos + xSize) << 4);
                gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos << 4);
                gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
                gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + xBegin + xSize) * 0.0009765625f;
                gfxPolyList[(int)gfxVertexSize].texCoord.Y = gfxPolyList[(int)(gfxVertexSize - 1)].texCoord.Y;
                gfxVertexSize += 1;
                gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos << 4);
                gfxPolyList[(int)gfxVertexSize].position.Y = (float)((yPos + ySize) << 4);
                gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
                gfxPolyList[(int)gfxVertexSize].texCoord.X = gfxPolyList[(int)(gfxVertexSize - 2)].texCoord.X;
                gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + yBegin + ySize) * 0.0009765625f;
                gfxVertexSize += 1;
                gfxPolyList[(int)gfxVertexSize].position.X = gfxPolyList[(int)(gfxVertexSize - 2)].position.X;
                gfxPolyList[(int)gfxVertexSize].position.Y = gfxPolyList[(int)(gfxVertexSize - 1)].position.Y;
                gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
                gfxPolyList[(int)gfxVertexSize].texCoord.X = gfxPolyList[(int)(gfxVertexSize - 2)].texCoord.X;
                gfxPolyList[(int)gfxVertexSize].texCoord.Y = gfxPolyList[(int)(gfxVertexSize - 1)].texCoord.Y;
                gfxVertexSize += 1;
                break;
            case 1:
                gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos << 4);
                gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos << 4);
                gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
                gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + xBegin + xSize) * 0.0009765625f;
                gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + yBegin) * 0.0009765625f;
                gfxVertexSize += 1;
                gfxPolyList[(int)gfxVertexSize].position.X = (float)((xPos + xSize) << 4);
                gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos << 4);
                gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
                gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + xBegin) * 0.0009765625f;
                gfxPolyList[(int)gfxVertexSize].texCoord.Y = gfxPolyList[(int)(gfxVertexSize - 1)].texCoord.Y;
                gfxVertexSize += 1;
                gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos << 4);
                gfxPolyList[(int)gfxVertexSize].position.Y = (float)((yPos + ySize) << 4);
                gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
                gfxPolyList[(int)gfxVertexSize].texCoord.X = gfxPolyList[(int)(gfxVertexSize - 2)].texCoord.X;
                gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + yBegin + ySize) * 0.0009765625f;
                gfxVertexSize += 1;
                gfxPolyList[(int)gfxVertexSize].position.X = gfxPolyList[(int)(gfxVertexSize - 2)].position.X;
                gfxPolyList[(int)gfxVertexSize].position.Y = gfxPolyList[(int)(gfxVertexSize - 1)].position.Y;
                gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
                gfxPolyList[(int)gfxVertexSize].texCoord.X = gfxPolyList[(int)(gfxVertexSize - 2)].texCoord.X;
                gfxPolyList[(int)gfxVertexSize].texCoord.Y = gfxPolyList[(int)(gfxVertexSize - 1)].texCoord.Y;
                gfxVertexSize += 1;
                break;
            case 2:
                gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos << 4);
                gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos << 4);
                gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
                gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + xBegin) * 0.0009765625f;
                gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + yBegin + ySize) * 0.0009765625f;
                gfxVertexSize += 1;
                gfxPolyList[(int)gfxVertexSize].position.X = (float)((xPos + xSize) << 4);
                gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos << 4);
                gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
                gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + xBegin + xSize) * 0.0009765625f;
                gfxPolyList[(int)gfxVertexSize].texCoord.Y = gfxPolyList[(int)(gfxVertexSize - 1)].texCoord.Y;
                gfxVertexSize += 1;
                gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos << 4);
                gfxPolyList[(int)gfxVertexSize].position.Y = (float)((yPos + ySize) << 4);
                gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
                gfxPolyList[(int)gfxVertexSize].texCoord.X = gfxPolyList[(int)(gfxVertexSize - 2)].texCoord.X;
                gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + yBegin) * 0.0009765625f;
                gfxVertexSize += 1;
                gfxPolyList[(int)gfxVertexSize].position.X = gfxPolyList[(int)(gfxVertexSize - 2)].position.X;
                gfxPolyList[(int)gfxVertexSize].position.Y = gfxPolyList[(int)(gfxVertexSize - 1)].position.Y;
                gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
                gfxPolyList[(int)gfxVertexSize].texCoord.X = gfxPolyList[(int)(gfxVertexSize - 2)].texCoord.X;
                gfxPolyList[(int)gfxVertexSize].texCoord.Y = gfxPolyList[(int)(gfxVertexSize - 1)].texCoord.Y;
                gfxVertexSize += 1;
                break;
            case 3:
                gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos << 4);
                gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos << 4);
                gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
                gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + xBegin + xSize) * 0.0009765625f;
                gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + yBegin + ySize) * 0.0009765625f;
                gfxVertexSize += 1;
                gfxPolyList[(int)gfxVertexSize].position.X = (float)((xPos + xSize) << 4);
                gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos << 4);
                gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
                gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + xBegin) * 0.0009765625f;
                gfxPolyList[(int)gfxVertexSize].texCoord.Y = gfxPolyList[(int)(gfxVertexSize - 1)].texCoord.Y;
                gfxVertexSize += 1;
                gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos << 4);
                gfxPolyList[(int)gfxVertexSize].position.Y = (float)((yPos + ySize) << 4);
                gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
                gfxPolyList[(int)gfxVertexSize].texCoord.X = gfxPolyList[(int)(gfxVertexSize - 2)].texCoord.X;
                gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + yBegin) * 0.0009765625f;
                gfxVertexSize += 1;
                gfxPolyList[(int)gfxVertexSize].position.X = gfxPolyList[(int)(gfxVertexSize - 2)].position.X;
                gfxPolyList[(int)gfxVertexSize].position.Y = gfxPolyList[(int)(gfxVertexSize - 1)].position.Y;
                gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
                gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
                gfxPolyList[(int)gfxVertexSize].texCoord.X = gfxPolyList[(int)(gfxVertexSize - 2)].texCoord.X;
                gfxPolyList[(int)gfxVertexSize].texCoord.Y = gfxPolyList[(int)(gfxVertexSize - 1)].texCoord.Y;
                gfxVertexSize += 1;
                break;
        }
        gfxIndexSize += 6;
    }
}
void GraphicsSystem_DrawBlendedSprite(int xPos, int yPos, int xSize, int ySize, int xBegin, int yBegin, int surfaceNum)
{
    if (gfxSurface[surfaceNum].texStartX > -1 && gfxVertexSize < VERTEX_LIMIT && xPos > -512 && xPos < 872 && yPos > -512 && yPos < 752)
    {
        gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos << 4);
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos << 4);
        gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.A = 128;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + xBegin) * 0.0009765625f;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + yBegin) * 0.0009765625f;
        gfxVertexSize += 1;
        gfxPolyList[(int)gfxVertexSize].position.X = (float)((xPos + xSize) << 4);
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos << 4);
        gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.A = 128;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + xBegin + xSize) * 0.0009765625f;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = gfxPolyList[(int)(gfxVertexSize - 1)].texCoord.Y;
        gfxVertexSize += 1;
        gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos << 4);
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)((yPos + ySize) << 4);
        gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.A = 128;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = gfxPolyList[(int)(gfxVertexSize - 2)].texCoord.X;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + yBegin + ySize) * 0.0009765625f;
        gfxVertexSize += 1;
        gfxPolyList[(int)gfxVertexSize].position.X = gfxPolyList[(int)(gfxVertexSize - 2)].position.X;
        gfxPolyList[(int)gfxVertexSize].position.Y = gfxPolyList[(int)(gfxVertexSize - 1)].position.Y;
        gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.A = 128;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = gfxPolyList[(int)(gfxVertexSize - 2)].texCoord.X;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = gfxPolyList[(int)(gfxVertexSize - 1)].texCoord.Y;
        gfxVertexSize += 1;
        gfxIndexSize += 6;
    }
}
void GraphicsSystem_DrawAlphaBlendedSprite(int xPos, int yPos, int xSize, int ySize, int xBegin, int yBegin, int alpha, int surfaceNum)
{
    if (gfxSurface[surfaceNum].texStartX > -1 && gfxVertexSize < VERTEX_LIMIT && xPos > -512 && xPos < 872 && yPos > -512 && yPos < 752)
    {
        gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos << 4);
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos << 4);
        gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.A = (uint8_t)alpha;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + xBegin) * 0.0009765625f;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + yBegin) * 0.0009765625f;
        gfxVertexSize += 1;
        gfxPolyList[(int)gfxVertexSize].position.X = (float)((xPos + xSize) << 4);
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos << 4);
        gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.A = (uint8_t)alpha;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + xBegin + xSize) * 0.0009765625f;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = gfxPolyList[(int)(gfxVertexSize - 1)].texCoord.Y;
        gfxVertexSize += 1;
        gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos << 4);
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)((yPos + ySize) << 4);
        gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.A = (uint8_t)alpha;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = gfxPolyList[(int)(gfxVertexSize - 2)].texCoord.X;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + yBegin + ySize) * 0.0009765625f;
        gfxVertexSize += 1;
        gfxPolyList[(int)gfxVertexSize].position.X = gfxPolyList[(int)(gfxVertexSize - 2)].position.X;
        gfxPolyList[(int)gfxVertexSize].position.Y = gfxPolyList[(int)(gfxVertexSize - 1)].position.Y;
        gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.A = (uint8_t)alpha;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = gfxPolyList[(int)(gfxVertexSize - 2)].texCoord.X;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = gfxPolyList[(int)(gfxVertexSize - 1)].texCoord.Y;
        gfxVertexSize += 1;
        gfxIndexSize += 6;
    }
}
void GraphicsSystem_DrawAdditiveBlendedSprite(int xPos, int yPos, int xSize, int ySize, int xBegin, int yBegin, int alpha, int surfaceNum)
{
    if (gfxSurface[surfaceNum].texStartX > -1 && gfxVertexSize < VERTEX_LIMIT && xPos > -512 && xPos < 872 && yPos > -512 && yPos < 752)
    {
        gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos << 4);
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos << 4);
        gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.A = (uint8_t)alpha;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + xBegin) * 0.0009765625f;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + yBegin) * 0.0009765625f;
        gfxVertexSize += 1;
        gfxPolyList[(int)gfxVertexSize].position.X = (float)((xPos + xSize) << 4);
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos << 4);
        gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.A = (uint8_t)alpha;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + xBegin + xSize) * 0.0009765625f;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = gfxPolyList[(int)(gfxVertexSize - 1)].texCoord.Y;
        gfxVertexSize += 1;
        gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos << 4);
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)((yPos + ySize) << 4);
        gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.A = (uint8_t)alpha;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = gfxPolyList[(int)(gfxVertexSize - 2)].texCoord.X;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + yBegin + ySize) * 0.0009765625f;
        gfxVertexSize += 1;
        gfxPolyList[(int)gfxVertexSize].position.X = gfxPolyList[(int)(gfxVertexSize - 2)].position.X;
        gfxPolyList[(int)gfxVertexSize].position.Y = gfxPolyList[(int)(gfxVertexSize - 1)].position.Y;
        gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.A = (uint8_t)alpha;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = gfxPolyList[(int)(gfxVertexSize - 2)].texCoord.X;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = gfxPolyList[(int)(gfxVertexSize - 1)].texCoord.Y;
        gfxVertexSize += 1;
        gfxIndexSize += 6;
    }
}
void GraphicsSystem_DrawSubtractiveBlendedSprite(int xPos, int yPos, int xSize, int ySize, int xBegin, int yBegin, int alpha, int surfaceNum)
{
    if (gfxSurface[surfaceNum].texStartX > -1 && gfxVertexSize < VERTEX_LIMIT && xPos > -512 && xPos < 872 && yPos > -512 && yPos < 752)
    {
        gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos << 4);
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos << 4);
        gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.A = (uint8_t)alpha;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + xBegin) * 0.0009765625f;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + yBegin) * 0.0009765625f;
        gfxVertexSize += 1;
        gfxPolyList[(int)gfxVertexSize].position.X = (float)((xPos + xSize) << 4);
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos << 4);
        gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.A = (uint8_t)alpha;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + xBegin + xSize) * 0.0009765625f;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = gfxPolyList[(int)(gfxVertexSize - 1)].texCoord.Y;
        gfxVertexSize += 1;
        gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos << 4);
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)((yPos + ySize) << 4);
        gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.A = (uint8_t)alpha;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = gfxPolyList[(int)(gfxVertexSize - 2)].texCoord.X;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + yBegin + ySize) * 0.0009765625f;
        gfxVertexSize += 1;
        gfxPolyList[(int)gfxVertexSize].position.X = gfxPolyList[(int)(gfxVertexSize - 2)].position.X;
        gfxPolyList[(int)gfxVertexSize].position.Y = gfxPolyList[(int)(gfxVertexSize - 1)].position.Y;
        gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.A = (uint8_t)alpha;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = gfxPolyList[(int)(gfxVertexSize - 2)].texCoord.X;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = gfxPolyList[(int)(gfxVertexSize - 1)].texCoord.Y;
        gfxVertexSize += 1;
        gfxIndexSize += 6;
    }
}
void GraphicsSystem_DrawRectangle(int xPos, int yPos, int xSize, int ySize, int r, int g, int b, int alpha)
{
    if (alpha > 255)
    {
        alpha = 0xFF;
    }
    if (gfxVertexSize < VERTEX_LIMIT)
    {
        gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos << 4);
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos << 4);
        gfxPolyList[(int)gfxVertexSize].color.R = (uint8_t)r;
        gfxPolyList[(int)gfxVertexSize].color.G = (uint8_t)g;
        gfxPolyList[(int)gfxVertexSize].color.B = (uint8_t)b;
        gfxPolyList[(int)gfxVertexSize].color.A = (uint8_t)alpha;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = 0.0f;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = 0.0f;
        gfxVertexSize += 1;
        gfxPolyList[(int)gfxVertexSize].position.X = (float)((xPos + xSize) << 4);
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos << 4);
        gfxPolyList[(int)gfxVertexSize].color.R = (uint8_t)r;
        gfxPolyList[(int)gfxVertexSize].color.G = (uint8_t)g;
        gfxPolyList[(int)gfxVertexSize].color.B = (uint8_t)b;
        gfxPolyList[(int)gfxVertexSize].color.A = (uint8_t)alpha;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = 0.01f;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = gfxPolyList[(int)(gfxVertexSize - 1)].texCoord.Y;
        gfxVertexSize += 1;
        gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos << 4);
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)((yPos + ySize) << 4);
        gfxPolyList[(int)gfxVertexSize].color.R = (uint8_t)r;
        gfxPolyList[(int)gfxVertexSize].color.G = (uint8_t)g;
        gfxPolyList[(int)gfxVertexSize].color.B = (uint8_t)b;
        gfxPolyList[(int)gfxVertexSize].color.A = (uint8_t)alpha;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = 0.0f;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = 0.01f;
        gfxVertexSize += 1;
        gfxPolyList[(int)gfxVertexSize].position.X = gfxPolyList[(int)(gfxVertexSize - 2)].position.X;
        gfxPolyList[(int)gfxVertexSize].position.Y = gfxPolyList[(int)(gfxVertexSize - 1)].position.Y;
        gfxPolyList[(int)gfxVertexSize].color.R = (uint8_t)r;
        gfxPolyList[(int)gfxVertexSize].color.G = (uint8_t)g;
        gfxPolyList[(int)gfxVertexSize].color.B = (uint8_t)b;
        gfxPolyList[(int)gfxVertexSize].color.A = (uint8_t)alpha;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = 0.01f;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = 0.01f;
        gfxVertexSize += 1;
        gfxIndexSize += 6;
    }
}
void GraphicsSystem_DrawTintRectangle(int xPos, int yPos, int xSize, int ySize)
{
    //Not supported
}
void GraphicsSystem_DrawTintSpriteMask(int xPos, int yPos, int xSize, int ySize, int xBegin, int yBegin, int tableNo, int surfaceNum)
{
    //Not supported
}
void GraphicsSystem_DrawScaledTintMask(uint8_t direction, int xPos, int yPos, int xPivot, int yPivot, int xScale, int yScale, int xSize, int ySize, int xBegin, int yBegin, int surfaceNum)
{
    //Not supported
}
void GraphicsSystem_DrawScaledSprite(uint8_t direction, int xPos, int yPos, int xPivot, int yPivot, int xScale, int yScale, int xSize, int ySize, int xBegin, int yBegin, int surfaceNum)
{
    if (gfxVertexSize < VERTEX_LIMIT && xPos > -512 && xPos < 872 && yPos > -512 && yPos < 752)
    {
        xScale <<= 2;
        yScale <<= 2;
        xPos -= xPivot * xScale >> 11;
        xScale = xSize * xScale >> 11;
        yPos -= yPivot * yScale >> 11;
        yScale = ySize * yScale >> 11;
        if (gfxSurface[surfaceNum].texStartX > -1)
        {
            gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos << 4);
            gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos << 4);
            gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
            gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + xBegin) * 0.0009765625f;
            gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + yBegin) * 0.0009765625f;
            gfxVertexSize += 1;
            gfxPolyList[(int)gfxVertexSize].position.X = (float)((xPos + xScale) << 4);
            gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos << 4);
            gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
            gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + xBegin + xSize) * 0.0009765625f;
            gfxPolyList[(int)gfxVertexSize].texCoord.Y = gfxPolyList[(int)(gfxVertexSize - 1)].texCoord.Y;
            gfxVertexSize += 1;
            gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos << 4);
            gfxPolyList[(int)gfxVertexSize].position.Y = (float)((yPos + yScale) << 4);
            gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
            gfxPolyList[(int)gfxVertexSize].texCoord.X = gfxPolyList[(int)(gfxVertexSize - 2)].texCoord.X;
            gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + yBegin + ySize) * 0.0009765625f;
            gfxVertexSize += 1;
            gfxPolyList[(int)gfxVertexSize].position.X = gfxPolyList[(int)(gfxVertexSize - 2)].position.X;
            gfxPolyList[(int)gfxVertexSize].position.Y = gfxPolyList[(int)(gfxVertexSize - 1)].position.Y;
            gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
            gfxPolyList[(int)gfxVertexSize].texCoord.X = gfxPolyList[(int)(gfxVertexSize - 2)].texCoord.X;
            gfxPolyList[(int)gfxVertexSize].texCoord.Y = gfxPolyList[(int)(gfxVertexSize - 1)].texCoord.Y;
            gfxVertexSize += 1;
            gfxIndexSize += 6;
        }
    }
}
void GraphicsSystem_DrawScaledChar(uint8_t direction, int xPos, int yPos, int xPivot, int yPivot, int xScale, int yScale, int xSize, int ySize, int xBegin, int yBegin, int surfaceNum)
{
    if (gfxVertexSize < VERTEX_LIMIT && xPos > -8192 && xPos < 13951 && yPos > -1024 && yPos < 4864)
    {
        xPos -= xPivot * xScale >> 5;
        xScale = xSize * xScale >> 5;
        yPos -= yPivot * yScale >> 5;
        yScale = ySize * yScale >> 5;
        if (gfxSurface[surfaceNum].texStartX > -1 && gfxVertexSize < 4096)
        {
            gfxPolyList[(int)gfxVertexSize].position.X = (float)xPos;
            gfxPolyList[(int)gfxVertexSize].position.Y = (float)yPos;
            gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
            gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + xBegin) * 0.0009765625f;
            gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + yBegin) * 0.0009765625f;
            gfxVertexSize += 1;
            gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos + xScale);
            gfxPolyList[(int)gfxVertexSize].position.Y = (float)yPos;
            gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
            gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + xBegin + xSize) * 0.0009765625f;
            gfxPolyList[(int)gfxVertexSize].texCoord.Y = gfxPolyList[(int)(gfxVertexSize - 1)].texCoord.Y;
            gfxVertexSize += 1;
            gfxPolyList[(int)gfxVertexSize].position.X = (float)xPos;
            gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos + yScale);
            gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
            gfxPolyList[(int)gfxVertexSize].texCoord.X = gfxPolyList[(int)(gfxVertexSize - 2)].texCoord.X;
            gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + yBegin + ySize) * 0.0009765625f;
            gfxVertexSize += 1;
            gfxPolyList[(int)gfxVertexSize].position.X = gfxPolyList[(int)(gfxVertexSize - 2)].position.X;
            gfxPolyList[(int)gfxVertexSize].position.Y = gfxPolyList[(int)(gfxVertexSize - 1)].position.Y;
            gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
            gfxPolyList[(int)gfxVertexSize].texCoord.X = gfxPolyList[(int)(gfxVertexSize - 2)].texCoord.X;
            gfxPolyList[(int)gfxVertexSize].texCoord.Y = gfxPolyList[(int)(gfxVertexSize - 1)].texCoord.Y;
            gfxVertexSize += 1;
            gfxIndexSize += 6;
        }
    }
}
void GraphicsSystem_DrawRotatedSprite(uint8_t direction, int xPos, int yPos, int xPivot, int yPivot, int xBegin, int yBegin, int xSize, int ySize, int rotAngle, int surfaceNum)
{
    xPos <<= 4;
    yPos <<= 4;
    rotAngle -= rotAngle >> 9 << 9;
    if (rotAngle < 0)
    {
        rotAngle += 512;
    }
    if (rotAngle != 0)
    {
        rotAngle = 512 - rotAngle;
    }
    int num = SinValue512[rotAngle];
    int num2 = CosValue512[rotAngle];
    if (gfxSurface[surfaceNum].texStartX > -1 && gfxVertexSize < VERTEX_LIMIT && xPos > -8192 && xPos < 13952 && yPos > -8192 && yPos < 12032)
    {
        int num3;
        int num4;
        if (direction == 0)
        {
            num3 = -xPivot;
            num4 = -yPivot;
            gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos + ((num3 * num2 + num4 * num) >> 5));
            gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos + ((num4 * num2 - num3 * num) >> 5));
            gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
            gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + xBegin) * 0.0009765625f;
            gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + yBegin) * 0.0009765625f;
            gfxVertexSize += 1;
            num3 = xSize - xPivot;
            num4 = -yPivot;
            gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos + ((num3 * num2 + num4 * num) >> 5));
            gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos + ((num4 * num2 - num3 * num) >> 5));
            gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
            gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + xBegin + xSize) * 0.0009765625f;
            gfxPolyList[(int)gfxVertexSize].texCoord.Y = gfxPolyList[(int)(gfxVertexSize - 1)].texCoord.Y;
            gfxVertexSize += 1;
            num3 = -xPivot;
            num4 = ySize - yPivot;
            gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos + ((num3 * num2 + num4 * num) >> 5));
            gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos + ((num4 * num2 - num3 * num) >> 5));
            gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
            gfxPolyList[(int)gfxVertexSize].texCoord.X = gfxPolyList[(int)(gfxVertexSize - 2)].texCoord.X;
            gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + yBegin + ySize) * 0.0009765625f;
            gfxVertexSize += 1;
            num3 = xSize - xPivot;
            num4 = ySize - yPivot;
            gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos + ((num3 * num2 + num4 * num) >> 5));
            gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos + ((num4 * num2 - num3 * num) >> 5));
            gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
            gfxPolyList[(int)gfxVertexSize].texCoord.X = gfxPolyList[(int)(gfxVertexSize - 2)].texCoord.X;
            gfxPolyList[(int)gfxVertexSize].texCoord.Y = gfxPolyList[(int)(gfxVertexSize - 1)].texCoord.Y;
            gfxVertexSize += 1;
            gfxIndexSize += 6;
            return;
        }
        num4 = -yPivot;
        gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos + ((xPivot * num2 + num4 * num) >> 5));
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos + ((num4 * num2 - xPivot * num) >> 5));
        gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + xBegin) * 0.0009765625f;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + yBegin) * 0.0009765625f;
        gfxVertexSize += 1;
        num3 = xPivot - xSize;
        num4 = -yPivot;
        gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos + ((num3 * num2 + num4 * num) >> 5));
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos + ((num4 * num2 - num3 * num) >> 5));
        gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + xBegin + xSize) * 0.0009765625f;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = gfxPolyList[(int)(gfxVertexSize - 1)].texCoord.Y;
        gfxVertexSize += 1;
        num4 = ySize - yPivot;
        gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos + ((xPivot * num2 + num4 * num) >> 5));
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos + ((num4 * num2 - xPivot * num) >> 5));
        gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = gfxPolyList[(int)(gfxVertexSize - 2)].texCoord.X;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + yBegin + ySize) * 0.0009765625f;
        gfxVertexSize += 1;
        num3 = xPivot - xSize;
        num4 = ySize - yPivot;
        gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos + ((num3 * num2 + num4 * num) >> 5));
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos + ((num4 * num2 - num3 * num) >> 5));
        gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = gfxPolyList[(int)(gfxVertexSize - 2)].texCoord.X;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = gfxPolyList[(int)(gfxVertexSize - 1)].texCoord.Y;
        gfxVertexSize += 1;
        gfxIndexSize += 6;
    }
}

void GraphicsSystem_DrawRotoZoomSprite(uint8_t direction, int xPos, int yPos, int xPivot, int yPivot, int xBegin, int yBegin, int xSize, int ySize, int rotAngle, int scale, int surfaceNum)
{
    xPos <<= 4;
    yPos <<= 4;
    rotAngle -= rotAngle >> 9 << 9;
    if (rotAngle < 0)
    {
        rotAngle += 512;
    }
    if (rotAngle != 0)
    {
        rotAngle = 512 - rotAngle;
    }
    int num = SinValue512[rotAngle] * scale >> 9;
    int num2 = CosValue512[rotAngle] * scale >> 9;
    if (gfxSurface[surfaceNum].texStartX > -1 && gfxVertexSize < VERTEX_LIMIT && xPos > -8192 && xPos < 13952 && yPos > -8192 && yPos < 12032)
    {
        int num3;
        int num4;
        if (direction == 0)
        {
            num3 = -xPivot;
            num4 = -yPivot;
            gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos + ((num3 * num2 + num4 * num) >> 5));
            gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos + ((num4 * num2 - num3 * num) >> 5));
            gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
            gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + xBegin) * 0.0009765625f;
            gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + yBegin) * 0.0009765625f;
            gfxVertexSize += 1;
            num3 = xSize - xPivot;
            num4 = -yPivot;
            gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos + ((num3 * num2 + num4 * num) >> 5));
            gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos + ((num4 * num2 - num3 * num) >> 5));
            gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
            gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + xBegin + xSize) * 0.0009765625f;
            gfxPolyList[(int)gfxVertexSize].texCoord.Y = gfxPolyList[(int)(gfxVertexSize - 1)].texCoord.Y;
            gfxVertexSize += 1;
            num3 = -xPivot;
            num4 = ySize - yPivot;
            gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos + ((num3 * num2 + num4 * num) >> 5));
            gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos + ((num4 * num2 - num3 * num) >> 5));
            gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
            gfxPolyList[(int)gfxVertexSize].texCoord.X = gfxPolyList[(int)(gfxVertexSize - 2)].texCoord.X;
            gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + yBegin + ySize) * 0.0009765625f;
            gfxVertexSize += 1;
            num3 = xSize - xPivot;
            num4 = ySize - yPivot;
            gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos + ((num3 * num2 + num4 * num) >> 5));
            gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos + ((num4 * num2 - num3 * num) >> 5));
            gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
            gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
            gfxPolyList[(int)gfxVertexSize].texCoord.X = gfxPolyList[(int)(gfxVertexSize - 2)].texCoord.X;
            gfxPolyList[(int)gfxVertexSize].texCoord.Y = gfxPolyList[(int)(gfxVertexSize - 1)].texCoord.Y;
            gfxVertexSize += 1;
            gfxIndexSize += 6;
            return;
        }
        num4 = -yPivot;
        gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos + ((xPivot * num2 + num4 * num) >> 5));
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos + ((num4 * num2 - xPivot * num) >> 5));
        gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + xBegin) * 0.0009765625f;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + yBegin) * 0.0009765625f;
        gfxVertexSize += 1;
        num3 = xPivot - xSize;
        num4 = -yPivot;
        gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos + ((num3 * num2 + num4 * num) >> 5));
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos + ((num4 * num2 - num3 * num) >> 5));
        gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + xBegin + xSize) * 0.0009765625f;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = gfxPolyList[(int)(gfxVertexSize - 1)].texCoord.Y;
        gfxVertexSize += 1;
        num4 = ySize - yPivot;
        gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos + ((xPivot * num2 + num4 * num) >> 5));
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos + ((num4 * num2 - xPivot * num) >> 5));
        gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = gfxPolyList[(int)(gfxVertexSize - 2)].texCoord.X;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + yBegin + ySize) * 0.0009765625f;
        gfxVertexSize += 1;
        num3 = xPivot - xSize;
        num4 = ySize - yPivot;
        gfxPolyList[(int)gfxVertexSize].position.X = (float)(xPos + ((num3 * num2 + num4 * num) >> 5));
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)(yPos + ((num4 * num2 - num3 * num) >> 5));
        gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = gfxPolyList[(int)(gfxVertexSize - 2)].texCoord.X;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = gfxPolyList[(int)(gfxVertexSize - 1)].texCoord.Y;
        gfxVertexSize += 1;
        gfxIndexSize += 6;
    }
}
void GraphicsSystem_DrawQuad(struct Quad2D* face, int rgbVal)
{
    if (gfxVertexSize < VERTEX_LIMIT)
    {
        gfxPolyList[(int)gfxVertexSize].position.X = (float)(face->vertex[0].x << 4);
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)(face->vertex[0].y << 4);
        gfxPolyList[(int)gfxVertexSize].color.R = (uint8_t)(rgbVal >> 16 & 255);
        gfxPolyList[(int)gfxVertexSize].color.G = (uint8_t)(rgbVal >> 8 & 255);
        gfxPolyList[(int)gfxVertexSize].color.B = (uint8_t)(rgbVal & 255);
        rgbVal = (rgbVal & 0x7F000000) >> 23;
        if (rgbVal > 253)
        {
            gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
        }
        else
        {
            gfxPolyList[(int)gfxVertexSize].color.A = (uint8_t)rgbVal;
        }
        gfxPolyList[(int)gfxVertexSize].texCoord.X = 0.01f;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = 0.01f;
        gfxVertexSize += 1;
        gfxPolyList[(int)gfxVertexSize].position.X = (float)(face->vertex[1].x << 4);
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)(face->vertex[1].y << 4);
        gfxPolyList[(int)gfxVertexSize].color.R = gfxPolyList[(int)(gfxVertexSize - 1)].color.R;
        gfxPolyList[(int)gfxVertexSize].color.G = gfxPolyList[(int)(gfxVertexSize - 1)].color.G;
        gfxPolyList[(int)gfxVertexSize].color.B = gfxPolyList[(int)(gfxVertexSize - 1)].color.B;
        gfxPolyList[(int)gfxVertexSize].color.A = gfxPolyList[(int)(gfxVertexSize - 1)].color.A;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = 0.01f;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = 0.01f;
        gfxVertexSize += 1;
        gfxPolyList[(int)gfxVertexSize].position.X = (float)(face->vertex[2].x << 4);
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)(face->vertex[2].y << 4);
        gfxPolyList[(int)gfxVertexSize].color.R = gfxPolyList[(int)(gfxVertexSize - 1)].color.R;
        gfxPolyList[(int)gfxVertexSize].color.G = gfxPolyList[(int)(gfxVertexSize - 1)].color.G;
        gfxPolyList[(int)gfxVertexSize].color.B = gfxPolyList[(int)(gfxVertexSize - 1)].color.B;
        gfxPolyList[(int)gfxVertexSize].color.A = gfxPolyList[(int)(gfxVertexSize - 1)].color.A;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = 0.01f;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = 0.01f;
        gfxVertexSize += 1;
        gfxPolyList[(int)gfxVertexSize].position.X = (float)(face->vertex[3].x << 4);
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)(face->vertex[3].y << 4);
        gfxPolyList[(int)gfxVertexSize].color.R = gfxPolyList[(int)(gfxVertexSize - 1)].color.R;
        gfxPolyList[(int)gfxVertexSize].color.G = gfxPolyList[(int)(gfxVertexSize - 1)].color.G;
        gfxPolyList[(int)gfxVertexSize].color.B = gfxPolyList[(int)(gfxVertexSize - 1)].color.B;
        gfxPolyList[(int)gfxVertexSize].color.A = gfxPolyList[(int)(gfxVertexSize - 1)].color.A;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = 0.01f;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = 0.01f;
        gfxVertexSize += 1;
        gfxIndexSize += 6;
    }
}
void GraphicsSystem_DrawTexturedQuad(struct Quad2D* face, int surfaceNum)
{
    if (gfxVertexSize < VERTEX_LIMIT)
    {
        gfxPolyList[(int)gfxVertexSize].position.X = (float)(face->vertex[0].x << 4);
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)(face->vertex[0].y << 4);
        gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + face->vertex[0].u) * 0.0009765625f;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + face->vertex[0].v) * 0.0009765625f;
        gfxVertexSize += 1;
        gfxPolyList[(int)gfxVertexSize].position.X = (float)(face->vertex[1].x << 4);
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)(face->vertex[1].y << 4);
        gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + face->vertex[1].u) * 0.0009765625f;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + face->vertex[1].v) * 0.0009765625f;
        gfxVertexSize += 1;
        gfxPolyList[(int)gfxVertexSize].position.X = (float)(face->vertex[2].x << 4);
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)(face->vertex[2].y << 4);
        gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + face->vertex[2].u) * 0.0009765625f;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + face->vertex[2].v) * 0.0009765625f;
        gfxVertexSize += 1;
        gfxPolyList[(int)gfxVertexSize].position.X = (float)(face->vertex[3].x << 4);
        gfxPolyList[(int)gfxVertexSize].position.Y = (float)(face->vertex[3].y << 4);
        gfxPolyList[(int)gfxVertexSize].color.R = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.G = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.B = 0xFF;
        gfxPolyList[(int)gfxVertexSize].color.A = 0xFF;
        gfxPolyList[(int)gfxVertexSize].texCoord.X = (float)(gfxSurface[surfaceNum].texStartX + face->vertex[3].u) * 0.0009765625f;
        gfxPolyList[(int)gfxVertexSize].texCoord.Y = (float)(gfxSurface[surfaceNum].texStartY + face->vertex[3].v) * 0.0009765625f;
        gfxVertexSize += 1;
        gfxIndexSize += 6;
    }
}
void GraphicsSystem_SetPaletteEntry(uint8_t entryPos, uint8_t cR, uint8_t cG, uint8_t cB)
{
    if (entryPos > 0)
    {
        tilePalette16_Data[texPaletteNum][(int)entryPos] = GraphicsSystem_RGB_16BIT5551(cR, cG, cB, 1);
    }
    else
    {
        tilePalette16_Data[texPaletteNum][(int)entryPos] = GraphicsSystem_RGB_16BIT5551(cR, cG, cB, 0);
    }
    tilePalette[(int)entryPos].red = cR;
    tilePalette[(int)entryPos].green = cG;
    tilePalette[(int)entryPos].blue = cB;
}
void GraphicsSystem_SetFade(uint8_t clrR, uint8_t clrG, uint8_t clrB, unsigned short clrA)
{
    fadeMode = 1;
    if (clrA > 255)
    {
        clrA = 0xFF;
    }
    fadeR = clrR;
    fadeG = clrG;
    fadeB = clrB;
    fadeA = (uint8_t)clrA;
}
void GraphicsSystem_SetLimitedFade(uint8_t paletteNum, uint8_t clrR, uint8_t clrG, uint8_t clrB, unsigned short clrA, int fStart, int fEnd)
{
    uint8_t array[3];
    paletteMode = paletteNum;
    if (paletteNum < 8)
    {
        if (clrA > 255)
        {
            clrA = 0xFF;
        }
        if (fEnd < 256)
        {
            fEnd++;
        }
        for (int i = fStart; i < fEnd; i++)
        {
            array[0] = (uint8_t)(((ushort)tilePalette[i].red * (255 - clrA) + clrA * (ushort)clrR) >> 8);
            array[1] = (uint8_t)(((ushort)tilePalette[i].green * (255 - clrA) + clrA * (ushort)clrG) >> 8);
            array[2] = (uint8_t)(((ushort)tilePalette[i].blue * (255 - clrA) + clrA * (ushort)clrB) >> 8);
            tilePalette16_Data[0][i] = GraphicsSystem_RGB_16BIT5551(array[0], array[1], array[2], 1);
        }
        tilePalette16_Data[0][0] = GraphicsSystem_RGB_16BIT5551(array[0], array[1], array[2], 0);
    }
}
void GraphicsSystem_SetActivePalette(uint8_t paletteNum, int minY, int maxY)
{
    if (paletteNum < 8)
    {
        texPaletteNum = (int)paletteNum;
    }
}
void GraphicsSystem_CopyPalette(uint8_t paletteSource, uint8_t paletteDest)
{
    if (paletteSource < 8 && paletteDest < 8)
    {
        for (int i = 0; i < 256; i++)
        {
            tilePalette16_Data[(int)paletteDest][i] = tilePalette16_Data[(int)paletteSource][i];
        }
    }
}
void GraphicsSystem_RotatePalette(uint8_t pStart, uint8_t pEnd, uint8_t pDirection)
{
    switch (pDirection)
    {
        case 0:
        {
            ushort num = tilePalette16_Data[texPaletteNum][(int)pStart];
            for (uint8_t b = pStart; b < pEnd; b += 1)
            {
                tilePalette16_Data[texPaletteNum][(int)b] = tilePalette16_Data[texPaletteNum][(int)(b + 1)];
            }
            tilePalette16_Data[texPaletteNum][(int)pEnd] = num;
            return;
        }
        case 1:
        {
            ushort num = tilePalette16_Data[texPaletteNum][(int)pEnd];
            for (uint8_t b = pEnd; b > pStart; b -= 1)
            {
                tilePalette16_Data[texPaletteNum][(int)b] = tilePalette16_Data[texPaletteNum][(int)(b - 1)];
            }
            tilePalette16_Data[texPaletteNum][(int)pStart] = num;
            return;
        }
        default:
            return;
    }
}
void GraphicsSystem_GenerateBlendLookupTable()
{
    int num = 0;
    for (int i = 0; i < 256; i++)
    {
        for (int j = 0; j < 32; j++)
        {
            blendLookupTable[num] = (unsigned short)(j * i >> 8);
            subtractiveLookupTable[num] = (unsigned short)((31 - j) * i >> 8);
            num++;
        }
    }
}
