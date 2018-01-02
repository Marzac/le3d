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

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <windows.h>

#include "timing.h"

/*****************************************************************************/
LeTiming::LeTiming() :
	fps(0.0f), targetFps(LE_TIMING_FPS),
	countsPerSec(0), countsPerFrame(0),
	lastCounter(0),
	enableFPSDisplay(true)
{
	LARGE_INTEGER pf;
	QueryPerformanceFrequency(&pf);

	countsPerSec = pf.QuadPart;
	countsPerFrame = countsPerSec / targetFps;

	printf("Timing: counts per seconds: %I64d\n", countsPerSec);
	printf("Timing: counts per frame: %I64d\n", countsPerFrame);
}

LeTiming::~LeTiming()
{
}

/*****************************************************************************/
void LeTiming::firstFrame()
{
	LARGE_INTEGER pc;
	QueryPerformanceCounter(&pc);
	lastCounter = pc.QuadPart;
	timeBeginPeriod(LE_TIMING_GRANULARITY);
}

void LeTiming::lastFrame()
{
    timeEndPeriod(LE_TIMING_GRANULARITY);
}

bool LeTiming::isNextFrame()
{
	LARGE_INTEGER pc;
	QueryPerformanceCounter(&pc);
	int64_t dt = pc.QuadPart - lastCounter;
	if (dt < countsPerFrame) return false;
	lastCounter = pc.QuadPart;

	fps = (float) countsPerSec / dt;
    if (enableFPSDisplay) displayFPS();

	return true;
}

void LeTiming::waitNextFrame()
{
	while(1) {
        LARGE_INTEGER pc;
        QueryPerformanceCounter(&pc);
        int64_t dt = pc.QuadPart - lastCounter;
        int64_t dg = countsPerFrame - dt;
        if (dg <= 0) break;

        int64_t ms = (1000 * dg) / countsPerSec;
        ms -= LE_TIMING_GRANULARITY;
        if (ms > 0) Sleep(ms);
	}

	LARGE_INTEGER pc;
    QueryPerformanceCounter(&pc);
    int64_t dt = pc.QuadPart - lastCounter;
    lastCounter = pc.QuadPart;

    fps = (float) countsPerSec / dt;
    if (enableFPSDisplay) displayFPS();
}

/*****************************************************************************/
void LeTiming::displayFPS()
{
    static int ttd = 0;
	if (ttd++ == LE_TIMING_FPS) {
		printf("FPS %f\n", fps);
		ttd = 0;
	}
}
