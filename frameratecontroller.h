/**
 * @author  Khalil El Henoud
 * @brief   Sets and optionally limit the frame rate of the application.
 */
#ifndef FRAMECONTROLLER_H
#define FRAMECONTROLLER_H

void frameRateControllerInitialize(const int frameRate, const int lock);
void frameRateControllerRelease(void);

void setFrameRate(const int frameRate);
int getSpecifiedFrameRate(void);
int getActualFrameRate(void);

/**
 * Used in conjunction with frameRateControllerEnd to report/lock framerate.
 * @return  The delta difference in milliseconds since the last frame.
 */
unsigned int frameRateControllerStart(void);

/// If required, endlessly loops till the required frame rate is reached.
void frameRateControllerEnd(void);

#endif