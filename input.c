#include <windows.h>

struct KeyState {
	unsigned char m_IsPressed;
	unsigned char m_IsTriggered;
	unsigned char m_WasPressed;
	unsigned char m_Reserved;
};

static struct KeyState m_Keys[3];

int isMouseLButtonPressed(void)
{
	return m_Keys[0].m_IsPressed;
}

int isMouseLButtonTriggered(void)
{
	return m_Keys[0].m_IsTriggered;
}

int isMouseMButtonPressed(void)
{
	return m_Keys[1].m_IsPressed;
}

int isMouseMButtonTriggered(void)
{
	return m_Keys[1].m_IsTriggered;
}

int isMouseRButtonPressed(void)
{
	return m_Keys[2].m_IsPressed;
}

int isMouseRButtonTriggered(void)
{
	return m_Keys[2].m_IsTriggered;
}

void getMousePosition(int *x, int *y)
{
	extern HWND g_hWnd;
	POINT pn;
	GetCursorPos(&pn);
	ScreenToClient(g_hWnd, &pn);
	*x = pn.x;
	*y = pn.y;
}

void mouseUpdate(void)
{
	int i;
	
	SHORT lbs = GetAsyncKeyState(VK_LBUTTON);
	SHORT mbs = GetAsyncKeyState(VK_MBUTTON);
	SHORT rbs = GetAsyncKeyState(VK_RBUTTON);

	SHORT ks[3] = {0};
	ks[0] = lbs;
	ks[1] = mbs;
	ks[2] = rbs;

	for (i = 0; i < 3; ++i) {
		m_Keys[i].m_IsPressed = ks[i] & (1 << 15) ? 1 : 0;

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

/* Holds the keyboard key states. */
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
	BYTE ks[256] = {0};
	GetKeyboardState(ks);
						
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