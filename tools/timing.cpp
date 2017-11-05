/**
	\file timing.cpp
	\brief LightEngine 3D (tools): Time measurement and framerate limiter
	\brief All platforms implementation
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

#include <x86intrin.h>

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
	lastCounter(0)
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
/*
void LeTiming::estimate()
{
	Time iTime;
	gettimeofday(&iTime, NULL);
	uint64_t iTicks = __rdtsc();
	uint64_t eTime = 0;
	while(eTime < 250000) {
		Time cTime;
		gettimeofday(&cTime, NULL);
		eTime = (cTime.tv_sec - iTime.tv_sec) * 1000000;
		eTime += cTime.tv_usec - iTime.tv_usec;
	}
	uint64_t cTicks = __rdtsc();
	ticksPerSec = (float) (cTicks - iTicks) / (eTime * 0.000001f);
}
*/

/*****************************************************************************/
void LeTiming::firstFrame()
{
	LARGE_INTEGER pc;
	QueryPerformanceCounter(&pc);
	lastCounter = pc.QuadPart;
}

bool LeTiming::isNextFrame()
{
	LARGE_INTEGER pc;
	QueryPerformanceCounter(&pc);
	int64_t dt = pc.QuadPart - lastCounter;
	if (dt < countsPerFrame) return false;

	lastCounter = pc.QuadPart + countsPerFrame - dt;
	fps = (float) countsPerSec / dt;
	return true;
}


/*
	int64_t dt = __rdtsc() - fpsLastTicks;
	if (dt < ticksPerFrame) return false;
	fpsLastTicks = __rdtsc() + ticksPerFrame - dt;
	fps = (float) ticksPerSec / dt;
	return true;
*/
