/**
	\file gamepad.cpp
	\brief LightEngine 3D: Native OS gamepad manager
	\brief Windows OS implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2017
	\version 1.2

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

#include "gamepad.h"

#include "global.h"
#include "config.h"

#include <math.h>
#include <windows.h>

int LeGamePad::lastCompatible = -1;

/*****************************************************************************/
LeGamePad::LeGamePad(int pad) :
	threshold(8192.0f), pad(pad)
{
}

/*****************************************************************************/
void LeGamePad::getStickPosition(LE_PAD_STICKS stick, float &x, float &y)
{
	JOYINFOEX info;

// Get gamepad state
	memset(&info, 0, sizeof(JOYINFOEX));
	info.dwSize = sizeof(JOYINFOEX);
	info.dwFlags = JOY_RETURNALL;
	joyGetPosEx(pad, &info);

// Retrieve coordinates
	switch(stick) {
	case LE_PAD_STICK_LEFT:
		x = applyThreshold(((int32_t) info.dwXpos) - 32768);
		y = applyThreshold(((int32_t) info.dwYpos) - 32768);
		return;
	case LE_PAD_STICK_RIGHT:
		x = applyThreshold(((int32_t) info.dwUpos) - 32768);
		y = applyThreshold(((int32_t) info.dwRpos) - 32768);
		return;
	case LE_PAD_TRIGGERS:
		{
		float t = applyThreshold(((int32_t) info.dwZpos) - 32768);
		x = t > 0.0f ? t : 0.0f;
		y = t < 0.0f ? -t : 0.0f;
		return;
		}
	case LE_PAD_DPAD:
		{
		uint16_t angle = info.dwPOV;
		if (angle == 0xffff) {
			x = 0.0f; y = 0.0f;
		}else{
			float a = (float) angle * (M_PI / 180.0f * 0.01f);
			x = sinf(a); y = cosf(a);
		}
		return;
		}
	}
}

void LeGamePad::getButtonsState(int &buttons)
{
	JOYINFOEX info;
	memset(&info, 0, sizeof(JOYINFOEX));
	info.dwSize = sizeof(JOYINFOEX);
	info.dwFlags = JOY_RETURNBUTTONS;
	joyGetPosEx(pad, &info);
	buttons = info.dwButtons;
}

/*****************************************************************************/
inline float LeGamePad::applyThreshold(float value)
{
	const float scale = 1.0f / (32768.0f - threshold);
	float sign = copysignf(1.0f, value);
	if (value * sign < threshold) return 0.0f;
	return (value - sign * threshold) * scale;
}

void LeGamePad::setStickThreshold(int threshold)
{
	this->threshold = threshold;
}

/*****************************************************************************/
int LeGamePad::getCompatiblePad(int minAxes, int minButtons)
{
	int nb = joyGetNumDevs();
	int pi = lastCompatible + 1;
	for (int i = 0; i < nb; i++) {
		if (pi >= nb) pi = 0;
		JOYCAPS caps;
		joyGetDevCaps(pi, &caps, sizeof(JOYCAPS));
		if ((int)caps.wNumAxes >= minAxes && (int) caps.wNumButtons >= minButtons) {
			lastCompatible = pi;
			return pi;
		}
	}
	lastCompatible = -1;
	return -1;
}
