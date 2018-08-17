//
//  TextSystem.h
//  rvm
//

#ifndef TextSystem_h
#define TextSystem_h

#include <stdio.h>
#include <string.h>
#include "FontCharacter.h"
#include "TextMenu.h"
#include "FileData.h"
#include "FileIO.h"
#include "GraphicsSystem.h"

extern int textMenuSurfaceNo;

void TextSystem_LoadFontFile(char* fileName);
void TextSystem_LoadTextFile(struct TextMenu* tMenu, char* fileName, uint8_t mapCode);
void TextSystem_DrawBitmapText(struct TextMenu* tMenu, int xPos, int yPos, int scale, int spacing, int rowStart, int numRows);
void TextSystem_SetupTextMenu(struct TextMenu* tMenu, int numRows);
void TextSystem_AddTextMenuEntry(struct TextMenu* tMenu, char* inputTxt);
void TextSystem_AddTextMenuEntryMapped(struct TextMenu* tMenu, char* inputTxt);
void TextSystem_SetTextMenuEntry(struct TextMenu* tMenu, char* inputTxt, int rowNum);
void TextSystem_EditTextMenuEntry(struct TextMenu* tMenu, char* inputTxt, int rowNum);
void TextSystem_DrawTextMenuEntry(struct TextMenu* tMenu, int rowNum, int xPos, int yPos, int textHighL);
void TextSystem_DrawStageTextEntry(struct TextMenu* tMenu, int rowNum, int xPos, int yPos, int textHighL);
void TextSystem_DrawBlendedTextMenuEntry(struct TextMenu* tMenu, int rowNum, int xPos, int yPos, int textHighL);
void TextSystem_DrawTextMenu(struct TextMenu* tMenu, int xPos, int yPos);
void TextSystem_LoadConfigListText(struct TextMenu* tMenu, int listNo);

#endif /* TextSystem_h */
