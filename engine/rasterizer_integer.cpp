/**
	\file rasterizer_integer.cpp
	\brief LightEngine 3D: Triangle rasterizer (fixed point)
	\brief All platforms implementation
	\brief Support textured triangles
	\brief Textures can have an alpha channel and mipmaps
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2018
	\version 1.75

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

#include "global.h"
#include "config.h"

#if LE_RENDERER_INTRASTER == 1

#include "rasterizer.h"
#include "draw.h"
#include "bmpcache.h"

#include <stdlib.h>
#include <string.h>
#include <float.h>

/*****************************************************************************/
/** Platform specific or reference fillers */
#if LE_USE_SIMD == 1 && LE_USE_SSE2 == 1
	#include "fillers/integer/sse/flattexzc.h"
	#include "fillers/integer/sse/flattexzcfog.h"
	#include "fillers/integer/sse/flattexalphazc.h"
	#include "fillers/integer/sse/flattexalphazcfog.h"
#elif LE_USE_SIMD == 1 && LE_USE_AMMX == 1
	#include "fillers/integer/ammx/flattexzc.h"
	#include "fillers/integer/ref/flattexzcfog.h"
	#include "fillers/integer/ref/flattexalphazc.h"
	#include "fillers/integer/ref/flattexalphazcfog.h"
#else
	#include "fillers/integer/ref/flattexzc.h"
	#include "fillers/integer/ref/flattexzcfog.h"
	#include "fillers/integer/ref/flattexalphazc.h"
	#include "fillers/integer/ref/flattexalphazcfog.h"
#endif

/*****************************************************************************/
LeRasterizer::LeRasterizer(int width, int height) :
	frame(),
	background(LeColor()),
	texDiffusePixels(NULL),
	texSizeU(0), texSizeV(0),
	texMaskU(0), texMaskV(0),
	curTriangle(NULL), curTrilist(NULL)
{
	memset(xs, 0, sizeof(int32_t) * 4);
	memset(ys, 0, sizeof(int32_t) * 4);
	memset(ws, 0, sizeof(int32_t) * 4);
	memset(us, 0, sizeof(int32_t) * 4);
	memset(vs, 0, sizeof(int32_t) * 4);

	frame.allocate(width, height);
	frame.clear(LeColor());
	pixels = (LeColor *) frame.data;
}

LeRasterizer::~LeRasterizer()
{
	frame.deallocate();
}

/*****************************************************************************/
/**
	\fn void LeRasterizer::flush()
	\brief Fill the frame buffer with the background color
*/
void LeRasterizer::flush()
{
	frame.clear(background);
}

