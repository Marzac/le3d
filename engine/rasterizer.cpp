/**
	\file rasterizer.cpp
	\brief LightEngine 3D: Triangle rasterizer (textured and textured with alpha channel)
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

#include "rasterizer.h"
#include "draw.h"
#include "bmpcache.h"

#include "emmintrin.h"
#include "mmintrin.h"
#include <stdlib.h>
#include <strings.h>

/*****************************************************************************/
LeRasterizer::LeRasterizer(int width, int height) :
	color(0xFFFFFF),
	texPixels(NULL),
	texSizeU(0), texSizeV(0),
	texMaskU(0), texMaskV(0),
	background(0)
{
	frame.allocate(width, height + 2);
	frame.data = ((uint32_t *) frame.data) + frame.tx;
	frame.ty -= 2;
	frame.clear(0);
}

LeRasterizer::~LeRasterizer()
{
	frame.data = ((uint32_t *) frame.data) - frame.tx;
	frame.ty += 2;
	frame.deallocate();
}

/*****************************************************************************/
void LeRasterizer::flush()
{
	frame.clear(background);
}

void LeRasterizer::setBackground(uint32_t color)
{
	background = color;
}

/*****************************************************************************/
void LeRasterizer::rasterList(TriList * trilist)
{
	trilist->zSort();
	for (int i = 0; i < trilist->noValid; i++) {
		Triangle * tri = &trilist->triangles[trilist->srcIndices[i]];

	// Retrieve the material
		LeBmpCache::Slot * slot = &bmpCache.slots[tri->tex];
		LeBitmap * bmp;
		if (slot->flags & LE_BMPCACHE_ANIMATION)
			bmp = &slot->extras[slot->cursor];
		else bmp = slot->bitmap;
		color = tri->color;

	// Retrieve texture information
		texPixels = (uint32_t *) bmp->data;
		texSizeU = bmp->txP2;
		texSizeV = bmp->tyP2;
		texMaskU = ((1 << bmp->txP2) - 1) << 16;
		texMaskV = ((1 << bmp->tyP2) - 1) << 16;

	// Order triangle vertexes
		int vt = 0, vb = 0, vm1 = 0, vm2 = 3;
		if (tri->ys[0] < tri->ys[1]) {
			if (tri->ys[0] < tri->ys[2]) {
				vt = 0;
				if (tri->ys[1] < tri->ys[2]) {vm1 = 1; vb = 2;}
				else {vm1 = 2; vb = 1;}
			}else{
				vt = 2;	vm1 = 0; vb = 1;
			}
		}else{
			if (tri->ys[1] < tri->ys[2]) {
				vt = 1;
				if (tri->ys[0] < tri->ys[2]) {vm1 = 0; vb = 2;}
				else {vm1 = 2; vb = 0;}
			}else{
				vt = 2; vm1 = 1; vb = 0;
			}
		}

	// Compute the mean point
		tri->ys[vb] = ceilf(tri->ys[vb]);
		float dy = tri->ys[vb] - tri->ys[vt];
		if (dy <= 0.0f) return;
		float n = (tri->ys[vm1] - tri->ys[vt]) / dy;
		tri->xs[3] = (tri->xs[vb] - tri->xs[vt]) * n + tri->xs[vt];
		tri->ys[3] = tri->ys[vm1];
		tri->us[3] = (tri->us[vb] - tri->us[vt]) * n + tri->us[vt];
		tri->vs[3] = (tri->vs[vb] - tri->vs[vt]) * n + tri->vs[vt];

	// Convert coordinates to integers
		cv4(tri->xs, xs);
		cv4(tri->ys, ys);
		cv4s(tri->us, us, 1 << bmp->txP2);
		cv4s(tri->vs, vs, 1 << bmp->tyP2);

	// Render the triangle
		int dx = xs[vm2] - xs[vm1];
		if (dx < 0) {int t = vm1; vm1 = vm2; vm2 = t;}
		topTriangle(vt, vm1, vm2);
		bottomTriangle(vm1, vm2, vb);

	#if LE_USE_MMX == 1
		_mm_empty();
	#endif
	}
}

