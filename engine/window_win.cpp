/**
	\file window_win.cpp
	\brief LightEngine 3D: Native OS window manager
	\brief Windows OS implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2018
	\version 1.7

	The MIT License (MIT)
	Copyright (c) 2015-2018 Frédéric Meslin

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#if defined(_WIN32)

/*****************************************************************************/
#include "window.h"

#include "global.h"
#include "config.h"

#include <windows.h>
#include <wingdi.h>

#include <stdio.h>
#include <string.h>

/*****************************************************************************/
LRESULT CALLBACK windowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
static void decodeMouseButton(LeWindow * window, uint32_t wParam, uint32_t lParam);
static void decodeKey(LeWindow * window, uint32_t wParam, uint32_t lParam);
static void createWindow(LeWindow * window, LeHandle & handle, const char * title, bool fullscreen);
static void destroyWindow(LeWindow * window, LeHandle & handle);

static const char * windowClassName = "LightEngine";

/*****************************************************************************/
LeWindow::LeWindow(const char * name, int width, int height, bool fullscreen) :
	width(width),
	height(height),
	fullScreen(false),
	visible(false),
	title(NULL),
	handle(0),
	keyCallback(NULL),
	mouseCallback(NULL)
{
	memset(&dc, 0, sizeof(LeDrawingContext));
	if (name) title = _strdup(name);

// Create the window class
	WNDCLASSEXA wincl;
	memset(&wincl, 0, sizeof(WNDCLASSEXA));
	wincl.hInstance = NULL;
	wincl.lpszClassName = windowClassName;
	wincl.lpfnWndProc = windowProcedure;
	wincl.style = CS_DBLCLKS;
	wincl.cbSize = sizeof(WNDCLASSEXA);
	wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
	wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
	wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
	wincl.lpszMenuName = NULL;
	wincl.cbClsExtra = 0;
	wincl.cbWndExtra = 32;
	wincl.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
	if (!RegisterClassExA(&wincl)) return;

// Open the window
	if (fullscreen) setFullScreen();
	else setWindowed();
}

LeWindow::~LeWindow()
{
	if (title) free(title);
	destroyWindow(this, handle);
}

/*****************************************************************************/
void createWindow(LeWindow * window, LeHandle & handle, const char * title, bool fullscreen)
{
// Compute the client size
	RECT size;
	size.top = 0;
	size.left = 0;
	size.right = window->width;
	size.bottom = window->height;

	DWORD style = WS_CAPTION | WS_SYSMENU | WS_BORDER | WS_MINIMIZEBOX;
	if (fullscreen) style = WS_POPUP | WS_VISIBLE | WS_SYSMENU;
	AdjustWindowRect(&size, style, 0);

// Create and display window
	if ((handle = (LeHandle) CreateWindowExA(
		0,
		windowClassName,
		title, style,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		size.right - size.left,
		size.bottom - size.top,
		HWND_DESKTOP,
		NULL, NULL,	NULL
	)) == 0) return;
	SetWindowLongPtrA((HWND) handle, 0, (long long) window);
}

void destroyWindow(LeWindow * window, LeHandle & handle)
{
	if (!handle) return;
	DestroyWindow((HWND) handle);
	handle = 0;
	window->fullScreen = false;
	window->visible = false;
}

/*****************************************************************************/
/**
	\fn void LeWindow::update()
	\brief Update window state and process events
*/
void LeWindow::update()
{
	// On MS Windows OS, this is done via callbacks
}

/*****************************************************************************/
/**
	\fn LeHandle LeWindow::getHandle()
	\brief Retrieve the native OS window handle
	\return handle to an OS window handle
*/
LeHandle LeWindow::getHandle()
{
	return handle;
}

/**
	\fn LeDrawingContext LeWindow::getContext()
	\brief Retrieve the native OS window graphic context
	\return handle to an OS window handle
*/
LeDrawingContext LeWindow::getContext()
{
	return dc;
}

/*****************************************************************************/
/**
	\fn void LeWindow::registerKeyCallback(KeyCallback callback)
	\brief Register a callback to receive keyboard events associated to the window
	\param[in] callback pointer to a callback function or NULL
*/
void LeWindow::registerKeyCallback(KeyCallback callback)
{
	keyCallback = callback;
}

/**
	\fn void LeWindow::registerMouseCallback(MouseCallback callback)
	\brief Register a callback to receive mouse events associated to the window
	\param[in] callback pointer to a callback function or NULL
*/
void LeWindow::registerMouseCallback(MouseCallback callback)
{
	mouseCallback = callback;
}

/*****************************************************************************/
/**
	\fn void LeWindow::sendKeyEvent(int code, int state)
	\brief Send a keyboard event to the window
	\param[in] code keyboard event code
	\param[in] state keyboard event state (mask)
*/
void LeWindow::sendKeyEvent(int code, int state)
{
	if (!keyCallback) return;
	keyCallback(code, state);
}

/**
	\fn void LeWindow::sendMouseEvent(int x, int y, int buttons)
	\brief Send a mouse event to the window
	\param[in] x horizontal position of the mouse (in client area and in pixels)
	\param[in] y vertical position of the mouse (in client area and in pixels)
	\param[in] buttons buttons state (mask)
*/
void LeWindow::sendMouseEvent(int x, int y, int buttons)
{
	if (!mouseCallback) return;
	mouseCallback(x, y, buttons);
}

