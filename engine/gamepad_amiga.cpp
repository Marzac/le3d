/**
	\file gamepad_amiga.cpp
	\brief LightEngine 3D: Native OS gamepad manager
	\brief Amiga implementation
	\author todo
	\twitter todo 
	\website todo
	\copyright todo
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

#include "gamepad.h"

#include "global.h"
#include "config.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

/*****************************************************************************/
LeGamePad::LeGamePad(int pad) :
	stickLeftX(0.0f), stickLeftY(0.0f),
	stickRightX(0.0f), stickRightY(0.0f),
	buttons(0), toggled(0), pressed(0), released(0),
	detected(false),
	pad(pad)
{
}

LeGamePad::~LeGamePad()
{
}

/*****************************************************************************/
/**
	\fn void LeGamePad::init()
	\brief Initialize gamepad state (default state)
*/
void LeGamePad::init()
{
	stickLeftX = 0.0f;
	stickLeftY = 0.0f;
	stickRightX = 0.0f;
	stickRightY = 0.0f;

	buttons = 0;
	toggled = 0;
	pressed = 0;
	released = 0;

	feedback(0.0f, 0.0f);
}

/*****************************************************************************/
/**
	\fn void LeGamePad::update()
	\brief Update gamepad state (call the driver)
*/
void LeGamePad::update()
{
	int lastButtons = buttons;
	
	buttons = 0;
	toggled = lastButtons ^ buttons;
	pressed = toggled & buttons;
	released = toggled & ~buttons;
}

/*****************************************************************************/
/**
	\fn void LeGamePad::feedback(float left, float right)
	\brief Send a force feedback order
	\param[in] left motor order (0.0 - 1.0)
	\param[in] right motor order (0.0 - 1.0)
*/
void LeGamePad::feedback(float left, float right)
{
}

/*****************************************************************************/
inline float LeGamePad::normalize(int32_t axis)
{
	float value = (float) axis;
	float sign = copysignf(1.0f, value);
	if (value * sign < LE_GAMEPAD_THRESHOLD) return 0.0f;
	const float scale = 1.0f / (32768.0f - LE_GAMEPAD_THRESHOLD);
	return (value - LE_GAMEPAD_THRESHOLD * sign) * scale;
}

/*****************************************************************************/
/**
	\fn void LeGamePad::setup()
	\brief Enumerate and initialize available gamepads
*/
void LeGamePad::setup()
{

}
/**
	\fn void LeGamePad::release()
	\brief Release initialized gamepads
*/
void LeGamePad::release()
{

}

#endif
