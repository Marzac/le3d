/**
	\file rasterizer_float.cpp
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

#include "global.h"
#include "config.h"

#if LE_RENDERER_INTRASTER == 0

#if LE_USE_MMX == 1
	#error MMX optimisation cannot be used with floating point rasterizer.
#endif

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
	bmp(NULL),
	texPixels(NULL),
	texSizeU(0), texSizeV(0),
	texMaskU(0), texMaskV(0),
	background(0)
{
	memset(xs, 0, sizeof(float) * 4);
	memset(ys, 0, sizeof(float) * 4);
	memset(ws, 0, sizeof(float) * 4);
	memset(us, 0, sizeof(float) * 4);
	memset(vs, 0, sizeof(float) * 4);

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
void LeRasterizer::rasterList(LeTriList * trilist)
{
	trilist->zSort();
	
	for (int i = 0; i < trilist->noValid; i++) {
		LeTriangle * tri = &trilist->triangles[trilist->srcIndices[i]];

	// Retrieve the material
		LeBmpCache::Slot * slot = &bmpCache.slots[tri->tex];
		if (slot->flags & LE_BMPCACHE_ANIMATION)
			bmp = &slot->extras[slot->cursor];
		else bmp = slot->bitmap;
		color = tri->color;

	// Convert position coordinates
		for (int i = 0; i < 3; i++) {
			xs[i] = tri->xs[i];
			ys[i] = tri->ys[i];
		#if LE_RENDERER_ZTEX == 1
			ws[i] = tri->zs[i];
		#endif // LE_RENDERER_ZTEX
		}
	
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
		float dy = ys[vb] - ys[vt];
		if (dy == 0.0f) continue;
	
	// Choose the mipmap level
	#if LE_RENDERER_MIPMAPS == 1
		if (bmp->mmLevels) {
			int r = (bmp->ty + dy * 0.5f) / dy;
			int l = LeGlobal::log2i32(r);
			l = cmin(l, bmp->mmLevels - 1);
			bmp = bmp->mipmaps[l];
		}
	#endif
	
	// Retrieve texture information
		texPixels = (uint32_t *) bmp->data;
		texSizeU = bmp->txP2;
		texSizeV = bmp->tyP2;
		texMaskU = (1 << bmp->txP2) - 1;
		texMaskV = (1 << bmp->tyP2) - 1;

	// Convert texture coordinates
		float sx = (float) (1 << bmp->txP2);
		float sy = (float) (1 << bmp->tyP2);
		for (int i = 0; i < 3; i++) {
			us[i] = tri->us[i] * sx;
			vs[i] = tri->vs[i] * sy;
		}

	// Compute the mean vertex
		float n = (ys[vm1] - ys[vt]) / dy;
		xs[3] = (xs[vb] - xs[vt]) * n + xs[vt];
		ys[3] = ys[vm1];
	#if LE_RENDERER_ZTEX == 1
		ws[3] = (ws[vb] - ws[vt]) * n + ws[vt];
	#endif // LE_RENDERER_ZTEX
		us[3] = (us[vb] - us[vt]) * n + us[vt];
		vs[3] = (vs[vb] - vs[vt]) * n + vs[vt];

	// Sort vertexes horizontally
		int dx = xs[vm2] - xs[vm1];
		if (dx < 0) {int t = vm1; vm1 = vm2; vm2 = t;}

	// Render the triangle
	#if LE_RENDERER_ZTEX == 1
		topTriangleZC(vt, vm1, vm2);
		bottomTriangleZC(vm1, vm2, vb);
	#else
		topTriangle(vt, vm1, vm2);
		bottomTriangle(vm1, vm2, vb);
	#endif
	}
}

/*****************************************************************************/
void LeRasterizer::topTriangleZC(int vt, int vm1, int vm2)
{
	float d = ys[vm1] - ys[vt];
	if (d == 0.0f) return;

	float ax1 = (xs[vm1] - xs[vt]) / d;
	float aw1 = (ws[vm1] - ws[vt]) / d;
	float au1 = (us[vm1] - us[vt]) / d;
	float av1 = (vs[vm1] - vs[vt]) / d;

	float ax2 = (xs[vm2] - xs[vt]) / d;
	float aw2 = (ws[vm2] - ws[vt]) / d;
	float au2 = (us[vm2] - us[vt]) / d;
	float av2 = (vs[vm2] - vs[vt]) / d;

	float x1 = xs[vt];
	float x2 = x1;
	float w1 = ws[vt];
	float w2 = w1;
	float u1 = us[vt];
	float u2 = u1;
	float v1 = vs[vt];
	float v2 = v1;

	int y1 = (int) ys[vt];
	int y2 = (int) ys[vm1];
	if (bmp->flags & LE_BMP_ALPHACHANNEL) {
		for (int y = y1; y < y2; y++) {
			fillFlatTexAlphaZC(y, x1, x2, w1, w2, u1, u2, v1, v2);
			x1 += ax1; x2 += ax2;
			u1 += au1; u2 += au2;
			v1 += av1; v2 += av2;
			w2 += aw2; w1 += aw1;
		}
	}else{
		for (int y = y1; y < y2; y++) {
			fillFlatTexZC(y, x1, x2, w1, w2, u1, u2, v1, v2);
			x1 += ax1; x2 += ax2;
			u1 += au1; u2 += au2;
			v1 += av1; v2 += av2;
			w2 += aw2; w1 += aw1;
		}
	}
}

