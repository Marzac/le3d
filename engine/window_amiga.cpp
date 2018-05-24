/**
	\file window_unix.cpp
	\brief LightEngine 3D: Native OS window manager
	\brief Amiga OS implementation
	\author Andreas Streichardt (andreas@mop.koeln)
	\twitter @m0ppers
	\website https://mop.koeln
	\copyright Frédéric Meslin 2015 - 2018
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

/*****************************************************************************/
#include "window.h"

#include "global.h"
#include "config.h"
#include "system.h"

#include <cybergraphx/cybergraphics.h>
#include <proto/cybergraphics.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>

#include <string.h>
#include <stdio.h>

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
	
	struct TagItem win_tags[] = {
		{WA_Left,			250},
		{WA_Top,			40},
		{WA_InnerWidth,		(ULONG) width},
		{WA_InnerHeight,	(ULONG) height},
		{WA_CloseGadget,	true},
		{WA_AutoAdjust,		true},
		{WA_DragBar,		true},
		{WA_DepthGadget,	true},
		{WA_Title,			(ULONG) name},
		{WA_IDCMP,			IDCMP_CLOSEWINDOW},
		{TAG_END, 0},
	};

	Window* window = OpenWindowTagList(NULL, win_tags);
	dc.display = (LeHandle) window->WScreen;
	dc.window = (LeHandle) window;
	dc.gc = (LeHandle) window->RPort;

	handle = (LeHandle) window;

	visible = true;
}

LeWindow::~LeWindow()
{
	if (title) free(title);
	if (!handle) {
		return;
	}
	CloseWindow((Window*) handle);
}

/*****************************************************************************/
/**
	\fn void LeWindow::update()
	\brief Update window state and process events
*/
void LeWindow::update()
{
	Window* window = (Window*) handle;

	struct IntuiMessage* msg = 0;
	ULONG cls;
	while ((msg = GT_GetIMsg(window->UserPort)) != 0) {
		cls = msg->Class;
		GT_ReplyIMsg(msg);

		if (cls == IDCMP_CLOSEWINDOW) {
			visible = false;
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

