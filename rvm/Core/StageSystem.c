//
//  StageSystem.c
//  rvm
//

#include "StageSystem.h"

const int ACTLAYOUT = 0;
const int LOADSTAGE = 0;
const int PLAYSTAGE = 1;
const int STAGEPAUSED = 2;
struct InputResult gKeyDown;
struct InputResult gKeyPress;
uint8_t stageMode;
uint8_t pauseEnabled;
int stageListPosition;
struct Mappings128x128 tile128x128;
struct LayoutMap stageLayouts[9];
uint8_t tLayerMidPoint;
uint8_t activeTileLayers[4];
struct CollisionMask16x16 tileCollisions[2];
struct LineScrollParallax hParallax;
struct LineScrollParallax vParallax;
int lastXSize;
int lastYSize;
int bgDeformationData0[0x240];
int bgDeformationData1[0x240];
int bgDeformationData2[0x240];
int bgDeformationData3[0x240];
int xBoundary1;
int xBoundary2;
int yBoundary1;
int yBoundary2;
int newXBoundary1;
int newXBoundary2;
int newYBoundary1;
int newYBoundary2;
uint8_t cameraEnabled;
signed char cameraTarget;
uint8_t cameraShift;
uint8_t cameraStyle;
int cameraAdjustY;
int xScrollOffset;
int yScrollOffset;
int yScrollA;
int yScrollB;
int xScrollA;
int xScrollB;
int xScrollMove;
int yScrollMove;
int screenShakeX;
int screenShakeY;
int waterLevel;
char titleCardText[24];
char titleCardWord2;
struct TextMenu gameMenu[2];
uint8_t timeEnabled;
uint8_t milliSeconds;
uint8_t seconds;
uint8_t minutes;
uint8_t debugMode;

void Init_StageSystem()
{
    xBoundary1 = 0;
    yBoundary1 = 0;
    newXBoundary1 = 0;
    newYBoundary1 = 0;
    cameraShift = 0;
    cameraStyle = 0;
    xScrollOffset = 0;
    yScrollOffset = 0;
    yScrollA = 0;
    yScrollB = 240;
    xScrollA = 0;
    xScrollB = 0x140;
    xScrollMove = 0;
    yScrollMove = 0;
    screenShakeX = 0;
    screenShakeY = 0;
    debugMode = 0;
}