void LeRasterizer::bottomTriangleZC(int vm1, int vm2, int vb)
{
	float d = ys[vb] - ys[vm1];
	if (d == 0.0f) return;

	float ax1 = (xs[vb] - xs[vm1]) / d;
	float aw1 = (ws[vb] - ws[vm1]) / d;
	float au1 = (us[vb] - us[vm1]) / d;
	float av1 = (vs[vb] - vs[vm1]) / d;
	float ax2 = (xs[vb] - xs[vm2]) / d;
	float aw2 = (ws[vb] - ws[vm2]) / d;
	float au2 = (us[vb] - us[vm2]) / d;
	float av2 = (vs[vb] - vs[vm2]) / d;

	float x1 = xs[vm1];
	float w1 = ws[vm1];
	float u1 = us[vm1];
	float v1 = vs[vm1];

	float x2 = xs[vm2];
	float w2 = ws[vm2];
	float u2 = us[vm2];
	float v2 = vs[vm2];

	int y1 = (int) ys[vm1];
	int y2 = (int) ys[vb];

	if (bmp->flags & LE_BMP_ALPHACHANNEL) {
		for (int y = y1; y < y2; y++) {
			fillFlatTexAlphaZC(y, x1, x2, w1, w2, u1, u2, v1, v2);
			x1 += ax1; x2 += ax2;
			w1 += aw1; w2 += aw2;
			u1 += au1; u2 += au2;
			v1 += av1; v2 += av2;
		}
	}else{
		for (int y = y1; y < y2; y++) {
			fillFlatTexZC(y, x1, x2, w1, w2, u1, u2, v1, v2);
			x1 += ax1; x2 += ax2;
			w1 += aw1; w2 += aw2;
			u1 += au1; u2 += au2;
			v1 += av1; v2 += av2;
		}
	}
}

void LeRasterizer::topTriangle(int vt, int vm1, int vm2)
{
	float d = ys[vm1] - ys[vt];
	if (d == 0.0f) return;

	float ax1 = (xs[vm1] - xs[vt]) / d;
	float au1 = (us[vm1] - us[vt]) / d;
	float av1 = (vs[vm1] - vs[vt]) / d;

	float ax2 = (xs[vm2] - xs[vt]) / d;
	float au2 = (us[vm2] - us[vt]) / d;
	float av2 = (vs[vm2] - vs[vt]) / d;

	float x1 = xs[vt];
	float x2 = x1;
	float u1 = us[vt];
	float u2 = u1;
	float v1 = vs[vt];
	float v2 = v1;

	int y1 = (int) ys[vt];
	int y2 = (int) ys[vm1];

	if (bmp->flags & LE_BMP_ALPHACHANNEL) {
		for (int y = y1; y < y2; y++) {
			fillFlatTexAlpha(y, x1, x2, u1, u2, v1, v2);
			x1 += ax1; x2 += ax2;
			u1 += au1; u2 += au2;
			v1 += av1; v2 += av2;
		}
	}else{
		for (int y = y1; y < y2; y++) {
			fillFlatTex(y, x1, x2, u1, u2, v1, v2);
			x1 += ax1; x2 += ax2;
			u1 += au1; u2 += au2;
			v1 += av1; v2 += av2;
		}

	}
}

void LeRasterizer::bottomTriangle(int vm1, int vm2, int vb)
{
	float d = ys[vb] - ys[vm1];
	if (d == 0.0f) return;

	float ax1 = (xs[vb] - xs[vm1]) / d;
	float au1 = (us[vb] - us[vm1]) / d;
	float av1 = (vs[vb] - vs[vm1]) / d;
	float ax2 = (xs[vb] - xs[vm2]) / d;
	float au2 = (us[vb] - us[vm2]) / d;
	float av2 = (vs[vb] - vs[vm2]) / d;

	float x1 = xs[vm1];
	float u1 = us[vm1];
	float v1 = vs[vm1];

	float x2 = xs[vm2];
	float u2 = us[vm2];
	float v2 = vs[vm2];

	int y1 = (int) ys[vm1];
	int y2 = (int) ys[vb];

	if (bmp->flags & LE_BMP_ALPHACHANNEL) {
		for (int y = y1; y < y2; y++) {
			fillFlatTexAlpha(y, x1, x2, u1, u2, v1, v2);
			x1 += ax1; x2 += ax2;
			u1 += au1; u2 += au2;
			v1 += av1; v2 += av2;
		}
	}else{
		for (int y = y1; y < y2; y++) {
			fillFlatTex(y, x1, x2, u1, u2, v1, v2);
			x1 += ax1; x2 += ax2;
			u1 += au1; u2 += au2;
			v1 += av1; v2 += av2;
		}
	}
}

