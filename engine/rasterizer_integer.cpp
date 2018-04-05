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
	\version 1.5

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

/*****************************************************************************/
LeRasterizer::LeRasterizer(int width, int height) :
	frame(),
	background(0),
	color(0xFFFFFF),
	bmp(NULL),
	texPixels(NULL),
	texSizeU(0), texSizeV(0),
	texMaskU(0), texMaskV(0)
{
	memset(xs, 0, sizeof(int32_t) * 4);
	memset(ys, 0, sizeof(int32_t) * 4);
	memset(ws, 0, sizeof(int32_t) * 4);
	memset(us, 0, sizeof(int32_t) * 4);
	memset(vs, 0, sizeof(int32_t) * 4);

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

	for (int i = 0; i < trilist->noValid; i++) {
		LeTriangle * tri = &trilist->triangles[trilist->srcIndices[i]];

	// Retrieve the material
		LeBmpCache::Slot * slot = &bmpCache.slots[tri->tex];
		if (slot->flags & LE_BMPCACHE_ANIMATION)
			bmp = &slot->extras[slot->cursor];
		else bmp = slot->bitmap;
		color = tri->color;

	// Convert position coordinates
		xs[0] = (int32_t) (tri->xs[0]) << 16;
		xs[1] = (int32_t) (tri->xs[1]) << 16;
		xs[2] = (int32_t) (tri->xs[2]) << 16;
		ys[0] = (int32_t) (tri->ys[0]);
		ys[1] = (int32_t) (tri->ys[1]);
		ys[2] = (int32_t) (tri->ys[2]);

		const float sw = 65536.0f * 4096.0f;
		ws[0] = (int32_t) (tri->zs[0] * sw);
		ws[1] = (int32_t) (tri->zs[1] * sw);
		ws[2] = (int32_t) (tri->zs[2] * sw);

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
	#if LE_RENDERER_MIPMAPS == 1
		if (bmp->mmLevels) {
			float utop = tri->us[vt] / tri->zs[vt];
			float ubot = tri->us[vb] / tri->zs[vb];
			float vtop = tri->vs[vt] / tri->zs[vt];
			float vbot = tri->vs[vb] / tri->zs[vb];
			float d = cmax(fabs(utop - ubot), fabs(vtop - vbot));

			int r = (d * bmp->ty + dy * 0.5f) / dy;
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

	#if LE_USE_SIMD == 1 && LE_USE_SSE2 == 1
		v4si zv = {0, 0, 0, 0};
		color_4.v = (V4SI) _mm_load_ss((float *) &color);
		color_4.v = (V4SI) _mm_unpacklo_epi8((__m128i)color_4.v, (__m128i)zv.v);
	#endif

	// Convert texture coordinates
		const float su = (float) (65536 << bmp->txP2);
		us[0] = (int32_t) (tri->us[0] * su);
		us[1] = (int32_t) (tri->us[1] * su);
		us[2] = (int32_t) (tri->us[2] * su);

		const float sv = (float) (65536 << bmp->tyP2);
		vs[0] = (int32_t) (tri->vs[0] * sv);
		vs[1] = (int32_t) (tri->vs[1] * sv);
		vs[2] = (int32_t) (tri->vs[2] * sv);

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
		topTriangleZC(vt, vm1, vm2);
		bottomTriangleZC(vm1, vm2, vb);
	}
}

/*****************************************************************************/
void LeRasterizer::topTriangleZC(int vt, int vm1, int vm2)
{
	int d = ys[vm1] - ys[vt];
	if (d == 0) return;

	int	ax1 = (xs[vm1] - xs[vt]) / d;
	int	aw1 = (ws[vm1] - ws[vt]) / d;
	int au1 = (us[vm1] - us[vt]) / d;
	int av1 = (vs[vm1] - vs[vt]) / d;

	int	ax2 = (xs[vm2] - xs[vt]) / d;
	int	aw2 = (ws[vm2] - ws[vt]) / d;
	int au2 = (us[vm2] - us[vt]) / d;
	int av2 = (vs[vm2] - vs[vt]) / d;

	int x1 = xs[vt];
	int x2 = x1;
	int	w1 = ws[vt];
	int	w2 = w1;

	int u1 = us[vt];
	int u2 = u1;
	int v1 = vs[vt];
	int v2 = v1;

	int y1 = ys[vt];
	int y2 = ys[vm1];
	x2 += 0xFFFF;

	if (bmp->flags & LE_BITMAP_RGBA) {
		for (int y = y1; y < y2; y++) {
			fillFlatTexAlphaZC(y, x1 >> 16, x2 >> 16, w1, w2, u1, u2, v1, v2);
			x1 += ax1; x2 += ax2;
			u1 += au1; u2 += au2;
			v1 += av1; v2 += av2;
			w1 += aw1; w2 += aw2;
		}
	}else{
		for (int y = y1; y < y2; y++) {
			fillFlatTexZC(y, x1 >> 16, x2 >> 16, w1, w2, u1, u2, v1, v2);
			x1 += ax1; x2 += ax2;
			u1 += au1; u2 += au2;
			v1 += av1; v2 += av2;
			w1 += aw1; w2 += aw2;
		}
	}
}

void LeRasterizer::bottomTriangleZC(int vm1, int vm2, int vb)
{
	int d = ys[vb] - ys[vm1];
	if (d == 0) return;

	int	ax1 = (xs[vb] - xs[vm1]) / d;
	int	aw1 = (ws[vb] - ws[vm1]) / d;
	int au1 = (us[vb] - us[vm1]) / d;
	int av1 = (vs[vb] - vs[vm1]) / d;

	int	ax2 = (xs[vb] - xs[vm2]) / d;
	int	aw2 = (ws[vb] - ws[vm2]) / d;
	int au2 = (us[vb] - us[vm2]) / d;
	int av2 = (vs[vb] - vs[vm2]) / d;

	int x1 = xs[vm1];
	int w1 = ws[vm1];
	int u1 = us[vm1];
	int v1 = vs[vm1];

	int x2 = xs[vm2];
	int w2 = ws[vm2];
	int u2 = us[vm2];
	int v2 = vs[vm2];

	int y1 = ys[vm1];
	int y2 = ys[vb];
	x2 += 0xFFFF;

	if (bmp->flags & LE_BITMAP_RGBA) {
		for (int y = y1; y < y2; y++) {
			fillFlatTexAlphaZC(y, x1 >> 16, x2 >> 16, w1, w2, u1, u2, v1, v2);
			x1 += ax1; x2 += ax2;
			u1 += au1; u2 += au2;
			v1 += av1; v2 += av2;
			w1 += aw1; w2 += aw2;
		}
	}else{
		for (int y = y1; y < y2; y++) {
			fillFlatTexZC(y, x1 >> 16, x2 >> 16, w1, w2, u1, u2, v1, v2);
			x1 += ax1; x2 += ax2;
			u1 += au1; u2 += au2;
			v1 += av1; v2 += av2;
			w1 += aw1; w2 += aw2;
		}
	}
}

/*****************************************************************************/
#if LE_USE_SIMD == 1 && LE_USE_SSE2 == 1
inline void LeRasterizer::fillFlatTexZC(int y, int x1, int x2, int w1, int w2, int u1, int u2, int v1, int v2)
{
	int d = x2 - x1;
	if (d == 0) return;

	int au = (u2 - u1) / d;
	int av = (v2 - v1) / d;
	int aw = (w2 - w1) / d;
	uint32_t * p = x1 + y * frame.tx + (uint32_t *) frame.data;

	for (int x = x1; x <= x2; x ++) {
		int32_t z = (1 << (24 + 4)) / (w1 >> (12 - 4));
		uint32_t tu = ((u1 * z) >> 24) & texMaskU;
		uint32_t tv = ((v1 * z) >> 24) & texMaskV;

		v4si zv = {0, 0, 0, 0};
		v4si tp;
		tp.v = (V4SI) _mm_load_ss((float *) &texPixels[tu + (tv << texSizeU)]);
		tp.v = (V4SI) _mm_unpacklo_epi8((__m128i)tp.v, (__m128i)zv.v);
		tp.v = (V4SI) _mm_mullo_epi16((__m128i)tp.v, (__m128i)color_4.v);
		tp.v = (V4SI) _mm_srli_epi16((__m128i)tp.v, 8);
		tp.v = (V4SI) _mm_packus_epi16((__m128i)tp.v, (__m128i)zv.v);
		*p++ = _mm_cvtsi128_si32((__m128i)tp.v);

		u1 += au;
		v1 += av;
		w1 += aw;
	}
}

inline void LeRasterizer::fillFlatTexAlphaZC(int y, int x1, int x2, int w1, int w2, int u1, int u2, int v1, int v2)
{
	int d = x2 - x1;
	if (d == 0) return;

	int au = (u2 - u1) / d;
	int av = (v2 - v1) / d;
	int aw = (w2 - w1) / d;

	uint32_t * p = x1 + y * frame.tx + (uint32_t *) frame.data;

	for (int x = x1; x <= x2; x ++) {
		int32_t z = (1 << (24 + 4)) / (w1 >> (12 - 4));
		uint32_t tu = ((u1 * z) >> 24) & texMaskU;
		uint32_t tv = ((v1 * z) >> 24) & texMaskV;

		v4si zv = {0, 0, 0, 0};
		v4si tp;
		v4si fp;
		fp.v = (V4SI) _mm_load_ss((float *) p);
		tp.v = (V4SI) _mm_load_ss((float *) &texPixels[tu + (tv << texSizeU)]);
		fp.v = (V4SI) _mm_unpacklo_epi8((__m128i)fp.v, (__m128i)zv.v);
		tp.v = (V4SI) _mm_unpacklo_epi8((__m128i)tp.v, (__m128i)zv.v);
		v4si ap;
		ap.v = (V4SI) _mm_set1_epi16(256 - (tp.i[1] >> 16));
		tp.v = (V4SI) _mm_mullo_epi16((__m128i)tp.v, (__m128i)color_4.v);
		fp.v = (V4SI) _mm_mullo_epi16((__m128i)fp.v, (__m128i)ap.v);
		tp.v = (V4SI) _mm_adds_epu16((__m128i)tp.v, (__m128i)fp.v);
		tp.v = (V4SI) _mm_srli_epi16((__m128i)tp.v, 8);
		tp.v = (V4SI) _mm_packus_epi16((__m128i)tp.v, (__m128i)zv.v);
		*p++ = _mm_cvtsi128_si32((__m128i)tp.v);

		u1 += au;
		v1 += av;
		w1 += aw;
	}
}

#else

inline void LeRasterizer::fillFlatTexZC(int y, int x1, int x2, int w1, int w2, int u1, int u2, int v1, int v2)
{
	uint8_t * c = (uint8_t *) &color;

	int d = x2 - x1;
	if (d == 0) return;

	int au = (u2 - u1) / d;
	int av = (v2 - v1) / d;
	int aw = (w2 - w1) / d;

	uint8_t * p = (uint8_t *) (x1 + y * frame.tx + (uint32_t *) frame.data);
	for (int x = x1; x <= x2; x++) {
		int32_t z = (1 << (24 + 4)) / (w1 >> (12 - 4));
		uint32_t tu = ((u1 * z) >> 24) & texMaskU;
		uint32_t tv = ((v1 * z) >> 24) & texMaskV;
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

inline void LeRasterizer::fillFlatTexAlphaZC(int y, int x1, int x2, int w1, int w2, int u1, int u2, int v1, int v2)
{
	uint8_t * c = (uint8_t *) &color;

	int d = x2 - x1;
	if (d == 0) return;

	int au = (u2 - u1) / d;
	int av = (v2 - v1) / d;
	int aw = (w2 - w1) / d;

	uint8_t * p = (uint8_t *) (x1 + y * frame.tx + (uint32_t *) frame.data);

	for (int x = x1; x <= x2; x++) {
		int32_t z = (1 << (24 + 4)) / (w1 >> (12 - 4));
		uint32_t tu = ((u1 * z) >> 24) & texMaskU;
		uint32_t tv = ((v1 * z) >> 24) & texMaskV;
		uint8_t * t = (uint8_t *) &texPixels[tu + (tv << texSizeU)];

		uint16_t a = 256 - t[3];
		p[0] = (p[0] * a + t[0] * c[0]) >> 8;
		p[1] = (p[1] * a + t[1] * c[1]) >> 8;
		p[2] = (p[2] * a + t[2] * c[2]) >> 8;
		p += 4;

		u1 += au;
		v1 += av;
		w1 += aw;
	}
}

#endif //LE_USE_SIMD && LE_USE_SSE2

#endif // LE_RENDERER_INTRASTER == 1