void StageSystem_Draw3DFloorLayer(uint8_t layerNum)
{
    int num;
    int num1;
    int num2;
    int num3;
    int num4;
    int num5;
    int sinValue512;
    int cosValue512;
    int j;
    int i;
    int* numArray = tile128x128.gfxDataPos;
    uint8_t* numArray1 = tile128x128.direction;
    int num6 = stageLayouts[activeTileLayers[layerNum]].xSize << 7;
    int num7 = stageLayouts[activeTileLayers[layerNum]].ySize << 7;
    unsigned short* numArray2 = stageLayouts[activeTileLayers[layerNum]].tileMap;
    vertexSize3D = 0;
    indexSize3D = 0;
    /*polyList3D[vertexSize3D].position.X = 0.0f;
    polyList3D[vertexSize3D].position.Y = 0.0f;
    polyList3D[vertexSize3D].position.Z = 0.0f;
    polyList3D[vertexSize3D].texCoord.X = 0.5f;
    polyList3D[vertexSize3D].texCoord.Y = 0.0f;
    polyList3D[vertexSize3D].color.R = 0xff;
    polyList3D[vertexSize3D].color.G = 0xff;
    polyList3D[vertexSize3D].color.B = 0xff;
    polyList3D[vertexSize3D].color.A = 0xff;
    vertexSize3D = (ushort)(vertexSize3D + 1);
    polyList3D[vertexSize3D].position.X = 4096.0f;
    polyList3D[vertexSize3D].position.Y = 0.0f;
    polyList3D[vertexSize3D].position.Z = 0.0f;
    polyList3D[vertexSize3D].texCoord.X = 1.0f;
    polyList3D[vertexSize3D].texCoord.Y = 0.0f;
    polyList3D[vertexSize3D].color.R = 0xff;
    polyList3D[vertexSize3D].color.G = 0xff;
    polyList3D[vertexSize3D].color.B = 0xff;
    polyList3D[vertexSize3D].color.A = 0xff;
    vertexSize3D = (ushort)(vertexSize3D + 1);
    polyList3D[vertexSize3D].position.X = 0.0f;
    polyList3D[vertexSize3D].position.Y = 0.0f;
    polyList3D[vertexSize3D].position.Z = 4096.0f;
    polyList3D[vertexSize3D].texCoord.X = 0.5f;
    polyList3D[vertexSize3D].texCoord.Y = 0.5f;
    polyList3D[vertexSize3D].color.R = 0xff;
    polyList3D[vertexSize3D].color.G = 0xff;
    polyList3D[vertexSize3D].color.B = 0xff;
    polyList3D[vertexSize3D].color.A = 0xff;
    vertexSize3D = (ushort)(vertexSize3D + 1);
    polyList3D[vertexSize3D].position.X = 4096.0f;
    polyList3D[vertexSize3D].position.Y = 0.0f;
    polyList3D[vertexSize3D].position.Z = 4096.0f;
    polyList3D[vertexSize3D].texCoord.X = 1.0f;
    polyList3D[vertexSize3D].texCoord.Y = 0.5f;
    polyList3D[vertexSize3D].color.R = 0xff;
    polyList3D[vertexSize3D].color.G = 0xff;
    polyList3D[vertexSize3D].color.B = 0xff;
    polyList3D[vertexSize3D].color.A = 0xff;
    vertexSize3D = (ushort)(vertexSize3D + 1);
    indexSize3D = (ushort)(indexSize3D + 2);
    if (HQ3DFloorEnabled)
    {
        sinValue512 = (stageLayouts[activeTileLayers[layerNum]].xPos >> 16) - 0x100;
        sinValue512 = sinValue512 + (SinValue512[stageLayouts[activeTileLayers[layerNum]].angle] >> 1);
        sinValue512 = sinValue512 >> 4 << 4;
        cosValue512 = (stageLayouts[activeTileLayers[layerNum]].zPos >> 16) - 0x100;
        cosValue512 = cosValue512 + (CosValue512[stageLayouts[activeTileLayers[layerNum]].angle] >> 1);
        cosValue512 = cosValue512 >> 4 << 4;
        for (i = 32; i > 0; i--)
        {
            for (j = 32; j > 0; j--)
            {
                if (sinValue512 > -1 && sinValue512 < num6 && cosValue512 > -1 && cosValue512 < num7)
                {
                    num1 = sinValue512 >> 7;
                    num2 = cosValue512 >> 7;
                    num3 = (sinValue512 & 127) >> 4;
                    num4 = (cosValue512 & 127) >> 4;
                    num5 = numArray2[num1 + (num2 << 8)] << 6;
                    num5 = num5 + num3 + (num4 << 3);
                    if (numArray[num5] > 0)
                    {
                        num = 0;
                        switch (numArray1[num5])
                        {
                            case 0:
                            {
                                polyList3D[vertexSize3D].position.X = (float)sinValue512;
                                polyList3D[vertexSize3D].position.Y = 0.0f;
                                polyList3D[vertexSize3D].position.Z = (float)cosValue512;
                                polyList3D[vertexSize3D].texCoord.X = tileUVArray[numArray[num5] + num];
                                num++;
                                polyList3D[vertexSize3D].texCoord.Y = tileUVArray[numArray[num5] + num];
                                num++;
                                polyList3D[vertexSize3D].color.R = 0xff;
                                polyList3D[vertexSize3D].color.G = 0xff;
                                polyList3D[vertexSize3D].color.B = 0xff;
                                polyList3D[vertexSize3D].color.A = 0xff;
                                vertexSize3D = (ushort)(vertexSize3D + 1);
                                polyList3D[vertexSize3D].position.X = (float)(sinValue512 + 16);
                                polyList3D[vertexSize3D].position.Y = 0.0f;
                                polyList3D[vertexSize3D].position.Z = polyList3D[vertexSize3D - 1].position.Z;
                                polyList3D[vertexSize3D].texCoord.X = tileUVArray[numArray[num5] + num];
                                num++;
                                polyList3D[vertexSize3D].texCoord.Y = polyList3D[vertexSize3D - 1].texCoord.Y;
                                polyList3D[vertexSize3D].color.R = 0xff;
                                polyList3D[vertexSize3D].color.G = 0xff;
                                polyList3D[vertexSize3D].color.B = 0xff;
                                polyList3D[vertexSize3D].color.A = 0xff;
                                vertexSize3D = (ushort)(vertexSize3D + 1);
                                polyList3D[vertexSize3D].position.X = polyList3D[vertexSize3D - 2].position.X;
                                polyList3D[vertexSize3D].position.Y = 0.0f;
                                polyList3D[vertexSize3D].position.Z = (float)(cosValue512 + 16);
                                polyList3D[vertexSize3D].texCoord.X = polyList3D[vertexSize3D - 2].texCoord.X;
                                polyList3D[vertexSize3D].texCoord.Y = tileUVArray[numArray[num5] + num];
                                polyList3D[vertexSize3D].color.R = 0xff;
                                polyList3D[vertexSize3D].color.G = 0xff;
                                polyList3D[vertexSize3D].color.B = 0xff;
                                polyList3D[vertexSize3D].color.A = 0xff;
                                vertexSize3D = (ushort)(vertexSize3D + 1);
                                polyList3D[vertexSize3D].position.X = polyList3D[vertexSize3D - 2].position.X;
                                polyList3D[vertexSize3D].position.Y = 0.0f;
                                polyList3D[vertexSize3D].position.Z = polyList3D[vertexSize3D - 1].position.Z;
                                polyList3D[vertexSize3D].texCoord.X = polyList3D[vertexSize3D - 2].texCoord.X;
                                polyList3D[vertexSize3D].texCoord.Y = polyList3D[vertexSize3D - 1].texCoord.Y;
                                polyList3D[vertexSize3D].color.R = 0xff;
                                polyList3D[vertexSize3D].color.G = 0xff;
                                polyList3D[vertexSize3D].color.B = 0xff;
                                polyList3D[vertexSize3D].color.A = 0xff;
                                vertexSize3D = (ushort)(vertexSize3D + 1);
                                indexSize3D = (ushort)(indexSize3D + 2);
                                break;
                            }
                            case 1:
                            {
                                polyList3D[vertexSize3D].position.X = (float)(sinValue512 + 16);
                                polyList3D[vertexSize3D].position.Y = 0.0f;
                                polyList3D[vertexSize3D].position.Z = (float)cosValue512;
                                polyList3D[vertexSize3D].texCoord.X = tileUVArray[numArray[num5] + num];
                                num++;
                                polyList3D[vertexSize3D].texCoord.Y = tileUVArray[numArray[num5] + num];
                                num++;
                                polyList3D[vertexSize3D].color.R = 0xff;
                                polyList3D[vertexSize3D].color.G = 0xff;
                                polyList3D[vertexSize3D].color.B = 0xff;
                                polyList3D[vertexSize3D].color.A = 0xff;
                                vertexSize3D = (ushort)(vertexSize3D + 1);
                                polyList3D[vertexSize3D].position.X = (float)sinValue512;
                                polyList3D[vertexSize3D].position.Y = 0.0f;
                                polyList3D[vertexSize3D].position.Z = polyList3D[vertexSize3D - 1].position.Z;
                                polyList3D[vertexSize3D].texCoord.X = tileUVArray[numArray[num5] + num];
                                num++;
                                polyList3D[vertexSize3D].texCoord.Y = polyList3D[vertexSize3D - 1].texCoord.Y;
                                polyList3D[vertexSize3D].color.R = 0xff;
                                polyList3D[vertexSize3D].color.G = 0xff;
                                polyList3D[vertexSize3D].color.B = 0xff;
                                polyList3D[vertexSize3D].color.A = 0xff;
                                vertexSize3D = (ushort)(vertexSize3D + 1);
                                polyList3D[vertexSize3D].position.X = polyList3D[vertexSize3D - 2].position.X;
                                polyList3D[vertexSize3D].position.Y = 0.0f;
                                polyList3D[vertexSize3D].position.Z = (float)(cosValue512 + 16);
                                polyList3D[vertexSize3D].texCoord.X = polyList3D[vertexSize3D - 2].texCoord.X;
                                polyList3D[vertexSize3D].texCoord.Y = tileUVArray[numArray[num5] + num];
                                polyList3D[vertexSize3D].color.R = 0xff;
                                polyList3D[vertexSize3D].color.G = 0xff;
                                polyList3D[vertexSize3D].color.B = 0xff;
                                polyList3D[vertexSize3D].color.A = 0xff;
                                vertexSize3D = (ushort)(vertexSize3D + 1);
                                polyList3D[vertexSize3D].position.X = polyList3D[vertexSize3D - 2].position.X;
                                polyList3D[vertexSize3D].position.Y = 0.0f;
                                polyList3D[vertexSize3D].position.Z = polyList3D[vertexSize3D - 1].position.Z;
                                polyList3D[vertexSize3D].texCoord.X = polyList3D[vertexSize3D - 2].texCoord.X;
                                polyList3D[vertexSize3D].texCoord.Y = polyList3D[vertexSize3D - 1].texCoord.Y;
                                polyList3D[vertexSize3D].color.R = 0xff;
                                polyList3D[vertexSize3D].color.G = 0xff;
                                polyList3D[vertexSize3D].color.B = 0xff;
                                polyList3D[vertexSize3D].color.A = 0xff;
                                vertexSize3D = (ushort)(vertexSize3D + 1);
                                indexSize3D = (ushort)(indexSize3D + 2);
                                break;
                            }
                            case 2:
                            {
                                polyList3D[vertexSize3D].position.X = (float)sinValue512;
                                polyList3D[vertexSize3D].position.Y = 0.0f;
                                polyList3D[vertexSize3D].position.Z = (float)(cosValue512 + 16);
                                polyList3D[vertexSize3D].texCoord.X = tileUVArray[numArray[num5] + num];
                                num++;
                                polyList3D[vertexSize3D].texCoord.Y = tileUVArray[numArray[num5] + num];
                                num++;
                                polyList3D[vertexSize3D].color.R = 0xff;
                                polyList3D[vertexSize3D].color.G = 0xff;
                                polyList3D[vertexSize3D].color.B = 0xff;
                                polyList3D[vertexSize3D].color.A = 0xff;
                                vertexSize3D = (ushort)(vertexSize3D + 1);
                                polyList3D[vertexSize3D].position.X = (float)(sinValue512 + 16);
                                polyList3D[vertexSize3D].position.Y = 0.0f;
                                polyList3D[vertexSize3D].position.Z = polyList3D[vertexSize3D - 1].position.Z;
                                polyList3D[vertexSize3D].texCoord.X = tileUVArray[numArray[num5] + num];
                                num++;
                                polyList3D[vertexSize3D].texCoord.Y = polyList3D[vertexSize3D - 1].texCoord.Y;
                                polyList3D[vertexSize3D].color.R = 0xff;
                                polyList3D[vertexSize3D].color.G = 0xff;
                                polyList3D[vertexSize3D].color.B = 0xff;
                                polyList3D[vertexSize3D].color.A = 0xff;
                                vertexSize3D = (ushort)(vertexSize3D + 1);
                                polyList3D[vertexSize3D].position.X = polyList3D[vertexSize3D - 2].position.X;
                                polyList3D[vertexSize3D].position.Y = 0.0f;
                                polyList3D[vertexSize3D].position.Z = (float)cosValue512;
                                polyList3D[vertexSize3D].texCoord.X = polyList3D[vertexSize3D - 2].texCoord.X;
                                polyList3D[vertexSize3D].texCoord.Y = tileUVArray[numArray[num5] + num];
                                polyList3D[vertexSize3D].color.R = 0xff;
                                polyList3D[vertexSize3D].color.G = 0xff;
                                polyList3D[vertexSize3D].color.B = 0xff;
                                polyList3D[vertexSize3D].color.A = 0xff;
                                vertexSize3D = (ushort)(vertexSize3D + 1);
                                polyList3D[vertexSize3D].position.X = polyList3D[vertexSize3D - 2].position.X;
                                polyList3D[vertexSize3D].position.Y = 0.0f;
                                polyList3D[vertexSize3D].position.Z = polyList3D[vertexSize3D - 1].position.Z;
                                polyList3D[vertexSize3D].texCoord.X = polyList3D[vertexSize3D - 2].texCoord.X;
                                polyList3D[vertexSize3D].texCoord.Y = polyList3D[vertexSize3D - 1].texCoord.Y;
                                polyList3D[vertexSize3D].color.R = 0xff;
                                polyList3D[vertexSize3D].color.G = 0xff;
                                polyList3D[vertexSize3D].color.B = 0xff;
                                polyList3D[vertexSize3D].color.A = 0xff;
                                vertexSize3D = (ushort)(vertexSize3D + 1);
                                indexSize3D = (ushort)(indexSize3D + 2);
                                break;
                            }
                            case 3:
                            {
                                polyList3D[vertexSize3D].position.X = (float)(sinValue512 + 16);
                                polyList3D[vertexSize3D].position.Y = 0.0f;
                                polyList3D[vertexSize3D].position.Z = (float)(cosValue512 + 16);
                                polyList3D[vertexSize3D].texCoord.X = tileUVArray[numArray[num5] + num];
                                num++;
                                polyList3D[vertexSize3D].texCoord.Y = tileUVArray[numArray[num5] + num];
                                num++;
                                polyList3D[vertexSize3D].color.R = 0xff;
                                polyList3D[vertexSize3D].color.G = 0xff;
                                polyList3D[vertexSize3D].color.B = 0xff;
                                polyList3D[vertexSize3D].color.A = 0xff;
                                vertexSize3D = (ushort)(vertexSize3D + 1);
                                polyList3D[vertexSize3D].position.X = (float)sinValue512;
                                polyList3D[vertexSize3D].position.Y = 0.0f;
                                polyList3D[vertexSize3D].position.Z = polyList3D[vertexSize3D - 1].position.Z;
                                polyList3D[vertexSize3D].texCoord.X = tileUVArray[numArray[num5] + num];
                                num++;
                                polyList3D[vertexSize3D].texCoord.Y = polyList3D[vertexSize3D - 1].texCoord.Y;
                                polyList3D[vertexSize3D].color.R = 0xff;
                                polyList3D[vertexSize3D].color.G = 0xff;
                                polyList3D[vertexSize3D].color.B = 0xff;
                                polyList3D[vertexSize3D].color.A = 0xff;
                                vertexSize3D = (ushort)(vertexSize3D + 1);
                                polyList3D[vertexSize3D].position.X = polyList3D[vertexSize3D - 2].position.X;
                                polyList3D[vertexSize3D].position.Y = 0.0f;
                                polyList3D[vertexSize3D].position.Z = (float)cosValue512;
                                polyList3D[vertexSize3D].texCoord.X = polyList3D[vertexSize3D - 2].texCoord.X;
                                polyList3D[vertexSize3D].texCoord.Y = tileUVArray[numArray[num5] + num];
                                polyList3D[vertexSize3D].color.R = 0xff;
                                polyList3D[vertexSize3D].color.G = 0xff;
                                polyList3D[vertexSize3D].color.B = 0xff;
                                polyList3D[vertexSize3D].color.A = 0xff;
                                vertexSize3D = (ushort)(vertexSize3D + 1);
                                polyList3D[vertexSize3D].position.X = polyList3D[vertexSize3D - 2].position.X;
                                polyList3D[vertexSize3D].position.Y = 0.0f;
                                polyList3D[vertexSize3D].position.Z = polyList3D[vertexSize3D - 1].position.Z;
                                polyList3D[vertexSize3D].texCoord.X = polyList3D[vertexSize3D - 2].texCoord.X;
                                polyList3D[vertexSize3D].texCoord.Y = polyList3D[vertexSize3D - 1].texCoord.Y;
                                polyList3D[vertexSize3D].color.R = 0xff;
                                polyList3D[vertexSize3D].color.G = 0xff;
                                polyList3D[vertexSize3D].color.B = 0xff;
                                polyList3D[vertexSize3D].color.A = 0xff;
                                vertexSize3D = (ushort)(vertexSize3D + 1);
                                indexSize3D = (ushort)(indexSize3D + 2);
                                break;
                            }
                        }
                    }
                }
                sinValue512 = sinValue512 + 16;
            }
            sinValue512 = sinValue512 - 0x200;
            cosValue512 = cosValue512 + 16;
        }
    }
    else
    {
        sinValue512 = (stageLayouts[activeTileLayers[layerNum]].xPos >> 16) - 160;
        sinValue512 = sinValue512 + SinValue512[stageLayouts[activeTileLayers[layerNum]].angle] / 3;
        sinValue512 = sinValue512 >> 4 << 4;
        cosValue512 = (stageLayouts[activeTileLayers[layerNum]].zPos >> 16) - 160;
        cosValue512 = cosValue512 + CosValue512[stageLayouts[activeTileLayers[layerNum]].angle] / 3;
        cosValue512 = cosValue512 >> 4 << 4;
        for (i = 20; i > 0; i--)
        {
            for (j = 20; j > 0; j--)
            {
                if (sinValue512 > -1 && sinValue512 < num6 && cosValue512 > -1 && cosValue512 < num7)
                {
                    num1 = sinValue512 >> 7;
                    num2 = cosValue512 >> 7;
                    num3 = (sinValue512 & 127) >> 4;
                    num4 = (cosValue512 & 127) >> 4;
                    num5 = numArray2[num1 + (num2 << 8)] << 6;
                    num5 = num5 + num3 + (num4 << 3);
                    if (numArray[num5] > 0)
                    {
                        num = 0;
                        switch (numArray1[num5])
                        {
                            case 0:
                            {
                                polyList3D[vertexSize3D].position.X = (float)sinValue512;
                                polyList3D[vertexSize3D].position.Y = 0.0f;
                                polyList3D[vertexSize3D].position.Z = (float)cosValue512;
                                polyList3D[vertexSize3D].texCoord.X = tileUVArray[numArray[num5] + num];
                                num++;
                                polyList3D[vertexSize3D].texCoord.Y = tileUVArray[numArray[num5] + num];
                                num++;
                                polyList3D[vertexSize3D].color.R = 0xff;
                                polyList3D[vertexSize3D].color.G = 0xff;
                                polyList3D[vertexSize3D].color.B = 0xff;
                                polyList3D[vertexSize3D].color.A = 0xff;
                                vertexSize3D = (ushort)(vertexSize3D + 1);
                                polyList3D[vertexSize3D].position.X = (float)(sinValue512 + 16);
                                polyList3D[vertexSize3D].position.Y = 0.0f;
                                polyList3D[vertexSize3D].position.Z = polyList3D[vertexSize3D - 1].position.Z;
                                polyList3D[vertexSize3D].texCoord.X = tileUVArray[numArray[num5] + num];
                                num++;
                                polyList3D[vertexSize3D].texCoord.Y = polyList3D[vertexSize3D - 1].texCoord.Y;
                                polyList3D[vertexSize3D].color.R = 0xff;
                                polyList3D[vertexSize3D].color.G = 0xff;
                                polyList3D[vertexSize3D].color.B = 0xff;
                                polyList3D[vertexSize3D].color.A = 0xff;
                                vertexSize3D = (ushort)(vertexSize3D + 1);
                                polyList3D[vertexSize3D].position.X = polyList3D[vertexSize3D - 2].position.X;
                                polyList3D[vertexSize3D].position.Y = 0.0f;
                                polyList3D[vertexSize3D].position.Z = (float)(cosValue512 + 16);
                                polyList3D[vertexSize3D].texCoord.X = polyList3D[vertexSize3D - 2].texCoord.X;
                                polyList3D[vertexSize3D].texCoord.Y = tileUVArray[numArray[num5] + num];
                                polyList3D[vertexSize3D].color.R = 0xff;
                                polyList3D[vertexSize3D].color.G = 0xff;
                                polyList3D[vertexSize3D].color.B = 0xff;
                                polyList3D[vertexSize3D].color.A = 0xff;
                                vertexSize3D = (ushort)(vertexSize3D + 1);
                                polyList3D[vertexSize3D].position.X = polyList3D[vertexSize3D - 2].position.X;
                                polyList3D[vertexSize3D].position.Y = 0.0f;
                                polyList3D[vertexSize3D].position.Z = polyList3D[vertexSize3D - 1].position.Z;
                                polyList3D[vertexSize3D].texCoord.X = polyList3D[vertexSize3D - 2].texCoord.X;
                                polyList3D[vertexSize3D].texCoord.Y = polyList3D[vertexSize3D - 1].texCoord.Y;
                                polyList3D[vertexSize3D].color.R = 0xff;
                                polyList3D[vertexSize3D].color.G = 0xff;
                                polyList3D[vertexSize3D].color.B = 0xff;
                                polyList3D[vertexSize3D].color.A = 0xff;
                                vertexSize3D = (ushort)(vertexSize3D + 1);
                                indexSize3D = (ushort)(indexSize3D + 2);
                                break;
                            }
                            case 1:
                            {
                                polyList3D[vertexSize3D].position.X = (float)(sinValue512 + 16);
                                polyList3D[vertexSize3D].position.Y = 0.0f;
                                polyList3D[vertexSize3D].position.Z = (float)cosValue512;
                                polyList3D[vertexSize3D].texCoord.X = tileUVArray[numArray[num5] + num];
                                num++;
                                polyList3D[vertexSize3D].texCoord.Y = tileUVArray[numArray[num5] + num];
                                num++;
                                polyList3D[vertexSize3D].color.R = 0xff;
                                polyList3D[vertexSize3D].color.G = 0xff;
                                polyList3D[vertexSize3D].color.B = 0xff;
                                polyList3D[vertexSize3D].color.A = 0xff;
                                vertexSize3D = (ushort)(vertexSize3D + 1);
                                polyList3D[vertexSize3D].position.X = (float)sinValue512;
                                polyList3D[vertexSize3D].position.Y = 0.0f;
                                polyList3D[vertexSize3D].position.Z = polyList3D[vertexSize3D - 1].position.Z;
                                polyList3D[vertexSize3D].texCoord.X = tileUVArray[numArray[num5] + num];
                                num++;
                                polyList3D[vertexSize3D].texCoord.Y = polyList3D[vertexSize3D - 1].texCoord.Y;
                                polyList3D[vertexSize3D].color.R = 0xff;
                                polyList3D[vertexSize3D].color.G = 0xff;
                                polyList3D[vertexSize3D].color.B = 0xff;
                                polyList3D[vertexSize3D].color.A = 0xff;
                                vertexSize3D = (ushort)(vertexSize3D + 1);
                                polyList3D[vertexSize3D].position.X = polyList3D[vertexSize3D - 2].position.X;
                                polyList3D[vertexSize3D].position.Y = 0.0f;
                                polyList3D[vertexSize3D].position.Z = (float)(cosValue512 + 16);
                                polyList3D[vertexSize3D].texCoord.X = polyList3D[vertexSize3D - 2].texCoord.X;
                                polyList3D[vertexSize3D].texCoord.Y = tileUVArray[numArray[num5] + num];
                                polyList3D[vertexSize3D].color.R = 0xff;
                                polyList3D[vertexSize3D].color.G = 0xff;
                                polyList3D[vertexSize3D].color.B = 0xff;
                                polyList3D[vertexSize3D].color.A = 0xff;
                                vertexSize3D = (ushort)(vertexSize3D + 1);
                                polyList3D[vertexSize3D].position.X = polyList3D[vertexSize3D - 2].position.X;
                                polyList3D[vertexSize3D].position.Y = 0.0f;
                                polyList3D[vertexSize3D].position.Z = polyList3D[vertexSize3D - 1].position.Z;
                                polyList3D[vertexSize3D].texCoord.X = polyList3D[vertexSize3D - 2].texCoord.X;
                                polyList3D[vertexSize3D].texCoord.Y = polyList3D[vertexSize3D - 1].texCoord.Y;
                                polyList3D[vertexSize3D].color.R = 0xff;
                                polyList3D[vertexSize3D].color.G = 0xff;
                                polyList3D[vertexSize3D].color.B = 0xff;
                                polyList3D[vertexSize3D].color.A = 0xff;
                                vertexSize3D = (ushort)(vertexSize3D + 1);
                                indexSize3D = (ushort)(indexSize3D + 2);
                                break;
                            }
                            case 2:
                            {
                                polyList3D[vertexSize3D].position.X = (float)sinValue512;
                                polyList3D[vertexSize3D].position.Y = 0.0f;
                                polyList3D[vertexSize3D].position.Z = (float)(cosValue512 + 16);
                                polyList3D[vertexSize3D].texCoord.X = tileUVArray[numArray[num5] + num];
                                num++;
                                polyList3D[vertexSize3D].texCoord.Y = tileUVArray[numArray[num5] + num];
                                num++;
                                polyList3D[vertexSize3D].color.R = 0xff;
                                polyList3D[vertexSize3D].color.G = 0xff;
                                polyList3D[vertexSize3D].color.B = 0xff;
                                polyList3D[vertexSize3D].color.A = 0xff;
                                vertexSize3D = (ushort)(vertexSize3D + 1);
                                polyList3D[vertexSize3D].position.X = (float)(sinValue512 + 16);
                                polyList3D[vertexSize3D].position.Y = 0.0f;
                                polyList3D[vertexSize3D].position.Z = polyList3D[vertexSize3D - 1].position.Z;
                                polyList3D[vertexSize3D].texCoord.X = tileUVArray[numArray[num5] + num];
                                num++;
                                polyList3D[vertexSize3D].texCoord.Y = polyList3D[vertexSize3D - 1].texCoord.Y;
                                polyList3D[vertexSize3D].color.R = 0xff;
                                polyList3D[vertexSize3D].color.G = 0xff;
                                polyList3D[vertexSize3D].color.B = 0xff;
                                polyList3D[vertexSize3D].color.A = 0xff;
                                vertexSize3D = (ushort)(vertexSize3D + 1);
                                polyList3D[vertexSize3D].position.X = polyList3D[vertexSize3D - 2].position.X;
                                polyList3D[vertexSize3D].position.Y = 0.0f;
                                polyList3D[vertexSize3D].position.Z = (float)cosValue512;
                                polyList3D[vertexSize3D].texCoord.X = polyList3D[vertexSize3D - 2].texCoord.X;
                                polyList3D[vertexSize3D].texCoord.Y = tileUVArray[numArray[num5] + num];
                                polyList3D[vertexSize3D].color.R = 0xff;
                                polyList3D[vertexSize3D].color.G = 0xff;
                                polyList3D[vertexSize3D].color.B = 0xff;
                                polyList3D[vertexSize3D].color.A = 0xff;
                                vertexSize3D = (ushort)(vertexSize3D + 1);
                                polyList3D[vertexSize3D].position.X = polyList3D[vertexSize3D - 2].position.X;
                                polyList3D[vertexSize3D].position.Y = 0.0f;
                                polyList3D[vertexSize3D].position.Z = polyList3D[vertexSize3D - 1].position.Z;
                                polyList3D[vertexSize3D].texCoord.X = polyList3D[vertexSize3D - 2].texCoord.X;
                                polyList3D[vertexSize3D].texCoord.Y = polyList3D[vertexSize3D - 1].texCoord.Y;
                                polyList3D[vertexSize3D].color.R = 0xff;
                                polyList3D[vertexSize3D].color.G = 0xff;
                                polyList3D[vertexSize3D].color.B = 0xff;
                                polyList3D[vertexSize3D].color.A = 0xff;
                                vertexSize3D = (ushort)(vertexSize3D + 1);
                                indexSize3D = (ushort)(indexSize3D + 2);
                                break;
                            }
                            case 3:
                            {
                                polyList3D[vertexSize3D].position.X = (float)(sinValue512 + 16);
                                polyList3D[vertexSize3D].position.Y = 0.0f;
                                polyList3D[vertexSize3D].position.Z = (float)(cosValue512 + 16);
                                polyList3D[vertexSize3D].texCoord.X = tileUVArray[numArray[num5] + num];
                                num++;
                                polyList3D[vertexSize3D].texCoord.Y = tileUVArray[numArray[num5] + num];
                                num++;
                                polyList3D[vertexSize3D].color.R = 0xff;
                                polyList3D[vertexSize3D].color.G = 0xff;
                                polyList3D[vertexSize3D].color.B = 0xff;
                                polyList3D[vertexSize3D].color.A = 0xff;
                                vertexSize3D = (ushort)(vertexSize3D + 1);
                                polyList3D[vertexSize3D].position.X = (float)sinValue512;
                                polyList3D[vertexSize3D].position.Y = 0.0f;
                                polyList3D[vertexSize3D].position.Z = polyList3D[vertexSize3D - 1].position.Z;
                                polyList3D[vertexSize3D].texCoord.X = tileUVArray[numArray[num5] + num];
                                num++;
                                polyList3D[vertexSize3D].texCoord.Y = polyList3D[vertexSize3D - 1].texCoord.Y;
                                polyList3D[vertexSize3D].color.R = 0xff;
                                polyList3D[vertexSize3D].color.G = 0xff;
                                polyList3D[vertexSize3D].color.B = 0xff;
                                polyList3D[vertexSize3D].color.A = 0xff;
                                vertexSize3D = (ushort)(vertexSize3D + 1);
                                polyList3D[vertexSize3D].position.X = polyList3D[vertexSize3D - 2].position.X;
                                polyList3D[vertexSize3D].position.Y = 0.0f;
                                polyList3D[vertexSize3D].position.Z = (float)cosValue512;
                                polyList3D[vertexSize3D].texCoord.X = polyList3D[vertexSize3D - 2].texCoord.X;
                                polyList3D[vertexSize3D].texCoord.Y = tileUVArray[numArray[num5] + num];
                                polyList3D[vertexSize3D].color.R = 0xff;
                                polyList3D[vertexSize3D].color.G = 0xff;
                                polyList3D[vertexSize3D].color.B = 0xff;
                                polyList3D[vertexSize3D].color.A = 0xff;
                                vertexSize3D = (ushort)(vertexSize3D + 1);
                                polyList3D[vertexSize3D].position.X = polyList3D[vertexSize3D - 2].position.X;
                                polyList3D[vertexSize3D].position.Y = 0.0f;
                                polyList3D[vertexSize3D].position.Z = polyList3D[vertexSize3D - 1].position.Z;
                                polyList3D[vertexSize3D].texCoord.X = polyList3D[vertexSize3D - 2].texCoord.X;
                                polyList3D[vertexSize3D].texCoord.Y = polyList3D[vertexSize3D - 1].texCoord.Y;
                                polyList3D[vertexSize3D].color.R = 0xff;
                                polyList3D[vertexSize3D].color.G = 0xff;
                                polyList3D[vertexSize3D].color.B = 0xff;
                                polyList3D[vertexSize3D].color.A = 0xff;
                                vertexSize3D = (ushort)(vertexSize3D + 1);
                                indexSize3D = (ushort)(indexSize3D + 2);
                                break;
                            }
                        }
                    }
                }
                sinValue512 = sinValue512 + 16;
            }
            sinValue512 = sinValue512 - 0x140;
            cosValue512 = cosValue512 + 16;
        }
    }*/
    floor3DPos.X = (float)(stageLayouts[activeTileLayers[layerNum]].xPos >> 8) * -0.00390625f;
    floor3DPos.Y = (float)(stageLayouts[activeTileLayers[layerNum]].yPos >> 8) * 0.00390625f;
    floor3DPos.Z = (float)(stageLayouts[activeTileLayers[layerNum]].zPos >> 8) * -0.00390625f;
    floor3DAngle = (float)stageLayouts[activeTileLayers[layerNum]].angle / 512.0f * -360.0f;
    render3DEnabled = true;
}

