/**
	\file physics.cpp
	\brief LightEngine 3D (tools): Collision routines
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

#ifndef PHYSICS_H
#define PHYSICS_H

	#include "../engine/le3d.h"

	typedef enum{
		PHYSICS_RECT_NO_COL		= 0,
		PHYSICS_RECT_LEFT_COL	= 1,
		PHYSICS_RECT_RIGHT_COL	= 2,
		PHYSICS_RECT_TOP_COL	= 3,
		PHYSICS_RECT_BOTTOM_COL = 4,
	}PHYSICS_RECT_RESULT;

	int collideRectRect(float &ansX, float &ansY, float srcX, float srcY, float srcW, float srcH, float dstX, float dstY, float dstW, float dstH);
	int collideSphereBox(LeVertex &ans, LeVertex pos, float radius, LeVertex size);
	int collideSphereMesh(LeVertex &ans, LeVertex pos, float radius, LeMesh * mesh);

	int traceBox(LeVertex pos, LeVertex size, LeVertex axis, float &distance);
	int traceMesh(LeMesh * mesh, LeAxis axis, float &distance);


#endif // PHYSICS_H
