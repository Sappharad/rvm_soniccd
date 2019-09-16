/* 
   rvm - Dreamcast
*/

#include <kos.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "GlobalAppDefinitions.h"

void Init_RetroVM(){
    printf("GlobalAppDefinitions_CalculateTrigAngles\n");
    GlobalAppDefinitions_CalculateTrigAngles();
    
    printf("InitRenderDevice\n");
    InitRenderDevice();
    
    printf("Init Input\n");
    Init_InputSystem();
    Init_AnimationSystem();
    
    printf("Init Player\n");
    Init_PlayerSystem();
    RenderDevice_SetScreenDimensions(640, 480);
    
    printf("Startup Retro Engine\n");
    EngineCallbacks_StartupRetroEngine();
    gameLanguage = 0;
    gameTrialMode = GAME_FULL;
}

void UpdateIO(){
    InputSystem_CheckKeyboardInput();
    InputSystem_ClearTouchData();
    
    if (stageMode != 2)
    {
        EngineCallbacks_ProcessMainLoop();
    }
}
void HandleNextFrame(){
    if (stageMode == 2)
    {
        EngineCallbacks_ProcessMainLoop();
    }
    if (highResMode == 0 && !render3DEnabled)
    {
        RenderDevice_FlipScreenHRes();
    }
    else
    {
        RenderDevice_FlipScreen();
    }
}

void mainLoop ()
{
    /*maple_device_t *cont;
    cont_state_t *state;*/
    //SDL_Event event;
    int done = 0;
    int fps = 60;
    int delay = 1000/fps;
    int thenTicks = -1;
    int nowTicks;
    
    while ( !done ) {
        /*cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
        state = (cont_state_t *)maple_dev_status(cont);

        if(!state) {
            printf("Error reading controller\n");
            break;
        }

        if(state->buttons & CONT_START)
            break;*/

        /* Check for events */
        /*while ( SDL_PollEvent (&event) ) {
            switch (event.type) {
                case SDL_MOUSEMOTION:
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    break;
                case SDL_KEYDOWN:
                    break;
                case SDL_QUIT:
                    done = 1;
                    break;
                default:
                    break;
            }
        }*/
        
        // Draw at 24 hz
        //     This approach is not normally recommended - it is better to
        //     use time-based animation and run as fast as possible
        UpdateIO();
        HandleNextFrame();
        /* Finish the frame - Notice there is no glKosBegin/FinshFrame */
        glutSwapBuffers();
        
        // Time how long each draw-swap-delay cycle takes
        // and adjust delay to get closer to target framerate
        /*if (thenTicks > 0) {
            nowTicks = SDL_GetTicks ();
            delay += (1000/fps - (nowTicks-thenTicks));
            thenTicks = nowTicks;
            if (delay < 0)
                delay = 1000/fps;
        }
        else {
            thenTicks = SDL_GetTicks ();
        }
        
        SDL_Delay (delay);*/
    }
}

int main(int argc, char **argv) {
    printf("RetroVM Startup\n");
    glKosInit();
    printf("KOS init complete\n");
    chdir("pc/");
    //chdir("cd/");

    
    Init_RetroVM();
    mainLoop();
    
    return 0;
}