void StageSystem_DrawHLineScrollLayer8(uint8_t layerNum)
{
    ushort* numArray;
    uint8_t* numArray1;
    int num;
    int num1;
    int num2;
    int num3;
    int num4;
    int num5;
    int num6;
    int i;
    int num7;
    uint8_t num8;
    int* numArray2;
    int* numArray3;
    int num9;
    int num10;
    int num11 = 0;
    int* numArray4 = tile128x128.gfxDataPos;
    uint8_t* numArray5 = tile128x128.direction;
    uint8_t* numArray6 = tile128x128.visualPlane;
    int num12 = stageLayouts[activeTileLayers[layerNum]].xSize;
    int num13 = stageLayouts[activeTileLayers[layerNum]].ySize;
    int sCREENXSIZE = (SCREEN_XSIZE >> 4) + 3;
    bool flag = false;
    num8 = (uint8_t)((layerNum < tLayerMidPoint ? 0 : 1));
    if (activeTileLayers[layerNum] != 0)
    {
        numArray = stageLayouts[activeTileLayers[layerNum]].tileMap;
        num3 = stageLayouts[activeTileLayers[layerNum]].parallaxFactor * yScrollOffset >> 8;
        num13 = num13 << 7;
        struct LayoutMap *layoutMap = &stageLayouts[activeTileLayers[layerNum]];
        layoutMap->scrollPosition = layoutMap->scrollPosition + stageLayouts[activeTileLayers[layerNum]].scrollSpeed;
        if (stageLayouts[activeTileLayers[layerNum]].scrollPosition > num13 << 16)
        {
            struct LayoutMap* layoutMap1 = &stageLayouts[activeTileLayers[layerNum]];
            layoutMap1->scrollPosition = layoutMap1->scrollPosition - (num13 << 16);
        }
        num3 = num3 + (stageLayouts[activeTileLayers[layerNum]].scrollPosition >> 16);
        num3 = num3 % num13;
        num13 = num13 >> 7;
        numArray1 = stageLayouts[activeTileLayers[layerNum]].lineScrollRef;
        num9 = stageLayouts[activeTileLayers[layerNum]].deformationPos + num3 & 0xff;
        num10 = stageLayouts[activeTileLayers[layerNum]].deformationPosW + num3 & 0xff;
        numArray2 = bgDeformationData2;
        numArray3 = bgDeformationData3;
    }
    else
    {
        numArray = stageLayouts[0].tileMap;
        lastXSize = num12;
        num3 = yScrollOffset;
        numArray1 = stageLayouts[0].lineScrollRef;
        hParallax.linePos[0] = xScrollOffset;
        num9 = stageLayouts[0].deformationPos + num3 & 0xff;
        num10 = stageLayouts[0].deformationPosW + num3 & 0xff;
        numArray2 = bgDeformationData0;
        numArray3 = bgDeformationData1;
        num3 = num3 % (num13 << 7);
    }
    uint8_t num14 = stageLayouts[activeTileLayers[layerNum]].type;
    if (num14 == 1)
    {
        if (lastXSize != num12)
        {
            num12 = num12 << 7;
            for (i = 0; i < hParallax.numEntries; i++)
            {
                hParallax.linePos[i] = hParallax.parallaxFactor[i] * xScrollOffset >> 8;
                hParallax.scrollPosition[i] = hParallax.scrollPosition[i] + hParallax.scrollSpeed[i];
                if (hParallax.scrollPosition[i] > num12 << 16)
                {
                    hParallax.scrollPosition[i] = hParallax.scrollPosition[i] - (num12 << 16);
                }
                hParallax.linePos[i] = hParallax.linePos[i] + (hParallax.scrollPosition[i] >> 16);
                hParallax.linePos[i] = hParallax.linePos[i] % num12;
            }
            num12 = num12 >> 7;
        }
        lastXSize = num12;
    }
    else if (num14 != 5)
    {
    }
    if (num3 < 0)
    {
        num3 = num3 + (num13 << 7);
    }
    int num15 = num3 >> 4 << 4;
    num11 = num11 + num15;
    num9 = num9 + (num15 - num3);
    num10 = num10 + (num15 - num3);
    if (num9 < 0)
    {
        num9 = num9 + 0x100;
    }
    if (num10 < 0)
    {
        num10 = num10 + 0x100;
    }
    num15 = -(num3 & 15);
    int num16 = num3 >> 7;
    int num17 = (num3 & 127) >> 4;
    num7 = (num15 != 0 ? 0x110 : 0x100);
    waterDrawPos = waterDrawPos << 4;
    num15 = num15 << 4;
    for (int j = num7; j > 0; j = j - 16)
    {
        int num18 = hParallax.linePos[numArray1[num11]] - 16;
        num11 = num11 + 8;
        if (num18 != hParallax.linePos[numArray1[num11]] - 16)
        {
            flag = true;
        }
        else if (hParallax.deformationEnabled[numArray1[num11]] != 1)
        {
            flag = false;
        }
        else
        {
            num5 = (num15 < waterDrawPos ? numArray2[num9] : numArray3[num10]);
            num9 = num9 + 8;
            num10 = num10 + 8;
            num6 = (num15 + 64 <= waterDrawPos ? numArray2[num9] : numArray3[num10]);
            flag = (num5 != num6 ? true : false);
            num9 = num9 - 8;
            num10 = num10 - 8;
        }
        num11 = num11 - 8;
        if (!flag)
        {
            i = num12 << 7;
            if (num18 < 0)
            {
                num18 = num18 + i;
            }
            if (num18 >= i)
            {
                num18 = num18 - i;
            }
            num = num18 >> 7;
            num1 = (num18 & 127) >> 4;
            num5 = -((num18 & 15) << 4);
            num5 = num5 - 0x100;
            num6 = num5;
            if (hParallax.deformationEnabled[numArray1[num11]] != 1)
            {
                num9 = num9 + 16;
                num10 = num10 + 16;
            }
            else
            {
                num5 = (num15 < waterDrawPos ? num5 - numArray2[num9] : num5 - numArray3[num10]);
                num9 = num9 + 16;
                num10 = num10 + 16;
                num6 = (num15 + 128 <= waterDrawPos ? num6 - numArray2[num9] : num6 - numArray3[num10]);
            }
            num11 = num11 + 16;
            num2 = (num <= -1 || num16 <= -1 ? 0 : numArray[num + (num16 << 8)] << 6);
            num2 = num2 + num1 + (num17 << 3);
            for (i = sCREENXSIZE; i > 0; i--)
            {
                if (numArray6[num2] == num8 && numArray4[num2] > 0)
                {
                    num4 = 0;
                    switch (numArray5[num2])
                    {
                        case 0:
                        {
                            gfxPolyList[gfxVertexSize].position.X = (float)num5;
                            gfxPolyList[gfxVertexSize].position.Y = (float)num15;
                            gfxPolyList[gfxVertexSize].texCoord.X = tileUVArray[numArray4[num2] + num4];
                            num4++;
                            gfxPolyList[gfxVertexSize].texCoord.Y = tileUVArray[numArray4[num2] + num4];
                            num4++;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)(num5 + 0x100);
                            gfxPolyList[gfxVertexSize].position.Y = (float)num15;
                            gfxPolyList[gfxVertexSize].texCoord.X = tileUVArray[numArray4[num2] + num4];
                            num4++;
                            gfxPolyList[gfxVertexSize].texCoord.Y = gfxPolyList[gfxVertexSize - 1].texCoord.Y;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)num6;
                            gfxPolyList[gfxVertexSize].position.Y = (float)(num15 + 0x100);
                            gfxPolyList[gfxVertexSize].texCoord.X = gfxPolyList[gfxVertexSize - 2].texCoord.X;
                            gfxPolyList[gfxVertexSize].texCoord.Y = tileUVArray[numArray4[num2] + num4];
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)(num6 + 0x100);
                            gfxPolyList[gfxVertexSize].position.Y = gfxPolyList[gfxVertexSize - 1].position.Y;
                            gfxPolyList[gfxVertexSize].texCoord.X = gfxPolyList[gfxVertexSize - 2].texCoord.X;
                            gfxPolyList[gfxVertexSize].texCoord.Y = gfxPolyList[gfxVertexSize - 1].texCoord.Y;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxIndexSize = gfxIndexSize + 6;
                            break;
                        }
                        case 1:
                        {
                            gfxPolyList[gfxVertexSize].position.X = (float)(num5 + 0x100);
                            gfxPolyList[gfxVertexSize].position.Y = (float)num15;
                            gfxPolyList[gfxVertexSize].texCoord.X = tileUVArray[numArray4[num2] + num4];
                            num4++;
                            gfxPolyList[gfxVertexSize].texCoord.Y = tileUVArray[numArray4[num2] + num4];
                            num4++;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)num5;
                            gfxPolyList[gfxVertexSize].position.Y = (float)num15;
                            gfxPolyList[gfxVertexSize].texCoord.X = tileUVArray[numArray4[num2] + num4];
                            num4++;
                            gfxPolyList[gfxVertexSize].texCoord.Y = gfxPolyList[gfxVertexSize - 1].texCoord.Y;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)(num6 + 0x100);
                            gfxPolyList[gfxVertexSize].position.Y = (float)(num15 + 0x100);
                            gfxPolyList[gfxVertexSize].texCoord.X = gfxPolyList[gfxVertexSize - 2].texCoord.X;
                            gfxPolyList[gfxVertexSize].texCoord.Y = tileUVArray[numArray4[num2] + num4];
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)num6;
                            gfxPolyList[gfxVertexSize].position.Y = gfxPolyList[gfxVertexSize - 1].position.Y;
                            gfxPolyList[gfxVertexSize].texCoord.X = gfxPolyList[gfxVertexSize - 2].texCoord.X;
                            gfxPolyList[gfxVertexSize].texCoord.Y = gfxPolyList[gfxVertexSize - 1].texCoord.Y;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxIndexSize = gfxIndexSize + 6;
                            break;
                        }
                        case 2:
                        {
                            gfxPolyList[gfxVertexSize].position.X = (float)num6;
                            gfxPolyList[gfxVertexSize].position.Y = (float)(num15 + 0x100);
                            gfxPolyList[gfxVertexSize].texCoord.X = tileUVArray[numArray4[num2] + num4];
                            num4++;
                            gfxPolyList[gfxVertexSize].texCoord.Y = tileUVArray[numArray4[num2] + num4];
                            num4++;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)(num6 + 0x100);
                            gfxPolyList[gfxVertexSize].position.Y = (float)(num15 + 0x100);
                            gfxPolyList[gfxVertexSize].texCoord.X = tileUVArray[numArray4[num2] + num4];
                            num4++;
                            gfxPolyList[gfxVertexSize].texCoord.Y = gfxPolyList[gfxVertexSize - 1].texCoord.Y;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)num5;
                            gfxPolyList[gfxVertexSize].position.Y = (float)num15;
                            gfxPolyList[gfxVertexSize].texCoord.X = gfxPolyList[gfxVertexSize - 2].texCoord.X;
                            gfxPolyList[gfxVertexSize].texCoord.Y = tileUVArray[numArray4[num2] + num4];
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)(num5 + 0x100);
                            gfxPolyList[gfxVertexSize].position.Y = gfxPolyList[gfxVertexSize - 1].position.Y;
                            gfxPolyList[gfxVertexSize].texCoord.X = gfxPolyList[gfxVertexSize - 2].texCoord.X;
                            gfxPolyList[gfxVertexSize].texCoord.Y = gfxPolyList[gfxVertexSize - 1].texCoord.Y;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxIndexSize = gfxIndexSize + 6;
                            break;
                        }
                        case 3:
                        {
                            gfxPolyList[gfxVertexSize].position.X = (float)(num6 + 0x100);
                            gfxPolyList[gfxVertexSize].position.Y = (float)(num15 + 0x100);
                            gfxPolyList[gfxVertexSize].texCoord.X = tileUVArray[numArray4[num2] + num4];
                            num4++;
                            gfxPolyList[gfxVertexSize].texCoord.Y = tileUVArray[numArray4[num2] + num4];
                            num4++;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)num6;
                            gfxPolyList[gfxVertexSize].position.Y = (float)(num15 + 0x100);
                            gfxPolyList[gfxVertexSize].texCoord.X = tileUVArray[numArray4[num2] + num4];
                            num4++;
                            gfxPolyList[gfxVertexSize].texCoord.Y = gfxPolyList[gfxVertexSize - 1].texCoord.Y;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)(num5 + 0x100);
                            gfxPolyList[gfxVertexSize].position.Y = (float)num15;
                            gfxPolyList[gfxVertexSize].texCoord.X = gfxPolyList[gfxVertexSize - 2].texCoord.X;
                            gfxPolyList[gfxVertexSize].texCoord.Y = tileUVArray[numArray4[num2] + num4];
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)num5;
                            gfxPolyList[gfxVertexSize].position.Y = gfxPolyList[gfxVertexSize - 1].position.Y;
                            gfxPolyList[gfxVertexSize].texCoord.X = gfxPolyList[gfxVertexSize - 2].texCoord.X;
                            gfxPolyList[gfxVertexSize].texCoord.Y = gfxPolyList[gfxVertexSize - 1].texCoord.Y;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxIndexSize = gfxIndexSize + 6;
                            break;
                        }
                    }
                }
                num5 = num5 + 0x100;
                num6 = num6 + 0x100;
                num1++;
                if (num1 <= 7)
                {
                    num2++;
                }
                else
                {
                    num++;
                    if (num == num12)
                    {
                        num = 0;
                    }
                    num1 = 0;
                    num2 = numArray[num + (num16 << 8)] << 6;
                    num2 = num2 + num1 + (num17 << 3);
                }
            }
            num15 = num15 + 0x100;
        }
        else
        {
            i = num12 << 7;
            if (num18 < 0)
            {
                num18 = num18 + i;
            }
            if (num18 >= i)
            {
                num18 = num18 - i;
            }
            num = num18 >> 7;
            num1 = (num18 & 127) >> 4;
            num5 = -((num18 & 15) << 4);
            num5 = num5 - 0x100;
            num6 = num5;
            if (hParallax.deformationEnabled[numArray1[num11]] != 1)
            {
                num9 = num9 + 8;
                num10 = num10 + 8;
            }
            else
            {
                num5 = (num15 < waterDrawPos ? num5 - numArray2[num9] : num5 - numArray3[num10]);
                num9 = num9 + 8;
                num10 = num10 + 8;
                num6 = (num15 + 64 <= waterDrawPos ? num6 - numArray2[num9] : num6 - numArray3[num10]);
            }
            num11 = num11 + 8;
            num2 = (num <= -1 || num16 <= -1 ? 0 : numArray[num + (num16 << 8)] << 6);
            num2 = num2 + num1 + (num17 << 3);
            for (i = sCREENXSIZE; i > 0; i--)
            {
                if (numArray6[num2] == num8 && numArray4[num2] > 0)
                {
                    num4 = 0;
                    switch (numArray5[num2])
                    {
                        case 0:
                        {
                            gfxPolyList[gfxVertexSize].position.X = (float)num5;
                            gfxPolyList[gfxVertexSize].position.Y = (float)num15;
                            gfxPolyList[gfxVertexSize].texCoord.X = tileUVArray[numArray4[num2] + num4];
                            num4++;
                            gfxPolyList[gfxVertexSize].texCoord.Y = tileUVArray[numArray4[num2] + num4];
                            num4++;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)(num5 + 0x100);
                            gfxPolyList[gfxVertexSize].position.Y = (float)num15;
                            gfxPolyList[gfxVertexSize].texCoord.X = tileUVArray[numArray4[num2] + num4];
                            num4++;
                            gfxPolyList[gfxVertexSize].texCoord.Y = gfxPolyList[gfxVertexSize - 1].texCoord.Y;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)num6;
                            gfxPolyList[gfxVertexSize].position.Y = (float)(num15 + 128);
                            gfxPolyList[gfxVertexSize].texCoord.X = gfxPolyList[gfxVertexSize - 2].texCoord.X;
                            gfxPolyList[gfxVertexSize].texCoord.Y = tileUVArray[numArray4[num2] + num4] - 8;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)(num6 + 0x100);
                            gfxPolyList[gfxVertexSize].position.Y = gfxPolyList[gfxVertexSize - 1].position.Y;
                            gfxPolyList[gfxVertexSize].texCoord.X = gfxPolyList[gfxVertexSize - 2].texCoord.X;
                            gfxPolyList[gfxVertexSize].texCoord.Y = gfxPolyList[gfxVertexSize - 1].texCoord.Y;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxIndexSize = gfxIndexSize + 6;
                            break;
                        }
                        case 1:
                        {
                            gfxPolyList[gfxVertexSize].position.X = (float)(num5 + 0x100);
                            gfxPolyList[gfxVertexSize].position.Y = (float)num15;
                            gfxPolyList[gfxVertexSize].texCoord.X = tileUVArray[numArray4[num2] + num4];
                            num4++;
                            gfxPolyList[gfxVertexSize].texCoord.Y = tileUVArray[numArray4[num2] + num4];
                            num4++;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)num5;
                            gfxPolyList[gfxVertexSize].position.Y = (float)num15;
                            gfxPolyList[gfxVertexSize].texCoord.X = tileUVArray[numArray4[num2] + num4];
                            num4++;
                            gfxPolyList[gfxVertexSize].texCoord.Y = gfxPolyList[gfxVertexSize - 1].texCoord.Y;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)(num6 + 0x100);
                            gfxPolyList[gfxVertexSize].position.Y = (float)(num15 + 128);
                            gfxPolyList[gfxVertexSize].texCoord.X = gfxPolyList[gfxVertexSize - 2].texCoord.X;
                            gfxPolyList[gfxVertexSize].texCoord.Y = tileUVArray[numArray4[num2] + num4] - 8;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)num6;
                            gfxPolyList[gfxVertexSize].position.Y = gfxPolyList[gfxVertexSize - 1].position.Y;
                            gfxPolyList[gfxVertexSize].texCoord.X = gfxPolyList[gfxVertexSize - 2].texCoord.X;
                            gfxPolyList[gfxVertexSize].texCoord.Y = gfxPolyList[gfxVertexSize - 1].texCoord.Y;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxIndexSize = gfxIndexSize + 6;
                            break;
                        }
                        case 2:
                        {
                            gfxPolyList[gfxVertexSize].position.X = (float)num6;
                            gfxPolyList[gfxVertexSize].position.Y = (float)(num15 + 128);
                            gfxPolyList[gfxVertexSize].texCoord.X = tileUVArray[numArray4[num2] + num4];
                            num4++;
                            gfxPolyList[gfxVertexSize].texCoord.Y = tileUVArray[numArray4[num2] + num4] + 8;
                            num4++;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)(num6 + 0x100);
                            gfxPolyList[gfxVertexSize].position.Y = (float)(num15 + 128);
                            gfxPolyList[gfxVertexSize].texCoord.X = tileUVArray[numArray4[num2] + num4];
                            num4++;
                            gfxPolyList[gfxVertexSize].texCoord.Y = gfxPolyList[gfxVertexSize - 1].texCoord.Y;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)num5;
                            gfxPolyList[gfxVertexSize].position.Y = (float)num15;
                            gfxPolyList[gfxVertexSize].texCoord.X = gfxPolyList[gfxVertexSize - 2].texCoord.X;
                            gfxPolyList[gfxVertexSize].texCoord.Y = tileUVArray[numArray4[num2] + num4];
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)(num5 + 0x100);
                            gfxPolyList[gfxVertexSize].position.Y = gfxPolyList[gfxVertexSize - 1].position.Y;
                            gfxPolyList[gfxVertexSize].texCoord.X = gfxPolyList[gfxVertexSize - 2].texCoord.X;
                            gfxPolyList[gfxVertexSize].texCoord.Y = gfxPolyList[gfxVertexSize - 1].texCoord.Y;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxIndexSize = gfxIndexSize + 6;
                            break;
                        }
                        case 3:
                        {
                            gfxPolyList[gfxVertexSize].position.X = (float)(num6 + 0x100);
                            gfxPolyList[gfxVertexSize].position.Y = (float)(num15 + 128);
                            gfxPolyList[gfxVertexSize].texCoord.X = tileUVArray[numArray4[num2] + num4];
                            num4++;
                            gfxPolyList[gfxVertexSize].texCoord.Y = tileUVArray[numArray4[num2] + num4] + 8;
                            num4++;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)num6;
                            gfxPolyList[gfxVertexSize].position.Y = (float)(num15 + 128);
                            gfxPolyList[gfxVertexSize].texCoord.X = tileUVArray[numArray4[num2] + num4];
                            num4++;
                            gfxPolyList[gfxVertexSize].texCoord.Y = gfxPolyList[gfxVertexSize - 1].texCoord.Y;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)(num5 + 0x100);
                            gfxPolyList[gfxVertexSize].position.Y = (float)num15;
                            gfxPolyList[gfxVertexSize].texCoord.X = gfxPolyList[gfxVertexSize - 2].texCoord.X;
                            gfxPolyList[gfxVertexSize].texCoord.Y = tileUVArray[numArray4[num2] + num4];
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)num5;
                            gfxPolyList[gfxVertexSize].position.Y = gfxPolyList[gfxVertexSize - 1].position.Y;
                            gfxPolyList[gfxVertexSize].texCoord.X = gfxPolyList[gfxVertexSize - 2].texCoord.X;
                            gfxPolyList[gfxVertexSize].texCoord.Y = gfxPolyList[gfxVertexSize - 1].texCoord.Y;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxIndexSize = gfxIndexSize + 6;
                            break;
                        }
                    }
                }
                num5 = num5 + 0x100;
                num6 = num6 + 0x100;
                num1++;
                if (num1 <= 7)
                {
                    num2++;
                }
                else
                {
                    num++;
                    if (num == num12)
                    {
                        num = 0;
                    }
                    num1 = 0;
                    num2 = numArray[num + (num16 << 8)] << 6;
                    num2 = num2 + num1 + (num17 << 3);
                }
            }
            num15 = num15 + 128;
            num18 = hParallax.linePos[numArray1[num11]] - 16;
            i = num12 << 7;
            if (num18 < 0)
            {
                num18 = num18 + i;
            }
            if (num18 >= i)
            {
                num18 = num18 - i;
            }
            num = num18 >> 7;
            num1 = (num18 & 127) >> 4;
            num5 = -((num18 & 15) << 4);
            num5 = num5 - 0x100;
            num6 = num5;
            if (hParallax.deformationEnabled[numArray1[num11]] != 1)
            {
                num9 = num9 + 8;
                num10 = num10 + 8;
            }
            else
            {
                num5 = (num15 < waterDrawPos ? num5 - numArray2[num9] : num5 - numArray3[num10]);
                num9 = num9 + 8;
                num10 = num10 + 8;
                num6 = (num15 + 64 <= waterDrawPos ? num6 - numArray2[num9] : num6 - numArray3[num10]);
            }
            num11 = num11 + 8;
            num2 = (num <= -1 || num16 <= -1 ? 0 : numArray[num + (num16 << 8)] << 6);
            num2 = num2 + num1 + (num17 << 3);
            for (i = sCREENXSIZE; i > 0; i--)
            {
                if (numArray6[num2] == num8 && numArray4[num2] > 0)
                {
                    num4 = 0;
                    switch (numArray5[num2])
                    {
                        case 0:
                        {
                            gfxPolyList[gfxVertexSize].position.X = (float)num5;
                            gfxPolyList[gfxVertexSize].position.Y = (float)num15;
                            gfxPolyList[gfxVertexSize].texCoord.X = tileUVArray[numArray4[num2] + num4];
                            num4++;
                            gfxPolyList[gfxVertexSize].texCoord.Y = tileUVArray[numArray4[num2] + num4] + 8;
                            num4++;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)(num5 + 0x100);
                            gfxPolyList[gfxVertexSize].position.Y = (float)num15;
                            gfxPolyList[gfxVertexSize].texCoord.X = tileUVArray[numArray4[num2] + num4];
                            num4++;
                            gfxPolyList[gfxVertexSize].texCoord.Y = gfxPolyList[gfxVertexSize - 1].texCoord.Y;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)num6;
                            gfxPolyList[gfxVertexSize].position.Y = (float)(num15 + 128);
                            gfxPolyList[gfxVertexSize].texCoord.X = gfxPolyList[gfxVertexSize - 2].texCoord.X;
                            gfxPolyList[gfxVertexSize].texCoord.Y = tileUVArray[numArray4[num2] + num4];
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)(num6 + 0x100);
                            gfxPolyList[gfxVertexSize].position.Y = gfxPolyList[gfxVertexSize - 1].position.Y;
                            gfxPolyList[gfxVertexSize].texCoord.X = gfxPolyList[gfxVertexSize - 2].texCoord.X;
                            gfxPolyList[gfxVertexSize].texCoord.Y = gfxPolyList[gfxVertexSize - 1].texCoord.Y;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxIndexSize = gfxIndexSize + 6;
                            break;
                        }
                        case 1:
                        {
                            gfxPolyList[gfxVertexSize].position.X = (float)(num5 + 0x100);
                            gfxPolyList[gfxVertexSize].position.Y = (float)num15;
                            gfxPolyList[gfxVertexSize].texCoord.X = tileUVArray[numArray4[num2] + num4];
                            num4++;
                            gfxPolyList[gfxVertexSize].texCoord.Y = tileUVArray[numArray4[num2] + num4] + 8;
                            num4++;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)num5;
                            gfxPolyList[gfxVertexSize].position.Y = (float)num15;
                            gfxPolyList[gfxVertexSize].texCoord.X = tileUVArray[numArray4[num2] + num4];
                            num4++;
                            gfxPolyList[gfxVertexSize].texCoord.Y = gfxPolyList[gfxVertexSize - 1].texCoord.Y;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)(num6 + 0x100);
                            gfxPolyList[gfxVertexSize].position.Y = (float)(num15 + 128);
                            gfxPolyList[gfxVertexSize].texCoord.X = gfxPolyList[gfxVertexSize - 2].texCoord.X;
                            gfxPolyList[gfxVertexSize].texCoord.Y = tileUVArray[numArray4[num2] + num4];
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)num6;
                            gfxPolyList[gfxVertexSize].position.Y = gfxPolyList[gfxVertexSize - 1].position.Y;
                            gfxPolyList[gfxVertexSize].texCoord.X = gfxPolyList[gfxVertexSize - 2].texCoord.X;
                            gfxPolyList[gfxVertexSize].texCoord.Y = gfxPolyList[gfxVertexSize - 1].texCoord.Y;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxIndexSize = (ushort)(gfxIndexSize + 6);
                            break;
                        }
                        case 2:
                        {
                            gfxPolyList[gfxVertexSize].position.X = (float)num6;
                            gfxPolyList[gfxVertexSize].position.Y = (float)(num15 + 128);
                            gfxPolyList[gfxVertexSize].texCoord.X = tileUVArray[numArray4[num2] + num4];
                            num4++;
                            gfxPolyList[gfxVertexSize].texCoord.Y = tileUVArray[numArray4[num2] + num4];
                            num4++;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)(num6 + 0x100);
                            gfxPolyList[gfxVertexSize].position.Y = (float)(num15 + 128);
                            gfxPolyList[gfxVertexSize].texCoord.X = tileUVArray[numArray4[num2] + num4];
                            num4++;
                            gfxPolyList[gfxVertexSize].texCoord.Y = gfxPolyList[gfxVertexSize - 1].texCoord.Y;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)num5;
                            gfxPolyList[gfxVertexSize].position.Y = (float)num15;
                            gfxPolyList[gfxVertexSize].texCoord.X = gfxPolyList[gfxVertexSize - 2].texCoord.X;
                            gfxPolyList[gfxVertexSize].texCoord.Y = tileUVArray[numArray4[num2] + num4] - 8;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)(num5 + 0x100);
                            gfxPolyList[gfxVertexSize].position.Y = gfxPolyList[gfxVertexSize - 1].position.Y;
                            gfxPolyList[gfxVertexSize].texCoord.X = gfxPolyList[gfxVertexSize - 2].texCoord.X;
                            gfxPolyList[gfxVertexSize].texCoord.Y = gfxPolyList[gfxVertexSize - 1].texCoord.Y;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxIndexSize = gfxIndexSize + 6;
                            break;
                        }
                        case 3:
                        {
                            gfxPolyList[gfxVertexSize].position.X = (float)(num6 + 0x100);
                            gfxPolyList[gfxVertexSize].position.Y = (float)(num15 + 128);
                            gfxPolyList[gfxVertexSize].texCoord.X = tileUVArray[numArray4[num2] + num4];
                            num4++;
                            gfxPolyList[gfxVertexSize].texCoord.Y = tileUVArray[numArray4[num2] + num4];
                            num4++;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)num6;
                            gfxPolyList[gfxVertexSize].position.Y = (float)(num15 + 128);
                            gfxPolyList[gfxVertexSize].texCoord.X = tileUVArray[numArray4[num2] + num4];
                            num4++;
                            gfxPolyList[gfxVertexSize].texCoord.Y = gfxPolyList[gfxVertexSize - 1].texCoord.Y;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)(num5 + 0x100);
                            gfxPolyList[gfxVertexSize].position.Y = (float)num15;
                            gfxPolyList[gfxVertexSize].texCoord.X = gfxPolyList[gfxVertexSize - 2].texCoord.X;
                            gfxPolyList[gfxVertexSize].texCoord.Y = tileUVArray[numArray4[num2] + num4] - 8;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxPolyList[gfxVertexSize].position.X = (float)num5;
                            gfxPolyList[gfxVertexSize].position.Y = gfxPolyList[gfxVertexSize - 1].position.Y;
                            gfxPolyList[gfxVertexSize].texCoord.X = gfxPolyList[gfxVertexSize - 2].texCoord.X;
                            gfxPolyList[gfxVertexSize].texCoord.Y = gfxPolyList[gfxVertexSize - 1].texCoord.Y;
                            gfxPolyList[gfxVertexSize].color.R = 0xff;
                            gfxPolyList[gfxVertexSize].color.G = 0xff;
                            gfxPolyList[gfxVertexSize].color.B = 0xff;
                            gfxPolyList[gfxVertexSize].color.A = 0xff;
                            gfxVertexSize = gfxVertexSize + 1;
                            gfxIndexSize = gfxIndexSize + 6;
                            break;
                        }
                    }
                }
                num5 = num5 + 0x100;
                num6 = num6 + 0x100;
                num1++;
                if (num1 <= 7)
                {
                    num2++;
                }
                else
                {
                    num++;
                    if (num == num12)
                    {
                        num = 0;
                    }
                    num1 = 0;
                    num2 = numArray[num + (num16 << 8)] << 6;
                    num2 = num2 + num1 + (num17 << 3);
                }
            }
            num15 = num15 + 128;
        }
        num17++;
        if (num17 > 7)
        {
            num16++;
            if (num16 == num13)
            {
                num16 = 0;
                num11 = num11 - (num13 << 7);
            }
            num17 = 0;
        }
    }
    waterDrawPos = waterDrawPos >> 4;
}