/*****************************************************************************/
inline void LeRasterizer::fillFlatTexZC(float y, float x1, float x2, float w1, float w2, float u1, float u2, float v1, float v2)
{
	uint8_t * c = (uint8_t *) &color;

	float d = x2 - x1;
	if (d == 0.0f) return;

	float au = (u2 - u1) / d;
	float av = (v2 - v1) / d;
	float aw = (w2 - w1) / d;

	int xb = (int) x1;
	int xe = (int) x2;
	uint8_t * p = (uint8_t *) (xb + ((int) y) * frame.tx + (uint32_t *) frame.data);

	for (int x = xb; x <= xe; x++) {
		float z = 1.0f / w1;
		uint32_t tu = ((int32_t) (u1 * z)) & texMaskU;
		uint32_t tv = ((int32_t) (v1 * z)) & texMaskV;
		uint8_t * t = (uint8_t *) &texPixels[tu + (tv << texSizeU)];

		p[0] = (t[0] * c[0]) >> 8;
		p[1] = (t[1] * c[1]) >> 8;
		p[2] = (t[2] * c[2]) >> 8;
		p += 4;

		u1 += au;
		v1 += av;
		w1 += aw;
	}
}

inline void LeRasterizer::fillFlatTex(float y, float x1, float x2, float u1, float u2, float v1, float v2)
{
	uint8_t * c = (uint8_t *) &color;

	float d = x2 - x1;
	if (d == 0.0f) return;

	float au = (u2 - u1) / d;
	float av = (v2 - v1) / d;

	int xb = (int) x1;
	int xe = (int) x2;
	uint8_t * p = (uint8_t *) (xb + ((int) y) * frame.tx + (uint32_t *) frame.data);

	for (int x = xb; x <= xe; x++) {
		uint32_t tu = ((int32_t) u1) & texMaskU;
		uint32_t tv = ((int32_t) v1) & texMaskV;
		uint8_t * t = (uint8_t *) &texPixels[tu + (tv << texSizeU)];

		p[0] = (t[0] * c[0]) >> 8;
		p[1] = (t[1] * c[1]) >> 8;
		p[2] = (t[2] * c[2]) >> 8;
		p += 4;

		u1 += au;
		v1 += av;
	}
}

/*****************************************************************************/
inline void LeRasterizer::fillFlatTexAlphaZC(float y, float x1, float x2, float w1, float w2, float u1, float u2, float v1, float v2)
{
	uint8_t * c = (uint8_t *) &color;

	float d = x2 - x1;
	if (d == 0.0f) return;

	float au = (u2 - u1) / d;
	float av = (v2 - v1) / d;
	float aw = (w2 - w1) / d;

	int xb = (int) x1;
	int xe = (int) x2;
	uint8_t * p = (uint8_t *) (xb + ((int) y) * frame.tx + (uint32_t *) frame.data);

	for (int x = xb; x <= xe; x++) {
		float z = 1.0f / w1;
		uint32_t tu = ((int32_t) (u1 * z)) & texMaskU;
		uint32_t tv = ((int32_t) (v1 * z)) & texMaskV;
		uint8_t * t = (uint8_t *) &texPixels[tu + (tv << texSizeU)];

		uint8_t a = t[3] ^ 0xFF;
		p[0] = (p[0] * a + t[0] * c[0]) >> 8;
		p[1] = (p[1] * a + t[1] * c[1]) >> 8;
		p[2] = (p[2] * a + t[2] * c[2]) >> 8;
		p += 4;

		u1 += au;
		v1 += av;
		w1 += aw;
	}
}

inline void LeRasterizer::fillFlatTexAlpha(float y, float x1, float x2, float u1, float u2, float v1, float v2)
{
	uint8_t * c = (uint8_t *) &color;

	float d = x2 - x1;
	if (d == 0.0f) return;

	float au = (u2 - u1) / d;
	float av = (v2 - v1) / d;

	int xb = (int) x1;
	int xe = (int) x2;
	uint8_t * p = (uint8_t *) (xb + ((int) y) * frame.tx + (uint32_t *) frame.data);

	for (int x = xb; x <= xe; x++) {
		uint32_t tu = ((int32_t) u1) & texMaskU;
		uint32_t tv = ((int32_t) v1) & texMaskV;
		uint8_t * t = (uint8_t *) &texPixels[tu + (tv << texSizeU)];

		uint8_t a = t[3] ^ 0xFF;
		p[0] = (p[0] * a + t[0] * c[0]) >> 8;
		p[1] = (p[1] * a + t[1] * c[1]) >> 8;
		p[2] = (p[2] * a + t[2] * c[2]) >> 8;
		p += 4;

		u1 += au;
		v1 += av;
	}
}

#endif // LE_RENDERER_INTRASTER == 0
