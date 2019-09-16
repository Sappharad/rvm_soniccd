//
//  InputSystem.c
//  rvm
//

#include "InputSystem_dc.h"

const int BUTTON_UP = 1;
const int BUTTON_DOWN = 2;
const int BUTTON_LEFT = 4;
const int BUTTON_RIGHT = 8;
const int BUTTON_A = 16;
const int BUTTON_B = 32;
const int BUTTON_C = 64;
const int BUTTON_START = 128;
const int ALL_BUTTONS = 255;
const int NO_BUTTONS = 0;
int touchWidth;
int touchHeight;
bool touchControls;
struct InputResult inputPress;
struct InputResult touchData;
maple_device_t *cont;

void Init_InputSystem()
{
    touchControls = false;
    cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
}

void InputSystem_AddTouch(float touchX, float touchY, int pointerID)
{
    for (int i = 0; i < 4; i++)
    {
        if (touchData.touchDown[i] == 0)
        {
            touchData.touchDown[i] = 1;
            touchData.touchY[i] = (int)touchY;
            touchData.touchX[i] = (int)touchX;
            touchData.touchID[i] = pointerID;
            i = 4;
        }
    }
}

void InputSystem_SetTouch(float touchX, float touchY, int pointerID)
{
    for (int i = 0; i < 4; i++)
    {
        if (touchData.touchID[i] == pointerID && touchData.touchDown[i] == 1)
        {
            touchData.touchY[i] = (int)touchY;
            touchData.touchX[i] = (int)touchX;
            i = 4;
        }
    }
}

void InputSystem_RemoveTouch(int pointerID)
{
    for (int i = 0; i < 4; i++)
    {
        if (touchData.touchID[i] == pointerID)
        {
            touchData.touchDown[i] = 0;
        }
    }
}

void InputSystem_ClearTouchData()
{
    touchData.touches = 0;
    touchData.touchDown[0] = 0;
    touchData.touchDown[1] = 0;
    touchData.touchDown[2] = 0;
    touchData.touchDown[3] = 0;
}

void InputSystem_CheckKeyboardInput()
{
    cont_state_t *state = (cont_state_t *)maple_dev_status(cont);
    if(state == NULL){
        printf("Maple Dev status is null!\n");
        return;
    }
    
    if (state->buttons & CONT_DPAD_UP)
    {
        touchData.up = 1;
    }
    else
    {
        touchData.up = 0;
    }
    if (state->buttons & CONT_DPAD_DOWN)
    {
        touchData.down = 1;
    }
    else
    {
        touchData.down = 0;
    }
    if (state->buttons & CONT_DPAD_LEFT)
    {
        touchData.left = 1;
    }
    else
    {
        touchData.left = 0;
    }
    if (state->buttons & CONT_DPAD_RIGHT)
    {
        touchData.right = 1;
    }
    else
    {
        touchData.right = 0;
    }
    if (state->buttons & CONT_A)
    {
        touchData.buttonA = 1;
    }
    else
    {
        touchData.buttonA = 0;
    }
    if (state->buttons & CONT_B)
    {
        touchData.buttonB = 1;
    }
    else
    {
        touchData.buttonB = 0;
    }
    if (state->buttons & CONT_X)
    {
        touchData.buttonC = 1;
    }
    else
    {
        touchData.buttonC = 0;
    }
    if (state->buttons & CONT_START)
    {
        touchData.start = 1;
    }
    else
    {
        touchData.start = 0;
    }
    /*if (state->buttons & (CONT_START | CONT_A ))
    {
        if (activeStageList != 0 && stageMode != 2)
        {
            gameMessage = 2;
        }
    }*/
}

void InputSystem_CheckKeyDown(struct InputResult* gameInput, uint8_t keyFlags)
{
    gameInput->touches = 0;
    for (int i = 0; i < 4; i++)
    {
        if (touchData.touchDown[i] == 1)
        {
            gameInput->touchDown[gameInput->touches] = touchData.touchDown[i];
            gameInput->touchX[gameInput->touches] = touchData.touchX[i] * SCREEN_XSIZE / touchWidth;
            gameInput->touchY[gameInput->touches] = touchData.touchY[i] * 240 / touchHeight;
            gameInput->touches++;
        }
    }
    if ((BUTTON_UP & keyFlags) == BUTTON_UP)
    {
        gameInput->up = touchData.up;
    }
    if ((BUTTON_DOWN & keyFlags) == BUTTON_DOWN)
    {
        gameInput->down = touchData.down;
    }
    if ((BUTTON_LEFT & keyFlags) == BUTTON_LEFT)
    {
        gameInput->left = touchData.left;
    }
    if ((BUTTON_RIGHT & keyFlags) == BUTTON_RIGHT)
    {
        gameInput->right = touchData.right;
    }
    if ((BUTTON_A & keyFlags) == BUTTON_A)
    {
        gameInput->buttonA = touchData.buttonA;
    }
    if ((BUTTON_B & keyFlags) == BUTTON_B)
    {
        gameInput->buttonB = touchData.buttonB;
    }
    if ((BUTTON_C & keyFlags) == BUTTON_C)
    {
        gameInput->buttonC = touchData.buttonC;
    }
    if ((BUTTON_START & keyFlags) == BUTTON_START)
    {
        gameInput->start = touchData.start;
    }
}