void StageSystem_DrawStageGfx()
{
    gfxVertexSize = 0;
    gfxIndexSize = 0;
    waterDrawPos = waterLevel - yScrollOffset;
    if (waterDrawPos < -16)
    {
        waterDrawPos = -16;
    }
    if (waterDrawPos >= 240)
    {
        waterDrawPos = 0x100;
    }
    ObjectSystem_DrawObjectList(0);
    if (activeTileLayers[0] < 9)
    {
        switch (stageLayouts[activeTileLayers[0]].type)
        {
            case 1:
            {
                StageSystem_DrawHLineScrollLayer8(0);
                break;
            }
            case 3:
            {
                StageSystem_Draw3DFloorLayer(0);
                break;
            }
            case 4:
            {
                StageSystem_Draw3DFloorLayer(0);
                break;
            }
        }
    }
    gfxIndexSizeOpaque = gfxIndexSize;
    gfxVertexSizeOpaque = gfxVertexSize;
    ObjectSystem_DrawObjectList(1);
    if (activeTileLayers[1] < 9)
    {
        switch (stageLayouts[activeTileLayers[1]].type)
        {
            case 1:
            {
                StageSystem_DrawHLineScrollLayer8(1);
                break;
            }
            case 3:
            {
                StageSystem_Draw3DFloorLayer(1);
                break;
            }
            case 4:
            {
                StageSystem_Draw3DFloorLayer(1);
                break;
            }
        }
    }
    ObjectSystem_DrawObjectList(2);
    if (activeTileLayers[2] < 9)
    {
        switch (stageLayouts[activeTileLayers[2]].type)
        {
            case 1:
            {
                StageSystem_DrawHLineScrollLayer8(2);
                break;
            }
            case 3:
            {
                StageSystem_Draw3DFloorLayer(2);
                break;
            }
            case 4:
            {
                StageSystem_Draw3DFloorLayer(2);
                break;
            }
        }
    }
    ObjectSystem_DrawObjectList(3);
    ObjectSystem_DrawObjectList(4);
    if (activeTileLayers[3] < 9)
    {
        switch (stageLayouts[activeTileLayers[3]].type)
        {
            case 1:
            {
                StageSystem_DrawHLineScrollLayer8(3);
                break;
            }
            case 3:
            {
                StageSystem_Draw3DFloorLayer(3);
                break;
            }
            case 4:
            {
                StageSystem_Draw3DFloorLayer(3);
                break;
            }
        }
    }
    ObjectSystem_DrawObjectList(5);
    ObjectSystem_DrawObjectList(6);
}

