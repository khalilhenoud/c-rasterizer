#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "application.h"

/* The name of your window class. */
const char *className = "MyWindow";
/* Targetting 640 by 480, will be read from a config file in the end. */
int g_WindowWidth = 640;
int g_WindowHeight = 480;
int g_ScreenWidth = 0;
int g_ScreenHeight = 0;
HWND g_hWnd;
FILE *stream;

LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nShowCmd)
{
    /* The window class struct. */
    WNDCLASSEX wc;
    /* Handle to our window. */
    HWND hWnd;
    MSG msg;

#ifdef _DEBUG
    #ifdef _MSC_VER
        AllocConsole();
        AttachConsole(GetCurrentProcessId());
        freopen_s(&stream, "CON", "w", stdout);
    #endif // _MSC_VER
#endif

	/* Getting the screen width and height, used in position centering computation. */
	g_ScreenWidth = GetSystemMetrics(SM_CXFULLSCREEN);
	g_ScreenHeight = GetSystemMetrics(SM_CYFULLSCREEN);

    /* The size of the structure. */
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
    wc.hCursor = LoadCursor(0, IDC_ARROW);
    wc.hIcon = 0;
    wc.hIconSm = 0;
    wc.hInstance = hInstance;
    wc.lpfnWndProc = wndProc;
    wc.lpszClassName = className;
    wc.lpszMenuName = 0;
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

    /* Register the class to be used with 'CreateWindow'. */
    if (!RegisterClassEx(&wc)) {
        MessageBox(NULL, "Could not register window.", "Error", MB_ICONEXCLAMATION | MB_OK);
        return -1;
    }

    /* Create the window using the 'MyWindow' class. */
    hWnd = CreateWindowEx(WS_EX_WINDOWEDGE, className, "C Project",
        WS_OVERLAPPEDWINDOW, (g_ScreenWidth - g_WindowWidth)/2, (g_ScreenHeight - g_WindowHeight)/2, g_WindowWidth, g_WindowHeight,
        NULL, NULL, hInstance, NULL);

    /* If the window was not created show error and exit. */
    if (hWnd == NULL) {
        MessageBox(NULL, "Could not create window.", "Error", MB_ICONEXCLAMATION | MB_OK);
        return -1;
    }

	/* Store the handle to the window. */
	g_hWnd = hWnd;

    /* Set the windows show state, to show it. */
    ShowWindow(hWnd, nShowCmd);
    UpdateWindow(hWnd);

	/* Application initialization. */
	initializeApplication();

	while (1) {
		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT)
				goto End;
			else
				DispatchMessage(&msg);
		}

		/* Application update. */
		updateApplication();
    }

End:
	/* Application end. */
	endApplication();

    return msg.wParam;
}

LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg,
                         WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CLOSE:
            DestroyWindow(hWnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}
