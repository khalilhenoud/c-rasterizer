/**
 * @file	input.h
 * @brief	Input functions.
 */
#ifndef CPROJ_INPUT
#define CPROJ_INPUT

int isMouseLButtonPressed(void);
int isMouseLButtonTriggered(void);

int isMouseMButtonPressed(void);
int isMouseMButtonTriggered(void);

int isMouseRButtonPressed(void);
int isMouseRButtonTriggered(void);

void getMousePosition(int *x, int *y);

int isKeyPressed(int vKey);
int isKeyTriggered(int vKey);

/**
 * Called each frame to update the corresponding input state.
 * @sa	keyboardUpdate()
 */
void mouseUpdate(void);

void keyboardUpdate(void);

#endif