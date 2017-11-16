/**
	\file rasterizer_float.h
	\brief LightEngine 3D: Triangle rasterizer
	\brief All platforms implementation
	\brief Floating point mathematics
	\brief Support textured triangles
	\brief Textures can have an alpha channel and mipmaps
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

#ifndef LE_RASTERIZER_FLOAT_H
#define LE_RASTERIZER_FLOAT_H

/*****************************************************************************/
#ifndef LE_RASTERIZER_H
	#error The file rasterizer_float.h should not be included directly. Include rasterizer.h instead.
#endif

/*****************************************************************************/
#include "global.h"
#include "config.h"

#include "draw.h"
#include "geometry.h"
#include "trilist.h"

#if LE_USE_SIMD == 1
	#include "simd.h"
#endif

/*****************************************************************************/
class LeRasterizer
{
public:
	LeRasterizer(int width, int height);
	~LeRasterizer();

	void setBackground(uint32_t color);
	void rasterList(LeTriList * trilist);
	void flush();

	LeBitmap frame;

private:
	void topTriangleZC(int vt, int vm1, int vm2);
	void bottomTriangleZC(int vm1, int vm2, int vb);

	inline void fillFlatTexZC(float y, float x1, float x2, float w1, float w2, float u1, float u2, float v1, float v2);
	inline void fillFlatTexAlphaZC(float y, float x1, float x2, float w1, float w2, float u1, float u2, float v1, float v2);

	uint32_t color;
	LeBitmap * bmp;

	uint32_t * texPixels;
	uint32_t texSizeU;
	uint32_t texSizeV;
	uint32_t texMaskU;
	uint32_t texMaskV;

#if LE_USE_SIMD == 1
	v4sf texScale_4;
	v4si texMaskU_4;
	v4si texMaskV_4;
	v4si color_4;
#endif // LE_USE_SIMD

	float xs[4], ys[4], ws[4];
	float us[4], vs[4];

	uint32_t background;
};

#endif // LE_RASTERIZER_FLOAT_H
