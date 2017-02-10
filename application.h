/**
 * @file	application.h
 * @brief	Systems initialization functions and core game loop.
 */
#ifndef CPROJ_APPLICATION
#define CPROJ_APPLICATION

/**
 * Initializes critical application systems. Initializes the renderer, the frame rate controller
 * and runs any used defined code in function 'mycodeInitialize'.
 */
void initializeApplication(void);

/**
 * Core application loop. Clears the current frame, updates input, runs user defined code,
 * finally swaps frame buffer and locks framerate.
 */
void updateApplication(void);

/** Releases the systems initialized in 'initializeApplication'. */
void endApplication(void);

#endif