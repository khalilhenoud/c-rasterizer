/**
 * @author  Khalil El Henoud
 * @brief   Keyboard, mouse input.
 */
#ifndef CPROJ_INPUT
#define CPROJ_INPUT

/// Use these key definitions with the functions below.
#define KEY_ESCAPE      0x1B
#define KEY_CONTROL     0x11
#define KEY_SHIFT       0x10
#define KEY_RETURN      0x0D
#define KEY_TAB         0x09
#define KEY_BACK        0x08
#define KEY_SPACE       0x20
#define KEY_PAGE_UP     0x21
#define KEY_PAGE_DOWN   0x22
#define KEY_END         0x23
#define KEY_HOME        0x24
#define KEY_LEFT        0x25
#define KEY_UP          0x26
#define KEY_RIGHT       0x27
#define KEY_DOWN        0x28
#define KEY_0           0x30
#define KEY_1           0x31
#define KEY_2           0x32
#define KEY_3           0x33
#define KEY_4           0x34
#define KEY_5           0x35
#define KEY_6           0x36
#define KEY_7           0x37
#define KEY_8           0x38
#define KEY_9           0x39
#define KEY_1           0x31
#define KEY_NUM_0       0x60
#define KEY_NUM_1       0x61
#define KEY_NUM_2       0x62
#define KEY_NUM_3       0x63
#define KEY_NUM_4       0x64
#define KEY_NUM_5       0x65
#define KEY_NUM_6       0x66
#define KEY_NUM_7       0x67
#define KEY_NUM_8       0x68
#define KEY_NUM_9       0x69
#define KEY_F1          0x70
#define KEY_F2          0x71
#define KEY_F3          0x72
#define KEY_F4          0x73
#define KEY_F5          0x74
#define KEY_F6          0x75
#define KEY_F7          0x76
#define KEY_F8          0x77
#define KEY_F9          0x78

int isMouseLeftButtonPressed(void);
int isMouseLeftButtonTriggered(void);

int isMouseMiddleButtonPressed(void);
int isMouseMiddleButtonTriggered(void);

int isMouseRightButtonPressed(void);
int isMouseRightButtonTriggered(void);

int isKeyPressed(int vKey);
int isKeyTriggered(int vKey);

/// Call each frame to get mouse and key status: pressed, triggered, !pressed.
void mouseUpdate(void);
void keyboardUpdate(void);

#endif