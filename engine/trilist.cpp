/**
	\file trilist.cpp
	\brief LightEngine 3D: Triangle lists
	\brief All platforms implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2017
	\version 1.0

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

#include "trilist.h"

#include <stdint.h>

/*****************************************************************************/
TriList::TriList() :
	srcIndices(NULL), dstIndices(NULL),
	noAllocated(0), noUsed(0), noValid(0)
{
	allocate(LE_TRILIST_MAX);
}

TriList::TriList(int noTriangles)
{
	allocate(noTriangles);
}

TriList::~TriList()
{
	if (triangles) delete[] triangles;
	if (srcIndices) delete[] srcIndices;
	if (dstIndices) delete[] dstIndices;
}

/*****************************************************************************/
void TriList::allocate(int noTriangles)
{
	triangles = new Triangle[noTriangles];
	srcIndices = new int[noTriangles * 3];
	dstIndices = new int[noTriangles * 3];
	noAllocated = noTriangles;
}

void TriList::zSort()
{
	if (!noValid) return;
	zMergeSort(srcIndices, dstIndices, noValid);
}

/*****************************************************************************/
void TriList::zMergeSort(int indices[], int tmp[], int nb)
{
	int h1 = nb >> 1;
	int h2 = nb - h1;
	if (h1 >= 2) zMergeSort(&indices[0], &tmp[0], h1);
	if (h2 >= 2) zMergeSort(&indices[h1], &tmp[h1], h2);
	int u = 0;
	int v = h1;
	float a = triangles[indices[u]].vd;
	float b = triangles[indices[v]].vd;
	for (int i = 0; i < nb; i++) {
		if (a > b) {
			tmp[i] = indices[u++];
			if (u == h1) {
				for (i++; i < nb; i++)
					tmp[i] = indices[v ++];
				for (int i = 0; i < nb; i++)
					indices[i] = tmp[i];
				return;
			}
			a = triangles[indices[u]].vd;

		}else{
			tmp[i] = indices[v++];
			if (v == nb) {
				for (i++; i < nb; i++)
					tmp[i] = indices[u ++];
				for (int i = 0; i < nb; i++)
					indices[i] = tmp[i];
				return;
			}
			b = triangles[indices[v]].vd;
		}
	}
}
