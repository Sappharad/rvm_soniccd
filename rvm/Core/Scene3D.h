//
//  Scene3D.h
//  rvm
//

#ifndef Scene3D_h
#define Scene3D_h

#include <stdio.h>
#include "GlobalAppDefinitions.h"
#include "Vertex3D.h"
#include "Face3D.h"
#include "SortList.h"
#include "Quad2D.h"
#include "GraphicsSystem.h"

extern struct Vertex3D vertexBuffer[256]; //4096
extern struct Vertex3D vertexBufferT[256]; //4096
extern struct Face3D indexBuffer[64]; //1024
extern struct SortList drawList[64]; //1024
extern int numVertices;
extern int numFaces;
extern int projectionX;
extern int projectionY;
extern int matWorld[16];
extern int matView[16];
extern int matFinal[16];
extern int matTemp[16];

void Init_Scene3D(void);
void Scene3D_SetIdentityMatrix(int* m);
void Scene3D_MatrixMultiply(int* a, int* b);
void Scene3D_MatrixTranslateXYZ(int* m, int xPos, int yPos, int zPos);
void Scene3D_MatrixScaleXYZ(int* m, int xScale, int yScale, int zScale);
void Scene3D_MatrixRotateX(int* m, int angle);
void Scene3D_MatrixRotateY(int* m, int angle);
void Scene3D_MatrixRotateZ(int* m, int angle);
void Scene3D_MatrixRotateXYZ(int* m, int angleX, int angleY, int angleZ);
void Scene3D_TransformVertexBuffer(void);
void Scene3D_TransformVertices(int* m, int vStart, int vEnd);
void Scene3D_Sort3DDrawList(void);
void Scene3D_Draw3DScene(int surfaceNum);

#endif /* Scene3D_h */
