/**
	\file gamepad.h
	\brief LightEngine 3D: Native OS gamepad manager
	\brief All platforms implementation
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

#ifndef LE_GAMEPAD_H
#define LE_GAMEPAD_H

#include "config.h"
#include <stdint.h>

/*****************************************************************************/
#define LE_GAMEPAD_THRESHOLD	4096.0f

/*****************************************************************************/
/**
	\enum LE_GAMEPAD_BUTTONS
	\brief gamepad buttons identifiers (masks)
*/
typedef enum{
	LE_GAMEPAD_DPAD_UP			= 0x0001,
	LE_GAMEPAD_DPAD_DOWN		= 0x0002,
	LE_GAMEPAD_DPAD_LEFT		= 0x0004,
	LE_GAMEPAD_DPAD_RIGHT		= 0x0008,
	LE_GAMEPAD_START			= 0x0010,
	LE_GAMEPAD_BACK				= 0x0020,
	LE_GAMEPAD_LEFT_THUMB		= 0x0040,
	LE_GAMEPAD_RIGHT_THUMB		= 0x0080,
	LE_GAMEPAD_LEFT_SHOULDER	= 0x0100,
	LE_GAMEPAD_RIGHT_SHOULDER	= 0x0200,
	LE_GAMEPAD_A				= 0x1000,
	LE_GAMEPAD_B				= 0x2000,
	LE_GAMEPAD_X				= 0x4000,
	LE_GAMEPAD_Y				= 0x8000,
}LE_GAMEPAD_BUTTONS;

/*****************************************************************************/
/**
	\class LeGamePad
	\brief Handle gamepads through OS native interface
*/
class LeGamePad
{
public:
	LeGamePad(int pad);
	~LeGamePad();

	void init();
	void update();
	void feedback(float left, float right);

	float stickLeftX;		/**< x coordinate of left stick */
	float stickLeftY;		/**< y coordinate of left stick */
	float stickRightX;		/**< x coordinate of right stick */
	float stickRightY;		/**< y coordinate of right stick */

	int buttons;			/**< buttons pressed */
	int toggled;			/**< buttons toggled (since last update) */

	static void setup();
	static void release();

private:
	float normalize(int32_t axis);
	int pad;
};

#endif // LE_GAMEPAD_H
