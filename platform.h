/** 
 * @author  Khalil El Henoud
 * @brief   Platform abstraction file; most functionality needed to abstract the OS.
 */
#ifndef PLATFORM_H
#define PLATFORM_H

// Window functionality.
#define ALREADYCREATED    1
#define COULDNOTREGISTER  2
#define COULDNOTCREATE    3

int setupWindow(const char *winClassType, int width, int height, int show);
void cleanupWindow(void);
void blitAndSwap(void *buffer);
void setWindowTitle(char const *title);
void signalQuitApplication(void);
int getWindowRect(long int *const x, long int *const y, long int *const width, long int *const height);
int getWindowClientAreaRect(long int *const width, long int *const height);
int screenToClient(long int *const x, long int *const y);
int clientToScreen(long int *const x, long int *const y);
int getWindowWidth(void);
int getWindowHeight(void);

// Cursor functionality.
int setCursorScreenPosition(int x, int y);
int getCursorScreenPosition(int *const x, int *const y);
int getCursorClientPosition(int *const x, int *const y);
int moveCursorToClientArea(void);
void showCursor(const int show);

// Timer functionality.
int startWatchPrecision(unsigned int period);
int endWatchPrecision(unsigned int period);
unsigned long getSystemTime(void);

// Input functionality.
int isLeftMouseButtonDown(void);
int isRightMouseButtonDown(void);
int isMiddleMouseButtonDown(void);
int getKeyboardState(unsigned char keys[256]);

#endif