void StageSystem_InitErrorMessage()
{
    xScrollOffset = 0;
    yScrollOffset = 0;
    AudioPlayback_StopMusic();
    AudioPlayback_StopAllSFX();
    AudioPlayback_ReleaseStageSFX();
    fadeMode = 0;
    playerMenuNum = 0;
    gameMode = 0;
    GraphicsSystem_ClearGraphicsData();
    AnimationSystem_ClearAnimationData();
    GraphicsSystem_LoadPalette("MasterPalette.act", 0, 0, 0, 0x100);
    textMenuSurfaceNo = 0;
    GraphicsSystem_LoadGIFFile("Data/Game/SystemText.gif", 0);
    gameMenu[0].alignment = 2;
    gameMenu[0].numSelections = 1;
    gameMenu[0].selection1 = 0;
    gameMenu[1].numVisibleRows = 0;
    gameMenu[1].visibleRowOffset = 0;
    RenderDevice_UpdateHardwareTextures();
    stageMode = 4;
}

void StageSystem_InitFirstStage()
{
    xScrollOffset = 0;
    yScrollOffset = 0;
    printf("Music Init from StageSystem\n");
    AudioPlayback_StopMusic();
    AudioPlayback_StopAllSFX();
    AudioPlayback_ReleaseStageSFX();
    fadeMode = 0;
    playerMenuNum = 0;
    printf("ClearData\n");
    GraphicsSystem_ClearGraphicsData();
    AnimationSystem_ClearAnimationData();
    printf("Load master palette\n");
    GraphicsSystem_LoadPalette("MasterPalette.act", 0, 0, 0, 0x100);
    activeStageList = 0;
    stageMode = 0;
    gameMode = 1;
    stageListPosition = 0;
}

