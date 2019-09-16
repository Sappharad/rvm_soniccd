//
//  FileIO.c
//  rvm
//

#include "FileIO.h"
#include <sys/stat.h>

unsigned char fileBuffer[8192];
uint bufferPosition;
uint fileSize;
uint readSize;
uint readPos;
bool useRSDKFile;
bool useByteCode;
bool useOldSdkLayout;
uint vFileSize;
uint virtualFileOffset;
int saveRAM[8192];
const char encryptionStringA[] = "4RaS9D7KaEbxcp2o5r6t";
const char encryptionStringB[] = "3tRaUxLmEaSn";
uint8_t eStringPosA;
uint8_t eStringPosB;
uint8_t eStringNo;
bool eNybbleSwap;
char currentStageFolder[8];
struct StageList pStageList[64];
struct StageList zStageList[128];
struct StageList bStageList[64];
struct StageList sStageList[64];
uint8_t activeStageList;
uint8_t noPresentationStages;
uint8_t noZoneStages;
uint8_t noBonusStages;
uint8_t noSpecialStages;
int actNumber;
FILE *fileReader;

void Init_FileIO()
{
    bufferPosition = 0u;
    fileSize = 0u;
    readSize = 0u;
    readPos = 0u;
    useRSDKFile = false;
    useByteCode = false;
    useOldSdkLayout = false;
    vFileSize = 0u;
    virtualFileOffset = 0u;
}
void FileIO_StrCopy(char* strA, int len_strA, char* strB, int len_strB)
{
    int i = 0;
    bool flag = true;
    if (i == len_strA || i == len_strB)
    {
        flag = false;
    }
    while (flag)
    {
        strA[i] = strB[i];
        if (strB[i] == '\0')
        {
            flag = false;
        }
        i++;
        if (i == len_strA || i == len_strB)
        {
            flag = false;
        }
    }
    while (i < len_strA)
    {
        strA[i] = '\0';
        i++;
    }
}
void FileIO_StrClear(char* strA, int len_strA)
{
    for (int i = 0; i < len_strA; i++)
    {
        strA[i] = '\0';
    }
}
void FileIO_StrCopy2D(char strA[][32], int len_strA, char* strB, int len_strB, int strPos)
{
    int i = 0;
    bool flag = true;
    if (i == len_strA || i == len_strB)
    {
        flag = false;
    }
    while (flag)
    {
        strA[strPos][i] = strB[i];
        if (strB[i] == '\0')
        {
            flag = false;
        }
        i++;
        if (i == len_strA || i == len_strB)
        {
            flag = false;
        }
    }
    while (i < len_strA)
    {
        strA[strPos][i] = '\0';
        i++;
    }
}
void FileIO_StrAdd(char* strA, int len_strA, char* strB, int len_strB)
{
    int i = 0;
    int num = 0;
    bool flag = true;
    while (strA[i] != '\0')
    {
        i++;
    }
    if (num == len_strA)
    {
        flag = false;
    }
    while (flag)
    {
        strA[i] = strB[num];
        if (strB[num] == '\0')
        {
            flag = false;
        }
        i++;
        num++;
        if (num == len_strB || i == len_strA)
        {
            flag = false;
        }
    }
    while (i < len_strA)
    {
        strA[i] = '\0';
        i++;
    }
}
bool FileIO_StringComp(char* strA, char* strB)
{
    bool result = true;
    int num = 0;
    int num2 = 0;
    while (true)
    {
        if (strA[num] != strB[num2] && strA[num] != strB[num2] + ' ' && strA[num] != strB[num2] - ' ')
        {
            result = false;
            break;
        }
        else
        {
            if (strA[num] == '\0')
            {
                break;
            }
            else
            {
                num++;
                num2++;
            }
        }
    }
    return result;
}
int FileIO_StringLength(char* strA, int len_strA)
{
    int num = 0;
    if (len_strA == 0)
    {
        return num;
    }
    while (strA[num] != '\0' && num < len_strA)
    {
        num++;
    }
    return num;
}
int FileIO_FindStringToken(char* strA, char* token, char instance)
{
    int num = 0;
    int result = -1;
    int num2 = 0;
    while (strA[num] != '\0')
    {
        int num3 = 0;
        bool flag = true;
        while (token[num3] != '\0')
        {
            if (strA[num + num3] == '\0')
            {
                return result;
            }
            if (strA[num + num3] != token[num3])
            {
                flag = false;
            }
            num3++;
        }
        if (flag)
        {
            num2++;
            if (num2 == (int)instance)
            {
                return num;
            }
        }
        num++;
    }
    return result;
}
bool FileIO_ConvertStringToInteger(char* strA, int len_strA, int* sValue)
{
    int num = 0;
    bool flag = false;
    *sValue = 0;
    if ((strA[num] > '/' && strA[num] < ':') || strA[num] == '-' || strA[num] == '+')
    {
        int i = FileIO_StringLength(strA, len_strA) - 1;
        if (strA[num] == '-')
        {
            flag = true;
            num++;
            i--;
        }
        else if (strA[num] == '+')
        {
            num++;
            i--;
        }
        while (i > -1)
        {
            if (strA[num] <= '/' || strA[num] >= ':')
            {
                return false;
            }
            if (i > 0)
            {
                int num2 = (int)(strA[num] - '0');
                for (int j = i; j > 0; j--)
                {
                    num2 *= 10;
                }
                *sValue += num2;
            }
            else
            {
                *sValue += (int)(strA[num] - '0');
            }
            i--;
            num++;
        }
        if (flag)
        {
            *sValue = -(*sValue);
        }
        return true;
    }
    return false;
}

