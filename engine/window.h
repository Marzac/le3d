/**
	\file window.h
	\brief LightEngine 3D: Native OS window manager
	\brief Windows OS implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2018
	\version 1.4

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

#ifndef LE_WINDOW_H
#define LE_WINDOW_H

#include "global.h"
#include "config.h"

/*****************************************************************************/
typedef enum {
	LE_WINDOW_MOUSE_LEFT		= 0x1,
	LE_WINDOW_MOUSE_RIGHT		= 0x2,
	LE_WINDOW_MOUSE_MIDDLE		= 0x4,
	LE_WINDOW_MOUSE_WHEEL_UP	= 0x8,
	LE_WINDOW_MOUSE_WHEEL_DOWN	= 0x8,
}LE_WINDOW_MOUSE_BUTTONS;

typedef enum {
	LE_WINDOW_KEY_UP			= 0x0,
	LE_WINDOW_KEY_DOWN			= 0x1,
	LE_WINDOW_KEY_SHIFT			= 0x2,
	LE_WINDOW_KEY_CTRL			= 0x4,
	LE_WINDOW_KEY_ALT			= 0x8,
}LE_WINDOW_KEYBOARD_STATE;

/*****************************************************************************/
class LeWindow
{
public:
	LeWindow(const char * name, int width, int height);
	~LeWindow();

	void setFullScreen();
	void setWindowed();

	LeHandle getContext();

	typedef void (* KeyCallback) (int key, int state);
	typedef void (* MouseCallback) (int x, int y, int buttons);

	void regKeyCallback(KeyCallback callback)
		{keyCallback = callback;}
	void regMouseCallback(MouseCallback callback)
		{mouseCallback = callback;}

	void sendKeyEvent(int code, int state);
	void sendMouseEvent(int x, int y, int buttons);

private:
	LeHandle hwnd;
	int width;
	int height;
	bool fullScreen;

	KeyCallback keyCallback;
	MouseCallback mouseCallback;
};

#endif // LE_WINDOW_H
