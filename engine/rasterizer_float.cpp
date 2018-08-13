/**
	\file rasterizer_float.cpp
	\brief LightEngine 3D: Triangle rasterizer (floating point)
	\brief All platforms implementation
	\brief Support textured triangles
	\brief Textures can have an alpha channel and mipmaps
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2018
	\version 1.7

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

#if LE_RENDERER_INTRASTER == 0

#include "rasterizer.h"
#include "draw.h"
#include "bmpcache.h"

#include <stdlib.h>
#include <string.h>
#include <float.h>

/*****************************************************************************/
/** Platform specific or reference fillers */
#if LE_USE_SIMD == 1 && LE_USE_SSE2 == 1
	#include "fillers/float/sse/flattexzc.h"
	#include "fillers/float/sse/flattexzcfog.h"
	#include "fillers/float/sse/flattexalphazc.h"
	#include "fillers/float/sse/flattexalphazcfog.h"
#elif LE_USE_SIMD == 1 && LE_USE_AMMX == 1
	#include "fillers/float/ammx/flattexzc.h"
	#include "fillers/float/ref/flattexzcfog.h"
	#include "fillers/float/ref/flattexalphazc.h"
	#include "fillers/float/ref/flattexalphazcfog.h"
#else
	#include "fillers/float/ref/flattexzc.h"
	#include "fillers/float/ref/flattexzcfog.h"
	#include "fillers/float/ref/flattexalphazc.h"
	#include "fillers/float/ref/flattexalphazcfog.h"
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
	memset(xs, 0, sizeof(float) * 4);
	memset(ys, 0, sizeof(float) * 4);
	memset(ws, 0, sizeof(float) * 4);
	memset(us, 0, sizeof(float) * 4);
	memset(vs, 0, sizeof(float) * 4);

	frame.allocate(width, height);
	frame.clear(background);
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
	//_controlfp(_MCW_RC, _RC_CHOP);
	//_controlfp(_MCW_DN, _DN_FLUSH);
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
		xs[0] = floorf(curTriangle->xs[0] + 0.5f);
		xs[1] = floorf(curTriangle->xs[1] + 0.5f);
		xs[2] = floorf(curTriangle->xs[2] + 0.5f);
		ys[0] = floorf(curTriangle->ys[0] + 0.5f);
		ys[1] = floorf(curTriangle->ys[1] + 0.5f);
		ys[2] = floorf(curTriangle->ys[2] + 0.5f);
		ws[0] = curTriangle->zs[0];
		ws[1] = curTriangle->zs[1];
		ws[2] = curTriangle->zs[2];

		if (xs[0] < 0 || xs[0] > 1023 ||
			xs[1] < 0 || xs[1] > 1023 ||
			xs[2] < 0 || xs[2] > 1023 ||
			ys[0] < 0 || ys[0] > 767 ||
			ys[1] < 0 || ys[1] > 767 ||
			ys[2] < 0 || ys[2] > 767)
			xs[0] = 0;

	// Sort vertexes vertically
		int vt = 0, vb = 0, vm1 = 0, vm2 = 3;
		if (ys[0] < ys[1]) {
			if (ys[0] < ys[2]) {
				vt = 0;
				if (ys[1] < ys[2]) { vm1 = 1; vb = 2; }
				else { vm1 = 2; vb = 1; }
			}
			else {
				vt = 2;	vm1 = 0; vb = 1;
			}
		}
		else {
			if (ys[1] < ys[2]) {
				vt = 1;
				if (ys[0] < ys[2]) { vm1 = 0; vb = 2; }
				else { vm1 = 2; vb = 0; }
			}
			else {
				vt = 2; vm1 = 1; vb = 0;
			}
		}

	// Get vertical span
		float dy = ys[vb] - ys[vt];
		if (dy == 0.0f) continue;

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
		float texSizeUFloat = (float) (1 << texSizeU);
		texScale_4 = _mm_set1_ps(texSizeUFloat);
		texMaskU_4 = _mm_set1_epi32(texMaskU);
		texMaskV_4 = _mm_set1_epi32(texMaskV << texSizeU);
		
		__m128i zv = _mm_set1_epi32(0);
		color_4 = _mm_loadu_si128((__m128i *) &curTriangle->solidColor);
		color_4 = _mm_unpacklo_epi32(color_4,color_4);
		color_4 = _mm_unpacklo_epi8(color_4, zv);
	#elif LE_USE_SIMD == 1 && LE_USE_AMMX == 1
		prepare_fill_texel(&curTriangle->solidColor);
	#endif	// LE_USE_SIMD && LE_USE_SSE2

	// Convert texture coordinates
		float sx = (float) (1 << bmp->txP2);
		us[0] = curTriangle->us[0] * sx;
		us[1] = curTriangle->us[1] * sx;
		us[2] = curTriangle->us[2] * sx;

		float sy = (float) (1 << bmp->tyP2);
		vs[0] = curTriangle->vs[0] * sy;
		vs[1] = curTriangle->vs[1] * sy;
		vs[2] = curTriangle->vs[2] * sy;

	// Compute the mean vertex
		float n = (ys[vm1] - ys[vt]) / dy;
		xs[3] = (xs[vb] - xs[vt]) * n + xs[vt];
		ys[3] = ys[vm1];
		ws[3] = (ws[vb] - ws[vt]) * n + ws[vt];
		us[3] = (us[vb] - us[vt]) * n + us[vt];
		vs[3] = (vs[vb] - vs[vt]) * n + vs[vt];

	// Sort vertexes horizontally
		int dx = (int) (xs[vm2] - xs[vm1]);
		if (dx < 0) {int t = vm1; vm1 = vm2; vm2 = t;}

	// Render the triangle
		fillTriangleZC(vt, vm1, vm2, true);
		fillTriangleZC(vm1, vm2, vb, false);
	}
}