bool FileIO_CheckRSDKFile()
{
    struct FileData fData;
    fileReader = fopen("Data.rsdk","rb");
    if (fileReader == NULL)
    {
        useRSDKFile = false;
        useByteCode = false;
        return false;
    }
    useRSDKFile = true;
    useByteCode = false;
    fclose(fileReader);
    
    if (FileIO_LoadFile("Data/Scripts/ByteCode/GlobalCode.bin", &fData))
    {
        useByteCode = true;
        FileIO_CloseFile();
        return true;
    }
    else if (FileIO_LoadFile("Data/Scripts/ByteCode/GS000.bin", &fData))
    {
        useOldSdkLayout = true;
        useByteCode = true;
        FileIO_CloseFile();
        return true;
    }
    return false;
}
bool FileIO_LoadFile(char* filePath, struct FileData *fData)
{
    int i = 0;
    bool flag = true;
    while (flag)
    {
        fData->fileName[i] = filePath[i];
        if (filePath[i] == '\0')
        {
            flag = false;
        }
        i++;
        if (i == strlen(filePath))
        {
            flag = false;
        }
    }
    while (i < sizeof(fData->fileName))
    {
        fData->fileName[i] = '\0';
        i++;
    }
    //Start of code to handle files outside of Data.rsdk
#if WINDOWS
	if (_access(filePath, 0) != -1)
#else
	if (access(filePath, F_OK) != -1)
#endif
    {
        useRSDKFile = false;
        if (fileReader != NULL)
        {
            fclose(fileReader);
            fileReader = NULL;
        }
        fileReader = fopen(filePath, "rb");
        fseek(fileReader, 0L, SEEK_END);
        fileSize = (unsigned int)ftell(fileReader);
        rewind(fileReader);
        bufferPosition = 0u;
        readSize = 0u;
        readPos = 0u;
        fData->fileSize = fileSize;
        fData->virtualFileOffset = 0u;
        return true;
    }
    else
    {
        useRSDKFile = true;
    }
    //End of outside RSDK code
    if (fileReader != NULL)
    {
        fclose(fileReader);
        fileReader = NULL;
    }
    fileReader = fopen("Data.rsdk","rb");
    fseek(fileReader, 0L, SEEK_END);
    fileSize = (unsigned int)ftell(fileReader);
    rewind(fileReader);
    bufferPosition = 0u;
    readSize = 0u;
    readPos = 0u;
    if (!FileIO_ParseVirtualFileSystem(fData->fileName))
    {
        fclose(fileReader);
        fileReader = NULL;
        return false;
    }
    fData->fileSize = vFileSize;
    fData->virtualFileOffset = virtualFileOffset;
    bufferPosition = 0u;
    readSize = 0u;
    return true;
}
void FileIO_CloseFile()
{
    if (fileReader != NULL)
    {
        fclose(fileReader);
        fileReader = NULL;
    }
}
bool FileIO_CheckCurrentStageFolder(int sNumber)
{
    switch (activeStageList)
    {
        case PRESENTATION_STAGE:
            if (FileIO_StringComp(currentStageFolder, pStageList[sNumber].stageFolderName))
            {
                return true;
            }
            FileIO_StrCopy(currentStageFolder, sizeof(currentStageFolder), pStageList[sNumber].stageFolderName, sizeof(pStageList[sNumber].stageFolderName));
            return false;
        case ZONE_STAGE:
            if (FileIO_StringComp(currentStageFolder, zStageList[sNumber].stageFolderName))
            {
                return true;
            }
            FileIO_StrCopy(currentStageFolder, sizeof(currentStageFolder), zStageList[sNumber].stageFolderName, sizeof(zStageList[sNumber].stageFolderName));
            return false;
        case BONUS_STAGE:
            if (FileIO_StringComp(currentStageFolder, bStageList[sNumber].stageFolderName))
            {
                return true;
            }
            FileIO_StrCopy(currentStageFolder, sizeof(currentStageFolder), bStageList[sNumber].stageFolderName, sizeof(bStageList[sNumber].stageFolderName));
            return false;
        case SPECIAL_STAGE:
            if (FileIO_StringComp(currentStageFolder, sStageList[sNumber].stageFolderName))
            {
                return true;
            }
            FileIO_StrCopy(currentStageFolder, sizeof(currentStageFolder), sStageList[sNumber].stageFolderName, sizeof(sStageList[sNumber].stageFolderName));
            return false;
        default:
            return false;
    }
}
void FileIO_ResetCurrentStageFolder()
{
    FileIO_StrCopy(currentStageFolder, sizeof(currentStageFolder), "", 1);
}
bool FileIO_LoadStageFile(char* filePath, int sNumber, struct FileData *fData)
{
    char filePath2[64];
    char* tempPath = "Data/Stages/";
    FileIO_StrCopy(filePath2, sizeof(filePath2), tempPath, (int)strlen(tempPath));
    switch (activeStageList)
    {
        case PRESENTATION_STAGE:
            FileIO_StrAdd(filePath2, sizeof(filePath2), pStageList[sNumber].stageFolderName, sizeof(pStageList[sNumber].stageFolderName));
            break;
        case ZONE_STAGE:
            FileIO_StrAdd(filePath2, sizeof(filePath2), zStageList[sNumber].stageFolderName, sizeof(zStageList[sNumber].stageFolderName));
            break;
        case BONUS_STAGE:
            FileIO_StrAdd(filePath2, sizeof(filePath2), bStageList[sNumber].stageFolderName, sizeof(bStageList[sNumber].stageFolderName));
            break;
        case SPECIAL_STAGE:
            FileIO_StrAdd(filePath2, sizeof(filePath2), sStageList[sNumber].stageFolderName, sizeof(sStageList[sNumber].stageFolderName));
            break;
    }
    tempPath = "/";
    FileIO_StrAdd(filePath2, sizeof(filePath2), tempPath, (int)strlen(tempPath));
    FileIO_StrAdd(filePath2, sizeof(filePath2), filePath, (int)strlen(filePath));
    return FileIO_LoadFile(filePath2, fData);
}
bool FileIO_LoadActFile(char* filePath, int sNumber, struct FileData *fData)
{
    char filePath2[64];
    char* tempPath = "Data/Stages/";
    FileIO_StrCopy(filePath2, sizeof(filePath2), tempPath, (int)strlen(tempPath));
    switch (activeStageList)
    {
        case PRESENTATION_STAGE:
            FileIO_StrAdd(filePath2, sizeof(filePath2), pStageList[sNumber].stageFolderName, sizeof(pStageList[sNumber].stageFolderName));
            break;
        case ZONE_STAGE:
            FileIO_StrAdd(filePath2, sizeof(filePath2), zStageList[sNumber].stageFolderName, sizeof(zStageList[sNumber].stageFolderName));
            break;
        case BONUS_STAGE:
            FileIO_StrAdd(filePath2, sizeof(filePath2), bStageList[sNumber].stageFolderName, sizeof(bStageList[sNumber].stageFolderName));
            break;
        case SPECIAL_STAGE:
            FileIO_StrAdd(filePath2, sizeof(filePath2), sStageList[sNumber].stageFolderName, sizeof(sStageList[sNumber].stageFolderName));
            break;
    }
    tempPath = "/Act";
    FileIO_StrAdd(filePath2, sizeof(filePath2), tempPath, (int)strlen(tempPath));
    switch (activeStageList)
    {
        case PRESENTATION_STAGE:
            FileIO_StrAdd(filePath2, sizeof(filePath2), pStageList[sNumber].actNumber, sizeof(pStageList[sNumber].actNumber));
            FileIO_ConvertStringToInteger(pStageList[sNumber].actNumber, sizeof(pStageList[sNumber].actNumber), &actNumber);
            break;
        case ZONE_STAGE:
            FileIO_StrAdd(filePath2, sizeof(filePath2), zStageList[sNumber].actNumber, sizeof(zStageList[sNumber].actNumber));
            FileIO_ConvertStringToInteger(zStageList[sNumber].actNumber, sizeof(zStageList[sNumber].actNumber), &actNumber);
            break;
        case BONUS_STAGE:
            FileIO_StrAdd(filePath2, sizeof(filePath2), bStageList[sNumber].actNumber, sizeof(bStageList[sNumber].actNumber));
            FileIO_ConvertStringToInteger(bStageList[sNumber].actNumber, sizeof(bStageList[sNumber].actNumber), &actNumber);
            break;
        case SPECIAL_STAGE:
            FileIO_StrAdd(filePath2, sizeof(filePath2), sStageList[sNumber].actNumber, sizeof(sStageList[sNumber].actNumber));
            FileIO_ConvertStringToInteger(sStageList[sNumber].actNumber, sizeof(sStageList[sNumber].actNumber), &actNumber);
            break;
    }
    FileIO_StrAdd(filePath2, sizeof(filePath2), filePath, (int)strlen(filePath));
    return FileIO_LoadFile(filePath2, fData);
}
bool FileIO_ParseVirtualFileSystem(char* filePath)
{
    char desiredDirectory[64];
    char desiredFile[64];
    char actualPath[64];
    int num = 0;
    int i = 0;
    virtualFileOffset = 0u;
    int j = 0;
    while (filePath[j] != '\0')
    {
        if (filePath[j] == '/')
        {
            num = j;
            i = 0;
        }
        else
        {
            i++;
        }
        desiredDirectory[j] = filePath[j];
        j++;
    }
    num++;
    for (j = 0; j < i; j++)
    {
        desiredFile[j] = filePath[num + j];
    }
    desiredFile[i] = '\0';
    desiredDirectory[num] = '\0';
    fseek(fileReader, 0L, SEEK_SET);
    useRSDKFile = false;
    bufferPosition = 0u;
    readSize = 0u;
    readPos = 0u;
    uint8_t b = FileIO_ReadByte();
    num = (int)b;
    b = FileIO_ReadByte();
    num += (int)b << 8;
    b = FileIO_ReadByte();
    num += (int)b << 16;
    b = FileIO_ReadByte();
    num += (int)b << 24;
    b = FileIO_ReadByte();
    int numPaths = b;
    b = FileIO_ReadByte();
    numPaths += (b << 8);
    j = 0;
    int num2 = 0;
    while (j < (int)numPaths)
    {
        b = FileIO_ReadByte();
        for (i = 0; i < (int)b; i++)
        {
            actualPath[i] = (char)(FileIO_ReadByte() ^ 255 - b);
        }
        actualPath[i] = '\0';
        if (FileIO_StringComp(desiredDirectory, actualPath))
        {
            b = 1;
        }
        else
        {
            b = 0;
        }
        if (b == 1)
        {
            j = numPaths;
            b = FileIO_ReadByte();
            num2 = (int)b;
            b = FileIO_ReadByte();
            num2 += (int)b << 8;
            b = FileIO_ReadByte();
            num2 += (int)b << 16;
            b = FileIO_ReadByte();
            num2 += (int)b << 24;
        }
        else
        {
            num2 = -1;
            b = FileIO_ReadByte();
            b = FileIO_ReadByte();
            b = FileIO_ReadByte();
            b = FileIO_ReadByte();
            j++;
        }
    }
    if (num2 == -1)
    {
        useRSDKFile = true;
        return false;
    }
    fseek(fileReader, (long)(num + num2), SEEK_SET);
    bufferPosition = 0u;
    readSize = 0u;
    readPos = 0u;
    virtualFileOffset = (uint)(num + num2);
    j = 0;
    num = 0; //Using this for number of attempts tracking
    while (j < 1)
    {
        b = FileIO_ReadByte();
        virtualFileOffset += 1u;
        i = 0;
        while (i < (int)b)
        {
            actualPath[i] = (char)(FileIO_ReadByte() ^ 255);
            i++;
            virtualFileOffset += 1u;
        }
        actualPath[i] = '\0';
        if (FileIO_StringComp(desiredFile, actualPath))
        {
            j = 1; //Found file, break out of loop
            b = FileIO_ReadByte();
            i = (int)b;
            b = FileIO_ReadByte();
            i += (int)b << 8;
            b = FileIO_ReadByte();
            i += (int)b << 16;
            b = FileIO_ReadByte();
            i += (int)b << 24;
            virtualFileOffset += 4u;
            vFileSize = (uint)i;
            if(fseek(fileReader, (long)(virtualFileOffset), SEEK_SET) != 0){
                return false; //FAILED
            }
            bufferPosition = 0u;
            readSize = 0u;
            readPos = virtualFileOffset;
        }
        else
        {
            b = FileIO_ReadByte();
            i = (int)b;
            b = FileIO_ReadByte();
            i += (int)b << 8;
            b = FileIO_ReadByte();
            i += (int)b << 16;
            b = FileIO_ReadByte();
            i += (int)b << 24;
            virtualFileOffset += 4u;
            virtualFileOffset += (uint)i;
            if(fseek(fileReader, (long)(virtualFileOffset), SEEK_SET) != 0){
                return false; //FAILED
            }
            bufferPosition = 0u;
            readSize = 0u;
            readPos = virtualFileOffset;
            num++;
            if(num==256){
                //There are never 256 files in a directory, we know we failed to find this file so bail out instead of looping forever.
                return false;
            }
        }
    }
    eStringNo = (uint8_t)((vFileSize & 0x1FC) >> 2);
    eStringPosB = (uint8_t)(1 + eStringNo % 9);
    eStringPosA = (uint8_t)(1 + eStringNo % eStringPosB);
    eNybbleSwap = false;
    useRSDKFile = true;
    return true;
}
uint8_t FileIO_ReadByte()
{
    uint8_t b = 0;
    if (readPos <= fileSize)
    {
        if (!useRSDKFile)
        {
            if (bufferPosition == readSize)
            {
                FileIO_FillFileBuffer();
            }
            b = fileBuffer[bufferPosition];
            bufferPosition += 1u;
        }
        else
        {
            if (bufferPosition == readSize)
            {
                FileIO_FillFileBuffer();
            }
            b = (uint8_t)((char)(fileBuffer[bufferPosition] ^ eStringNo) ^ encryptionStringB[eStringPosB]);
            if (eNybbleSwap)
            {
                b = (uint8_t)((b >> 4) + ((int)(b & 0xF) << 4));
            }
            b ^= (uint8_t)encryptionStringA[eStringPosA];
            eStringPosA += 1;
            eStringPosB += 1;
            if (eStringPosA > 0x13 && eStringPosB > 0xB)
            {
                eStringNo += 1;
                eStringNo &= 0x7F;
                if (!eNybbleSwap)
                {
                    eNybbleSwap = true;
                    eStringPosA = (uint8_t)(3 + eStringNo % 0xF);
                    eStringPosB = (uint8_t)(1 + eStringNo % 0x7);
                }
                else
                {
                    eNybbleSwap = false;
                    eStringPosA = (uint8_t)(6 + eStringNo % 0xC);
                    eStringPosB = (uint8_t)(4 + eStringNo % 0x5);
                }
            }
            else
            {
                if (eStringPosA > 0x13)
                {
                    eStringPosA = 1;
                    eNybbleSwap = !eNybbleSwap;
                }
                if (eStringPosB > 0xB)
                {
                    eStringPosB = 1;
                    eNybbleSwap = !eNybbleSwap;
                }
            }
            bufferPosition += 1u;
        }
    }
    return b;
}
void FileIO_ReadByteArray(uint8_t* byteP, int numBytes)
{
    int num = 0;
    if (readPos <= fileSize)
    {
        if (!useRSDKFile)
        {
            while (numBytes > 0)
            {
                if (bufferPosition == readSize)
                {
                    FileIO_FillFileBuffer();
                }
                byteP[num] = fileBuffer[bufferPosition];
                bufferPosition += 1u;
                num++;
                numBytes--;
            }
            return;
        }
        while (numBytes > 0)
        {
            if (bufferPosition >= readSize)
            {
                FileIO_FillFileBuffer();
            }
            byteP[num] = (uint8_t)((char)(fileBuffer[bufferPosition] ^ eStringNo) ^ encryptionStringB[(int)eStringPosB]);
            if (eNybbleSwap)
            {
                byteP[num] = (uint8_t)((byteP[num] >> 4) + ((int)(byteP[num] & 0xF) << 4));
            }
            byteP[num] ^= (uint8_t)encryptionStringA[eStringPosA];
            eStringPosA += 1;
            eStringPosB += 1;
            if (eStringPosA > 19 && eStringPosB > 11)
            {
                eStringNo += 1;
                eStringNo &= 0x7F;
                if (!eNybbleSwap)
                {
                    eNybbleSwap = true;
                    eStringPosA = (uint8_t)(3 + eStringNo % 0xF);
                    eStringPosB = (uint8_t)(1 + eStringNo % 0x7);
                }
                else
                {
                    eNybbleSwap = false;
                    eStringPosA = (uint8_t)(6 + eStringNo % 0xC);
                    eStringPosB = (uint8_t)(4 + eStringNo % 0x5);
                }
            }
            else
            {
                if (eStringPosA > 19)
                {
                    eStringPosA = 1;
                    eNybbleSwap = !eNybbleSwap;
                }
                if (eStringPosB > 11)
                {
                    eStringPosB = 1;
                    eNybbleSwap = !eNybbleSwap;
                }
            }
            bufferPosition += 1u;
            num++;
            numBytes--;
        }
    }
}
void FileIO_ReadCharArray(char* charP, int numBytes)
{
    int num = 0;
    if (readPos <= fileSize)
    {
        if (!useRSDKFile)
        {
            while (numBytes > 0)
            {
                if (bufferPosition == readSize)
                {
                    FileIO_FillFileBuffer();
                }
                charP[num] = (char)fileBuffer[bufferPosition];
                bufferPosition += 1u;
                num++;
                numBytes--;
            }
            return;
        }
        while (numBytes > 0)
        {
            if (bufferPosition == readSize)
            {
                FileIO_FillFileBuffer();
            }
            uint8_t b = (uint8_t)((char)(fileBuffer[bufferPosition] ^ eStringNo) ^ encryptionStringB[(int)eStringPosB]);
            if (eNybbleSwap)
            {
                b = (uint8_t)((b >> 4) + ((int)(b & 0xF) << 4));
            }
            b ^= (uint8_t)encryptionStringA[(int)eStringPosA];
            charP[num] = (char)b;
            eStringPosA += 1;
            eStringPosB += 1;
            if (eStringPosA > 19 && eStringPosB > 11)
            {
                eStringNo += 1;
                eStringNo &= 0x7F;
                if (!eNybbleSwap)
                {
                    eNybbleSwap = true;
                    eStringPosA = (uint8_t)(3 + eStringNo % 0xF);
                    eStringPosB = (uint8_t)(1 + eStringNo % 0x7);
                }
                else
                {
                    eNybbleSwap = false;
                    eStringPosA = (uint8_t)(6 + eStringNo % 0xC);
                    eStringPosB = (uint8_t)(4 + eStringNo % 0x5);
                }
            }
            else
            {
                if (eStringPosA > 19)
                {
                    eStringPosA = 1;
                    eNybbleSwap = !eNybbleSwap;
                }
                if (eStringPosB > 11)
                {
                    eStringPosB = 1;
                    eNybbleSwap = !eNybbleSwap;
                }
            }
            bufferPosition += 1u;
            num++;
            numBytes--;
        }
    }
}
void FileIO_FillFileBuffer()
{
    if (readPos + 0x2000 > fileSize)
    {
        readSize = fileSize - readPos;
    }
    else
    {
        readSize = 0x2000;
    }
    fread(fileBuffer, 1, readSize, fileReader);
    readPos += readSize;
    bufferPosition = 0u;
}
void FileIO_GetFileInfo(struct FileData *fData)
{
    fData->bufferPos = bufferPosition;
    if (useRSDKFile)
        fData->position = readPos - readSize;
    else
        fData->position = readPos - readSize + bufferPosition;
    fData->eStringPosA = eStringPosA;
    fData->eStringPosB = eStringPosB;
    fData->eStringNo = eStringNo;
    fData->eNybbleSwap = eNybbleSwap;
    fData->inRsdkFile = useRSDKFile;
}
void FileIO_SetFileInfo(struct FileData *fData)
{
    useRSDKFile = fData->inRsdkFile;
    if (useRSDKFile)
    {
        if (fileReader != NULL)
        {
            fclose(fileReader);
            fileReader = NULL;
        }
        fileReader = fopen("Data.rsdk", "rb");
        virtualFileOffset = fData->virtualFileOffset;
        vFileSize = fData->fileSize;
        
        fseek(fileReader, 0L, SEEK_END);
        fileSize = (unsigned int)ftell(fileReader);
        rewind(fileReader);
        
        readPos = fData->position;
        fseek(fileReader, readPos, SEEK_SET);
        FileIO_FillFileBuffer();
        bufferPosition = fData->bufferPos;
        eStringPosA = fData->eStringPosA;
        eStringPosB = fData->eStringPosB;
        eStringNo = fData->eStringNo;
        eNybbleSwap = fData->eNybbleSwap;
    }
    else
    {
        FileIO_LoadFile(fData->fileName, fData);
        for (int i = 0; i < fData->position; i++)
        {
            FileIO_ReadByte(); //This is awful, but I need the buffer in the correct state.
        }
    }
}
uint FileIO_GetFilePosition()
{
    if (useRSDKFile)
    {
        return readPos - readSize + bufferPosition - virtualFileOffset;
    }
    return readPos - readSize + bufferPosition;
}
void FileIO_SetFilePosition(uint newFilePos)
{
    if (useRSDKFile)
    {
        readPos = newFilePos + virtualFileOffset;
        eStringNo = (uint8_t)((vFileSize & 508u) >> 2);
        eStringPosB = (uint8_t)(1 + eStringNo % 9);
        eStringPosA = (uint8_t)(1 + eStringNo % eStringPosB);
        eNybbleSwap = false;
        while (newFilePos > 0u)
        {
            eStringPosA += 1;
            eStringPosB += 1;
            if (eStringPosA > 19 && eStringPosB > 11)
            {
                eStringNo += 1;
                eStringNo &= 0x7F;
                if (!eNybbleSwap)
                {
                    eNybbleSwap = true;
                    eStringPosA = (uint8_t)(3 + eStringNo % 0xF);
                    eStringPosB = (uint8_t)(1 + eStringNo % 0x7);
                }
                else
                {
                    eNybbleSwap = false;
                    eStringPosA = (uint8_t)(6 + eStringNo % 0xC);
                    eStringPosB = (uint8_t)(4 + eStringNo % 0x5);
                }
            }
            else
            {
                if (eStringPosA > 19)
                {
                    eStringPosA = 1;
                    eNybbleSwap = !eNybbleSwap;
                }
                if (eStringPosB > 11)
                {
                    eStringPosB = 1;
                    eNybbleSwap = !eNybbleSwap;
                }
            }
            newFilePos -= 1u;
        }
    }
    else
    {
        readPos = newFilePos;
    }
    fseek(fileReader, readPos, SEEK_SET);
    FileIO_FillFileBuffer();
}
bool FileIO_ReachedEndOfFile()
{
    if (!useRSDKFile)
    {
        return (readPos - readSize + bufferPosition) >= fileSize;
    }
    return (readPos - readSize + bufferPosition - virtualFileOffset) >= vFileSize;
}
const char* SAVE_GAME_FILE = "SGame.bin";
uint8_t FileIO_ReadSaveRAMData()
{
    //TODO: This should probably go into the appropriate platform specific directory instead.
#if WINDOWS
	if (_access(SAVE_GAME_FILE, 0) != -1)
#else
    if(access(SAVE_GAME_FILE, F_OK) != -1)
#endif
    {
        FILE *sramReader = fopen(SAVE_GAME_FILE, "rb");
        fseek(sramReader, 0, SEEK_SET);
        fread(saveRAM, 4, 8192, sramReader);
        fclose(sramReader);
    }
    return 1;
}
uint8_t FileIO_WriteSaveRAMData()
{
    FILE* sramWriter = fopen(SAVE_GAME_FILE, "wb");
    fwrite(saveRAM, 4, 8192, sramWriter);
    fclose(sramWriter);
    return 1;
}
