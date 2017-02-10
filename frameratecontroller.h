/**
 * @file	frameratecontroller.h
 * @brief	Functions to specify and lock the frame rate.
 */
#ifndef CPROJ_FRAMECONTROLLER
#define CPROJ_FRAMECONTROLLER

void frameRateControllerInitialize(const int frameRate);
void frameRateControllerRelease(void);

void setFrameRate(const int frameRate);
int getSpecifiedFrameRate(void);

/**
 * Returns current frame rate which might be different from the specified one.
 * @return	The current frame rate as an integer.
 */
int getCurrentFrameRate(void);

/**
 * Used in conjunction with 'frameRateControllerEnd' to lock the frame rate.
 * @return	A const int indicating the delta difference in time since the last frame.
 * @sa	frameRateControllerEnd()
 */
unsigned int frameRateControllerStart(void);

/** Locks application till the required frame rate is reached. */
void frameRateControllerEnd(void);

#endif