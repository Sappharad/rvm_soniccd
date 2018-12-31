//
//  FileIO.h
//  rvm
//

#ifndef FileIO_h
#define FileIO_h

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#ifndef WINDOWS
#include <unistd.h>
#else
#define uint unsigned int
#endif
#include "StageList.h"
#include "FileData.h"
#include "SDL.h"

#define PRESENTATION_STAGE 0
#define ZONE_STAGE 1
#define BONUS_STAGE 2
#define SPECIAL_STAGE 3

extern unsigned char fileBuffer[8192];
extern uint bufferPosition;
extern uint fileSize;
extern uint readSize;
extern uint readPos;
extern bool useRSDKFile;
extern bool useByteCode;
extern bool useOldSdkLayout;
extern uint vFileSize;
extern uint virtualFileOffset;
extern int saveRAM[8192];
extern uint8_t eStringPosA;
extern uint8_t eStringPosB;
extern uint8_t eStringNo;
extern bool eNybbleSwap;
extern char currentStageFolder[8];
extern struct StageList pStageList[64];
extern struct StageList zStageList[128];
extern struct StageList bStageList[64];
extern struct StageList sStageList[64];
extern uint8_t activeStageList;
extern uint8_t noPresentationStages;
extern uint8_t noZoneStages;
extern uint8_t noBonusStages;
extern uint8_t noSpecialStages;
extern int actNumber;

void Init_FileIO(void);
void FileIO_StrCopy(char* strA, int len_strA, char* strB, int len_strB);
void FileIO_StrClear(char* strA, int len_strA);
void FileIO_StrCopy2D(char strA[][32], int len_strA, char* strB, int len_strB, int strPos);
void FileIO_StrAdd(char* strA, int len_strA, char* strB, int len_strB);
bool FileIO_StringComp(char* strA, char* strB);
int FileIO_StringLength(char* strA, int len_strA);
int FileIO_FindStringToken(char* strA, char* token, char instance);
bool FileIO_ConvertStringToInteger(char* strA, int len_strA, int* sValue);
bool FileIO_CheckRSDKFile(void);
bool FileIO_LoadFile(char* filePath, struct FileData *fData);
void FileIO_CloseFile(void);
bool FileIO_CheckCurrentStageFolder(int sNumber);
void FileIO_ResetCurrentStageFolder(void);
bool FileIO_LoadStageFile(char* filePath, int sNumber, struct FileData *fData);
bool FileIO_LoadActFile(char* filePath, int sNumber, struct FileData *fData);
bool FileIO_ParseVirtualFileSystem(char* filePath);
uint8_t FileIO_ReadByte(void);
void FileIO_ReadByteArray(uint8_t* byteP, int numBytes);
void FileIO_ReadCharArray(char* charP, int numBytes);
void FileIO_FillFileBuffer(void);
void FileIO_GetFileInfo(struct FileData *fData);
void FileIO_SetFileInfo(struct FileData *fData);
uint FileIO_GetFilePosition(void);
void FileIO_SetFilePosition(uint newFilePos);
bool FileIO_ReachedEndOfFile(void);
uint8_t FileIO_ReadSaveRAMData(void);
uint8_t FileIO_WriteSaveRAMData(void);

#endif /* FileIO_h */