void StageSystem_InitStageSelectMenu()
{
    xScrollOffset = 0;
    yScrollOffset = 0;
    AudioPlayback_StopMusic();
    AudioPlayback_StopAllSFX();
    AudioPlayback_ReleaseStageSFX();
    fadeMode = 0;
    playerMenuNum = 0;
    gameMode = 0;
    GraphicsSystem_ClearGraphicsData();
    AnimationSystem_ClearAnimationData();
    GraphicsSystem_LoadPalette("MasterPalette.act", 0, 0, 0, 0x100);
    textMenuSurfaceNo = 0;
    GraphicsSystem_LoadGIFFile("Data/Game/SystemText.gif", 0);
    stageMode = 0;
    TextSystem_SetupTextMenu(&gameMenu[0], 0);
    TextSystem_AddTextMenuEntry(&gameMenu[0], "RETRO ENGINE DEV MENU");
    TextSystem_AddTextMenuEntry(&gameMenu[0], " ");
    TextSystem_AddTextMenuEntry(&gameMenu[0], "SONIC CD Version");
    TextSystem_AddTextMenuEntry(&gameMenu[0], gameVersion);
    TextSystem_AddTextMenuEntry(&gameMenu[0], " ");
    TextSystem_AddTextMenuEntry(&gameMenu[0], " ");
    TextSystem_AddTextMenuEntry(&gameMenu[0], " ");
    TextSystem_AddTextMenuEntry(&gameMenu[0], "PLAY GAME");
    TextSystem_AddTextMenuEntry(&gameMenu[0], " ");
    TextSystem_AddTextMenuEntry(&gameMenu[0], "STAGE SELECT");
    gameMenu[0].alignment = 2;
    gameMenu[0].numSelections = 2;
    gameMenu[0].selection1 = 0;
    gameMenu[0].selection2 = 7;
    gameMenu[1].numVisibleRows = 0;
    gameMenu[1].visibleRowOffset = 0;
    RenderDevice_UpdateHardwareTextures();
}

void StageSystem_Load128x128Mappings()
{
    struct FileData fileDatum;
    int num = 0;
    uint8_t numArray[2];
    if (FileIO_LoadStageFile("128x128Tiles.bin", stageListPosition, &fileDatum))
    {
        while (num < 0x8000)
        {
            FileIO_ReadByteArray(numArray, 2);
            numArray[0] = (uint8_t)(numArray[0] - (numArray[0] >> 6 << 6));
            tile128x128.visualPlane[num] = (uint8_t)(numArray[0] >> 4);
            numArray[0] = (uint8_t)(numArray[0] - (numArray[0] >> 4 << 4));
            tile128x128.direction[num] = (uint8_t)(numArray[0] >> 2);
            numArray[0] = (uint8_t)(numArray[0] - (numArray[0] >> 2 << 2));
            tile128x128.tile16x16[num] = (ushort)((numArray[0] << 8) + numArray[1]);
            tile128x128.gfxDataPos[num] = tile128x128.tile16x16[num] << 2;
            numArray[0] = FileIO_ReadByte();
            tile128x128.collisionFlag[0][num] = (uint8_t)(numArray[0] >> 4);
            tile128x128.collisionFlag[1][num] = (uint8_t)(numArray[0] - (numArray[0] >> 4 << 4));
            num++;
        }
        FileIO_CloseFile();
    }
}

void StageSystem_LoadActLayout()
{
    int i;
    int k;
    struct FileData fileDatum;
    if (FileIO_LoadActFile(".bin", stageListPosition, &fileDatum))
    {
        uint8_t num = FileIO_ReadByte();
        int j = num;
        titleCardWord2 = (char)num;
        for (i = 0; i < j; i++)
        {
            titleCardText[i] = (char)FileIO_ReadByte();
            if (titleCardText[i] == '-')
            {
                titleCardWord2 = (char)(i + 1);
            }
        }
        titleCardText[i] = '\0';
        for (i = 0; i < 4; i++)
        {
            num = FileIO_ReadByte();
            activeTileLayers[i] = num;
        }
        num = FileIO_ReadByte();
        tLayerMidPoint = num;
        stageLayouts[0].xSize = FileIO_ReadByte();
        stageLayouts[0].ySize = FileIO_ReadByte();
        xBoundary1 = 0;
        newXBoundary1 = 0;
        yBoundary1 = 0;
        newYBoundary1 = 0;
        xBoundary2 = stageLayouts[0].xSize << 7;
        yBoundary2 = stageLayouts[0].ySize << 7;
        waterLevel = yBoundary2 + 128;
        newXBoundary2 = xBoundary2;
        newYBoundary2 = yBoundary2;
        for (i = 0; i < 0x10000; i++)
        {
            stageLayouts[0].tileMap[i] = 0;
        }
        for (i = 0; i < stageLayouts[0].ySize; i++)
        {
            for (j = 0; j < stageLayouts[0].xSize; j++)
            {
                num = FileIO_ReadByte();
                stageLayouts[0].tileMap[(i << 8) + j] = (ushort)(num << 8);
                num = FileIO_ReadByte();
                stageLayouts[0].tileMap[(i << 8) + j] = (ushort)(stageLayouts[0].tileMap[(i << 8) + j] + num);
            }
        }
        num = FileIO_ReadByte();
        i = num;
        for (j = 0; j < i; j++)
        {
            num = FileIO_ReadByte();
            for (k = num; k > 0; k--)
            {
                num = FileIO_ReadByte();
            }
        }
        num = FileIO_ReadByte();
        k = num << 8;
        num = FileIO_ReadByte();
        k = k + num;
        j = 32;
        for (i = 0; i < k; i++)
        {
            num = FileIO_ReadByte();
            objectEntityList[j].type = num;
            num = FileIO_ReadByte();
            objectEntityList[j].propertyValue = num;
            num = FileIO_ReadByte();
            objectEntityList[j].xPos = num << 8;
            num = FileIO_ReadByte();
            struct ObjectEntity* objectEntity = &objectEntityList[j];
            objectEntity->xPos = objectEntity->xPos + num;
            struct ObjectEntity* objectEntity1 = &objectEntityList[j];
            objectEntity1->xPos = objectEntity1->xPos << 16;
            num = FileIO_ReadByte();
            objectEntityList[j].yPos = num << 8;
            num = FileIO_ReadByte();
            struct ObjectEntity* objectEntity2 = &objectEntityList[j];
            objectEntity2->yPos = objectEntity2->yPos + num;
            struct ObjectEntity* objectEntity3 = &objectEntityList[j];
            objectEntity3->yPos = objectEntity3->yPos << 16;
            j++;
        }
        stageLayouts[0].type = 1;
        FileIO_CloseFile();
    }
}

void StageSystem_LoadStageBackground()
{
    int i;
    int j;
    struct FileData fileDatum;
    uint8_t num = 0;
    uint8_t num1 = 0;
    uint8_t numArray[3];
    uint8_t numArray1[2];
    for (i = 0; i < 9; i++)
    {
        stageLayouts[i].type = 0;
        stageLayouts[i].deformationPos = 0;
        stageLayouts[i].deformationPosW = 0;
    }
    for (i = 0; i < 0x100; i++)
    {
        hParallax.scrollPosition[i] = 0;
        vParallax.scrollPosition[i] = 0;
    }
    for (i = 0; i < 0x8000; i++)
    {
        stageLayouts[0].lineScrollRef[i] = 0;
    }
    if (FileIO_LoadStageFile("Backgrounds.bin", stageListPosition, &fileDatum))
    {
        num = FileIO_ReadByte();
        num1 = FileIO_ReadByte();
        hParallax.numEntries = num1;
        for (i = 0; i < hParallax.numEntries; i++)
        {
            num1 = FileIO_ReadByte();
            hParallax.parallaxFactor[i] = num1 << 8;
            num1 = FileIO_ReadByte();
            hParallax.parallaxFactor[i] = hParallax.parallaxFactor[i] + num1;
            num1 = FileIO_ReadByte();
            hParallax.scrollSpeed[i] = num1 << 10;
            hParallax.scrollPosition[i] = 0;
            num1 = FileIO_ReadByte();
            hParallax.deformationEnabled[i] = num1;
        }
        num1 = FileIO_ReadByte();
        vParallax.numEntries = num1;
        for (i = 0; i < vParallax.numEntries; i++)
        {
            num1 = FileIO_ReadByte();
            vParallax.parallaxFactor[i] = num1 << 8;
            num1 = FileIO_ReadByte();
            vParallax.parallaxFactor[i] = vParallax.parallaxFactor[i] + num1;
            num1 = FileIO_ReadByte();
            vParallax.scrollSpeed[i] = num1 << 10;
            vParallax.scrollPosition[i] = 0;
            num1 = FileIO_ReadByte();
            vParallax.deformationEnabled[i] = num1;
        }
        for (i = 1; i < num + 1; i++)
        {
            num1 = FileIO_ReadByte();
            stageLayouts[i].xSize = num1;
            num1 = FileIO_ReadByte();
            stageLayouts[i].ySize = num1;
            num1 = FileIO_ReadByte();
            stageLayouts[i].type = num1;
            num1 = FileIO_ReadByte();
            stageLayouts[i].parallaxFactor = num1 << 8;
            num1 = FileIO_ReadByte();
            struct LayoutMap* layoutMap = &stageLayouts[i];
            layoutMap->parallaxFactor = layoutMap->parallaxFactor + num1;
            num1 = FileIO_ReadByte();
            stageLayouts[i].scrollSpeed = num1 << 10;
            stageLayouts[i].scrollPosition = 0;
            for (j = 0; j < 0x10000; j++)
            {
                stageLayouts[i].tileMap[j] = 0;
            }
            for (j = 0; j < 0x8000; j++)
            {
                stageLayouts[i].lineScrollRef[j] = 0;
            }
            int num2 = 0;
            j = 0;
            while (j < 1)
            {
                numArray[0] = FileIO_ReadByte();
                if (numArray[0] != 0xff)
                {
                    stageLayouts[i].lineScrollRef[num2] = numArray[0];
                    num2++;
                }
                else
                {
                    numArray[1] = FileIO_ReadByte();
                    if (numArray[1] != 0xff)
                    {
                        numArray[2] = FileIO_ReadByte();
                        numArray1[0] = numArray[1];
                        numArray1[1] = (uint8_t)(numArray[2] - 1);
                        for (int k = 0; k < numArray1[1]; k++)
                        {
                            stageLayouts[i].lineScrollRef[num2] = numArray1[0];
                            num2++;
                        }
                    }
                    else
                    {
                        j = 1;
                    }
                }
            }
            for (int l = 0; l < stageLayouts[i].ySize; l++)
            {
                for (j = 0; j < stageLayouts[i].xSize; j++)
                {
                    num1 = FileIO_ReadByte();
                    stageLayouts[i].tileMap[(l << 8) + j] = (ushort)(num1 << 8);
                    num1 = FileIO_ReadByte();
                    stageLayouts[i].tileMap[(l << 8) + j] = (ushort)(stageLayouts[i].tileMap[(l << 8) + j] + num1);
                }
            }
        }
        FileIO_CloseFile();
    }
}

void StageSystem_LoadStageCollisions()
{
    uint8_t num;
    struct FileData fileDatum;
    int i = 0;
    int k = 0;
    int j = 0;
    int num1 = 0;
    if (FileIO_LoadStageFile("CollisionMasks.bin", stageListPosition, &fileDatum))
    {
        for (i = 0; i < 0x400; i++)
        {
            for (j = 0; j < 2; j++)
            {
                uint8_t l = FileIO_ReadByte();
                int num2 = l >> 4;
                tileCollisions[j].flags[i] = (uint8_t)(l & 15);
                l = FileIO_ReadByte();
                tileCollisions[j].angle[i] = l;
                l = FileIO_ReadByte();
                tileCollisions[j].angle[i] = (uint)(tileCollisions[j].angle[i] + (l << 8));
                l = FileIO_ReadByte();
                tileCollisions[j].angle[i] = (uint)(tileCollisions[j].angle[i] + (l << 16));
                l = FileIO_ReadByte();
                tileCollisions[j].angle[i] = (uint)(tileCollisions[j].angle[i] + (l << 24));
                if (num2 != 0)
                {
                    for (k = 0; k < 16; k = k + 2)
                    {
                        l = FileIO_ReadByte();
                        tileCollisions[j].roofMask[num1 + k] = (char)(l >> 4);
                        tileCollisions[j].roofMask[num1 + k + 1] = (char)(l & 15);
                    }
                    l = FileIO_ReadByte();
                    num = 1;
                    for (k = 0; k < 8; k++)
                    {
                        if ((l & num) >= 1)
                        {
                            tileCollisions[j].floorMask[num1 + k + 8] = 0;
                        }
                        else
                        {
                            tileCollisions[j].floorMask[num1 + k + 8] = 64;
                            tileCollisions[j].roofMask[num1 + k + 8] = -64;
                        }
                        num = (uint8_t)(num << 1);
                    }
                    l = FileIO_ReadByte();
                    num = 1;
                    for (k = 0; k < 8; k++)
                    {
                        if ((l & num) >= 1)
                        {
                            tileCollisions[j].floorMask[num1 + k] = 0;
                        }
                        else
                        {
                            tileCollisions[j].floorMask[num1 + k] = 64;
                            tileCollisions[j].roofMask[num1 + k] = -64;
                        }
                        num = (uint8_t)(num << 1);
                    }
                    for (l = 0; l < 16; l = (uint8_t)(l + 1))
                    {
                        k = 0;
                        while (k > -1)
                        {
                            if (k == 16)
                            {
                                tileCollisions[j].leftWallMask[num1 + l] = 64;
                                k = -1;
                            }
                            else if (l > tileCollisions[j].roofMask[num1 + k])
                            {
                                k++;
                            }
                            else
                            {
                                tileCollisions[j].leftWallMask[num1 + l] = (char)k;
                                k = -1;
                            }
                        }
                    }
                    for (l = 0; l < 16; l = (uint8_t)(l + 1))
                    {
                        k = 15;
                        while (k < 16)
                        {
                            if (k == -1)
                            {
                                tileCollisions[j].rightWallMask[num1 + l] = -64;
                                k = 16;
                            }
                            else if (l > tileCollisions[j].roofMask[num1 + k])
                            {
                                k--;
                            }
                            else
                            {
                                tileCollisions[j].rightWallMask[num1 + l] = (char)k;
                                k = 16;
                            }
                        }
                    }
                }
                else
                {
                    for (k = 0; k < 16; k = k + 2)
                    {
                        l = FileIO_ReadByte();
                        tileCollisions[j].floorMask[num1 + k] = (char)(l >> 4);
                        tileCollisions[j].floorMask[num1 + k + 1] = (char)(l & 15);
                    }
                    l = FileIO_ReadByte();
                    num = 1;
                    for (k = 0; k < 8; k++)
                    {
                        if ((l & num) >= 1)
                        {
                            tileCollisions[j].roofMask[num1 + k + 8] = 15;
                        }
                        else
                        {
                            tileCollisions[j].floorMask[num1 + k + 8] = 64;
                            tileCollisions[j].roofMask[num1 + k + 8] = -64;
                        }
                        num = (uint8_t)(num << 1);
                    }
                    l = FileIO_ReadByte();
                    num = 1;
                    for (k = 0; k < 8; k++)
                    {
                        if ((l & num) >= 1)
                        {
                            tileCollisions[j].roofMask[num1 + k] = 15;
                        }
                        else
                        {
                            tileCollisions[j].floorMask[num1 + k] = 64;
                            tileCollisions[j].roofMask[num1 + k] = -64;
                        }
                        num = (uint8_t)(num << 1);
                    }
                    for (l = 0; l < 16; l = (uint8_t)(l + 1))
                    {
                        k = 0;
                        while (k > -1)
                        {
                            if (k == 16)
                            {
                                tileCollisions[j].leftWallMask[num1 + l] = 64;
                                k = -1;
                            }
                            else if (l < tileCollisions[j].floorMask[num1 + k])
                            {
                                k++;
                            }
                            else
                            {
                                tileCollisions[j].leftWallMask[num1 + l] = (char)k;
                                k = -1;
                            }
                        }
                    }
                    for (l = 0; l < 16; l = (uint8_t)(l + 1))
                    {
                        k = 15;
                        while (k < 16)
                        {
                            if (k == -1)
                            {
                                tileCollisions[j].rightWallMask[num1 + l] = -64;
                                k = 16;
                            }
                            else if (l < tileCollisions[j].floorMask[num1 + k])
                            {
                                k--;
                            }
                            else
                            {
                                tileCollisions[j].rightWallMask[num1 + l] = (char)k;
                                k = 16;
                            }
                        }
                    }
                }
            }
            num1 = num1 + 16;
        }
        FileIO_CloseFile();
    }
}

