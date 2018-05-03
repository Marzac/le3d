/**
	\file draw_win.cpp
	\brief LightEngine 3D: Native OS graphic context
	\brief Windows OS implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
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

#if defined(_WIN32)

/*****************************************************************************/
#include "draw.h"

#include "global.h"
#include "config.h"

#define WINVER	0x0600
#include <windows.h>

/*****************************************************************************/
LeDraw::LeDraw(LeDrawingContext context, int width, int height) :
	width(width), height(height),
	frontContext(context),
	bitmap(0)
{
	if (!frontContext.gc) {
		frontContext.window = 0;
		frontContext.gc = (LeHandle) GetDC(NULL);
	}
}

LeDraw::~LeDraw()
{
}

/*****************************************************************************/
/**
	\fn void LeDraw::setPixels(void * data)
	\brief Set the graphic content of the context
	\param[in] data pointer to an array of pixels
*/
void LeDraw::setPixels(const void * data)
{
	BITMAPV4HEADER info;
	info.bV4Size = sizeof(BITMAPV4HEADER);
	info.bV4Width = width;
	info.bV4Height = -height;
	info.bV4Planes = 1;
	info.bV4BitCount = 32;
	info.bV4V4Compression = BI_BITFIELDS;
	info.bV4RedMask = 0xFF;
	info.bV4GreenMask = 0xFF00;
	info.bV4BlueMask = 0xFF0000;
	info.bV4AlphaMask = 0xFF000000;

	SetDIBitsToDevice((HDC) frontContext.gc, 0, 0, width, height, 0, 0, 0, height, data, (BITMAPINFO *) &info, DIB_RGB_COLORS);
}

#endif
