/**
	\file gamepad_win.cpp
	\brief LightEngine 3D: Native OS gamepad manager
	\brief Windows OS implementation (with XInput V1.3)
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2018
	\version 1.5

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

#if defined(_WIN32)

/*****************************************************************************/
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
typedef void __stdcall DLLXInputEnable(bool enable);
typedef uint32_t __stdcall DLLXInputGetCapabilities(uint32_t userIndex, uint32_t flags, XInputCaps * capabilities);
typedef uint32_t __stdcall DLLXInputGetState(uint32_t userIndex, XInputState * state);
typedef uint32_t __stdcall DLLXInputSetState(uint32_t userIndex, XInputVibration * vibration);

/*****************************************************************************/
/** XInput driver fallback functions */
void __stdcall XInputEnableFB(bool enable){}
uint32_t __stdcall XInputGetCapabilitiesFB(uint32_t userIndex, uint32_t flags, XInputCaps * capabilities){return 0;}
uint32_t __stdcall XInputGetStateFB(uint32_t userIndex, XInputState * state){return 0;}
uint32_t __stdcall XInputSetStateFB(uint32_t userIndex, XInputVibration * vibration){return 0;}

/** XInput driver entry points	*/
static HMODULE XInputModule = 0;
static DLLXInputEnable * XInputEnable = XInputEnableFB;
static DLLXInputGetCapabilities * XInputGetCapabilities = XInputGetCapabilitiesFB;
static DLLXInputGetState * XInputGetState = XInputGetStateFB;
static DLLXInputSetState * XInputSetState = XInputSetStateFB;

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

	feedback(0.0f, 0.0f);
}

/*****************************************************************************/
/**
	\fn void LeGamePad::update()
	\brief Update gamepad state (call the driver)
*/
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

/*****************************************************************************/
/**
	\fn void LeGamePad::feedback(float left, float right)
	\brief Send a force feedback order
	\param[in] left motor order (0.0 - 1.0)
	\param[in] right motor order (0.0 - 1.0)
*/
void LeGamePad::feedback(float left, float right)
{
	XInputVibration vibration;
	vibration.leftMotorSpeed = (uint16_t) (left * 0xFFFF);
	vibration.rightMotorSpeed = (uint16_t) (right * 0xFFFF);
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
/** XInput driver init */
void LeGamePad::setup()
{
	XInputModule = LoadLibraryA("xinput1_3.dll");
	if (XInputModule) {
		void * pa;
		pa = (void *) GetProcAddress(XInputModule, "XInputEnable");
		if (pa) XInputEnable = (DLLXInputEnable *) pa;
		pa = (void *) GetProcAddress(XInputModule, "XInputGetCapabilities");
		if (pa) XInputGetCapabilities = (DLLXInputGetCapabilities *) pa; 
		pa = (void *) GetProcAddress(XInputModule, "XInputGetState");
		if (pa) XInputGetState = (DLLXInputGetState *) pa; 
		pa = (void *) GetProcAddress(XInputModule, "XInputSetState");
		if (pa) XInputSetState = (DLLXInputSetState *) pa; 
	}else printf("gamepad: unable to load MS xInput driver (V1.3)");

	XInputEnable(true);
}

/** XInput driver deinit */
void LeGamePad::release()
{
	XInputEnable = XInputEnableFB;
	XInputGetCapabilities = XInputGetCapabilitiesFB;
	XInputGetState = XInputGetStateFB;
	XInputSetState = XInputSetStateFB;

	if (XInputModule) FreeLibrary(XInputModule);
	XInputModule = 0;
}

#endif