/*****************************************************************************/
void LeRasterizer::fillTriangleZC(int vi1, int vi2, int vi3, bool top)
{
	float ax1, aw1, au1, av1;
	float ax2, aw2, au2, av2;
	int y1, y2;
	float x1, x2, w1, w2;
	float u1, u2, v1, v2;

	if (top) {
	// Top triangle
		float d = ys[vi2] - ys[vi1];
		if (d == 0.0f) return;

		float id = 1.0f / d;
		ax1 = (xs[vi2] - xs[vi1]) * id;
		aw1 = (ws[vi2] - ws[vi1]) * id;
		au1 = (us[vi2] - us[vi1]) * id;
		av1 = (vs[vi2] - vs[vi1]) * id;
		ax2 = (xs[vi3] - xs[vi1]) * id;
		aw2 = (ws[vi3] - ws[vi1]) * id;
		au2 = (us[vi3] - us[vi1]) * id;
		av2 = (vs[vi3] - vs[vi1]) * id;

		x1 = xs[vi1];
		x2 = x1;
		w1 = ws[vi1];
		w2 = w1;
		u1 = us[vi1];
		u2 = u1;
		v1 = vs[vi1];
		v2 = v1;

		y1 = (int) ys[vi1];
		y2 = (int) ys[vi2];
	}else{
	// Bottom triangle
		float d = ys[vi3] - ys[vi1];
		if (d == 0.0f) return;

		float id = 1.0f / d;
		ax1 = (xs[vi3] - xs[vi1]) * id;
		aw1 = (ws[vi3] - ws[vi1]) * id;
		au1 = (us[vi3] - us[vi1]) * id;
		av1 = (vs[vi3] - vs[vi1]) * id;
		ax2 = (xs[vi3] - xs[vi2]) * id;
		aw2 = (ws[vi3] - ws[vi2]) * id;
		au2 = (us[vi3] - us[vi2]) * id;
		av2 = (vs[vi3] - vs[vi2]) * id;

		x1 = xs[vi1];
		w1 = ws[vi1];
		u1 = us[vi1];
		v1 = vs[vi1];

		x2 = xs[vi2];
		w2 = ws[vi2];
		u2 = us[vi2];
		v2 = vs[vi2];

		y1 = (int) ys[vi1];
		y2 = (int) ys[vi3];
	}

	if (curTriangle->flags & LE_TRIANGLE_BLENDED) {
		if (curTriangle->flags & LE_TRIANGLE_FOGGED) {
			for (int y = y1; y <= y2; y++) {
				fillFlatTexAlphaZCFog(y, x1, x2, w1, w2, u1, u2, v1, v2);
				x1 += ax1; x2 += ax2;
				u1 += au1; u2 += au2;
				v1 += av1; v2 += av2;
				w1 += aw1; w2 += aw2;
			}
		}
		else {
			for (int y = y1; y <= y2; y++) {
				fillFlatTexAlphaZC(y, x1, x2, w1, w2, u1, u2, v1, v2);
				x1 += ax1; x2 += ax2;
				u1 += au1; u2 += au2;
				v1 += av1; v2 += av2;
				w1 += aw1; w2 += aw2;
			}
		}
	}
	else {
		if (curTriangle->flags & LE_TRIANGLE_FOGGED) {
			for (int y = y1; y <= y2; y++) {
				fillFlatTexZCFog(y, x1, x2, w1, w2, u1, u2, v1, v2);
				x1 += ax1; x2 += ax2;
				u1 += au1; u2 += au2;
				v1 += av1; v2 += av2;
				w1 += aw1; w2 += aw2;
			}
		}
		else {
			for (int y = y1; y <= y2; y++) {
				fillFlatTexZC(y, x1, x2, w1, w2, u1, u2, v1, v2);
				x1 += ax1; x2 += ax2;
				u1 += au1; u2 += au2;
				v1 += av1; v2 += av2;
				w1 += aw1; w2 += aw2;
			}
		}
	}
}

#endif // LE_RENDERER_INTRASTER == 0
