/**
	\file window_amiga.cpp
	\brief LightEngine 3D: Native OS window manager
	\brief Amiga OS implementation
	\author Andreas Streichardt (andreas@mop.koeln)
	\twitter @m0ppers
	\website https://mop.koeln
	\copyright Frédéric Meslin 2015 - 2018
	\version 1.75

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
#if defined(AMIGA)

#include "window.h"

#include "global.h"
#include "config.h"
#include "system.h"

#include <cybergraphx/cybergraphics.h>
#include <proto/cybergraphics.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/gadtools.h>

#include <string.h>
#include <stdio.h>

#define ESC_KEY 0x45

struct Library *CyberGfxBase = NULL;

static unsigned short Pointer[] = {0,0,0,0,0,0,0,0,0,0,0,0};

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
	CyberGfxBase = OpenLibrary("cybergraphics.library", 41);
	if (!CyberGfxBase) {
	    printf("ERROR: can`t open cybergraphics.library V41.\n");	
	}

	ULONG display_id = BestCModeIDTags(CYBRBIDTG_NominalWidth, width, CYBRBIDTG_NominalHeight, height, CYBRBIDTG_Depth, 32,TAG_DONE);

#if LE_USE_SAGA_FB == 1
	if (display_id == INVALID_ID) {
		printf("Couldn't find display!\n");
		return;
	}

	struct TagItem screen_tags[] = {
		{SA_Left, 0},
		{SA_Top, 0},
		{SA_Width, (ULONG) width},
		{SA_Height, (ULONG) height},
		{SA_Depth, 32},
		{SA_DisplayID, display_id},
		{SA_ShowTitle, 0},
		{SA_Type, SCREENQUIET|CUSTOMSCREEN },
		{TAG_END, 0},
	};

	Screen* screen = OpenScreenTagList(NULL, screen_tags);
	dc.display = (LeHandle) screen;
#endif
	if (name) title = _strdup(name);
	
	struct TagItem win_tags[] = {
#if LE_USE_SAGA_FB == 1
		{WA_CustomScreen, (ULONG) screen},
		{WA_Backdrop, 1},
		{WA_Borderless, 1},
		{WA_Activate, 1},
		{WA_SizeGadget, 0},
		{WA_DepthGadget, 0},
		{WA_CloseGadget, 0},
		{WA_DragBar, 0},
		{WA_IDCMP, IDCMP_CLOSEWINDOW | IDCMP_RAWKEY},
#else
		{WA_Left,			250},
		{WA_Top,			40},
		{WA_InnerWidth,		(ULONG) width},
		{WA_InnerHeight,	(ULONG) height},
		{WA_CloseGadget,	true},
		{WA_AutoAdjust,		true},
		{WA_DragBar,		true},
		{WA_DepthGadget,	true},
		{WA_Title,			(ULONG) title},
		{WA_IDCMP,			IDCMP_CLOSEWINDOW},
#endif
		{TAG_END, 0},
	};

	Window* window = OpenWindowTagList(NULL, win_tags);
#if LE_USE_SAGA_FB == 1
	SetPointer(window, (unsigned short *) Pointer, 2, 16, 0, 0);
#endif

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
#if LE_USE_SAGA_FB == 1
	CloseScreen((Screen*) dc.display);
#endif
	if (CyberGfxBase) {
		CloseLibrary(CyberGfxBase);
	}
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
#if LE_USE_SAGA_FB == 1
		} else if (cls == IDCMP_RAWKEY) {
			if (msg->Code == ESC_KEY) {
				visible = false;
			}
#endif
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

#endif
