//
//  Scene3D.c
//  rvm
//

#include "Scene3D.h"

struct Vertex3D vertexBuffer[4096];
struct Vertex3D vertexBufferT[4096];
struct Face3D indexBuffer[1024];
struct SortList drawList[1024];
int numVertices;
int numFaces;
int projectionX;
int projectionY;
int matWorld[16];
int matView[16];
int matFinal[16];
int matTemp[16];

void Init_Scene3D()
{
    numVertices = 0;
    numFaces = 0;
    projectionX = 136;
    projectionY = 160;
}

void Scene3D_SetIdentityMatrix(int* m)
{
    m[0] = 256;
    m[1] = 0;
    m[2] = 0;
    m[3] = 0;
    m[4] = 0;
    m[5] = 256;
    m[6] = 0;
    m[7] = 0;
    m[8] = 0;
    m[9] = 0;
    m[10] = 256;
    m[11] = 0;
    m[12] = 0;
    m[13] = 0;
    m[14] = 0;
    m[15] = 256;
}
void Scene3D_MatrixMultiply(int* a, int* b)
{
    int array[16];
    for (uint num = 0u; num < 16u; num += 1u)
    {
        uint num2 = num & 3u;
        uint num3 = num & 12u;
        array[(int)(num)] = (b[(int)(num2)] * a[(int)(num3)] >> 8) + (b[(int)((num2 + 4u))] * a[(int)((num3 + 1u))] >> 8) + (b[(int)((num2 + 8u))] * a[(int)((num3 + 2u))] >> 8) + (b[(int)((num2 + 12u))] * a[(int)((num3 + 3u))] >> 8);
    }
    for (uint num = 0u; num < 16u; num += 1u)
    {
        a[(int)(num)] = array[(int)(num)];
    }
}
void Scene3D_MatrixTranslateXYZ(int* m, int xPos, int yPos, int zPos)
{
    m[0] = 256;
    m[1] = 0;
    m[2] = 0;
    m[3] = 0;
    m[4] = 0;
    m[5] = 256;
    m[6] = 0;
    m[7] = 0;
    m[8] = 0;
    m[9] = 0;
    m[10] = 256;
    m[11] = 0;
    m[12] = xPos;
    m[13] = yPos;
    m[14] = zPos;
    m[15] = 256;
}
void Scene3D_MatrixScaleXYZ(int* m, int xScale, int yScale, int zScale)
{
    m[0] = xScale;
    m[1] = 0;
    m[2] = 0;
    m[3] = 0;
    m[4] = 0;
    m[5] = yScale;
    m[6] = 0;
    m[7] = 0;
    m[8] = 0;
    m[9] = 0;
    m[10] = zScale;
    m[11] = 0;
    m[12] = 0;
    m[13] = 0;
    m[14] = 0;
    m[15] = 256;
}
void Scene3D_MatrixRotateX(int* m, int angle)
{
    if (angle < 0)
    {
        angle = 512 - angle;
    }
    angle &= 511;
    int num = SinValue512[angle] >> 1;
    int num2 = CosValue512[angle] >> 1;
    m[0] = 256;
    m[1] = 0;
    m[2] = 0;
    m[3] = 0;
    m[4] = 0;
    m[5] = num2;
    m[6] = num;
    m[7] = 0;
    m[8] = 0;
    m[9] = -num;
    m[10] = num2;
    m[11] = 0;
    m[12] = 0;
    m[13] = 0;
    m[14] = 0;
    m[15] = 256;
}
void Scene3D_MatrixRotateY(int* m, int angle)
{
    if (angle < 0)
    {
        angle = 512 - angle;
    }
    angle &= 511;
    int num = SinValue512[angle] >> 1;
    int num2 = CosValue512[angle] >> 1;
    m[0] = num2;
    m[1] = 0;
    m[2] = num;
    m[3] = 0;
    m[4] = 0;
    m[5] = 256;
    m[6] = 0;
    m[7] = 0;
    m[8] = -num;
    m[9] = 0;
    m[10] = num2;
    m[11] = 0;
    m[12] = 0;
    m[13] = 0;
    m[14] = 0;
    m[15] = 256;
}
void Scene3D_MatrixRotateZ(int* m, int angle)
{
    if (angle < 0)
    {
        angle = 512 - angle;
    }
    angle &= 511;
    int num = SinValue512[angle] >> 1;
    int num2 = CosValue512[angle] >> 1;
    m[0] = num2;
    m[1] = 0;
    m[2] = num;
    m[3] = 0;
    m[4] = 0;
    m[5] = 256;
    m[6] = 0;
    m[7] = 0;
    m[8] = -num;
    m[9] = 0;
    m[10] = num2;
    m[11] = 0;
    m[12] = 0;
    m[13] = 0;
    m[14] = 0;
    m[15] = 256;
}
void Scene3D_MatrixRotateXYZ(int* m, int angleX, int angleY, int angleZ)
{
    if (angleX < 0)
    {
        angleX = 512 - angleX;
    }
    angleX &= 511;
    if (angleY < 0)
    {
        angleY = 512 - angleY;
    }
    angleY &= 511;
    if (angleZ < 0)
    {
        angleZ = 512 - angleZ;
    }
    angleZ &= 511;
    int num = SinValue512[angleX] >> 1;
    int num2 = CosValue512[angleX] >> 1;
    int num3 = SinValue512[angleY] >> 1;
    int num4 = CosValue512[angleY] >> 1;
    int num5 = SinValue512[angleZ] >> 1;
    int num6 = CosValue512[angleZ] >> 1;
    m[0] = (num4 * num6 >> 8) + ((num * num3 >> 8) * num5 >> 8);
    m[1] = (num4 * num5 >> 8) - ((num * num3 >> 8) * num6 >> 8);
    m[2] = num2 * num3 >> 8;
    m[3] = 0;
    m[4] = -num2 * num5 >> 8;
    m[5] = num2 * num6 >> 8;
    m[6] = num;
    m[7] = 0;
    m[8] = ((num * num4 >> 8) * num5 >> 8) - (num3 * num6 >> 8);
    m[9] = (-num3 * num5 >> 8) - ((num * num4 >> 8) * num6 >> 8);
    m[10] = num2 * num4 >> 8;
    m[11] = 0;
    m[12] = 0;
    m[13] = 0;
    m[14] = 0;
    m[15] = 256;
}
void Scene3D_TransformVertexBuffer()
{
    int num = 0;
    int num2 = 0;
    for (int i = 0; i < 16; i++)
    {
        matFinal[i] = matWorld[i];
    }
    Scene3D_MatrixMultiply(matFinal, matView);
    for (int i = 0; i < numVertices; i++)
    {
        struct Vertex3D* vertex3D = &vertexBuffer[num];
        vertexBufferT[num2].x = (matFinal[0] * vertex3D->x >> 8) + (matFinal[4] * vertex3D->y >> 8) + (matFinal[8] * vertex3D->z >> 8) + matFinal[12];
        vertexBufferT[num2].y = (matFinal[1] * vertex3D->x >> 8) + (matFinal[5] * vertex3D->y >> 8) + (matFinal[9] * vertex3D->z >> 8) + matFinal[13];
        vertexBufferT[num2].z = (matFinal[2] * vertex3D->x >> 8) + (matFinal[6] * vertex3D->y >> 8) + (matFinal[10] * vertex3D->z >> 8) + matFinal[14];
        if (vertexBufferT[num2].z < 1 && vertexBufferT[num2].z > 0)
        {
            vertexBufferT[num2].z = 1;
        }
        num++;
        num2++;
    }
}
void Scene3D_TransformVertices(int* m, int vStart, int vEnd)
{
    int num = 0;
    struct Vertex3D vertex3D;
    vEnd++;
    for (int i = vStart; i < vEnd; i++)
    {
        struct Vertex3D* vertex3D2 = &vertexBuffer[i];
        vertex3D.x = (m[0] * vertex3D2->x >> 8) + (m[4] * vertex3D2->y >> 8) + (m[8] * vertex3D2->z >> 8) + m[12];
        vertex3D.y = (m[1] * vertex3D2->x >> 8) + (m[5] * vertex3D2->y >> 8) + (m[9] * vertex3D2->z >> 8) + m[13];
        vertex3D.z = (m[2] * vertex3D2->x >> 8) + (m[6] * vertex3D2->y >> 8) + (m[10] * vertex3D2->z >> 8) + m[14];
        vertexBuffer[i].x = vertex3D.x;
        vertexBuffer[i].y = vertex3D.y;
        vertexBuffer[i].z = vertex3D.z;
        num++;
    }
}
void Scene3D_Sort3DDrawList()
{
    for (int i = 0; i < numFaces; i++)
    {
        drawList[i].z = (vertexBufferT[indexBuffer[i].a].z + vertexBufferT[indexBuffer[i].b].z + vertexBufferT[indexBuffer[i].c].z + vertexBufferT[indexBuffer[i].d].z) >> 2;
        drawList[i].index = i;
    }
    for (int i = 0; i < numFaces; i++)
    {
        for (int j = numFaces - 1; j > i; j--)
        {
            if (drawList[j].z > drawList[j - 1].z)
            {
                int index = drawList[j].index;
                int z = drawList[j].z;
                drawList[j].index = drawList[j - 1].index;
                drawList[j].z = drawList[j - 1].z;
                drawList[j - 1].index = index;
                drawList[j - 1].z = z;
            }
        }
    }
}
void Scene3D_Draw3DScene(int surfaceNum)
{
    struct Quad2D quad2D;
    for (int i = 0; i < numFaces; i++)
    {
        struct Face3D* face3D = &indexBuffer[drawList[i].index];
        switch (face3D->flag)
        {
            case 0:
                if (vertexBufferT[face3D->a].z > 256 && vertexBufferT[face3D->b].z > 256 && vertexBufferT[face3D->c].z > 256 && vertexBufferT[face3D->d].z > 256)
                {
                    quad2D.vertex[0].x = SCREEN_CENTER + vertexBufferT[face3D->a].x * projectionX / vertexBufferT[face3D->a].z;
                    quad2D.vertex[0].y = 120 - vertexBufferT[face3D->a].y * projectionY / vertexBufferT[face3D->a].z;
                    quad2D.vertex[1].x = SCREEN_CENTER + vertexBufferT[face3D->b].x * projectionX / vertexBufferT[face3D->b].z;
                    quad2D.vertex[1].y = 120 - vertexBufferT[face3D->b].y * projectionY / vertexBufferT[face3D->b].z;
                    quad2D.vertex[2].x = SCREEN_CENTER + vertexBufferT[face3D->c].x * projectionX / vertexBufferT[face3D->c].z;
                    quad2D.vertex[2].y = 120 - vertexBufferT[face3D->c].y * projectionY / vertexBufferT[face3D->c].z;
                    quad2D.vertex[3].x = SCREEN_CENTER + vertexBufferT[face3D->d].x * projectionX / vertexBufferT[face3D->d].z;
                    quad2D.vertex[3].y = 120 - vertexBufferT[face3D->d].y * projectionY / vertexBufferT[face3D->d].z;
                    quad2D.vertex[0].u = vertexBuffer[face3D->a].u;
                    quad2D.vertex[0].v = vertexBuffer[face3D->a].v;
                    quad2D.vertex[1].u = vertexBuffer[face3D->b].u;
                    quad2D.vertex[1].v = vertexBuffer[face3D->b].v;
                    quad2D.vertex[2].u = vertexBuffer[face3D->c].u;
                    quad2D.vertex[2].v = vertexBuffer[face3D->c].v;
                    quad2D.vertex[3].u = vertexBuffer[face3D->d].u;
                    quad2D.vertex[3].v = vertexBuffer[face3D->d].v;
                    GraphicsSystem_DrawTexturedQuad(&quad2D, surfaceNum);
                }
                break;
            case 1: //Used for Special Stage background
                quad2D.vertex[0].x = vertexBuffer[face3D->a].x;
                quad2D.vertex[0].y = vertexBuffer[face3D->a].y;
                quad2D.vertex[1].x = vertexBuffer[face3D->b].x;
                quad2D.vertex[1].y = vertexBuffer[face3D->b].y;
                quad2D.vertex[2].x = vertexBuffer[face3D->c].x;
                quad2D.vertex[2].y = vertexBuffer[face3D->c].y;
                quad2D.vertex[3].x = vertexBuffer[face3D->d].x;
                quad2D.vertex[3].y = vertexBuffer[face3D->d].y;
                quad2D.vertex[0].u = vertexBuffer[face3D->a].u;
                quad2D.vertex[0].v = vertexBuffer[face3D->a].v;
                quad2D.vertex[1].u = vertexBuffer[face3D->b].u;
                quad2D.vertex[1].v = vertexBuffer[face3D->b].v;
                quad2D.vertex[2].u = vertexBuffer[face3D->c].u;
                quad2D.vertex[2].v = vertexBuffer[face3D->c].v;
                quad2D.vertex[3].u = vertexBuffer[face3D->d].u;
                quad2D.vertex[3].v = vertexBuffer[face3D->d].v;
                GraphicsSystem_DrawTexturedQuad(&quad2D, surfaceNum);
                break;
            case 2:
                if (vertexBufferT[face3D->a].z > 256 && vertexBufferT[face3D->b].z > 256 && vertexBufferT[face3D->c].z > 256 && vertexBufferT[face3D->d].z > 256)
                {
                    quad2D.vertex[0].x = SCREEN_CENTER + vertexBufferT[face3D->a].x * projectionX / vertexBufferT[face3D->a].z;
                    quad2D.vertex[0].y = 120 - vertexBufferT[face3D->a].y * projectionY / vertexBufferT[face3D->a].z;
                    quad2D.vertex[1].x = SCREEN_CENTER + vertexBufferT[face3D->b].x * projectionX / vertexBufferT[face3D->b].z;
                    quad2D.vertex[1].y = 120 - vertexBufferT[face3D->b].y * projectionY / vertexBufferT[face3D->b].z;
                    quad2D.vertex[2].x = SCREEN_CENTER + vertexBufferT[face3D->c].x * projectionX / vertexBufferT[face3D->c].z;
                    quad2D.vertex[2].y = 120 - vertexBufferT[face3D->c].y * projectionY / vertexBufferT[face3D->c].z;
                    quad2D.vertex[3].x = SCREEN_CENTER + vertexBufferT[face3D->d].x * projectionX / vertexBufferT[face3D->d].z;
                    quad2D.vertex[3].y = 120 - vertexBufferT[face3D->d].y * projectionY / vertexBufferT[face3D->d].z;
                    GraphicsSystem_DrawQuad(&quad2D, face3D->color);
                }
                break;
            case 3:
                quad2D.vertex[0].x = vertexBuffer[face3D->a].x;
                quad2D.vertex[0].y = vertexBuffer[face3D->a].y;
                quad2D.vertex[1].x = vertexBuffer[face3D->b].x;
                quad2D.vertex[1].y = vertexBuffer[face3D->b].y;
                quad2D.vertex[2].x = vertexBuffer[face3D->c].x;
                quad2D.vertex[2].y = vertexBuffer[face3D->c].y;
                quad2D.vertex[3].x = vertexBuffer[face3D->d].x;
                quad2D.vertex[3].y = vertexBuffer[face3D->d].y;
                GraphicsSystem_DrawQuad(&quad2D, face3D->color);
                break;
        }
    }
}
