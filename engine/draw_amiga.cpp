/**
	\file draw_unix.cpp
	\brief LightEngine 3D: Native OS graphic context
	\brief Amiga OS implementation
	\author Andreas Streichardt (andreas@mop.koeln)
	\twitter @m0ppers
	\website https://mop.koeln
	\copyright Frédéric Meslin 2015 - 2018
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
#include "draw.h"

#include "global.h"
#include "config.h"

#include <stdio.h>

#include <cybergraphx/cybergraphics.h>
#include <proto/cybergraphics.h>
#include <proto/intuition.h>
#include <proto/exec.h>

struct Library *CyberGfxBase = NULL;

/*****************************************************************************/
LeDraw::LeDraw(LeDrawingContext context, int width, int height) :
	width(width), height(height),
	frontContext(context),
	bitmap(0)
{
	CyberGfxBase = OpenLibrary("cybergraphics.library", 41);
	if (!CyberGfxBase) {
		printf("ERROR: can`t open cybergraphics.library V41.\n");	
	}

	// TODO check that the display supports our depth/resolution requirements
}

LeDraw::~LeDraw()
{
	if (CyberGfxBase) {
		CloseLibrary(CyberGfxBase);
	}
}

/*****************************************************************************/
/**
	\fn void LeDraw::setContext(LeDrawingContext context)
	\brief Set the graphic context where to draw
	\param[in] context graphic context
*/
void LeDraw::setContext(LeDrawingContext context)
{
	frontContext = context;
}

/**
	\fn void LeDraw::setPixels(void * data)
	\brief Set the graphic content of the context
	\param[in] data pointer to an array of pixels
*/
void LeDraw::setPixels(const void * data)
{
	Window* window = (Window*) frontContext.window;
	WritePixelArray((APTR) data, 0, 0, 4 * width, window->RPort, window->BorderLeft, window->BorderTop, width, height, RECTFMT_RGBA);
}