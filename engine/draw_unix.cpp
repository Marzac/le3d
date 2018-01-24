/**
	\file draw_unix.cpp
	\brief LightEngine 3D: Native OS graphic context
	\brief Unix OS implementation (with X.Org / XLib)
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2018
	\version 1.5

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

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <stdio.h>

/*****************************************************************************/
LeDraw::LeDraw(LeDrawingContext context, int width, int height) :
	width(width), height(height),
	frontContext(context),
	bitmap(0)
{
	Visual * visual = DefaultVisual((Display *) context.display, 0);
	if (visual->c_class != TrueColor) {
		printf("Draw: can only draw on truecolor displays!\n");
		return;
	}
	bitmap = (LeHandle) XCreateImage((Display *) context.display, visual, 24, ZPixmap, 0, (char *) NULL, width, height, 32, 0);
}

LeDraw::~LeDraw()
{
	if (bitmap) XDestroyImage((XImage *) bitmap);
}

/*****************************************************************************/
/**
	\fn void LeDraw::setPixels(void * data)
	\brief Set the graphic content of the context
	\param[in] data pointer to an array of pixels
*/
void LeDraw::setPixels(const void * data)
{
	XImage * image = (XImage *) bitmap;
	image->data = (char *) data;
	XPutImage((Display *) frontContext.display, (Drawable) frontContext.window, (GC) frontContext.gc, image, 0, 0, 0, 0, width, height);
}

#endif
