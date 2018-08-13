/**
	\file timing.h
	\brief LightEngine 3D (tools): Native OS time measurement
	\brief Windows OS implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2018
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


#ifndef LE_TIMING_H
#define LE_TIMING_H

#include <stdint.h>

/*****************************************************************************/
/**
	\class LeTiming
	\brief Abstract OS native time measurement & process yielding
*/
class LeTiming {
public:
	LeTiming();
	~LeTiming();

    void setup(int targetFPS);

	void firstFrame();
	void lastFrame();

	bool isNextFrame();
	void waitNextFrame();

	float fps;					/**< current game FPS */
	bool enableDisplay;			/**< periodically displays FPS in console */

private:
	int64_t countsPerSec;		/** number of ticks per second */
	int64_t countsPerFrame;		/** number of ticks per frame */
	int64_t lastCounter;		/** counter last tick number */

	void display();
};

extern LeTiming timing;

#endif // LE_TIMING_H
