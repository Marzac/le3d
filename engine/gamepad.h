/**
	\file gamepad.h
	\brief LightEngine 3D: Native OS gamepad manager
	\brief Windows implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2017
	\version 1.0

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

#ifndef GAMEPAD_H
#define GAMEPAD_H

/*****************************************************************************/
typedef enum{
	PAD_LEFT_STICK = 0,
	PAD_RIGHT_STICK,
	PAD_TRIGGERS,
	PAD_DPAD,
}PAD_STICKS;

/*****************************************************************************/
class GamePad
{
public:
	GamePad(int pad);

	void getStickPosition(PAD_STICKS stick, float &x, float &y);
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

#endif // GAMEPAD_H
