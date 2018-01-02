/**
	\file bset.cpp
	\brief LightEngine 3D: Billboard set container and manipulator
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

#include "bset.h"
#include <string.h>

/*****************************************************************************/
LeBSet::LeBSet() :
    view(),
    pos(), scale(1.0f, 1.0f, 1.0f), angle(),
    places(NULL), sizes(NULL),
    colors(NULL), texSlots(NULL),
    flags(NULL),
    noBillboards(0),
    shades(NULL),
    allocated(false)
{
    updateMatrix();
}

LeBSet::LeBSet(int noBillboards) :
    view(),
    pos(), scale(1.0f, 1.0f, 1.0f), angle(),
    places(NULL), sizes(NULL),
    colors(NULL), texSlots(NULL),
    flags(NULL),
    noBillboards(0),
    shades(NULL),
    allocated(false)
{
    allocate(noBillboards);
    basic();
    updateMatrix();
}

LeBSet::~LeBSet()
{
    deallocate();
}

/*****************************************************************************/
void LeBSet::shadowCopy(LeBSet * copy) const
{
    if (copy->allocated) copy->deallocate();

    copy->places = places;
    copy->sizes = sizes;
    copy->colors = colors;
    copy->texSlots = texSlots;
    copy->flags = flags;

    if (shades) {
        copy->shades = new uint32_t[noBillboards];
        memcpy(copy->shades, shades, noBillboards * sizeof(uint32_t));
    }
}

void LeBSet::copy(LeBSet * copy) const
{
    if (copy->allocated) copy->deallocate();

    copy->allocate(noBillboards);

    memcpy(copy->places, places, noBillboards * sizeof(LeVertex));
    memcpy(copy->sizes, sizes, noBillboards * sizeof(float) * 2);
    memcpy(copy->colors, colors, noBillboards * sizeof(uint32_t));
    memcpy(copy->texSlots, texSlots, noBillboards * sizeof(int));
    memcpy(copy->flags, flags, noBillboards * sizeof(int));
}

/*****************************************************************************/
void LeBSet::basic()
{
    for (int b = 0; b < noBillboards; b++) {
        sizes[b*2+0] = 32.0f;
        sizes[b*2+1] = 32.0f;
        colors[b] = 0x00FFFFFF;
        texSlots[b] = 0;
        flags[b] = 0;
    }
}

/*****************************************************************************/
void LeBSet::allocate(int noBillboards)
{
    if (allocated) deallocate();

	places = new LeVertex[noBillboards];
	sizes = new float[noBillboards * 2];
	colors = new uint32_t[noBillboards];
    texSlots = new int[noBillboards];
	flags = new int[noBillboards];

	this->noBillboards = noBillboards;
	allocated = false;
}

void LeBSet::deallocate()
{
    if (allocated) {
        if (places) delete[] places;
        places = NULL;
        if (sizes) delete[] sizes;
        sizes = NULL;
        if (colors) delete[] colors;
        colors = NULL;
        if (texSlots) delete[] texSlots;
        texSlots = NULL;
        if (flags) delete[] flags;
        flags = NULL;

        noBillboards = 0;
        allocated = false;
    }

    if (shades) delete[] shades;
    shades = NULL;
}

/*****************************************************************************/
void LeBSet::transform(const LeMatrix &matrix)
{
	updateMatrix();
	view = matrix * view;
}

void LeBSet::setMatrix(const LeMatrix &matrix)
{
    view = matrix;
}

void LeBSet::updateMatrix()
{
	view.identity();
	view.scale(scale);
	view.rotate(angle * d2r);
	view.translate(pos);
}
