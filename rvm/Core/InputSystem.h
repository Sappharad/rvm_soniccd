//
//  InputSystem.h
//  rvm
//

#ifndef InputSystem_h
#define InputSystem_h

#include <stdio.h>
#include <stdbool.h>
#include "InputResult.h"
#include "GlobalAppDefinitions.h"
#include "SDL.h"

extern int touchWidth;
extern int touchHeight;

void Init_InputSystem(void);
void InputSystem_Dispose(void);
void InputSystem_AddTouch(float touchX, float touchY, int pointerID);
void InputSystem_SetTouch(float touchX, float touchY, int pointerID);
void InputSystem_RemoveTouch(int pointerID);
void InputSystem_ClearTouchData(void);
void InputSystem_CheckKeyboardInput(void);
void InputSystem_CheckGamepadInput(void);
void InputSystem_CheckKeyDown(struct InputResult* gameInput, uint8_t keyFlags);
void InputSystem_MenuKeyDown(struct InputResult* gameInput, uint8_t keyFlags);
void InputSystem_CheckKeyPress(struct InputResult* gameInput, uint8_t keyFlags);

#endif /* InputSystem_h */
