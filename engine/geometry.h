/**
	\file geometry.h
	\brief LightEngine 3D: Vertex / axis / plan / matrix objects
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

#ifndef LE_GEOMETRY_H
#define LE_GEOMETRY_H

#include "global.h"
#include "config.h"

/*****************************************************************************/
#ifndef LE_USE_SIMD
	#error LE_USE_SIMD undefined.
	#error Use LE_USE_SIMD in (config.h) to enable or disable SIMD acceleration for calculations.
#endif // LE_USE_SIMD

/*****************************************************************************/
#if LE_USE_SIMD == 1
    #include "geometry_simd.h"
#else
    #include "geometry_scalar.h"
#endif // LE_USE_SIMD

#endif // LE_GEOMETRY_H
