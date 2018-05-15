/**
	\file timing_amiga.cpp
	\brief LightEngine 3D (tools): Native OS time measurement
	\brief Amiga OS implementation
	\author Andreas Streichardt (andreas@mop.koeln)
	\twitter @m0ppers
	\website https://mop.koeln
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

/*****************************************************************************/
#include "timing.h"

#include <stdio.h>

#include <proto/exec.h>
#include <proto/timer.h>

/*****************************************************************************/
LeTiming timing;

struct timerequest* timereq = NULL;
struct MsgPort *port = NULL;
struct EClockVal ecval;
struct Device* TimerBase = NULL;

/*****************************************************************************/
LeTiming::LeTiming() :
	fps(0.0f),
    enableDisplay(true),
	countsPerSec(0), countsPerFrame(0),
	lastCounter(0)
{
	port = CreateMsgPort();
	if (!port) {
		printf("Couldn't create message port!\n");
		return;
	}
	timereq = (timerequest*) AllocMem(sizeof(timerequest), MEMF_CLEAR|MEMF_PUBLIC);
	if (!timereq) {
		printf("Couldn't alloc timerequest!\n");
		return;
	}

	uint32_t error = OpenDevice("timer.device", UNIT_WAITECLOCK, &timereq->tr_node, 0);
	if (error > 0) {
		printf("Couldn't open timer.device %d!\n", error);
		return;
	}
	TimerBase = (struct Device*) timereq->tr_node.io_Device;
	timereq->tr_node.io_Message.mn_ReplyPort = port;
	timereq->tr_node.io_Command = TR_ADDREQUEST;
}

LeTiming::~LeTiming()
{
	if (timereq) {
		CloseDevice(&timereq->tr_node);
		FreeMem(timereq, sizeof(timerequest));
	}
	if (port) {
		DeleteMsgPort(port);
	}
}

/*****************************************************************************/
/**
	\fn void LeTiming::setup(int targetFPS)
	\brief targetFPS desired application FPS
*/
void LeTiming::setup(int targetFPS)
{
	countsPerSec = ReadEClock(&ecval);
	
	countsPerFrame = countsPerSec / targetFPS;
	
	printf("Timing: counts per seconds: %d\n", (int) countsPerSec);
	printf("Timing: counts per frame: %d\n", (int) countsPerFrame);
}

/*****************************************************************************/
/**
	\fn void LeTiming::firstFrame()
	\brief mark the first frame
*/
void LeTiming::firstFrame()
{
	ReadEClock(&ecval);
	lastCounter = ((int64_t) ecval.ev_hi << 32) | ecval.ev_lo;
}

/**
	\fn void LeTiming::lastFrame()
	\brief mark the last frame
*/
void LeTiming::lastFrame()
{
}

/*****************************************************************************/
/**
	\fn bool LeTiming::isNextFrame()
	\brief is it the time to display the next frame?
	\return true if it is time, false else
*/
bool LeTiming::isNextFrame()
{
	ReadEClock(&ecval);

	int64_t pc = ((int64_t) ecval.ev_hi << 32) | ecval.ev_lo;
	int64_t dt = pc - lastCounter;
	if (dt < countsPerFrame) return false;
	lastCounter = pc;

    fps = (float) countsPerSec / dt;
    if (enableDisplay) display();
	return true;
}

/**
	\fn void LeTiming::waitNextFrame()
	\brief wait until it is time to display the next frame
*/
void LeTiming::waitNextFrame()
{
	while(1) {
		ReadEClock(&ecval);

		int64_t pc = ((int64_t) ecval.ev_hi << 32) | ecval.ev_lo;

		int64_t dt = pc - lastCounter;
		int64_t dg = countsPerFrame - dt;
		if (dg <= 0) {
			break;
		}

		ULONG us = dg / 1000000;
		if (us > 0) {
			SendIO(&timereq->tr_node);
			WaitPort(port);
			GetMsg(port);
		}
	}
	ReadEClock(&ecval);
	int64_t pc = ((int64_t) ecval.ev_hi << 32) | ecval.ev_lo;
	int64_t dt = pc - lastCounter;
	lastCounter = pc;

	fps = (float) countsPerSec / dt;
	if (enableDisplay) display();

}

/*****************************************************************************/
/**
	\fn void LeTiming::display()
	\brief display the current application FPS in the console
*/
void LeTiming::display()
{
	static int ttd = 0;
	if (ttd++ == 30) {
		printf("FPS %f\n", fps);
		ttd = 0;
	}
}