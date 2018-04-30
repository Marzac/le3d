/**
	\file draw.h
	\brief LightEngine 3D: Native OS graphic context
	\brief All platforms implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2018
	\version 1.5

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

#ifndef LE_DRAW_H
#define LE_DRAW_H

#include "global.h"
#include "config.h"

#include "bitmap.h"
#include "window.h"

/*****************************************************************************/
/**
	\class LeDraw
	\brief Create and handle an OS native drawing context
*/
class LeDraw
{
public:
	LeDraw(LeDrawingContext context, int width = LE_RESOX_DEFAULT, int heigth = LE_RESOY_DEFAULT);
	~LeDraw();

	void setPixels(const void * data);

	int width;		/**< Width of context (in pixels) */
	int height;		/**< Height of context (in pixels) */

private:
	LeDrawingContext frontContext;
	LeHandle bitmap;

};

#endif	//LE_DRAW_H