void StageSystem_LoadStageFiles()
{
    int i;
    struct FileData fileDatum;
    uint8_t numArray[3];
    char chrArray[64];
    int num = 1;
    AudioPlayback_StopAllSFX();
    if (!FileIO_CheckCurrentStageFolder(stageListPosition))
    {
        AudioPlayback_ReleaseStageSFX();
        GraphicsSystem_LoadPalette("MasterPalette.act", 0, 0, 0, 0x100);
        ObjectSystem_ClearScriptData();
        for (i = 16; i > 0; i--)
        {
            GraphicsSystem_RemoveGraphicsFile("", i - 1);
        }
        if (FileIO_LoadStageFile("StageConfig.bin", stageListPosition, &fileDatum))
        {
            numArray[0] = FileIO_ReadByte();
            FileIO_CloseFile();
        }
        if (numArray[0] == 1 && FileIO_LoadFile("Data/Game/GameConfig.bin", &fileDatum))
        {
            numArray[0] = FileIO_ReadByte();
            for (i = 0; i < numArray[0]; i++)
            {
                numArray[1] = FileIO_ReadByte();
            }
            numArray[0] = FileIO_ReadByte();
            for (i = 0; i < numArray[0]; i++)
            {
                numArray[1] = FileIO_ReadByte();
            }
            numArray[0] = FileIO_ReadByte();
            for (i = 0; i < numArray[0]; i++)
            {
                numArray[1] = FileIO_ReadByte();
            }
            numArray[0] = FileIO_ReadByte();
            for (i = 0; i < numArray[0]; i++)
            {
                numArray[1] = FileIO_ReadByte();
                FileIO_ReadCharArray(chrArray, (int)numArray[1]);
                chrArray[numArray[1]] = '\0';
                ObjectSystem_SetObjectTypeName(chrArray, num + i);
            }
            if (useByteCode)
            {
                FileIO_GetFileInfo(&fileDatum);
                FileIO_CloseFile();
                ObjectSystem_LoadByteCodeFile(4, num);
                num = num + numArray[0];
                FileIO_SetFileInfo(&fileDatum);
            }
            FileIO_CloseFile();
        }
        if (FileIO_LoadStageFile("StageConfig.bin", stageListPosition, &fileDatum))
        {
            numArray[0] = FileIO_ReadByte();
            for (i = 96; i < 128; i++)
            {
                FileIO_ReadByteArray(numArray, 3);
                GraphicsSystem_SetPaletteEntry((uint8_t)i, numArray[0], numArray[1], numArray[2]);
            }
            numArray[0] = FileIO_ReadByte();
            for (i = 0; i < numArray[0]; i++)
            {
                numArray[1] = FileIO_ReadByte();
                FileIO_ReadCharArray(chrArray, (int)numArray[1]);
                chrArray[numArray[1]] = '\0';
                ObjectSystem_SetObjectTypeName(chrArray, i + num);
            }
            if (useByteCode)
            {
                for (i = 0; i < numArray[0]; i++)
                {
                    numArray[1] = FileIO_ReadByte();
                    FileIO_ReadCharArray(chrArray, (int)numArray[1]);
                    chrArray[numArray[1]] = '\0';
                }
                FileIO_GetFileInfo(&fileDatum);
                FileIO_CloseFile();
                ObjectSystem_LoadByteCodeFile((int)activeStageList, num);
                FileIO_SetFileInfo(&fileDatum);
            }
            numArray[0] = FileIO_ReadByte();
            numStageSFX = numArray[0];
            for (i = 0; i < numArray[0]; i++)
            {
                numArray[1] = FileIO_ReadByte();
                FileIO_ReadCharArray(chrArray, (int)numArray[1]);
                chrArray[numArray[1]] = '\0';
                FileIO_GetFileInfo(&fileDatum);
                FileIO_CloseFile();
                AudioPlayback_LoadSfx(chrArray, i + numGlobalSFX);
                FileIO_SetFileInfo(&fileDatum);
            }
            FileIO_CloseFile();
        }
        GraphicsSystem_LoadStageGIFFile(stageListPosition);
        StageSystem_LoadStageCollisions();
        StageSystem_LoadStageBackground();
    }
    StageSystem_Load128x128Mappings();
    for (i = 0; i < 16; i++)
    {
        AudioPlayback_SetMusicTrack("", i, 0, 0);
    }
    for (i = 0; i < 0x4a0; i++)
    {
        objectEntityList[i].type = 0;
        objectEntityList[i].direction = 0;
        objectEntityList[i].animation = 0;
        objectEntityList[i].prevAnimation = 0;
        objectEntityList[i].animationSpeed = 0;
        objectEntityList[i].animationTimer = 0;
        objectEntityList[i].frame = 0;
        objectEntityList[i].priority = 0;
        objectEntityList[i].direction = 0;
        objectEntityList[i].rotation = 0;
        objectEntityList[i].state = 0;
        objectEntityList[i].propertyValue = 0;
        objectEntityList[i].xPos = 0;
        objectEntityList[i].yPos = 0;
        objectEntityList[i].drawOrder = 3;
        objectEntityList[i].scale = 0x200;
        objectEntityList[i].inkEffect = 0;
        objectEntityList[i].value[0] = 0;
        objectEntityList[i].value[1] = 0;
        objectEntityList[i].value[2] = 0;
        objectEntityList[i].value[3] = 0;
        objectEntityList[i].value[4] = 0;
        objectEntityList[i].value[5] = 0;
        objectEntityList[i].value[6] = 0;
        objectEntityList[i].value[7] = 0;
    }
    StageSystem_LoadActLayout();
    ObjectSystem_ProcessStartupScripts();
    xScrollA = (playerList[0].xPos >> 16) - 160;
    xScrollB = xScrollA + 0x140;
    yScrollA = (playerList[0].yPos >> 16) - 104;
    yScrollB = yScrollA + 240;
}

void StageSystem_ProcessStage()
{
    int i;
    switch (stageMode)
    {
        case 0:
        {
            AudioPlayback_StopMusic();
            fadeMode = 0;
            paletteMode = 0;
            GraphicsSystem_SetActivePalette(0, 0, 0x100);
            cameraEnabled = 1;
            cameraTarget = -1;
            cameraAdjustY = 0;
            xScrollOffset = 0;
            yScrollOffset = 0;
            yScrollA = 0;
            yScrollB = 240;
            xScrollA = 0;
            xScrollB = 0x140;
            xScrollMove = 0;
            yScrollMove = 0;
            screenShakeX = 0;
            screenShakeY = 0;
            numVertices = 0;
            numFaces = 0;
            for (i = 0; i < 2; i++)
            {
                playerList[i].xPos = 0;
                playerList[i].yPos = 0;
                playerList[i].xVelocity = 0;
                playerList[i].yVelocity = 0;
                playerList[i].angle = 0;
                playerList[i].visible = 1;
                playerList[i].collisionPlane = 0;
                playerList[i].collisionMode = 0;
                playerList[i].gravity = 1;
                playerList[i].speed = 0;
                playerList[i].tileCollisions = 1;
                playerList[i].objectInteraction = 1;
                playerList[i].value[0] = 0;
                playerList[i].value[1] = 0;
                playerList[i].value[2] = 0;
                playerList[i].value[3] = 0;
                playerList[i].value[4] = 0;
                playerList[i].value[5] = 0;
                playerList[i].value[6] = 0;
                playerList[i].value[7] = 0;
            }
            pauseEnabled = 0;
            timeEnabled = 0;
            milliSeconds = 0;
            seconds = 0;
            minutes = 0;
            frameCounter = 0;
            StageSystem_ResetBackgroundSettings();
            StageSystem_LoadStageFiles();
            texBufferMode = 0;
            for (i = 0; i < 9; i++)
            {
                if (stageLayouts[i].type == 4)
                {
                    texBufferMode = 1;
                }
            }
            for (i = 0; i < hParallax.numEntries; i++)
            {
                if (hParallax.deformationEnabled[i] == 1)
                {
                    texBufferMode = 1;
                }
            }
            if (tileGfx[0x32002] > 0)
            {
                texBufferMode = 0;
            }
            if (texBufferMode != 0)
            {
                for (i = 0; i < 0x1000; i = i + 4)
                {
                    tileUVArray[i] = (float)((i >> 2) % 28 * 18 + 1) * 0.0009765625f;
                    tileUVArray[i + 1] = (float)((i >> 2) / 28 * 18 + 1) * 0.0009765625f;
                    tileUVArray[i + 2] = tileUVArray[i] + 0.015625f;
                    tileUVArray[i + 3] = tileUVArray[i + 1] + 0.015625f;
                }
                tileUVArray[0xffc] = 0.475585938f;
                tileUVArray[0xffd] = 0.475585938f;
                tileUVArray[0xffe] = 0.491210938f;
                tileUVArray[0xfff] = 0.491210938f;
            }
            else
            {
                for (i = 0; i < 0x1000; i = i + 4)
                {
                    tileUVArray[i] = (float)((i >> 2 & 31) * 16) * 0.0009765625f;
                    tileUVArray[i + 1] = (float)((i >> 2 >> 5) * 16) * 0.0009765625f;
                    tileUVArray[i + 2] = tileUVArray[i] + 0.015625f;
                    tileUVArray[i + 3] = tileUVArray[i + 1] + 0.015625f;
                }
            }
            RenderDevice_UpdateHardwareTextures();
            gfxIndexSize = 0;
            gfxVertexSize = 0;
            gfxIndexSizeOpaque = 0;
            gfxVertexSizeOpaque = 0;
            stageMode = 1;
            return;
        }
        case 1:
        {
            if (fadeMode > 0)
            {
                fadeMode = (uint8_t)(fadeMode - 1);
            }
            if (paletteMode > 0)
            {
                paletteMode = 0;
                texPaletteNum = 0;
            }
            lastXSize = -1;
            lastYSize = -1;
            InputSystem_CheckKeyDown(&gKeyDown, 0xff);
            InputSystem_CheckKeyPress(&gKeyPress, 0xff);
            if (pauseEnabled == 1 && gKeyPress.start == 1)
            {
                stageMode = 2;
                AudioPlayback_PauseSound();
            }
            if (timeEnabled == 1)
            {
                frameCounter = (uint8_t)(frameCounter + 1);
                if (frameCounter == 60)
                {
                    frameCounter = 0;
                    seconds = (uint8_t)(seconds + 1);
                    if (seconds > 59)
                    {
                        seconds = 0;
                        minutes = (uint8_t)(minutes + 1);
                        if (minutes > 59)
                        {
                            minutes = 0;
                        }
                    }
                }
                milliSeconds = (uint8_t)(frameCounter * 100 / 60);
            }
            ObjectSystem_ProcessObjects();
            if (cameraTarget > -1)
            {
                if (cameraEnabled != 1)
                {
                    PlayerSystem_SetPlayerLockedScreenPosition(&playerList[cameraTarget]);
                }
                else
                {
                    switch (cameraStyle)
                    {
                        case 0:
                        {
                            PlayerSystem_SetPlayerScreenPosition(&playerList[cameraTarget]);
                            break;
                        }
                        case 1:
                        {
                            PlayerSystem_SetPlayerScreenPositionCDStyle(&playerList[cameraTarget]);
                            break;
                        }
                        case 2:
                        {
                            PlayerSystem_SetPlayerScreenPositionCDStyle(&playerList[cameraTarget]);
                            break;
                        }
                        case 3:
                        {
                            PlayerSystem_SetPlayerScreenPositionCDStyle(&playerList[cameraTarget]);
                            break;
                        }
                        case 4:
                        {
                            PlayerSystem_SetPlayerHLockedScreenPosition(&playerList[cameraTarget]);
                            break;
                        }
                    }
                }
            }
            StageSystem_DrawStageGfx();
            if (fadeMode > 0)
            {
                GraphicsSystem_DrawRectangle(0, 0, SCREEN_XSIZE, 240, (int)fadeR, (int)fadeG, (int)fadeB, (int)fadeA);
            }
            if (stageMode != 2)
            {
                break;
            }
            gameMode = 8;
            return;
        }
        case 2:
        {
            if (fadeMode > 0)
            {
                fadeMode = (uint8_t)(fadeMode - 1);
            }
            if (paletteMode > 0)
            {
                paletteMode = 0;
                texPaletteNum = 0;
            }
            lastXSize = -1;
            lastYSize = -1;
            InputSystem_CheckKeyDown(&gKeyDown, 0xff);
            InputSystem_CheckKeyPress(&gKeyPress, 0xff);
            gfxIndexSize = 0;
            gfxVertexSize = 0;
            gfxIndexSizeOpaque = 0;
            gfxVertexSizeOpaque = 0;
            ObjectSystem_ProcessPausedObjects();
            ObjectSystem_DrawObjectList(0);
            ObjectSystem_DrawObjectList(1);
            ObjectSystem_DrawObjectList(2);
            ObjectSystem_DrawObjectList(3);
            ObjectSystem_DrawObjectList(4);
            ObjectSystem_DrawObjectList(5);
            ObjectSystem_DrawObjectList(6);
            if (pauseEnabled != 1 || gKeyPress.start != 1)
            {
                break;
            }
            stageMode = 1;
            AudioPlayback_ResumeSound();
            break;
        }
        default:
        {
            return;
        }
    }
}

