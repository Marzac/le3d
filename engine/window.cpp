/**
	\file window.cpp
	\brief LightEngine 3D: Native OS window manager
	\brief Windows implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2017
	\version 1.3

	The MIT License (MIT)
	Copyright (c) 2017 Frédéric Meslin

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

#include "window.h"

#include "global.h"
#include "config.h"

#include <windows.h>
#include <wingdi.h>

LRESULT CALLBACK windowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

/*****************************************************************************/
const char * className = "LightEngine";
LeWindow::LeWindow(const char * name, int width, int height) :
	width(width),
	height(height),
	fullScreen(false),
	keyCallback(NULL),
	mouseCallback(NULL)
{
// Create the window class
	WNDCLASSEX wincl;
	memset(&wincl, 0, sizeof(WNDCLASSEX));
	wincl.hInstance = NULL;
	wincl.lpszClassName = className;
	wincl.lpfnWndProc = windowProcedure;
	wincl.style = CS_DBLCLKS;
	wincl.cbSize = sizeof(WNDCLASSEX);
	wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
	wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
	wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
	wincl.lpszMenuName = NULL;
	wincl.cbClsExtra = 0;
	wincl.cbWndExtra = 32;
	wincl.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
	if (!RegisterClassEx (&wincl)) return;

// Compute the client size
	RECT size;
	size.top    = 0;
	size.left   = 0;
	size.right  = width;
	size.bottom = height;
	AdjustWindowRect(&size, WS_OVERLAPPEDWINDOW, 0);

// Create and display window
	if ((hwnd = (LeHandle) CreateWindowEx(
		   0,
		   wincl.lpszClassName,
		   name,
		   WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		   CW_USEDEFAULT,
		   CW_USEDEFAULT,
		   size.right - size.left,
		   size.bottom - size.top,
		   HWND_DESKTOP,
		   NULL,
		   NULL,
		   NULL
	)) == 0) return;

	SetWindowLongPtr((HWND) hwnd, 0, (long long) this);
}

LeWindow::~LeWindow()
{
	if (hwnd) DestroyWindow((HWND) hwnd);
}

/*****************************************************************************/
LeHandle LeWindow::getContext()
{
	return (LeHandle) GetDC((HWND) hwnd);
}

/*****************************************************************************/
void LeWindow::sendKeyEvent(int code, int state)
{
	if (!keyCallback) return;
	int tState = 0;
	if (state & 0x80000000)
		tState = LE_WINDOW_KEY_UP;
	else tState = LE_WINDOW_KEY_DOWN;
	if (GetKeyState(VK_SHIFT) & 0x8000)   tState |= LE_WINDOW_KEY_SHIFT;
	if (GetKeyState(VK_CONTROL) & 0x8000) tState |= LE_WINDOW_KEY_CTRL;
	if (GetKeyState(VK_MENU) & 0x8000)    tState |= LE_WINDOW_KEY_ALT;
	keyCallback(code, tState);
}

void LeWindow::sendMouseEvent(int x, int y, int buttons)
{
	if (!mouseCallback) return;
	int tButtons = 0;
	if (buttons & MK_LBUTTON) tButtons |= LE_WINDOW_MOUSE_LEFT;
	if (buttons & MK_RBUTTON) tButtons |= LE_WINDOW_MOUSE_RIGHT;
	if (buttons & MK_MBUTTON) tButtons |= LE_WINDOW_MOUSE_MIDDLE;
	mouseCallback(x, y, tButtons);
}

/*****************************************************************************/
LRESULT CALLBACK windowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
// Process window messages
	LeWindow * window = (LeWindow *) GetWindowLongPtr(hwnd, 0);
	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_CLOSE:
		break;

	case WM_KEYUP:
	case WM_SYSKEYUP:
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		window->sendKeyEvent(wParam, lParam);
		break;

	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
		window->sendMouseEvent(lParam & 0xFFFF, lParam >> 16, wParam);
		break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

/*****************************************************************************/
DEVMODE devMode;
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

// Change windows position
	SetWindowPos((HWND) hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE);
	ShowWindow((HWND) hwnd, SW_MAXIMIZE);

// Switch to full screen
	ChangeDisplaySettings (&newMode, CDS_FULLSCREEN);
	fullScreen = true;
}

void LeWindow::setWindowed()
{
	if (!fullScreen) return;

	SetWindowPos((HWND) hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	ShowWindow((HWND) hwnd, SW_NORMAL);
	ChangeDisplaySettings (&devMode, CDS_RESET);
	fullScreen = false;
}

