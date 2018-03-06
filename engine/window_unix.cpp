/**
	\file window_unix.cpp
	\brief LightEngine 3D: Native OS window manager
	\brief Unix OS implementation (with X.Org / XLib)
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2018
	\version 1.5

	The MIT License (MIT)
	Copyright (c) 2015-2018 Fr�d�ric Meslin

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

/*****************************************************************************/
#include "window.h"

#include "global.h"
#include "config.h"

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <stdio.h>
#include <string.h>

/*****************************************************************************/
static Display * usedXDisplay = NULL;
static int usedXDisplayCount = 0;
static const int xEventMask = KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | ButtonMotionMask | PointerMotionMask;

/*****************************************************************************/
LeWindow::LeWindow(const char * name, int width, int height) :
	handle(0),
	width(width),
	height(height),
	fullScreen(false),
	keyCallback(NULL),
	mouseCallback(NULL)
{
	memset(&dc, 0, sizeof(LeDrawingContext));

	if (!usedXDisplay) {
		usedXDisplay = XOpenDisplay(NULL);
		if (!usedXDisplay) {
			printf("window: unable to connect to X Server\n");
			return;
		}
	}
	usedXDisplayCount ++;

	handle = (LeHandle) XCreateSimpleWindow(usedXDisplay, RootWindow(usedXDisplay, 0), 0, 0, width, height, 1, 0, 0);
	XStoreName(usedXDisplay, (Window) handle, name);


	XSelectInput(usedXDisplay, (Window) handle, ExposureMask | xEventMask);
	XMapWindow(usedXDisplay, (Window) handle);

	dc.display = (LeHandle) usedXDisplay;
	dc.window = handle;
	dc.gc = (LeHandle) DefaultGC(usedXDisplay, 0);
}

LeWindow::~LeWindow()
{
	if (handle) XDestroyWindow(usedXDisplay, (Window) handle);

	usedXDisplayCount --;
	if (usedXDisplayCount) return;
	if (usedXDisplay) XCloseDisplay(usedXDisplay);
}

/*****************************************************************************/
/**
	\fn void LeWindow::update()
	\brief Update window state and process events
*/
void LeWindow::update()
{
	if (!handle) return;

	XEvent event;
	while (XCheckWindowEvent((Display *) dc.display, (Window) dc.window, xEventMask, &event) > 0) {
		if (event.type == KeyPress ||
			event.type == KeyRelease) {

			int uniState = 0;
			if (event.type == KeyPress)
				uniState = LE_WINDOW_KEY_DOWN;
			else uniState = LE_WINDOW_KEY_UP;

			if (event.xkey.state & ShiftMask) uniState |= LE_WINDOW_KEY_SHIFT;
			if (event.xkey.state & ControlMask) uniState |= LE_WINDOW_KEY_CTRL;

			sendKeyEvent(event.xkey.keycode, uniState);
		}else
		if (event.type == ButtonPress ||
		    event.type == ButtonRelease) {

			int uniState = 0;
			if (event.xbutton.state & Button1Mask)
				uniState |= LE_WINDOW_MOUSE_LEFT;
			if (event.xbutton.state & Button2Mask)
				uniState |= LE_WINDOW_MOUSE_MIDDLE;
			if (event.xbutton.state & Button3Mask)
				uniState |= LE_WINDOW_MOUSE_RIGHT;
			if (event.xbutton.state & Button4Mask)
				uniState |= LE_WINDOW_MOUSE_WHEEL_UP;
            if (event.xbutton.state & Button5Mask)
				uniState |= LE_WINDOW_MOUSE_WHEEL_DOWN;

			sendMouseEvent(event.xbutton.x, event.xbutton.y, uniState);
		}else
		if (event.type == MotionNotify) {

			int uniState = 0;
			if (event.xmotion.state & Button1Mask)
				uniState |= LE_WINDOW_MOUSE_LEFT;
			if (event.xmotion.state & Button2Mask)
				uniState |= LE_WINDOW_MOUSE_MIDDLE;
			if (event.xmotion.state & Button3Mask)
				uniState |= LE_WINDOW_MOUSE_RIGHT;
			if (event.xmotion.state & Button4Mask)
				uniState |= LE_WINDOW_MOUSE_WHEEL_UP;
            if (event.xmotion.state & Button5Mask)
				uniState |= LE_WINDOW_MOUSE_WHEEL_DOWN;

			sendMouseEvent(event.xmotion.x, event.xmotion.y, uniState);
		}
	}
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
/**
	\fn void LeWindow::setFullScreen()
	\brief Set the window to fullscreen mode
*/
void LeWindow::setFullScreen()
{
	printf("Window: fullscreen mode is only supported for Windows OS\n!");
}

/**
	\fn void LeWindow::setWindowed()
	\brief Set the window to windowed mode
*/
void LeWindow::setWindowed()
{
	if (!fullScreen) return;
}

