/**
	\file draw_unix.cpp
	\brief LightEngine 3D: OS native graphic context
	\brief Unix OS implementation (with X.Org / XLib)
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2018
	\version 1.4

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

#if defined(__unix__) || defined(__unix)

/*****************************************************************************/
#include "draw.h"

#include "global.h"
#include "config.h"

#define WINVER	0x0600
#include <windef.h>
#include <windows.h>

/*****************************************************************************/
LeDraw::LeDraw(LeHandle context, int width, int height) :
	width(width), height(height),
	frontContext(context)
{
	if (!frontContext) frontContext = (LeHandle) GetDC(NULL);
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
	info.bV4V4Compression = BI_RGB;
	SetDIBitsToDevice((HDC) frontContext, 0, 0, width, height, 0, 0, 0, height, data, (BITMAPINFO *) &info, DIB_RGB_COLORS);
}

#endif