void InputSystem_MenuKeyDown(struct InputResult* gameInput, uint8_t keyFlags)
{
    gameInput->touches = 0;
    for (int i = 0; i < 4; i++)
    {
        if (touchData.touchDown[i] == 1)
        {
            gameInput->touchDown[gameInput->touches] = touchData.touchDown[i];
            gameInput->touchX[gameInput->touches] = touchData.touchX[i] * SCREEN_XSIZE / touchWidth;
            gameInput->touchY[gameInput->touches] = touchData.touchY[i] * 240 / touchHeight;
            gameInput->touches++;
        }
    }
}

void InputSystem_CheckKeyPress(struct InputResult* gameInput, uint8_t keyFlags)
{
    if ((BUTTON_UP & keyFlags) == BUTTON_UP)
    {
        if (touchData.up == 1)
        {
            if (inputPress.up == 0)
            {
                inputPress.up = 1;
                gameInput->up = 1;
            }
            else
            {
                gameInput->up = 0;
            }
        }
        else
        {
            gameInput->up = 0;
            inputPress.up = 0;
        }
    }
    if ((BUTTON_DOWN & keyFlags) == BUTTON_DOWN)
    {
        if (touchData.down == 1)
        {
            if (inputPress.down == 0)
            {
                inputPress.down = 1;
                gameInput->down = 1;
            }
            else
            {
                gameInput->down = 0;
            }
        }
        else
        {
            gameInput->down = 0;
            inputPress.down = 0;
        }
    }
    if ((BUTTON_LEFT & keyFlags) == BUTTON_LEFT)
    {
        if (touchData.left == 1)
        {
            if (inputPress.left == 0)
            {
                inputPress.left = 1;
                gameInput->left = 1;
            }
            else
            {
                gameInput->left = 0;
            }
        }
        else
        {
            gameInput->left = 0;
            inputPress.left = 0;
        }
    }
    if ((BUTTON_RIGHT & keyFlags) == BUTTON_RIGHT)
    {
        if (touchData.right == 1)
        {
            if (inputPress.right == 0)
            {
                inputPress.right = 1;
                gameInput->right = 1;
            }
            else
            {
                gameInput->right = 0;
            }
        }
        else
        {
            gameInput->right = 0;
            inputPress.right = 0;
        }
    }
    if ((BUTTON_A & keyFlags) == BUTTON_A)
    {
        if (touchData.buttonA == 1)
        {
            if (inputPress.buttonA == 0)
            {
                inputPress.buttonA = 1;
                gameInput->buttonA = 1;
            }
            else
            {
                gameInput->buttonA = 0;
            }
        }
        else
        {
            gameInput->buttonA = 0;
            inputPress.buttonA = 0;
        }
    }
    if ((BUTTON_B & keyFlags) == BUTTON_B)
    {
        if (touchData.buttonB == 1)
        {
            if (inputPress.buttonB == 0)
            {
                inputPress.buttonB = 1;
                gameInput->buttonB = 1;
            }
            else
            {
                gameInput->buttonB = 0;
            }
        }
        else
        {
            gameInput->buttonB = 0;
            inputPress.buttonB = 0;
        }
    }
    if ((BUTTON_C & keyFlags) == BUTTON_C)
    {
        if (touchData.buttonC == 1)
        {
            if (inputPress.buttonC == 0)
            {
                inputPress.buttonC = 1;
                gameInput->buttonC = 1;
            }
            else
            {
                gameInput->buttonC = 0;
            }
        }
        else
        {
            gameInput->buttonC = 0;
            inputPress.buttonC = 0;
        }
    }
    if ((BUTTON_START & keyFlags) == BUTTON_START)
    {
        if (touchData.start == 1)
        {
            if (inputPress.start == 0)
            {
                inputPress.start = 1;
                gameInput->start = 1;
                return;
            }
            gameInput->start = 0;
            return;
        }
        else
        {
            gameInput->start = 0;
            inputPress.start = 0;
        }
    }
}