/*****************************************************************************/
void LeRasterizer::topTriangle(int vt, int vm1, int vm2)
{
	int y1 = ys[vt] >> 16;
	int y2 = ys[vm1] >> 16;
	int d = y2 - y1;
	if (d == 0) return;

	int	ax1 = (xs[vm1] - xs[vt]) / d;
	int au1 = (us[vm1] - us[vt]) / d;
	int av1 = (vs[vm1] - vs[vt]) / d;

	int	ax2 = (xs[vm2] - xs[vt]) / d;
	int au2 = (us[vm2] - us[vt]) / d;
	int av2 = (vs[vm2] - vs[vt]) / d;

	int x1 = xs[vt];
	int u1 = us[vt];
	int v1 = vs[vt];

	int x2 = xs[vt];
	int u2 = us[vt];
	int v2 = vs[vt];

	x2 += 0xFFFF;
	for (int y = y1; y < y2; y++) {
		fillFlatTex(y, x1 >> 16, x2 >> 16, u1, u2, v1, v2);
		x1 += ax1;
		u1 += au1;
		v1 += av1;
		x2 += ax2;
		u2 += au2;
		v2 += av2;
	}
}

void LeRasterizer::bottomTriangle(int vm1, int vm2, int vb)
{
	int y1 = ys[vm1] >> 16;
	int y2 = ys[vb] >> 16;
	int d = y2 - y1;
	if (d == 0) return;

	int	ax1 = (xs[vb] - xs[vm1]) / d;
	int au1 = (us[vb] - us[vm1]) / d;
	int av1 = (vs[vb] - vs[vm1]) / d;

	int	ax2 = (xs[vb] - xs[vm2]) / d;
	int au2 = (us[vb] - us[vm2]) / d;
	int av2 = (vs[vb] - vs[vm2]) / d;

	int x1 = xs[vm1];
	int u1 = us[vm1];
	int v1 = vs[vm1];

	int x2 = xs[vm2];
	int u2 = us[vm2];
	int v2 = vs[vm2];

	x2 += 0xFFFF;
	for (int y = y1; y < y2; y++) {
		fillFlatTex(y, x1 >> 16, x2 >> 16, u1, u2, v1, v2);
		x1 += ax1;
		u1 += au1;
		v1 += av1;
		x2 += ax2;
		u2 += au2;
		v2 += av2;
	}
}

/*****************************************************************************/
#if LE_USE_MMX == 1
inline void LeRasterizer::fillFlatTex(int y, int x1, int x2, int u1, int u2, int v1, int v2)
{
	__m64 zv = _mm_setzero_si64();
	__m64 cv = (__m64) (uint64_t) color;
	cv = _mm_unpacklo_pi8(cv, zv);

	int d = x2 - x1;
	if (d == 0) return;

	int au = (u2 - u1) / d;
	int av = (v2 - v1) / d;
	uint32_t * fd = y * frame.tx + (uint32_t *) frame.data;

	for (int x = x1; x < x2; x ++) {
		uint32_t tu = (u1 & texMaskU) >> 16;
		uint32_t tv = (v1 & texMaskV) >> (16 - texSizeU);
		uint32_t t = texPixels[tu + tv];

		__m64 k = (__m64) (uint64_t) t;
		k = _mm_unpacklo_pi8(k, zv);
		k = _mm_mullo_pi16(k, cv);
		k = _mm_srli_pi16(k, 8);
		k = _mm_packs_pu16(k, zv);

		u1 += au;
		v1 += av;
		fd[x] = (int64_t) k;
	}
}
#else
inline void LeRasterizer::fillFlatTex(int y, int x1, int x2, int u1, int u2, int v1, int v2)
{
	uint8_t * c = (uint8_t *) &color;

	int d = x2 - x1;
	if (d == 0) return;

	int au = (u2 - u1) / d;
	int av = (v2 - v1) / d;
	uint32_t * fd = y * frame.tx + (uint32_t *) frame.data;

	for (int x = x1; x < x2; x++) {
		uint32_t tu = (u1 & texMaskU) >> 16;
		uint32_t tv = (v1 & texMaskV) >> (16 - texSizeU);
		uint8_t * t = (uint8_t *) &texPixels[tu + tv];
		uint8_t * p = (uint8_t *) &fd[x];
		p[0] = (t[0] * c[0]) >> 8;
		p[1] = (t[1] * c[1]) >> 8;
		p[2] = (t[2] * c[2]) >> 8;

		u1 += au;
		v1 += av;
	}
}
#endif

