/**
	\file rasterizer.h
	\brief LightEngine 3D: Triangle rasterizer (textured and textured with alpha channel)
	\brief All platforms implementation
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

#ifndef LE_RASTERIZER_H
#define LE_RASTERIZER_H

#include "global.h"
#include "config.h"

/*****************************************************************************/
#ifndef LE_RENDERER_INTRASTER
	#error LE_RENDERER_INTRASTER undefined.
	#error Use LE_RENDERER_INTRASTER (in config.h) to select between fixed point or floating point triangle rasterization.
#endif // LE_RENDERER_INTRASTER

/*****************************************************************************/
#if LE_RENDERER_INTRASTER == 1
    #include "rasterizer_integer.h"
#else
    #include "rasterizer_float.h"
#endif

#endif // LE_RASTERIZER_H
