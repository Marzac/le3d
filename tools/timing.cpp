/**
	\file timing.cpp
	\brief LightEngine 3D (tools): Native OS time measurement
	\brief Windows OS implementation
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

#include "timing.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <windows.h>

/*****************************************************************************/
#define TIMING_SCHEDULER_GRANULARITY	1
LeTiming timing;

/*****************************************************************************/
LeTiming::LeTiming() :
	fps(0.0f),
    enableDisplay(true),
	countsPerSec(0), countsPerFrame(0),
	lastCounter(0)
{
}

LeTiming::~LeTiming()
{
}

/*****************************************************************************/
/**
	\fn void LeTiming::setup(int targetFPS)
	\brief targetFPS desired application FPS
*/
void LeTiming::setup(int targetFPS)
{
    LARGE_INTEGER pf;
	QueryPerformanceFrequency(&pf);

	countsPerSec = pf.QuadPart;
	countsPerFrame = countsPerSec / targetFPS;
	printf("Timing: counts per seconds: %I64d\n", countsPerSec);
	printf("Timing: counts per frame: %I64d\n", countsPerFrame);
}

/*****************************************************************************/
/**
	\fn void LeTiming::firstFrame()
	\brief mark the first frame
*/
void LeTiming::firstFrame()
{
	LARGE_INTEGER pc;
	QueryPerformanceCounter(&pc);

	lastCounter = pc.QuadPart;
	timeBeginPeriod(TIMING_SCHEDULER_GRANULARITY);
}

/**
	\fn void LeTiming::lastFrame()
	\brief mark the last frame
*/
void LeTiming::lastFrame()
{
	timeEndPeriod(TIMING_SCHEDULER_GRANULARITY);
}

/*****************************************************************************/
/**
	\fn bool LeTiming::isNextFrame()
	\brief is it the time to display the next frame?
	\return true if it is time, false else
*/
bool LeTiming::isNextFrame()
{
	LARGE_INTEGER pc;
	QueryPerformanceCounter(&pc);

	int64_t dt = pc.QuadPart - lastCounter;
	if (dt < countsPerFrame) return false;
	lastCounter = pc.QuadPart;

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
		LARGE_INTEGER pc;
		QueryPerformanceCounter(&pc);

		int64_t dt = pc.QuadPart - lastCounter;
		int64_t dg = countsPerFrame - dt;
		if (dg <= 0) break;

		int64_t ms = (1000 * dg) / countsPerSec - TIMING_SCHEDULER_GRANULARITY;
		if (ms > 0) Sleep(ms);
	}

	LARGE_INTEGER pc;
	QueryPerformanceCounter(&pc);

	int64_t dt = pc.QuadPart - lastCounter;
	lastCounter = pc.QuadPart;

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