/*****************************************************************************/
#if LE_USE_MMX == 1
inline void LeRasterizer::fillFlatTexAlpha(int y, int x1, int x2, int u1, int u2, int v1, int v2)
{
	__m64 zv = _mm_setzero_si64();
	__m64 cv = (__m64) (uint64_t) color;
	cv = _mm_unpacklo_pi8(cv, zv);

	int d = x2 - x1;
	if (d == 0) return;

	int au = (u2 - u1) / d;
	int av = (v2 - v1) / d;
	uint32_t * fd = y * frame.tx + (uint32_t *) frame.data;

	for (int x = x1; x < x2; x ++) {
		uint32_t tu = (u1 & texMaskU) >> 16;
		uint32_t tv = (v1 & texMaskV) >> (16 - texSizeU);
		uint32_t t = texPixels[tu + tv];
		uint32_t f = fd[x];

		__m64 k =(__m64) (uint64_t) t;
		__m64 l =(__m64) (uint64_t) f;

		uint8_t b = (t >> 24) ^ 0xFF;
		__m64 bv = _mm_set1_pi16(b);

		k = _mm_unpacklo_pi8(k, zv);
		k = _mm_mullo_pi16(k, cv);

		l = _mm_unpacklo_pi8(l, zv);
		l = _mm_mullo_pi16(l, bv);

		k = _mm_adds_pu16(k, l);
		k = _mm_srli_pi16(k, 8);
		k = _mm_packs_pu16(k, zv);

		u1 += au;
		v1 += av;
		fd[x] = (int64_t) k;
	}
}

#else
inline void LeRasterizer::fillFlatTexAlpha(int y, int x1, int x2, int u1, int u2, int v1, int v2)
{
	uint8_t * c = (uint8_t *) &color;

	int d = x2 - x1;
	if (d == 0) return;

	int au = (u2 - u1) / d;
	int av = (v2 - v1) / d;
	uint32_t * fd = y * frame.tx + (uint32_t *) frame.data;

	for (int x = x1; x < x2; x++) {
		uint32_t tu = (u1 & texMaskU) >> 16;
		uint32_t tv = (v1 & texMaskV) >> (16 - texSizeU);
		uint8_t * t = (uint8_t *) &texPixels[tu + tv];
		uint8_t * p = (uint8_t *) &fd[x];

		uint8_t a = t[3] ^ 0xFF;
		p[0] = (p[0] * a + t[0] * c[0]) >> 8;
		p[1] = (p[1] * a + t[1] * c[1]) >> 8;
		p[2] = (p[2] * a + t[2] * c[2]) >> 8;

		u1 += au;
		v1 += av;
	}
}
#endif

/*****************************************************************************/
inline void LeRasterizer::cv4(const float * in, int32_t * out)
{
	out[0] = (int) (in[0] * 65536.0f);
	out[1] = (int) (in[1] * 65536.0f);
	out[2] = (int) (in[2] * 65536.0f);
	out[3] = (int) (in[3] * 65536.0f);
}

inline void LeRasterizer::cv4s(const float * in, int32_t * out, float s)
{
	const float k = 65536.0f * s;
	out[0]  = (int) (in[0] * k);
	out[1]  = (int) (in[1] * k);
	out[2]  = (int) (in[2] * k);
	out[3]  = (int) (in[3] * k);
}

