#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

static const char *g_WindowClassIdent = "cWindow";      // Custom window class identifier.

// TODO: Ultimately to be read from config.
static int g_WindowWidth = 1500;
static int g_WindowHeight = 800;
static int g_ScreenWidth = 0;
static int g_ScreenHeight = 0;
HWND g_hWnd;

static FILE *stream;

void Initialize(void);
void Update(void);
void Cleanup(void);

LRESULT CALLBACK wndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nShowCmd)
{
  WNDCLASSEX wc;
  MSG msg;

#if defined(_DEBUG) && defined(_MSC_VER)
  {
    AllocConsole();
    AttachConsole(GetCurrentProcessId());
    freopen_s(&stream, "CON", "w", stdout);
  }
#endif

  g_ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
  g_ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

  wc.cbSize = sizeof(WNDCLASSEX);
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
  wc.hCursor = LoadCursor(0, IDC_ARROW);
  wc.hIcon = 0;
  wc.hIconSm = 0;
  wc.hInstance = hInstance;
  wc.lpfnWndProc = wndProc;
  wc.lpszClassName = g_WindowClassIdent;
  wc.lpszMenuName = 0;
  wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

  // Register custom window class, then use with CreateWindowEx.
  if (!RegisterClassEx(&wc)) {
    MessageBox(NULL, "Could not register window.", "Error", MB_ICONEXCLAMATION | MB_OK);
    return -1;
  }

  // Create window and store its handle.
  g_hWnd = CreateWindowEx(WS_EX_WINDOWEDGE, g_WindowClassIdent, "C Project",
    WS_OVERLAPPEDWINDOW, (g_ScreenWidth - g_WindowWidth) / 2, (g_ScreenHeight - g_WindowHeight) / 2, g_WindowWidth, g_WindowHeight,
    NULL, NULL, hInstance, NULL);

  if (g_hWnd == NULL) {
    MessageBox(NULL, "Could not create window.", "Error", MB_ICONEXCLAMATION | MB_OK);
    return -1;
  }

  ShowWindow(g_hWnd, nShowCmd);
  UpdateWindow(g_hWnd);

  // Entry point to user code.
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