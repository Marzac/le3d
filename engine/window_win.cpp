/**
	\file window_win.cpp
	\brief LightEngine 3D: Native OS window manager
	\brief Windows OS implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2018
	\version 1.6

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

/*****************************************************************************/
LeWindow::LeWindow(const char * name, int width, int height) :
	visible(false),
	handle(0),
	width(width),
	height(height),
	fullScreen(false),
	keyCallback(NULL),
	mouseCallback(NULL)
{
	static const char * className = "LightEngine";
	memset(&dc, 0, sizeof(LeDrawingContext));

// Create the window class
	WNDCLASSEXA wincl;
	memset(&wincl, 0, sizeof(WNDCLASSEXA));
	wincl.hInstance = NULL;
	wincl.lpszClassName = className;
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

// Compute the client size
	RECT size;
	size.top	= 0;
	size.left	= 0;
	size.right	= width;
	size.bottom = height;
	AdjustWindowRect(&size, WS_OVERLAPPEDWINDOW, 0);

// Create and display window
	if ((handle = (LeHandle) CreateWindowExA(
		   0,
		   wincl.lpszClassName,
		   name,
		   WS_OVERLAPPEDWINDOW,
		   CW_USEDEFAULT,
		   CW_USEDEFAULT,
		   size.right - size.left,
		   size.bottom - size.top,
		   HWND_DESKTOP,
		   NULL,
		   NULL,
		   NULL
	)) == 0) return;

	SetWindowLongPtrA((HWND) handle, 0, (long long) this);

	dc.display = 0;
	dc.window = handle;
	dc.gc = (LeHandle) GetDC((HWND) handle);
	
	ShowWindow((HWND) handle, SW_SHOW);
	visible = true;
}

LeWindow::~LeWindow()
{
	if (handle) DestroyWindow((HWND) handle);
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
	\fn LeHandle LeWindow::getWindowHandle()
	\brief Retrieve the native OS window handle
	\return handle to an OS window handle
*/
LeHandle LeWindow::getHandle()
{
	return handle;
}

/**
	\fn LeDrawingContext LeWindow::getWindowContext()
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
		{
			int uniState = 0;
			int uniCode = wParam;

			if (lParam & 0x80000000)
				uniState = LE_WINDOW_KEY_UP;
			else uniState = LE_WINDOW_KEY_DOWN;

			if (GetKeyState(VK_SHIFT) & 0x8000) uniState |= LE_WINDOW_KEY_SHIFT;
			if (GetKeyState(VK_CONTROL) & 0x8000) uniState |= LE_WINDOW_KEY_CTRL;
			if (GetKeyState(VK_MENU) & 0x8000) uniState |= LE_WINDOW_KEY_ALT;

			window->sendKeyEvent(uniCode, uniState);
		}
		break;

	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
		{
			int uniButtons = 0;
			int uniX = lParam & 0xFFFF;
			int uniY = lParam >> 16;

			if (wParam & MK_LBUTTON) uniButtons |= LE_WINDOW_MOUSE_LEFT;
			if (wParam & MK_RBUTTON) uniButtons |= LE_WINDOW_MOUSE_RIGHT;
			if (wParam & MK_MBUTTON) uniButtons |= LE_WINDOW_MOUSE_MIDDLE;

			window->sendMouseEvent(uniX, uniY, uniButtons);
		}
		break;
	}

	return DefWindowProcA(hwnd, msg, wParam, lParam);
}

/*****************************************************************************/
/**
	\fn void LeWindow::setFullScreen()
	\brief Set the window to fullscreen mode
*/
static DEVMODE devMode;
void LeWindow::setFullScreen()
{
	DEVMODE newMode;
	if (fullScreen) return;

// Retrieve display characteristics
	EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &devMode);
	memcpy(&newMode, &devMode, sizeof(DEVMODE));
	newMode.dmPelsWidth = width;
	newMode.dmPelsHeight = height;
	newMode.dmFields = DM_PELSHEIGHT | DM_PELSWIDTH;

// Change window state
	SetWindowPos((HWND) handle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE);
	ShowWindow((HWND) handle, SW_MAXIMIZE);

// Change display mode
	ChangeDisplaySettings (&newMode, CDS_FULLSCREEN);
	fullScreen = true;
}

/**
	\fn void LeWindow::setWindowed()
	\brief Set the window to windowed mode
*/
void LeWindow::setWindowed()
{
	if (!fullScreen) return;

// Revert window state
	SetWindowPos((HWND) handle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	ShowWindow((HWND) handle, SW_NORMAL);

// Revert display mode
	ChangeDisplaySettings (&devMode, CDS_RESET);
	fullScreen = false;
}

#endif
