//
//  TextSystem.c
//  rvm
//

#include "TextSystem.h"

int textMenuSurfaceNo;
struct FontCharacter fontCharacterList[1024];

void TextSystem_LoadFontFile(char* fileName)
{
    int num = 0;
    struct FileData fData;
    if (FileIO_LoadFile(fileName, &fData))
    {
        while (!FileIO_ReachedEndOfFile())
        {
            uint8_t b = FileIO_ReadByte();
            fontCharacterList[num].id = (int)b;
            b = FileIO_ReadByte();
            fontCharacterList[num].id = fontCharacterList[num].id + ((int)b << 8);
            b = FileIO_ReadByte();
            fontCharacterList[num].id = fontCharacterList[num].id + ((int)b << 16);
            b = FileIO_ReadByte();
            fontCharacterList[num].id = fontCharacterList[num].id + ((int)b << 24);
            b = FileIO_ReadByte();
            fontCharacterList[num].left = (short)b;
            b = FileIO_ReadByte();
            fontCharacterList[num].left = (short)(fontCharacterList[num].left + (short)(b << 8));
            b = FileIO_ReadByte();
            fontCharacterList[num].top = (short)b;
            b = FileIO_ReadByte();
            fontCharacterList[num].top = (short)(fontCharacterList[num].top + (short)(b << 8));
            b = FileIO_ReadByte();
            fontCharacterList[num].xSize = (short)b;
            b = FileIO_ReadByte();
            fontCharacterList[num].xSize = (short)(fontCharacterList[num].xSize + (short)(b << 8));
            b = FileIO_ReadByte();
            fontCharacterList[num].ySize = (short)b;
            b = FileIO_ReadByte();
            fontCharacterList[num].ySize = (short)(fontCharacterList[num].ySize + (short)(b << 8));
            b = FileIO_ReadByte();
            fontCharacterList[num].xPivot = (short)b;
            b = FileIO_ReadByte();
            if (b > 128)
            {
                fontCharacterList[num].xPivot = (short)(fontCharacterList[num].xPivot + (short)((b - 128) << 8));
                fontCharacterList[num].xPivot = (short)(-(short)(32768 - (int)fontCharacterList[num].xPivot));
            }
            else
            {
                fontCharacterList[num].xPivot = (short)(fontCharacterList[num].xPivot + (short)(b << 8));
            }
            b = FileIO_ReadByte();
            fontCharacterList[num].yPivot = (short)b;
            b = FileIO_ReadByte();
            if (b > 128)
            {
                fontCharacterList[num].yPivot = (short)(fontCharacterList[num].yPivot + (short)((b - 128) << 8));
                fontCharacterList[num].yPivot = (short)(-(short)(32768 - (int)fontCharacterList[num].xPivot));
            }
            else
            {
                fontCharacterList[num].yPivot = (short)(fontCharacterList[num].yPivot + (short)(b << 8));
            }
            b = FileIO_ReadByte();
            fontCharacterList[num].xAdvance = (short)b;
            b = FileIO_ReadByte();
            if (b > 128)
            {
                fontCharacterList[num].xAdvance = (short)(fontCharacterList[num].xAdvance + (short)((b - 128) << 8));
                fontCharacterList[num].xAdvance = (short)(-(short)(32768 - (int)fontCharacterList[num].xAdvance));
            }
            else
            {
                fontCharacterList[num].xAdvance = (short)(fontCharacterList[num].xAdvance + (short)(b << 8));
            }
            b = FileIO_ReadByte();
            b = FileIO_ReadByte();
            num++;
        }
        FileIO_CloseFile();
    }
}
void TextSystem_LoadTextFile(struct TextMenu* tMenu, char* fileName, uint8_t mapCode)
{
    bool flag = false;
    struct FileData fData;
    if (FileIO_LoadFile(fileName, &fData))
    {
        tMenu->textDataPos = 0;
        tMenu->numRows = 0;
        tMenu->entryStart[(int)tMenu->numRows] = tMenu->textDataPos;
        tMenu->entrySize[(int)tMenu->numRows] = 0;
        uint8_t b = FileIO_ReadByte();
        if (b == 255)
        {
            b = FileIO_ReadByte();
            while (!flag)
            {
                b = FileIO_ReadByte();
                ushort num = (ushort)b;
                b = FileIO_ReadByte();
                num += (ushort)(b << 8);
                ushort num2 = num;
                if (num2 != 10)
                {
                    if (num2 == 13)
                    {
                        tMenu->numRows += 1;
                        if (tMenu->numRows > 511)
                        {
                            flag = true;
                        }
                        else
                        {
                            tMenu->entryStart[(int)tMenu->numRows] = tMenu->textDataPos;
                            tMenu->entrySize[(int)tMenu->numRows] = 0;
                        }
                    }
                    else
                    {
                        if (mapCode == 1)
                        {
                            int i = 0;
                            while (i < 1024)
                            {
                                if (fontCharacterList[i].id == (int)num)
                                {
                                    num = (ushort)i;
                                    i = 1025;
                                }
                                else
                                {
                                    i++;
                                }
                            }
                            if (i == 1024)
                            {
                                num = 0;
                            }
                        }
                        tMenu->textData[tMenu->textDataPos] = (char)num;
                        tMenu->textDataPos++;
                        tMenu->entrySize[(int)tMenu->numRows]++;
                    }
                }
                if (!flag)
                {
                    flag = FileIO_ReachedEndOfFile();
                    if (tMenu->textDataPos >= 10240)
                    {
                        flag = true;
                    }
                }
            }
        }
        else
        {
            ushort num = (ushort)b;
            ushort num3 = num;
            if (num3 != 10)
            {
                if (num3 == 13)
                {
                    tMenu->numRows += 1;
                    tMenu->entryStart[(int)tMenu->numRows] = tMenu->textDataPos;
                    tMenu->entrySize[(int)tMenu->numRows] = 0;
                }
                else
                {
                    if (mapCode == 1)
                    {
                        int i = 0;
                        while (i < 1024)
                        {
                            if (fontCharacterList[i].id == (int)num)
                            {
                                num = (ushort)i;
                                i = 1025;
                            }
                            else
                            {
                                i++;
                            }
                        }
                        if (i == 1024)
                        {
                            num = 0;
                        }
                    }
                    tMenu->textData[tMenu->textDataPos] = (char)num;
                    tMenu->textDataPos++;
                    tMenu->entrySize[(int)tMenu->numRows]++;
                }
            }
            while (!flag)
            {
                b = FileIO_ReadByte();
                num = (ushort)b;
                ushort num4 = num;
                if (num4 != 10)
                {
                    if (num4 == 13)
                    {
                        tMenu->numRows += 1;
                        if (tMenu->numRows > 511)
                        {
                            flag = true;
                        }
                        else
                        {
                            tMenu->entryStart[(int)tMenu->numRows] = tMenu->textDataPos;
                            tMenu->entrySize[(int)tMenu->numRows] = 0;
                        }
                    }
                    else
                    {
                        if (mapCode == 1)
                        {
                            int i = 0;
                            while (i < 1024)
                            {
                                if (fontCharacterList[i].id == (int)num)
                                {
                                    num = (ushort)i;
                                    i = 1025;
                                }
                                else
                                {
                                    i++;
                                }
                            }
                            if (i == 1024)
                            {
                                num = 0;
                            }
                        }
                        tMenu->textData[tMenu->textDataPos] = (char)num;
                        tMenu->textDataPos++;
                        tMenu->entrySize[(int)tMenu->numRows]++;
                    }
                }
                if (!flag)
                {
                    flag = FileIO_ReachedEndOfFile();
                    if (tMenu->textDataPos >= 10240)
                    {
                        flag = true;
                    }
                }
            }
        }
        tMenu->numRows += 1;
        FileIO_CloseFile();
    }
}
void TextSystem_DrawBitmapText(struct TextMenu* tMenu, int xPos, int yPos, int scale, int spacing, int rowStart, int numRows)
{
    int num = yPos << 9;
    if (numRows < 0)
    {
        numRows = (int)tMenu->numRows;
    }
    if (rowStart + numRows > (int)tMenu->numRows)
    {
        numRows = (int)tMenu->numRows - rowStart;
    }
    while (numRows > 0)
    {
        int num2 = 0;
        int i = tMenu->entrySize[rowStart];
        int num3 = xPos << 9;
        while (i > 0)
        {
            char c = tMenu->textData[tMenu->entryStart[rowStart] + num2];
            GraphicsSystem_DrawScaledChar(0, num3 >> 5, num >> 5, (int)(-(int)fontCharacterList[(int)c].xPivot), (int)(-(int)fontCharacterList[(int)c].yPivot), scale, scale, (int)fontCharacterList[(int)c].xSize, (int)fontCharacterList[(int)c].ySize, (int)fontCharacterList[(int)c].left, (int)fontCharacterList[(int)c].top, textMenuSurfaceNo);
            num3 += (int)fontCharacterList[(int)c].xAdvance * scale;
            num2++;
            i--;
        }
        num += spacing * scale;
        rowStart++;
        numRows--;
    }
}
void TextSystem_SetupTextMenu(struct TextMenu* tMenu, int numRows)
{
    tMenu->textDataPos = 0;
    tMenu->numRows = (ushort)numRows;
}
void TextSystem_AddTextMenuEntry(struct TextMenu* tMenu, char* inputTxt)
{
    int i = 0;
    tMenu->entryStart[(int)tMenu->numRows] = tMenu->textDataPos;
    tMenu->entrySize[(int)tMenu->numRows] = 0;
    while (i < (int)strlen(inputTxt))
    {
        if (inputTxt[i] != '\0')
        {
            tMenu->textData[tMenu->textDataPos] = inputTxt[i];
            tMenu->textDataPos++;
            tMenu->entrySize[(int)tMenu->numRows]++;
            i++;
        }
        else
        {
            break;
        }
    }
    tMenu->numRows += 1;
}
void TextSystem_AddTextMenuEntryMapped(struct TextMenu* tMenu, char* inputTxt)
{
    int i = 0;
    tMenu->entryStart[(int)tMenu->numRows] = tMenu->textDataPos;
    tMenu->entrySize[(int)tMenu->numRows] = 0;
    while (i < (int)strlen(inputTxt))
    {
        if (inputTxt[i] != '\0')
        {
            ushort num = (ushort)inputTxt[i];
            int j = 0;
            while (j < 1024)
            {
                if (fontCharacterList[j].id == (int)num)
                {
                    num = (ushort)j;
                    j = 1025;
                }
                else
                {
                    j++;
                }
            }
            if (j == 1024)
            {
                num = 0;
            }
            tMenu->textData[tMenu->textDataPos] = (char)num;
            tMenu->textDataPos++;
            tMenu->entrySize[(int)tMenu->numRows]++;
            i++;
        }
        else
        {
            break;
        }
    }
    tMenu->numRows += 1;
}
void TextSystem_SetTextMenuEntry(struct TextMenu* tMenu, char* inputTxt, int rowNum)
{
    int i = 0;
    tMenu->entryStart[rowNum] = tMenu->textDataPos;
    tMenu->entrySize[rowNum] = 0;
    while (i < (int)strlen(inputTxt))
    {
        if (inputTxt[i] != '\0')
        {
            tMenu->textData[tMenu->textDataPos] = inputTxt[i];
            tMenu->textDataPos++;
            tMenu->entrySize[rowNum]++;
            i++;
        }
        else
        {
            break;
        }
    }
}
void TextSystem_EditTextMenuEntry(struct TextMenu* tMenu, char* inputTxt, int rowNum)
{
    int i = 0;
    int num = tMenu->entryStart[rowNum];
    tMenu->entrySize[rowNum] = 0;
    while (i < (int)strlen(inputTxt))
    {
        if (inputTxt[i] != '\0')
        {
            tMenu->textData[num] = inputTxt[i];
            num++;
            tMenu->entrySize[rowNum]++;
            i++;
        }
        else
        {
            break;
        }
    }
}
void TextSystem_DrawTextMenuEntry(struct TextMenu* tMenu, int rowNum, int xPos, int yPos, int textHighL)
{
    int num = tMenu->entryStart[rowNum];
    for (int i = 0; i < tMenu->entrySize[rowNum]; i++)
    {
        GraphicsSystem_DrawSprite(xPos + (i << 3), yPos, 8, 8, (int)((int)(tMenu->textData[num] & 0xF) << 3), (int)((int)(tMenu->textData[num] >> 4) << 3) + textHighL, textMenuSurfaceNo);
        num++;
    }
}
void TextSystem_DrawStageTextEntry(struct TextMenu* tMenu, int rowNum, int xPos, int yPos, int textHighL)
{
    int num = tMenu->entryStart[rowNum];
    for (int i = 0; i < tMenu->entrySize[rowNum]; i++)
    {
        if (i == tMenu->entrySize[rowNum] - 1)
        {
            GraphicsSystem_DrawSprite(xPos + (i << 3), yPos, 8, 8, (int)((int)(tMenu->textData[num] & 0xF) << 3), (int)((int)(tMenu->textData[num] >> 4) << 3), textMenuSurfaceNo);
        }
        else
        {
            GraphicsSystem_DrawSprite(xPos + (i << 3), yPos, 8, 8, (int)((int)(tMenu->textData[num] & 0xF) << 3), (int)((int)(tMenu->textData[num] >> 4) << 3) + textHighL, textMenuSurfaceNo);
        }
        num++;
    }
}
void TextSystem_DrawBlendedTextMenuEntry(struct TextMenu* tMenu, int rowNum, int xPos, int yPos, int textHighL)
{
    int num = tMenu->entryStart[rowNum];
    for (int i = 0; i < tMenu->entrySize[rowNum]; i++)
    {
        GraphicsSystem_DrawBlendedSprite(xPos + (i << 3), yPos, 8, 8, (int)((int)(tMenu->textData[num] & 0xF) << 3), (int)((int)(tMenu->textData[num] >> 4) << 3) + textHighL, textMenuSurfaceNo);
        num++;
    }
}
void TextSystem_DrawTextMenu(struct TextMenu* tMenu, int xPos, int yPos)
{
    int num;
    if (tMenu->numVisibleRows > 0)
    {
        num = (int)(tMenu->numVisibleRows + tMenu->visibleRowOffset);
    }
    else
    {
        tMenu->visibleRowOffset = 0;
        num = (int)tMenu->numRows;
    }
    if (tMenu->numSelections == 3)
    {
        tMenu->selection2 = -1;
        for (int i = 0; i < tMenu->selection1 + 1; i++)
        {
            if (tMenu->entryHighlight[i] == 1)
            {
                tMenu->selection2 = i;
            }
        }
    }
    switch (tMenu->alignment)
    {
        case 0:
            for (int i = (int)tMenu->visibleRowOffset; i < num; i++)
            {
                switch (tMenu->numSelections)
                {
                    case 1:
                        if (i == tMenu->selection1)
                        {
                            TextSystem_DrawTextMenuEntry(tMenu, i, xPos, yPos, 128);
                        }
                        else
                        {
                            TextSystem_DrawTextMenuEntry(tMenu, i, xPos, yPos, 0);
                        }
                        break;
                    case 2:
                        if (i == tMenu->selection1 || i == tMenu->selection2)
                        {
                            TextSystem_DrawTextMenuEntry(tMenu, i, xPos, yPos, 128);
                        }
                        else
                        {
                            TextSystem_DrawTextMenuEntry(tMenu, i, xPos, yPos, 0);
                        }
                        break;
                    case 3:
                        if (i == tMenu->selection1)
                        {
                            TextSystem_DrawTextMenuEntry(tMenu, i, xPos, yPos, 128);
                        }
                        else
                        {
                            TextSystem_DrawTextMenuEntry(tMenu, i, xPos, yPos, 0);
                        }
                        if (i == tMenu->selection2 && i != tMenu->selection1)
                        {
                            TextSystem_DrawStageTextEntry(tMenu, i, xPos, yPos, 128);
                        }
                        break;
                }
                yPos += 8;
            }
            return;
        case 1:
            for (int i = (int)tMenu->visibleRowOffset; i < num; i++)
            {
                int xPos2 = xPos - (tMenu->entrySize[i] << 3);
                switch (tMenu->numSelections)
                {
                    case 1:
                        if (i == tMenu->selection1)
                        {
                            TextSystem_DrawTextMenuEntry(tMenu, i, xPos2, yPos, 128);
                        }
                        else
                        {
                            TextSystem_DrawTextMenuEntry(tMenu, i, xPos2, yPos, 0);
                        }
                        break;
                    case 2:
                        if (i == tMenu->selection1 || i == tMenu->selection2)
                        {
                            TextSystem_DrawTextMenuEntry(tMenu, i, xPos2, yPos, 128);
                        }
                        else
                        {
                            TextSystem_DrawTextMenuEntry(tMenu, i, xPos2, yPos, 0);
                        }
                        break;
                    case 3:
                        if (i == tMenu->selection1)
                        {
                            TextSystem_DrawTextMenuEntry(tMenu, i, xPos2, yPos, 128);
                        }
                        else
                        {
                            TextSystem_DrawTextMenuEntry(tMenu, i, xPos2, yPos, 0);
                        }
                        if (i == tMenu->selection2 && i != tMenu->selection1)
                        {
                            TextSystem_DrawStageTextEntry(tMenu, i, xPos2, yPos, 128);
                        }
                        break;
                }
                yPos += 8;
            }
            return;
        case 2:
            for (int i = (int)tMenu->visibleRowOffset; i < num; i++)
            {
                int xPos2 = xPos - (tMenu->entrySize[i] >> 1 << 3);
                switch (tMenu->numSelections)
                {
                    case 1:
                        if (i == tMenu->selection1)
                        {
                            TextSystem_DrawTextMenuEntry(tMenu, i, xPos2, yPos, 128);
                        }
                        else
                        {
                            TextSystem_DrawTextMenuEntry(tMenu, i, xPos2, yPos, 0);
                        }
                        break;
                    case 2:
                        if (i == tMenu->selection1 || i == tMenu->selection2)
                        {
                            TextSystem_DrawTextMenuEntry(tMenu, i, xPos2, yPos, 128);
                        }
                        else
                        {
                            TextSystem_DrawTextMenuEntry(tMenu, i, xPos2, yPos, 0);
                        }
                        break;
                    case 3:
                        if (i == tMenu->selection1)
                        {
                            TextSystem_DrawTextMenuEntry(tMenu, i, xPos2, yPos, 128);
                        }
                        else
                        {
                            TextSystem_DrawTextMenuEntry(tMenu, i, xPos2, yPos, 0);
                        }
                        if (i == tMenu->selection2 && i != tMenu->selection1)
                        {
                            TextSystem_DrawStageTextEntry(tMenu, i, xPos2, yPos, 128);
                        }
                        break;
                }
                yPos += 8;
            }
            return;
        default:
            return;
    }
}
void TextSystem_LoadConfigListText(struct TextMenu* tMenu, int listNo)
{
    struct FileData fData;
    char array[32];
    if (FileIO_LoadFile("Data/Game/GameConfig.bin", &fData))
    {
        uint8_t b = FileIO_ReadByte();
        for (int i = 0; i < (int)b; i++)
        {
            FileIO_ReadByte();
        }
        b = FileIO_ReadByte();
        for (int i = 0; i < (int)b; i++)
        {
            FileIO_ReadByte();
        }
        b = FileIO_ReadByte();
        for (int i = 0; i < (int)b; i++)
        {
            FileIO_ReadByte();
        }
        uint8_t b3 = FileIO_ReadByte();
        for (int j = 0; j < (int)b3; j++)
        {
            b = FileIO_ReadByte();
            for (int i = 0; i < (int)b; i++)
            {
                FileIO_ReadByte();
            }
        }
        for (int j = 0; j < (int)b3; j++)
        {
            b = FileIO_ReadByte();
            for (int i = 0; i < (int)b; i++)
            {
                FileIO_ReadByte();
            }
        }
        b3 = FileIO_ReadByte();
        for (int j = 0; j < (int)b3; j++)
        {
            b = FileIO_ReadByte();
            uint8_t b2;
            for (int i = 0; i < (int)b; i++)
            {
                b2 = FileIO_ReadByte();
            }
            b2 = FileIO_ReadByte();
            b2 = FileIO_ReadByte();
            b2 = FileIO_ReadByte();
            b2 = FileIO_ReadByte();
        }
        b3 = FileIO_ReadByte();
        for (int j = 0; j < (int)b3; j++)
        {
            b = FileIO_ReadByte();
            for (int i = 0; i < (int)b; i++)
            {
                FileIO_ReadByte();
            }
        }
        b3 = FileIO_ReadByte();
        for (int j = 0; j < (int)b3; j++)
        {
            b = FileIO_ReadByte();
            int i;
            for (i = 0; i < (int)b; i++)
            {
                uint8_t b2 = FileIO_ReadByte();
                array[i] = (char)b2;
            }
            array[i] = '\0';
            if (listNo == 0)
            {
                TextSystem_AddTextMenuEntry(tMenu, array);
            }
        }
        for (int k = 1; k < 5; k++)
        {
            b3 = FileIO_ReadByte();
            for (int j = 0; j < (int)b3; j++)
            {
                b = FileIO_ReadByte();
                int i;
                uint8_t b2;
                for (i = 0; i < (int)b; i++)
                {
                    b2 = FileIO_ReadByte();
                }
                b = FileIO_ReadByte();
                for (i = 0; i < (int)b; i++)
                {
                    b2 = FileIO_ReadByte();
                }
                b = FileIO_ReadByte();
                for (i = 0; i < (int)b; i++)
                {
                    b2 = FileIO_ReadByte();
                    array[i] = (char)b2;
                }
                array[i] = '\0';
                b2 = FileIO_ReadByte();
                if (listNo == k)
                {
                    tMenu->entryHighlight[j] = b2;
                    TextSystem_AddTextMenuEntry(tMenu, array);
                }
            }
        }
        FileIO_CloseFile();
    }
}