/*****************************************************************************/
LRESULT CALLBACK windowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LeWindow * window = (LeWindow *) GetWindowLongPtr(hwnd, 0);

	switch (msg) {
	case WM_DESTROY:
		window->visible = false;
		break;
		
	case WM_KEYUP:
	case WM_SYSKEYUP:
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		decodeKey(window, wParam, lParam);
		break;

	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
		decodeMouseButton(window, wParam, lParam);
		break;
	}

	return DefWindowProcA(hwnd, msg, wParam, lParam);
}

/*****************************************************************************/
void decodeMouseButton(LeWindow * window, uint32_t wParam, uint32_t lParam)
{
	int uniButtons = 0;
	int uniX = lParam & 0xFFFF;
	int uniY = lParam >> 16;

	if (wParam & MK_LBUTTON) uniButtons |= LE_WINDOW_MOUSE_LEFT;
	if (wParam & MK_RBUTTON) uniButtons |= LE_WINDOW_MOUSE_RIGHT;
	if (wParam & MK_MBUTTON) uniButtons |= LE_WINDOW_MOUSE_MIDDLE;

	window->sendMouseEvent(uniX, uniY, uniButtons);
}

void decodeKey(LeWindow * window, uint32_t wParam, uint32_t lParam)
{
	int uniState = 0;
	int uniCode = wParam;

	switch (uniCode) {
		case VK_UP:	uniCode = LE_WINDOW_KEYCODE_UP; break;
		case VK_DOWN: uniCode = LE_WINDOW_KEYCODE_DOWN; break;
		case VK_LEFT: uniCode = LE_WINDOW_KEYCODE_LEFT; break;
		case VK_RIGHT: uniCode = LE_WINDOW_KEYCODE_RIGHT; break;
		case VK_SHIFT: uniCode = LE_WINDOW_KEYCODE_SHIFT; break;
		case VK_CONTROL: uniCode = LE_WINDOW_KEYCODE_CTRL; break;
		case VK_MENU: uniCode = LE_WINDOW_KEYCODE_ALT; break;
		case VK_TAB: uniCode = LE_WINDOW_KEYCODE_TAB; break;
		case VK_ESCAPE: uniCode = LE_WINDOW_KEYCODE_ESC; break;
		case VK_BACK: uniCode = LE_WINDOW_KEYCODE_BACKSPACE; break;
		case VK_RETURN: uniCode = LE_WINDOW_KEYCODE_ENTER; break;
	#if LE_WINDOW_EXTENDED_KEYS == 1
		case VK_INSERT: uniCode = LE_WINDOW_KEYCODE_INSERT; break;
		case VK_DELETE: uniCode = LE_WINDOW_KEYCODE_DELETE; break;
		case VK_HOME: uniCode = LE_WINDOW_KEYCODE_HOME; break;
		case VK_END: uniCode = LE_WINDOW_KEYCODE_END; break;
		case VK_PRIOR: uniCode = LE_WINDOW_KEYCODE_PAGEUP; break;
		case VK_NEXT: uniCode = LE_WINDOW_KEYCODE_PAGEDOWN; break;
	#endif
		default: break;
	}

	if (lParam & 0x80000000)
		uniState = LE_WINDOW_KEYSTATE_RELEASED;
	else 
		uniState = LE_WINDOW_KEYSTATE_PRESSED;

	if (GetKeyState(VK_SHIFT) & 0x8000)
		uniState |= LE_WINDOW_KEYSTATE_SHIFT;
	if (GetKeyState(VK_CONTROL) & 0x8000)
		uniState |= LE_WINDOW_KEYSTATE_CTRL;
	if (GetKeyState(VK_MENU) & 0x8000)
		uniState |= LE_WINDOW_KEYSTATE_ALT;

	window->sendKeyEvent(uniCode, uniState);
}

/*****************************************************************************/
/**
	\fn void LeWindow::setFullScreen()
	\brief Set the window to fullscreen mode
*/
static DEVMODE previousDevMode;
static bool previousDevModeValid = false;

void LeWindow::setFullScreen()
{
// Change display mode
	if (!previousDevModeValid) {
		EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &previousDevMode);
		previousDevModeValid = true;

		DEVMODE newDevMode;
		memcpy(&newDevMode, &previousDevMode, sizeof(DEVMODE));
		newDevMode.dmPelsWidth = width;
		newDevMode.dmPelsHeight = height;
		newDevMode.dmFields = DM_PELSHEIGHT | DM_PELSWIDTH;
		ChangeDisplaySettings(&newDevMode, CDS_FULLSCREEN);
	}

// Create a new window
	destroyWindow(this, handle);
	createWindow(this, handle, title, true);
	dc.display = 0;
	dc.window = handle;
	dc.gc = (LeHandle) GetDC((HWND) handle);

// Display the window
	SetWindowPos((HWND) handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	ShowWindow((HWND) handle, SW_MAXIMIZE);
	fullScreen = true;
	visible = true;
}

/**
	\fn void LeWindow::setWindowed()
	\brief Set the window to windowed mode
*/
void LeWindow::setWindowed()
{
// Revert display mode
	if (previousDevModeValid) {
		ChangeDisplaySettings(&previousDevMode, CDS_RESET);
		previousDevModeValid = false;
	}

// Create a new window
	destroyWindow(this, handle);
	createWindow(this, handle, title, false);
	dc.display = 0;
	dc.window = handle;
	dc.gc = (LeHandle) GetDC((HWND) handle);

// Display the window
	SetWindowPos((HWND) handle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE);
	ShowWindow((HWND) handle, SW_NORMAL);
	fullScreen = false;
	visible = true;
}

#endif
