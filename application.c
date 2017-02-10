#include <windows.h>
#include <stdio.h>
#include "frameratecontroller.h"
#include "renderer.h"
#include "input.h"

#ifndef _MSC_VER
    #define sprintf_s(A, B, C, D) sprintf(A, C, D)
#endif

/* User defined functions. */
extern void mycodeInitialize(void);
extern void mycodeUpdate(const unsigned int delta);
extern void mycodeEnd(void);

void initializeApplication(void)
{
	frameRateControllerInitialize(60);
	initializeRenderer();

	mycodeInitialize();
}

static char array[100] = {0};

void updateApplication(void)
{
	extern HWND g_hWnd;

	clearColorAndDepthBuffer();

	/* Update input. */
	keyboardUpdate();
	mouseUpdate();

	/* User defined update code. */
	mycodeUpdate(frameRateControllerStart());

	swapBuffer();

	sprintf_s(array, sizeof(array), "C Project: %ifps", getCurrentFrameRate());
	SetWindowText(g_hWnd, array);

	frameRateControllerEnd();
}

void endApplication(void)
{
	frameRateControllerRelease();
	releaseRenderer();

	mycodeEnd();
}
