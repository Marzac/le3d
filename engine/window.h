/**
	\file window.h
	\brief LightEngine 3D: Native OS window manager
	\brief All platforms implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2018
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

#ifndef LE_WINDOW_H
#define LE_WINDOW_H

#include "global.h"
#include "config.h"

/*****************************************************************************/
/**
	\enum LE_WINDOW_MOUSE_BUTTONS
	\brief mouse buttons identifiers
*/
typedef enum {
	LE_WINDOW_MOUSE_LEFT		= 0x1,
	LE_WINDOW_MOUSE_RIGHT		= 0x2,
	LE_WINDOW_MOUSE_MIDDLE		= 0x4,
	LE_WINDOW_MOUSE_WHEEL_UP	= 0x8,
	LE_WINDOW_MOUSE_WHEEL_DOWN	= 0x8,
}LE_WINDOW_MOUSE_BUTTONS;

/**
	\enum LE_WINDOW_KEY_STATE
	\brief keyboard keys state and modifiers
*/
typedef enum {
	LE_WINDOW_KEYSTATE_PRESSED		= 0x01,
	LE_WINDOW_KEYSTATE_RELEASED		= 0x02,
	LE_WINDOW_KEYSTATE_SHIFT		= 0x10,
	LE_WINDOW_KEYSTATE_CTRL			= 0x20,
	LE_WINDOW_KEYSTATE_ALT			= 0x40,
}LE_WINDOW_KEY_STATES;

typedef enum {
	LE_WINDOW_KEYCODE_NONE			= 0,
	LE_WINDOW_KEYCODE_UP			= 1,
	LE_WINDOW_KEYCODE_DOWN			= 2,
	LE_WINDOW_KEYCODE_LEFT			= 3,
	LE_WINDOW_KEYCODE_RIGHT			= 4,
	LE_WINDOW_KEYCODE_SHIFT			= 5,
	LE_WINDOW_KEYCODE_CTRL			= 6,
	LE_WINDOW_KEYCODE_ALT			= 7,
	LE_WINDOW_KEYCODE_TAB			= 8,
	LE_WINDOW_KEYCODE_ESC			= 9,
	LE_WINDOW_KEYCODE_BACKSPACE		= 10,
	LE_WINDOW_KEYCODE_ENTER			= 11,

#if LE_WINDOW_EXTENDED_KEYS == 1
	LE_WINDOW_KEYCODE_INSERT		= 12,
	LE_WINDOW_KEYCODE_DELETE		= 13,
	LE_WINDOW_KEYCODE_HOME			= 14,
	LE_WINDOW_KEYCODE_END			= 15,
	LE_WINDOW_KEYCODE_PAGEUP		= 16,
	LE_WINDOW_KEYCODE_PAGEDOWN		= 17,
#endif

}LE_WINDOW_KEY_CODES;

/*****************************************************************************/
/**
	\struct LeDrawingContext
	\brief Represent an OS specific drawing context
*/
typedef struct {
	LeHandle display;		/**< Handle to a native display resource */
	LeHandle window;		/**< Handle to a native window resource */
	LeHandle gc;			/**< Handle to a native graphic context resource */
}LeDrawingContext;

/*****************************************************************************/
/**
	\class LeWindow
	\brief Create and handle an OS native window
*/
class LeWindow
{
public:
	LeWindow(const char * name, int width = LE_RESOX_DEFAULT, int height = LE_RESOY_DEFAULT, bool fullscreen = false);
	~LeWindow();

	void update();
	
	void setFullScreen();
	void setWindowed();

	LeHandle getHandle();
	LeDrawingContext getContext();
	
	typedef void (* KeyCallback) (int key, int state);
	typedef void (* MouseCallback) (int x, int y, int buttons);

	void registerKeyCallback(KeyCallback callback);
	void registerMouseCallback(MouseCallback callback);

	void sendKeyEvent(int code, int state);
	void sendMouseEvent(int x, int y, int buttons);

	int width;						/**< Width of window client region (in pixels) */
	int height;						/**< Height of window client region (in pixels) */
	bool fullScreen;				/**< Fullscreen state of window */
	bool visible;					/**< Is the window open or closed */

private:
	char * title;					/**< Window title in title bar */
	LeHandle handle;				/**< OS native window handle */
	LeDrawingContext dc;			/**< OS native drawing context handle */
	
	KeyCallback keyCallback;		/**< Registrated callback for keyboard events */
	MouseCallback mouseCallback;	/**< Registrated callback for mouse events */
};

#endif // LE_WINDOW_H
