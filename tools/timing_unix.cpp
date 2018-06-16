/**
	\file timing_unix.cpp
	\brief LightEngine 3D (tools): Native OS time measurement
	\brief Windows OS implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2018
	\version 1.7

	The MIT License (MIT)
	Copyright (c) 2015-2018 Fr�d�ric Meslin

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
#if defined(__unix__) || defined(__unix) || \
    defined(__APPLE__) && defined(__MACH__)
	
#include "timing.h"

#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <inttypes.h>

/*****************************************************************************/
/** Dirty patch for MacOS */
#ifdef __MACH__
	#include <mach/mach_time.h>
	#define CLOCK_MONOTONIC_RAW 0
	int clock_getres(int clk_id, struct timespec *t)
	{
		mach_timebase_info_data_t timebase;
		mach_timebase_info(&timebase);
		t->tv_sec = 0;
		t->tv_nsec = (double) timebase.numer / timebase.denom;
		return 0;
	}
	
	int clock_gettime(int clk_id, struct timespec *t)
	{
		mach_timebase_info_data_t timebase;
		mach_timebase_info(&timebase);
		uint64_t time = mach_absolute_time();
		double r = ((double) time * timebase.numer) / timebase.denom;
		t->tv_sec = (time_t) (r * 1e-9);
		t->tv_nsec = (long) r;
		return 0;
	}
#else
	#include <time.h>
#endif

/*****************************************************************************/
#define TIMING_SCHEDULER_GRANULARITY	1000
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
	\brief Configure the frame timing system
	\param[in] targetFPS desired application FPS
*/
void LeTiming::setup(int targetFPS)
{
    struct timespec tp;
	int r = clock_getres(CLOCK_MONOTONIC_RAW, &tp);
	if (r < 0) {
		printf("Timing: cannot retrieve clock resolution\n");
		return;
	}

	int16_t cr = (int64_t) tp.tv_sec * 1000000000 + tp.tv_nsec;
	printf("Timing: clock resolution (ms): %f\n", cr / 1000000.0f);

	countsPerSec = 1000000000;
	countsPerFrame = countsPerSec / targetFPS;
	printf("Timing: counts per seconds: %" PRId64 "\n", countsPerSec);
	printf("Timing: counts per frame: %" PRId64 "\n", countsPerFrame);
}

/*****************************************************************************/
/**
	\fn void LeTiming::firstFrame()
	\brief Mark the time of the first frame
*/
void LeTiming::firstFrame()
{
	struct timespec tp;
	clock_gettime(CLOCK_MONOTONIC_RAW, &tp);
	lastCounter = (int64_t) tp.tv_sec * 1000000000 + tp.tv_nsec;
}

/**
	\fn void LeTiming::lastFrame()
	\brief Mark the time of the last frame
*/
void LeTiming::lastFrame()
{
}

/*****************************************************************************/
/**
	\fn bool LeTiming::isNextFrame()
	\brief Is it the time to display the next frame?
	\return true if it is time, false else
*/
bool LeTiming::isNextFrame()
{
	struct timespec tp;
	clock_gettime(CLOCK_MONOTONIC_RAW, &tp);
	int64_t pc = (int64_t) tp.tv_sec * 1000000000 + tp.tv_nsec;

	int64_t dt = pc - lastCounter;
	if (dt < countsPerFrame) return false;
	lastCounter = pc;

    fps = (float) countsPerSec / dt;
    if (enableDisplay) display();
	return true;
}

/**
	\fn void LeTiming::waitNextFrame()
	\brief Wait until it is time to display the next frame
*/
void LeTiming::waitNextFrame()
{
	while(1) {
		struct timespec tp;
		clock_gettime(CLOCK_MONOTONIC_RAW, &tp);
		int64_t pc = (int64_t) tp.tv_sec * 1000000000 + tp.tv_nsec;

		int64_t dt = pc - lastCounter;
		int64_t dg = countsPerFrame - dt;
		if (dg <= 0) break;

		int64_t us = (1000000 * dg) / countsPerSec;
		if (us > TIMING_SCHEDULER_GRANULARITY)
			usleep(us - TIMING_SCHEDULER_GRANULARITY);
	}

	struct timespec tp;
	clock_gettime(CLOCK_MONOTONIC_RAW, &tp);
	int64_t pc = (int64_t) tp.tv_sec * 1000000000 + tp.tv_nsec;

	int64_t dt = pc - lastCounter;
	lastCounter = pc;

	fps = (float) countsPerSec / dt;
	if (enableDisplay) display();
}

/*****************************************************************************/
/**
	\fn void LeTiming::display()
	\brief Display the current application FPS in the console
*/
void LeTiming::display()
{
	static int ttd = 0;
	if (ttd++ == 30) {
		printf("FPS %f\n", fps);
		ttd = 0;
	}
}

#endif