/*****************************************************************************/
/**
	\fn void LeRasterizer::rasterList(LeTriList * trilist)
	\brief Rasterize the given triangle list
	\param[in] trilist pointer to a triangle list
*/
void LeRasterizer::rasterList(LeTriList * trilist)
{
	trilist->zSort();

#if defined(__i386__) || defined(_M_IX86) || defined(_X86_) || defined(__x86_64__) || defined(_M_X64)
	_controlfp(_MCW_RC, _RC_CHOP);
	_controlfp(_MCW_DN, _DN_FLUSH);
#endif

	curTrilist = trilist;
	for (int i = 0; i < trilist->noValid; i++) {
		curTriangle = &trilist->triangles[trilist->srcIndices[i]];

	// Retrieve the material
		LeBmpCache::Slot * slot = &bmpCache.cacheSlots[curTriangle->diffuseTexture];
		LeBitmap * bmp = slot->bitmap;
		if (slot->flags & LE_BMPCACHE_ANIMATION)
			bmp = &slot->extras[slot->cursor];

	// Convert position coordinates
		xs[0] = cmbound((int32_t) (curTriangle->xs[0] + 0.5f), 0, frame.tx) << 16;
		xs[1] = cmbound((int32_t) (curTriangle->xs[1] + 0.5f), 0, frame.tx) << 16;
		xs[2] = cmbound((int32_t) (curTriangle->xs[2] + 0.5f), 0, frame.tx) << 16;
		ys[0] = cmbound((int32_t) (curTriangle->ys[0] + 0.5f), 0, frame.ty);
		ys[1] = cmbound((int32_t) (curTriangle->ys[1] + 0.5f), 0, frame.ty);
		ys[2] = cmbound((int32_t) (curTriangle->ys[2] + 0.5f), 0, frame.ty);

		const float sw = 0x1p30;
		ws[0] = (int32_t) (curTriangle->zs[0] * sw);
		ws[1] = (int32_t) (curTriangle->zs[1] * sw);
		ws[2] = (int32_t) (curTriangle->zs[2] * sw);

	// Sort vertexes vertically
		int vt = 0, vb = 0, vm1 = 0, vm2 = 3;
		if (ys[0] < ys[1]) {
			if (ys[0] < ys[2]) {
				vt = 0;
				if (ys[1] < ys[2]) {vm1 = 1; vb = 2;}
				else {vm1 = 2; vb = 1;}
			}else{
				vt = 2;	vm1 = 0; vb = 1;
			}
		}else{
			if (ys[1] < ys[2]) {
				vt = 1;
				if (ys[0] < ys[2]) {vm1 = 0; vb = 2;}
				else {vm1 = 2; vb = 0;}
			}else{
				vt = 2; vm1 = 1; vb = 0;
			}
		}

	// Get vertical span
		int dy = ys[vb] - ys[vt];
		if (dy == 0) continue;

	// Choose the mipmap level
		if (curTriangle->flags & LE_TRIANGLE_MIPMAPPED) {
			if (bmp->mmLevels) {
				float utop = curTriangle->us[vt] / curTriangle->zs[vt];
				float ubot = curTriangle->us[vb] / curTriangle->zs[vb];
				float vtop = curTriangle->vs[vt] / curTriangle->zs[vt];
				float vbot = curTriangle->vs[vb] / curTriangle->zs[vb];
				float d = cmmax(fabsf(utop - ubot), fabsf(vtop - vbot));

				int r = (int)((d * bmp->ty + dy * 0.5f) / dy);
				int l = LeGlobal::log2i32(r);
				l = cmmin(l, bmp->mmLevels - 1);
				bmp = bmp->mipmaps[l];
			}
		}

	// Retrieve texture information
		texDiffusePixels = (LeColor *) bmp->data;
		texSizeU = bmp->txP2;
		texSizeV = bmp->tyP2;
		texMaskU = (1 << bmp->txP2) - 1;
		texMaskV = (1 << bmp->tyP2) - 1;

	// Architecture specific pre-calculations
	#if LE_USE_SIMD == 1 && LE_USE_SSE2 == 1
		__m128i zv = _mm_set1_epi32(0);
		color_4 = _mm_loadu_si128((__m128i *) &curTriangle->solidColor);
		color_4 = _mm_unpacklo_epi32(color_4, color_4);
		color_4 = _mm_unpacklo_epi8(color_4, zv);
	#endif	// LE_USE_SIMD && LE_USE_SSE2
	
	#if LE_USE_AMMX == 1
		prepare_fill_texel(&curTriangle->solidColor);
	#endif	// LE_USE_AMMX

	// Convert texture coordinates
		const float su = (float) (65536 << bmp->txP2);
		us[0] = (int32_t) (curTriangle->us[0] * su);
		us[1] = (int32_t) (curTriangle->us[1] * su);
		us[2] = (int32_t) (curTriangle->us[2] * su);

		const float sv = (float) (65536 << bmp->tyP2);
		vs[0] = (int32_t) (curTriangle->vs[0] * sv);
		vs[1] = (int32_t) (curTriangle->vs[1] * sv);
		vs[2] = (int32_t) (curTriangle->vs[2] * sv);

	// Compute the mean vertex
		int n = ((ys[vm1] - ys[vt]) << 16) / dy;
		xs[3] = (((int64_t) (xs[vb] - xs[vt]) * n) >> 16) + xs[vt];
		ys[3] = ys[vm1];
		ws[3] = (((int64_t) (ws[vb] - ws[vt]) * n) >> 16) + ws[vt];
		us[3] = (((int64_t) (us[vb] - us[vt]) * n) >> 16) + us[vt];
		vs[3] = (((int64_t) (vs[vb] - vs[vt]) * n) >> 16) + vs[vt];

	// Sort vertexes horizontally
		int dx = xs[vm2] - xs[vm1];
		if (dx < 0) {int t = vm1; vm1 = vm2; vm2 = t;}

	// Render the triangle
		fillTriangleZC(vt, vm1, vm2, true);
		fillTriangleZC(vm1, vm2, vb, false);
	}
}

