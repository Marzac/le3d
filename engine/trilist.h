/**
	\file trilist.h
	\brief LightEngine 3D: Triangle lists
	\brief All platforms implementation
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

#ifndef LE_TRILIST_H
#define LE_TRILIST_H

#include "global.h"
#include "config.h"

/*****************************************************************************/
/**
	\class LeTriangle
	\brief Represent a rasterizable triangle 
*/
struct LeTriangle
{
	float xs[4];		/**< x coordinate of vertexes */
	float ys[4];		/**< y coordinate of vertexes */
	float zs[4];		/**< z coordinate of vertexes */
	float us[4];		/**< u texture coordinate of vertexes */
	float vs[4];		/**< v texture coordinate of vertexes */
	float vd;			/**< average view distance */
	uint32_t color;		/**< solid color */
	int tex;			/**< texture slot */
};

/*****************************************************************************/
/**
	\class LeTriList
	\brief Contain and manage triangle lists
*/
class LeTriList
{
public:
	LeTriList();
	LeTriList(int noTrangles);
	~LeTriList();

	void allocate(int noTriangles);
	void zSort();

public:
	int * srcIndices;
	int * dstIndices;
	LeTriangle * triangles;

	int noAllocated;
	int noUsed;
	int noValid;

private:
	void zMergeSort(int indices[], int tmp[], int nb);
};

#endif // LE_TRILIST_H
