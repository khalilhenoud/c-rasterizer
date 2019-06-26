#include <windows.h>
#include <GL/gl.h>
#include "platform.h"

static int g_WindowWidth;
static int g_WindowHeight;
static HWND g_hWnd;

static HDC hWindowDC;
static HGLRC hWindowRC;

void Initialize(void);
void Update(void);
void Cleanup(void);

LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nShowCmd)
{
  MSG msg;
 
  Initialize();

  while (1) {
    while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
      if (msg.message == WM_QUIT)
        goto End;
      else
        DispatchMessage(&msg);
    }

    Update();
  }

End:
  Cleanup();

  return msg.wParam;
}

LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
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

int setupWindow(const char *winClassType, int width, int height, int show)
{
  WNDCLASSEX wc;
  int screenWidth = GetSystemMetrics(SM_CXSCREEN);
  int screenHeight = GetSystemMetrics(SM_CYSCREEN);

  if (g_hWnd)
    return ALREADYCREATED;

  wc.cbSize = sizeof(WNDCLASSEX);
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
  wc.hCursor = LoadCursor(0, IDC_ARROW);
  wc.hIcon = 0;
  wc.hIconSm = 0;
  wc.hInstance = GetModuleHandle(NULL);
  wc.lpfnWndProc = wndProc;
  wc.lpszClassName = winClassType;
  wc.lpszMenuName = 0;
  wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

  if (!RegisterClassEx(&wc))
    return COULDNOTREGISTER;

  // Create window and store its handle.
  g_hWnd = CreateWindowEx(WS_EX_WINDOWEDGE, winClassType, "C Project",
    WS_OVERLAPPEDWINDOW, (screenWidth - width) / 2, (screenHeight - height) / 2, width, height,
    NULL, NULL, GetModuleHandle(NULL), NULL);

  if (!g_hWnd)
    return COULDNOTCREATE;

  ShowWindow(g_hWnd, show);
  UpdateWindow(g_hWnd);

  g_WindowWidth = width;
  g_WindowHeight = height;

  // Initialize OpenGL.
  PIXELFORMATDESCRIPTOR kPFD = { 0 };
  int iPixelFormat;

  hWindowDC = GetDC(g_hWnd);
  kPFD.nSize = sizeof(PIXELFORMATDESCRIPTOR);
  kPFD.nVersion = 1;
  kPFD.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER | PFD_GENERIC_ACCELERATED;
  kPFD.dwLayerMask = PFD_MAIN_PLANE;
  kPFD.iPixelType = PFD_TYPE_RGBA;
  kPFD.cColorBits = 32;
  kPFD.cDepthBits = 24;

  iPixelFormat = ChoosePixelFormat(hWindowDC, &kPFD);
  SetPixelFormat(hWindowDC, iPixelFormat, &kPFD);
  hWindowRC = wglCreateContext(hWindowDC);
  wglMakeCurrent(hWindowDC, hWindowRC);

  return 0;
}

void cleanupWindow(void)
{
  wglDeleteContext(hWindowRC);
  ReleaseDC(g_hWnd, hWindowDC);
}

void blitAndSwap(void *buffer)
{
  glDrawPixels(g_WindowWidth, g_WindowHeight, GL_RGB, GL_UNSIGNED_BYTE, buffer);
  SwapBuffers(hWindowDC);
}

int getWindowWidth(void)
{
  return g_WindowWidth;
}

int getWindowHeight(void)
{
  return g_WindowHeight;
}

void setWindowTitle(char const *title)
{
  SetWindowText(g_hWnd, title);
}

void signalQuitApplication(void)
{
  PostQuitMessage(0);
}

int getWindowRect(long int *const x, long int *const y, long int *const width, long int *const height)
{
  RECT rect;
  if (GetWindowRect(g_hWnd, &rect)) {
    *x = rect.left;
    *y = rect.top;
    *width = rect.right - 1 - rect.left;
    *height = rect.bottom - 1 - rect.top;

    return 1;
  }
 
  return 0;
}

int getWindowClientAreaRect(long int *const width, long int *const height)
{
  RECT rect;
  if (GetClientRect(g_hWnd, &rect)) {
    *width = rect.right - 1;
    *height = rect.bottom - 1;

    return 1;
  }

  return 0;
}

int screenToClient(long int *const x, long int *const y)
{
  POINT pt = { *x, *y };
  if (ScreenToClient(g_hWnd, &pt)) {
    *x = pt.x;
    *y = pt.y;

    return 1;
  }

  return 0;
}

int clientToScreen(long int *const x, long int *const y)
{
  POINT pt = { *x, *y };
  if (ClientToScreen(g_hWnd, &pt)) {
    *x = pt.x;
    *y = pt.y;

    return 1;
  }

  return 0;
}

int setCursorScreenPosition(int x, int y)
{
  if (SetCursorPos(x, y))
    return 1;

  return 0;
}

int getCursorScreenPosition(int *const x, int *const y)
{
  POINT pt;
  if (GetCursorPos(&pt)) {
    *x = pt.x;
    *y = pt.y;

    return 1;
  }

  return 0;
}

int getCursorClientPosition(int *const x, int *const y)
{
  POINT pt;
  if (GetCursorPos(&pt) && ScreenToClient(g_hWnd, &pt)) {
    *x = pt.x;
    *y = pt.y;

    return 1;
  }

  return 0;
}

int moveCursorToClientArea(void)
{
  long int width, height;
  if (getWindowClientAreaRect(&width, &height)) {
    int topleftx = 0, toplefty = 0;
    int bottomrightx = (int)width, bottomrighty = (int)height;

    if (clientToScreen(&topleftx, &toplefty) && clientToScreen(&bottomrightx, &bottomrighty)) {
      setCursorScreenPosition((topleftx + bottomrightx)/2, (toplefty + bottomrighty)/2);

      return 1;
    }
  }

  return 0;
}

void showCursor(const int show)
{
  ShowCursor(show);
}

// NOTE: Must be used in conjunction with endWatchPrecision, periods must match (in milliseconds).
int startWatchPrecision(unsigned int period)
{
  if (timeBeginPeriod(period) == TIMERR_NOERROR)
    return 1;

  return 0;
}

int endWatchPrecision(unsigned int period)
{
  if (timeEndPeriod(period) == TIMERR_NOERROR)
    return 1;

  return 0;
}

unsigned long getSystemTime(void)
{
  return (unsigned long)timeGetTime();
}

int isLeftMouseButtonDown(void)
{
  if (GetSystemMetrics(SM_SWAPBUTTON))
    return GetAsyncKeyState(VK_RBUTTON) & (1 << 15) ? 1 : 0;
  else
    return GetAsyncKeyState(VK_LBUTTON) & (1 << 15) ? 1 : 0;
}

int isRightMouseButtonDown(void)
{
  if (GetSystemMetrics(SM_SWAPBUTTON))
    return GetAsyncKeyState(VK_LBUTTON) & (1 << 15) ? 1 : 0;
  else
    return GetAsyncKeyState(VK_RBUTTON) & (1 << 15) ? 1 : 0;
}

int isMiddleMouseButtonDown(void)
{
  return GetAsyncKeyState(VK_MBUTTON) & (1 << 15) ? 1 : 0;
}

int getKeyboardState(unsigned char keys[256])
{
  if (GetKeyboardState(keys))
    return 1;

  return 0;
}