/*****************************************************************************/
void LeRasterizer::fillTriangleZC(int vi1, int vi2, int vi3, bool top)
{
	int	ax1, aw1, au1, av1;
	int	ax2, aw2, au2, av2;
	int x1, x2, y1, y2, w1, w2;
	int u1, u2, v1, v2;

	if (top) {
	// Top triangle
		int d = ys[vi2] - ys[vi1];
		if (d == 0) return;

		ax1 = (xs[vi2] - xs[vi1]) / d;
		aw1 = (ws[vi2] - ws[vi1]) / d;
		au1 = (us[vi2] - us[vi1]) / d;
		av1 = (vs[vi2] - vs[vi1]) / d;

		ax2 = (xs[vi3] - xs[vi1]) / d;
		aw2 = (ws[vi3] - ws[vi1]) / d;
		au2 = (us[vi3] - us[vi1]) / d;
		av2 = (vs[vi3] - vs[vi1]) / d;

		x1 = xs[vi1];
		x2 = x1;
		w1 = ws[vi1];
		w2 = w1;

		u1 = us[vi1];
		u2 = u1;
		v1 = vs[vi1];
		v2 = v1;

		y1 = ys[vi1];
		y2 = ys[vi2];
		x2 += 0xFFFF;

	}else{
	// Bottom triangle
		int d = ys[vi3] - ys[vi1];
		if (d == 0) return;

		ax1 = (xs[vi3] - xs[vi1]) / d;
		aw1 = (ws[vi3] - ws[vi1]) / d;
		au1 = (us[vi3] - us[vi1]) / d;
		av1 = (vs[vi3] - vs[vi1]) / d;

		ax2 = (xs[vi3] - xs[vi2]) / d;
		aw2 = (ws[vi3] - ws[vi2]) / d;
		au2 = (us[vi3] - us[vi2]) / d;
		av2 = (vs[vi3] - vs[vi2]) / d;
		
		x1 = xs[vi1];
		w1 = ws[vi1];
		u1 = us[vi1];
		v1 = vs[vi1];

		x2 = xs[vi2];
		w2 = ws[vi2];
		u2 = us[vi2];
		v2 = vs[vi2];
		
		y1 = ys[vi1];
		y2 = ys[vi3];
		x2 += 0xFFFF;
	}

	if (curTriangle->flags & LE_TRIANGLE_BLENDED) {
		if (curTriangle->flags & LE_TRIANGLE_FOGGED) {
			for (int y = y1; y < y2; y++) {
				fillFlatTexAlphaZCFog(y, x1 >> 16, x2 >> 16, w1, w2, u1, u2, v1, v2);
				x1 += ax1; x2 += ax2;
				u1 += au1; u2 += au2;
				v1 += av1; v2 += av2;
				w1 += aw1; w2 += aw2;
			}
		}
		else {
			for (int y = y1; y < y2; y++) {
				fillFlatTexAlphaZC(y, x1 >> 16, x2 >> 16, w1, w2, u1, u2, v1, v2);
				x1 += ax1; x2 += ax2;
				u1 += au1; u2 += au2;
				v1 += av1; v2 += av2;
				w1 += aw1; w2 += aw2;
			}
		}
	}else{
		if (curTriangle->flags & LE_TRIANGLE_FOGGED) {
			for (int y = y1; y < y2; y++) {
				fillFlatTexZCFog(y, x1 >> 16, x2 >> 16, w1, w2, u1, u2, v1, v2);
				x1 += ax1; x2 += ax2;
				u1 += au1; u2 += au2;
				v1 += av1; v2 += av2;
				w1 += aw1; w2 += aw2;
			}
		}else {
			for (int y = y1; y < y2; y++) {
				fillFlatTexZC(y, x1 >> 16, x2 >> 16, w1, w2, u1, u2, v1, v2);
				x1 += ax1; x2 += ax2;
				u1 += au1; u2 += au2;
				v1 += av1; v2 += av2;
				w1 += aw1; w2 += aw2;
			}
		}
	}
}

#endif // LE_RENDERER_INTRASTER == 1
