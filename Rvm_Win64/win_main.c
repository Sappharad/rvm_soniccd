#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "SDL.h"
#include "GlobalAppDefinitions.h"
#include <Windows.h>
#include <GL/glew.h>
#include "GraphicsSystem.h"

static SDL_Window* gWindow;

static void initAttributes()
{
	// Setup attributes we want for the OpenGL context

	int value;

	// Don't set color bit sizes (SDL_GL_RED_SIZE, etc)
	//    Mac OS X will always use 8-8-8-8 ARGB for 32-bit screens and
	//    5-5-5 RGB for 16-bit screens

	// Request a 16-bit depth buffer (without this, there is no depth buffer)
	value = 16;
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, value);


	// Request double-buffered OpenGL
	//     The fact that windows are double-buffered on Mac OS X has no effect
	//     on OpenGL double buffering.
	value = 1;
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, value);
}

static void printAttributes()
{
	// Print out attributes of the context we created
	int nAttr;
	int i;

	int  attr[] = { SDL_GL_RED_SIZE, SDL_GL_BLUE_SIZE, SDL_GL_GREEN_SIZE,
		SDL_GL_ALPHA_SIZE, SDL_GL_BUFFER_SIZE, SDL_GL_DEPTH_SIZE };

	char *desc[] = { "Red size: %d bits\n", "Blue size: %d bits\n", "Green size: %d bits\n",
		"Alpha size: %d bits\n", "Color buffer size: %d bits\n",
		"Depth bufer size: %d bits\n" };

	nAttr = sizeof(attr) / sizeof(int);

	for (i = 0; i < nAttr; i++) {

		int value;
		SDL_GL_GetAttribute(attr[i], &value);
		printf(desc[i], value);
	}
}

static int onWindowEvent(void* data, SDL_Event* event) {
	if (event->type == SDL_WINDOWEVENT && event->window.event == SDL_WINDOWEVENT_RESIZED) {
		SDL_Window* win = SDL_GetWindowFromID(event->window.windowID);
		if (win == (SDL_Window*)data) {
			int width, height;
			SDL_GetWindowSize(win, &width, &height);
			RenderDevice_ScaleViewport(width, height);
		}
		return 0;
	}
}

static void createSurface(int fullscreen)
{
	Uint32 flags = 0;

	flags = SDL_WINDOW_OPENGL;
	flags |= SDL_WINDOW_RESIZABLE;
	if (fullscreen)
		flags |= SDL_WINDOW_FULLSCREEN;

	// Create window
	gWindow = SDL_CreateWindow("RVM - Sonic CD",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		800, 480, flags);
	if (gWindow == NULL) {
		fprintf(stderr, "Couldn't set 800x480 OpenGL video mode: %s\n", SDL_GetError());
		SDL_Quit();
		exit(2);
	}
	SDL_AddEventWatch(onWindowEvent, gWindow);
	SDL_GL_CreateContext(gWindow);
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}

	glViewport(0, 0, 800, 480);
	//glViewport(0, 0, 864, 480);
	//glViewport(0, 0, 1040, 480);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-2.0, 2.0, -2.0, 2.0, -20.0, 20.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_DEPTH_TEST);

	glDepthFunc(GL_LESS);

	glShadeModel(GL_SMOOTH);
}

void UpdateIO() {
	InputSystem_CheckKeyboardInput();
	InputSystem_CheckGamepadInput();
	InputSystem_ClearTouchData();

	if (stageMode != 2)
	{
		EngineCallbacks_ProcessMainLoop();
	}
}
void HandleNextFrame() {
	if (stageMode == 2)
	{
		EngineCallbacks_ProcessMainLoop();
	}
	if (highResMode == 0)
	{
		RenderDevice_FlipScreen();
	}
	else
	{
		RenderDevice_FlipScreenHRes();
	}
}

static void mainLoop()
{
	SDL_Event event;
	int done = 0;
	int fps = 60;
	int delay = 1000 / fps;
	int thenTicks = -1;
	int nowTicks;

	while (!done) {

		/* Check for events */
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_MOUSEMOTION:
				break;
			case SDL_MOUSEBUTTONDOWN:
				break;
			case SDL_KEYDOWN:
				//DumpTexBuffer();
				break;
			case SDL_QUIT:
				done = 1;
				break;
			default:
				break;
			}
		}

		// Draw at 24 hz
		//     This approach is not normally recommended - it is better to
		//     use time-based animation and run as fast as possible
		UpdateIO();
		HandleNextFrame();
		SDL_GL_SwapWindow(gWindow);

		// Time how long each draw-swap-delay cycle takes
		// and adjust delay to get closer to target framerate
		if (thenTicks > 0) {
			nowTicks = SDL_GetTicks();
			delay += (1000 / fps - (nowTicks - thenTicks));
			thenTicks = nowTicks;
			if (delay < 0)
				delay = 1000 / fps;
		}
		else {
			thenTicks = SDL_GetTicks();
		}

		SDL_Delay(delay);
	}
}

void Init_RetroVM() {
	Init_GlobalAppDefinitions();
	GlobalAppDefinitions_CalculateTrigAngles();
	InitRenderDevice();
	Init_FileIO();
	Init_InputSystem();
	Init_ObjectSystem();
	Init_AnimationSystem();
	Init_PlayerSystem();
	Init_StageSystem();
	Init_Scene3D();
	RenderDevice_SetScreenDimensions(800, 480);
	//RenderDevice_SetScreenDimensions(864, 480);
	//RenderDevice_SetScreenDimensions(1040, 480);
	EngineCallbacks_StartupRetroEngine();
	gameLanguage = 0;
	gameTrialMode = GAME_FULL;
}

int SDL_main(int argc, char *argv[])
{
	// Init SDL video subsystem
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) < 0) {

		fprintf(stderr, "Couldn't initialize SDL: %s\n",
			SDL_GetError());
		exit(1);
	}

	// Set GL context attributes
	initAttributes();

	// Create GL context
	createSurface(0);

	// Get GL context attributes
	printAttributes();

	Init_RetroVM();

	// Draw, get events...
	mainLoop();

	// Cleanup
	InputSystem_Dispose();
	SDL_Quit();

	return 0;
}
