#include "platform.h"

struct KeyState {
	unsigned char m_IsPressed;
	unsigned char m_IsTriggered;
	unsigned char m_WasPressed;
	unsigned char m_Reserved;
};

static struct KeyState m_Keys[3];

int isMouseLeftButtonPressed(void)
{
	return m_Keys[0].m_IsPressed;
}

int isMouseLeftButtonTriggered(void)
{
	return m_Keys[0].m_IsTriggered;
}

int isMouseMiddleButtonPressed(void)
{
	return m_Keys[1].m_IsPressed;
}

int isMouseMiddleButtonTriggered(void)
{
	return m_Keys[1].m_IsTriggered;
}

int isMouseRightButtonPressed(void)
{
	return m_Keys[2].m_IsPressed;
}

int isMouseRightButtonTriggered(void)
{
	return m_Keys[2].m_IsTriggered;
}

void mouseUpdate(void)
{
	int i;
	int pressed[3] = { isLeftMouseButtonDown(), isMiddleMouseButtonDown(), isRightMouseButtonDown() };

	for (i = 0; i < 3; ++i) {
    m_Keys[i].m_IsPressed = pressed[i];

		if (m_Keys[i].m_IsPressed) {
			if (m_Keys[i].m_WasPressed)
				m_Keys[i].m_IsTriggered = 0;
			else {
                m_Keys[i].m_WasPressed = 1;
                m_Keys[i].m_IsTriggered = 1;
			}
		} else {
			m_Keys[i].m_IsTriggered = 0;
			m_Keys[i].m_WasPressed = 0;
		}
	}
}

static struct KeyState m_KeyboardKeys[256];

int isKeyPressed(int vKey)
{
	return m_KeyboardKeys[vKey].m_IsPressed;
}

int isKeyTriggered(int vKey)
{
	return m_KeyboardKeys[vKey].m_IsTriggered;
}

void keyboardUpdate(void)
{
	int i;
	unsigned char ks[256] = {0};
	getKeyboardState(ks);
						
	for (i = 0; i < 256; ++i) {
		m_KeyboardKeys[i].m_IsPressed = ks[i] & (1 << 7) ? 1 : 0;
		
		if (m_KeyboardKeys[i].m_IsPressed) {		
			if (m_KeyboardKeys[i].m_WasPressed)
				m_KeyboardKeys[i].m_IsTriggered = 0;						
			else {
                m_KeyboardKeys[i].m_WasPressed = 1;
                m_KeyboardKeys[i].m_IsTriggered = 1;
			}
		} else {
			m_KeyboardKeys[i].m_IsTriggered = 0;
			m_KeyboardKeys[i].m_WasPressed = 0;
		}
	}
}