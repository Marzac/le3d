/**
	\file gamepad.h
	\brief LightEngine 3D: Native OS gamepad manager
	\brief Windows OS implementation
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

#ifndef LE_GAMEPAD_H
#define LE_GAMEPAD_H

#include "global.h"
#include "config.h"

/*****************************************************************************/
typedef enum{
	LE_PAD_STICK_LEFT = 0,
	LE_PAD_STICK_RIGHT,
	LE_PAD_TRIGGERS,
	LE_PAD_DPAD,
}LE_PAD_STICKS;

typedef enum{
	LE_PAD_BUTTON_A 		= 0x0001,
	LE_PAD_BUTTON_B 		= 0x0002,
	LE_PAD_BUTTON_X 		= 0x0004,
	LE_PAD_BUTTON_Y			= 0x0008,
	LE_PAD_LEFT_BUMPER		= 0x0010,
	LE_PAD_RIGHT_BUMPER		= 0x0020,
	LE_PAD_BUTTON_BACK		= 0x0040,
	LE_PAD_BUTTON_START		= 0x0080,
	LE_PAD_LEFT_CLICK		= 0x0100,
	LE_PAD_RIGHT_CLICK		= 0x0200,

}LE_PAD_BUTTONS;

/*****************************************************************************/
class LeGamePad
{
public:
	LeGamePad(int pad);

	void getStickPosition(LE_PAD_STICKS stick, float &x, float &y);
	void getButtonsState(int &buttons);
	void setStickThreshold(int threshold);

	static int getCompatiblePad(int minAxes, int minButtons);

private:
	float applyThreshold(float value);

private:
	static int lastCompatible;
	float threshold;
	int pad;
};

#endif // LE_GAMEPAD_H
