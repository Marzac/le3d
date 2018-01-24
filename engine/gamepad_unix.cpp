/**
	\file gamepad_unix.cpp
	\brief LightEngine 3D: Native OS gamepad manager
	\brief Unix OS implementation (with evdev)
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

#if defined(__unix__) || defined(__unix)

/*****************************************************************************/
#include "gamepad.h"

#include "global.h"
#include "config.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <unistd.h>
#include <linux/input.h>

/*****************************************************************************/
static void initEffects(int fd);

/*****************************************************************************/
static int unixJoyFiles[32];
static int unixJoyCount = 0;

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
	if (!unixJoyCount) return;
	if (unixJoyFiles[pad] < 0) return;
	int lastButtons = buttons;
	while (1) {
		input_event event;
		memset(&event, 0, sizeof(input_event));

		int bytes = read(unixJoyFiles[pad], &event, sizeof(input_event));
		if (bytes < 0) break;

		if (event.type == EV_KEY) {
		int b = LE_GAMEPAD_A;
		switch(event.code) {
		case BTN_SOUTH: b = LE_GAMEPAD_A; break;
		case BTN_EAST: b = LE_GAMEPAD_B; break;
		case BTN_WEST: b = LE_GAMEPAD_X; break;
		case BTN_NORTH: b = LE_GAMEPAD_Y; break;
		case BTN_START: b = LE_GAMEPAD_START; break;
		case BTN_SELECT: b = LE_GAMEPAD_START; break;
		//case BTN_MODE: b = LE_GAMEPAD_XBOX; break;
		case BTN_THUMBL: b = LE_GAMEPAD_LEFT_THUMB; break;
		case BTN_THUMBR: b = LE_GAMEPAD_RIGHT_THUMB; break;
		case BTN_TL: b = LE_GAMEPAD_LEFT_SHOULDER; break;
		case BTN_TR: b = LE_GAMEPAD_RIGHT_SHOULDER; break;
		}
			if (event.value) buttons |= b;
			else buttons &= ~b;
			//printf("Button: %i\n", event.code);

			}else if (event.type == EV_ABS) {
			switch (event.code) {
				case ABS_X: stickLeftX = normalize(event.value); break;
				case ABS_Y: stickLeftY = normalize(event.value); break;
				case ABS_Z: stickRightX = normalize(event.value); break;
				case ABS_RX: stickRightY = normalize(event.value); break;
				case ABS_HAT0X:
					if (event.value > 0) {
						buttons |=  LE_GAMEPAD_DPAD_RIGHT;
						buttons &= ~LE_GAMEPAD_DPAD_LEFT;
					}else if (event.value < 0) {
						buttons |=  LE_GAMEPAD_DPAD_LEFT;
						buttons &= ~LE_GAMEPAD_DPAD_RIGHT;
					}else{
						buttons &= ~LE_GAMEPAD_DPAD_LEFT;
						buttons &= ~LE_GAMEPAD_DPAD_RIGHT;
					}
					break;

				case ABS_HAT0Y:
					if (event.value > 0) {
						buttons |=  LE_GAMEPAD_DPAD_DOWN;
						buttons &= ~LE_GAMEPAD_DPAD_UP;
					}else if (event.value < 0) {
						buttons |=  LE_GAMEPAD_DPAD_UP;
						buttons &= ~LE_GAMEPAD_DPAD_DOWN;
					}else{
						buttons &= ~LE_GAMEPAD_DPAD_UP;
						buttons &= ~LE_GAMEPAD_DPAD_DOWN;
					}
					break;

				default: break;
			}
			//printf("Axis: %i\n", event.code);
		}
	}

	toggled = lastButtons ^ buttons;
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
    if (!unixJoyCount) return;
    if (unixJoyFiles[pad] < 0) return;

// Remove previous effect
	int fd = unixJoyFiles[pad];
	ioctl(fd, EVIOCRMFF, 0);

	if (left == 0.0f && right == 0.0f)
		return;

// Register the effect
	ff_effect effect;
	memset(&effect, 0, sizeof(ff_effect));
	effect.type = FF_RUMBLE;
	effect.id = -1;
	effect.replay.length = 0x7FFF;
	effect.replay.delay = 0;
	effect.u.rumble.strong_magnitude = (int) (left * 0xFFFF);
	effect.u.rumble.weak_magnitude = (int) (right * 0xFFFF);
	ioctl(fd, EVIOCSFF, &effect);

// Start the playback
	input_event play;
	memset(&play, 0, sizeof(input_event));
	play.type = EV_FF;
	play.code = 0;
	play.value = 1;
	write(unixJoyFiles[pad], &play, sizeof(input_event));	
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
	uint32_t version;
	char name[256];

	memset(unixJoyFiles, -1, sizeof(int));

	char filePath[24];
	for (int i = 0; i < 32; i++) {
	// Open the event file
		sprintf(filePath, "/dev/input/event%i", i);
		int fd = open(filePath, O_RDWR | O_NONBLOCK);
		if (fd < 0) continue;

	// Detect if gamepad
		uint8_t keyFeatures[1 + KEY_MAX / 8];
		memset(keyFeatures, 0, sizeof(keyFeatures));
		int r = ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keyFeatures)), keyFeatures);
		if (r < 1) {close(fd); continue;}

		#define tb(bit, array) (((array)[(bit) >> 3] >> ((bit) & 0x7)) & 1)
		if (!tb(BTN_GAMEPAD, keyFeatures)) {close(fd); continue;}

	// Get informations
		unixJoyFiles[unixJoyCount] = fd;
		ioctl(unixJoyFiles[unixJoyCount], EVIOCGNAME(256), name);
		ioctl(unixJoyFiles[unixJoyCount], EVIOCGVERSION, &version);
		printf("GamePad: name: %s\n", name);
		printf("GamePad: version: %i\n", version);

		initEffects(fd);

		unixJoyCount++;
	}

}
/**
	\fn void LeGamePad::release()
	\brief Release initialized gamepads
*/
void LeGamePad::release()
{
	for (int i = 0; i < unixJoyCount; i++) {
		if (unixJoyFiles[i] >= 0)
			close(unixJoyFiles[i]);
		unixJoyFiles[i] = -1;
	}
	unixJoyCount = 0;
}

/*****************************************************************************/
void initEffects(int fd)
{
// Set maximum effect gain
	input_event ie;
	memset(&ie, 0, sizeof(input_event));
	ie.type = EV_FF;
	ie.code = FF_GAIN;
	ie.value = 0xFFFF;
	write(fd, &ie, sizeof(input_event));
}

#endif