void StageSystem_ProcessStageSelectMenu()
{
    gfxVertexSize = 0;
    gfxIndexSize = 0;
    GraphicsSystem_ClearScreen(240);
    InputSystem_MenuKeyDown(&gKeyDown, 131);
    GraphicsSystem_DrawSprite(32, 66, 16, 16, 78, 240, 0);
    GraphicsSystem_DrawSprite(32, 178, 16, 16, 95, 240, 0);
    GraphicsSystem_DrawSprite(SCREEN_XSIZE - 32, 208, 16, 16, 112, 240, 0);
    gKeyPress.start = 0;
    gKeyPress.up = 0;
    gKeyPress.down = 0;
    if (gKeyDown.touches > 0)
    {
        if (gKeyDown.touchX[0] < 120)
        {
            if (gKeyDown.touchY[0] < 120)
            {
                if (gKeyDown.up == 0)
                {
                    gKeyPress.up = 1;
                }
                gKeyDown.up = 1;
            }
            else
            {
                if (gKeyDown.down == 0)
                {
                    gKeyPress.down = 1;
                }
                gKeyDown.down = 1;
            }
        }
        if (gKeyDown.touchX[0] > 200)
        {
            if (gKeyDown.start == 0)
            {
                gKeyPress.start = 1;
            }
            gKeyDown.start = 1;
        }
    }
    else
    {
        gKeyDown.start = 0;
        gKeyDown.up = 0;
        gKeyDown.down = 0;
    }
    switch (stageMode)
    {
        case 0:
            if (gKeyPress.down == 1)
            {
                gameMenu[0].selection2 += 2;
            }
            if (gKeyPress.up == 1)
            {
                gameMenu[0].selection2 -= 2;
            }
            if (gameMenu[0].selection2 > 9)
            {
                gameMenu[0].selection2 = 7;
            }
            if (gameMenu[0].selection2 < 7)
            {
                gameMenu[0].selection2 = 9;
            }
            TextSystem_DrawTextMenu(&gameMenu[0], SCREEN_CENTER, 72);
            if (gKeyPress.start == 1)
            {
                if (gameMenu[0].selection2 == 7)
                {
                    stageMode = 0;
                    gameMode = 1;
                    activeStageList = 0;
                    stageListPosition = 0;
                }
                else
                {
                    TextSystem_SetupTextMenu(&gameMenu[0], 0);
                    TextSystem_AddTextMenuEntry(&gameMenu[0], "CHOOSE A PLAYER");
                    TextSystem_SetupTextMenu(&gameMenu[1], 0);
                    TextSystem_LoadConfigListText(&gameMenu[1], 0);
                    gameMenu[1].alignment = 0;
                    gameMenu[1].numSelections = 1;
                    gameMenu[1].selection1 = 0;
                    stageMode = 1;
                }
            }
            break;
        case 1:
            if (gKeyPress.down == 1)
            {
                gameMenu[1].selection1++;
            }
            if (gKeyPress.up == 1)
            {
                gameMenu[1].selection1--;
            }
            if (gameMenu[1].selection1 == (int)gameMenu[1].numRows)
            {
                gameMenu[1].selection1 = 0;
            }
            if (gameMenu[1].selection1 < 0)
            {
                gameMenu[1].selection1 = (int)(gameMenu[1].numRows - 1);
            }
            TextSystem_DrawTextMenu(&gameMenu[0], SCREEN_CENTER - 4, 72);
            TextSystem_DrawTextMenu(&gameMenu[1], SCREEN_CENTER - 40, 96);
            if (gKeyPress.start == 1)
            {
                playerMenuNum = (uint8_t)gameMenu[1].selection1;
                TextSystem_SetupTextMenu(&gameMenu[0], 0);
                TextSystem_AddTextMenuEntry(&gameMenu[0], "SELECT A STAGE LIST");
                TextSystem_AddTextMenuEntry(&gameMenu[0], " ");
                TextSystem_AddTextMenuEntry(&gameMenu[0], " ");
                TextSystem_AddTextMenuEntry(&gameMenu[0], "   PRESENTATION");
                TextSystem_AddTextMenuEntry(&gameMenu[0], " ");
                TextSystem_AddTextMenuEntry(&gameMenu[0], "   REGULAR");
                TextSystem_AddTextMenuEntry(&gameMenu[0], " ");
                TextSystem_AddTextMenuEntry(&gameMenu[0], "   SPECIAL");
                TextSystem_AddTextMenuEntry(&gameMenu[0], " ");
                TextSystem_AddTextMenuEntry(&gameMenu[0], "   BONUS");
                gameMenu[0].alignment = 0;
                gameMenu[0].selection2 = 3;
                stageMode = 2;
            }
            break;
        case 2:
        {
            if (gKeyPress.down == 1)
            {
                gameMenu[0].selection2 += 2;
            }
            if (gKeyPress.up == 1)
            {
                gameMenu[0].selection2 -= 2;
            }
            if (gameMenu[0].selection2 > 9)
            {
                gameMenu[0].selection2 = 3;
            }
            if (gameMenu[0].selection2 < 3)
            {
                gameMenu[0].selection2 = 9;
            }
            TextSystem_DrawTextMenu(&gameMenu[0], SCREEN_CENTER - 80, 72);
            int num = 0;
            switch (gameMenu[0].selection2)
            {
                case 3:
                    if (noPresentationStages > 0)
                    {
                        num = 1;
                    }
                    activeStageList = 0;
                    break;
                case 5:
                    if (noZoneStages > 0)
                    {
                        num = 1;
                    }
                    activeStageList = 1;
                    break;
                case 7:
                    if (noSpecialStages > 0)
                    {
                        num = 1;
                    }
                    activeStageList = 3;
                    break;
                case 9:
                    if (noBonusStages > 0)
                    {
                        num = 1;
                    }
                    activeStageList = 2;
                    break;
            }
            if (gKeyPress.start == 1 && num == 1)
            {
                TextSystem_SetupTextMenu(&gameMenu[0], 0);
                TextSystem_AddTextMenuEntry(&gameMenu[0], "SELECT A STAGE");
                TextSystem_SetupTextMenu(&gameMenu[1], 0);
                TextSystem_LoadConfigListText(&gameMenu[1], 1 + ((gameMenu[0].selection2 - 3) >> 1));
                gameMenu[1].alignment = 1;
                gameMenu[1].numSelections = 3;
                gameMenu[1].selection1 = 0;
                if (gameMenu[1].numRows > 18)
                {
                    gameMenu[1].numVisibleRows = 18;
                }
                gameMenu[0].alignment = 2;
                gameMenu[0].numSelections = 1;
                gameMenu[1].timer = 0;
                stageMode = 3;
            }
            break;
        }
        case 3:
            if (gKeyDown.down == 1)
            {
                gameMenu[1].timer += 1;
                if (gameMenu[1].timer > 4)
                {
                    gameMenu[1].timer = 0;
                    gKeyPress.down = 1;
                }
            }
            else
            {
                if (gKeyDown.up == 1)
                {
                    gameMenu[1].timer -= 1;
                    if (gameMenu[1].timer < -4)
                    {
                        gameMenu[1].timer = 0;
                        gKeyPress.up = 1;
                    }
                }
                else
                {
                    gameMenu[1].timer = 0;
                }
            }
            if (gKeyPress.down == 1)
            {
                gameMenu[1].selection1++;
                if (gameMenu[1].selection1 - (int)gameMenu[1].visibleRowOffset >= (int)gameMenu[1].numVisibleRows)
                {
                    gameMenu[1].visibleRowOffset += 1;
                }
            }
            if (gKeyPress.up == 1)
            {
                gameMenu[1].selection1--;
                if (gameMenu[1].selection1 - (int)gameMenu[1].visibleRowOffset < 0)
                {
                    gameMenu[1].visibleRowOffset -= 1;
                }
            }
            if (gameMenu[1].selection1 == (int)gameMenu[1].numRows)
            {
                gameMenu[1].selection1 = 0;
                gameMenu[1].visibleRowOffset = 0;
            }
            if (gameMenu[1].selection1 < 0)
            {
                gameMenu[1].selection1 = (int)(gameMenu[1].numRows - 1);
                gameMenu[1].visibleRowOffset = (ushort)(gameMenu[1].numRows - gameMenu[1].numVisibleRows);
            }
            TextSystem_DrawTextMenu(&gameMenu[0], SCREEN_CENTER - 4, 40);
            TextSystem_DrawTextMenu(&gameMenu[1], SCREEN_CENTER + 100, 64);
            if (gKeyPress.start == 1)
            {
                if (gKeyDown.touches > 1)
                {
                    debugMode = 1;
                }
                else
                {
                    debugMode = 0;
                }
                stageMode = 0;
                gameMode = 1;
                stageListPosition = gameMenu[1].selection1;
            }
            break;
        case 4:
            TextSystem_DrawTextMenu(&gameMenu[0], SCREEN_CENTER, 72);
            if (gKeyPress.start == 1)
            {
                stageMode = 0;
                TextSystem_SetupTextMenu(&gameMenu[0], 0);
                TextSystem_AddTextMenuEntry(&gameMenu[0], "RETRO ENGINE DEV MENU");
                TextSystem_AddTextMenuEntry(&gameMenu[0], " ");
                TextSystem_AddTextMenuEntry(&gameMenu[0], " ");
                TextSystem_AddTextMenuEntry(&gameMenu[0], " ");
                TextSystem_AddTextMenuEntry(&gameMenu[0], " ");
                TextSystem_AddTextMenuEntry(&gameMenu[0], " ");
                TextSystem_AddTextMenuEntry(&gameMenu[0], " ");
                TextSystem_AddTextMenuEntry(&gameMenu[0], "PLAY GAME");
                TextSystem_AddTextMenuEntry(&gameMenu[0], " ");
                TextSystem_AddTextMenuEntry(&gameMenu[0], "STAGE SELECT");
                gameMenu[0].alignment = 2;
                gameMenu[0].numSelections = 2;
                gameMenu[0].selection1 = 0;
                gameMenu[0].selection2 = 7;
                gameMenu[1].numVisibleRows = 0;
                gameMenu[1].visibleRowOffset = 0;
            }
            break;
    }
    gfxIndexSizeOpaque = gfxIndexSize;
    gfxVertexSizeOpaque = gfxVertexSize;
}

void StageSystem_ResetBackgroundSettings()
{
    int i;
    for (i = 0; i < 9; i++)
    {
        stageLayouts[i].deformationPos = 0;
        stageLayouts[i].deformationPosW = 0;
        stageLayouts[i].scrollPosition = 0;
    }
    for (i = 0; i < 0x100; i++)
    {
        hParallax.scrollPosition[i] = 0;
        vParallax.scrollPosition[i] = 0;
    }
    for (i = 0; i < 0x240; i++)
    {
        bgDeformationData0[i] = 0;
        bgDeformationData1[i] = 0;
        bgDeformationData2[i] = 0;
        bgDeformationData3[i] = 0;
    }
}

void StageSystem_SetLayerDeformation(int selectedDef, int waveLength, int waveWidth, int wType, int yPos, int wSize)
{
    int i = 0;
    int num = 0;
    switch (selectedDef)
    {
        case 0:
        {
            switch (wType)
            {
                case 0:
                {
                    for (i = 0; i < 0x20000; i = i + 0x200)
                    {
                        bgDeformationData0[num] = SinValue512[i / waveLength & 0x1ff] * waveWidth >> 5;
                        num++;
                    }
                    break;
                }
                case 1:
                {
                    num = num + yPos;
                    for (i = 0; i < wSize; i++)
                    {
                        bgDeformationData0[num] = SinValue512[(i << 9) / waveLength & 0x1ff] * waveWidth >> 5;
                        num++;
                    }
                    break;
                }
            }
            for (i = 0x100; i < 0x240; i++)
            {
                bgDeformationData0[i] = bgDeformationData0[i - 0x100];
            }
            return;
        }
        case 1:
        {
            switch (wType)
            {
                case 0:
                {
                    for (i = 0; i < 0x20000; i = i + 0x200)
                    {
                        bgDeformationData1[num] = SinValue512[i / waveLength & 0x1ff] * waveWidth >> 5;
                        num++;
                    }
                    break;
                }
                case 1:
                {
                    num = num + yPos;
                    for (i = 0; i < wSize; i++)
                    {
                        bgDeformationData1[num] = SinValue512[(i << 9) / waveLength & 0x1ff] * waveWidth >> 5;
                        num++;
                    }
                    break;
                }
            }
            for (i = 0x100; i < 0x240; i++)
            {
                bgDeformationData1[i] = bgDeformationData1[i - 0x100];
            }
            return;
        }
        case 2:
        {
            switch (wType)
            {
                case 0:
                {
                    for (i = 0; i < 0x20000; i = i + 0x200)
                    {
                        bgDeformationData2[num] = SinValue512[i / waveLength & 0x1ff] * waveWidth >> 5;
                        num++;
                    }
                    break;
                }
                case 1:
                {
                    num = num + yPos;
                    for (i = 0; i < wSize; i++)
                    {
                        bgDeformationData2[num] = SinValue512[(i << 9) / waveLength & 0x1ff] * waveWidth >> 5;
                        num++;
                    }
                    break;
                }
            }
            for (i = 0x100; i < 0x240; i++)
            {
                bgDeformationData2[i] = bgDeformationData2[i - 0x100];
            }
            return;
        }
        case 3:
        {
            switch (wType)
            {
                case 0:
                {
                    for (i = 0; i < 0x20000; i = i + 0x200)
                    {
                        bgDeformationData3[num] = SinValue512[i / waveLength & 0x1ff] * waveWidth >> 5;
                        num++;
                    }
                    break;
                }
                case 1:
                {
                    num = num + yPos;
                    for (i = 0; i < wSize; i++)
                    {
                        bgDeformationData3[num] = SinValue512[(i << 9) / waveLength & 0x1ff] * waveWidth >> 5;
                        num++;
                    }
                    break;
                }
            }
            for (i = 0x100; i < 0x240; i++)
            {
                bgDeformationData3[i] = bgDeformationData3[i - 0x100];
            }
            return;
        }
        default:
        {
            return;
        }
    }
}
