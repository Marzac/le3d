/**
	\file gamepad.cpp
	\brief LightEngine 3D: Native OS gamepad manager
	\brief Windows OS implementation (XInput V1.3)
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

#include "gamepad.h"

#include "global.h"
#include "config.h"

#include <stdio.h>
#include <math.h>
#include <windows.h>

/*****************************************************************************/
/** XInput driver structures and constants */
typedef struct {
    uint16_t buttons;
    uint8_t leftTrigger;
    uint8_t rightTrigger;
    int16_t thumbLX;
    int16_t thumbLY;
    int16_t thumbRX;
    int16_t thumbRY;
}XInputGamepad;

typedef struct {
    uint32_t packetNumber;
    XInputGamepad gamepad;
}XInputState;

typedef struct {
    uint16_t leftMotorSpeed;
    uint16_t rightMotorSpeed;
}XInputVibration;

typedef struct {
    uint8_t type;
    uint8_t subType;
    uint16_t flags;
    XInputGamepad gamepad;
    XInputVibration vibration;
}XInputCaps;

/** XInput driver function types */
typedef __stdcall void DLLXInputEnable(bool enable);
typedef __stdcall uint32_t DLLXInputGetCapabilities(uint32_t userIndex, uint32_t flags, XInputCaps * capabilities);
typedef __stdcall uint32_t DLLXInputGetState(uint32_t userIndex, XInputState * state);
typedef __stdcall uint32_t DLLXInputSetState(uint32_t userIndex, XInputVibration * vibration);

/** XInput driver entry points  */
DLLXInputEnable * XInputEnable = NULL;
DLLXInputGetCapabilities * XInputGetCapabilities = NULL;
DLLXInputGetState * XInputGetState = NULL;
DLLXInputSetState * XInputSetState = NULL;

/*****************************************************************************/
LeGamePad::LeGamePad(int pad) :
    stickLeftX(0.0f), stickLeftY(0.0f),
    stickRightX(0.0f), stickRightY(0.0f),
    buttons(0), toggled(), pad(pad)
{
}

LeGamePad::~LeGamePad()
{
}

/*****************************************************************************/
void LeGamePad::init()
{
    stickLeftX = 0.0f;
    stickLeftY = 0.0f;
    stickRightX = 0.0f;
    stickRightY = 0.0f;
    buttons = 0;

    feedback(0.0f, 0.0f);
}

/*****************************************************************************/
void LeGamePad::update()
{
    XInputState state;
    memset(&state, 0, sizeof(XInputState));
    XInputGetState(pad, &state);

    stickLeftX = normalize(state.gamepad.thumbLX);
    stickLeftY = normalize(state.gamepad.thumbLY);
    stickRightX = normalize(state.gamepad.thumbRX);
    stickRightY = normalize(state.gamepad.thumbRY);

    toggled = buttons ^ state.gamepad.buttons;
    buttons = state.gamepad.buttons;
}

void LeGamePad::feedback(float left, float right)
{
    XInputVibration vibration;
    vibration.leftMotorSpeed = left * 0xFFFF;
    vibration.rightMotorSpeed = right * 0xFFFF;
    XInputSetState(pad, &vibration);
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
/** XInput driver fallback functions */
__stdcall void XInputEnableFB(bool enable){}
__stdcall uint32_t XInputGetCapabilitiesFB(uint32_t userIndex, uint32_t flags, XInputCaps * capabilities){return 0;}
__stdcall uint32_t XInputGetStateFB(uint32_t userIndex, XInputState * state){return 0;}
__stdcall uint32_t XInputSetStateFB(uint32_t userIndex, XInputVibration * vibration){return 0;}

/** XInput driver init */
void LeGamePad::setup()
{
    HMODULE module = LoadLibrary("xinput1_3.dll");
    if (module) {
        XInputEnable = (DLLXInputEnable *) GetProcAddress(module, "XInputEnable");
        XInputGetCapabilities = (DLLXInputGetCapabilities *) GetProcAddress(module, "XInputGetCapabilities");
        XInputGetState = (DLLXInputGetState *) GetProcAddress(module, "XInputGetState");
        XInputSetState = (DLLXInputSetState *) GetProcAddress(module, "XInputSetState");
    }else printf("gamepad: unable to load MS xInput driver (V1.3)");

    if (!XInputEnable) XInputEnable = XInputEnableFB;
    if (!XInputGetCapabilities) XInputGetCapabilities = XInputGetCapabilitiesFB;
    if (!XInputGetState) XInputGetState = XInputGetStateFB;
    if (!XInputSetState) XInputSetState = XInputSetStateFB;

    XInputEnable(true);
}
