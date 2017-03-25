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

	scansAlloc = new ScanStr[height + 2];
	memset(scansAlloc, 0, sizeof(ScanStr) * (height + 2));
	scans = &scansAlloc[1];
}

LeRasterizer::~LeRasterizer()
{
	frame.data = ((uint32_t *) frame.data) - frame.tx;
	frame.ty += 2;
	frame.deallocate();
	if (scansAlloc) delete scansAlloc;
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
void cv4(const float * in, int32_t * out)
{
	out[0] = (int) (in[0] * 65536.0f);
	out[1] = (int) (in[1] * 65536.0f);
	out[2] = (int) (in[2] * 65536.0f);
	out[3] = (int) (in[3] * 65536.0f);
}

void cv4s(const float * in, int32_t * out, float s)
{
	const float k = 65536.0f * s;
	out[0]  = (int) (in[0] * k);
	out[1]  = (int) (in[1] * k);
	out[2]  = (int) (in[2] * k);
	out[3]  = (int) (in[3] * k);
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

	// Convert coordinates to integers
		cv4(tri->xs, xs);
		cv4(tri->ys, ys);
		cv4s(tri->us, us, 1 << bmp->txP2);
		cv4s(tri->vs, vs, 1 << bmp->tyP2);

	// Sort triangle vertexes
		int vt = 0;
		int vb = 0;
		int vm = 0;
		if (ys[0] < ys[1]) {
			if (ys[0] < ys[2]) {
				vt = 0;
				if (ys[1] < ys[2]) {vm = 1; vb = 2;}
				else{vm = 2; vb = 1;}
			}else{
				vt = 2;	vm = 0;	vb = 1;
			}
		}else{
			if (ys[1] < ys[2]) {
				vt = 1;
				if (ys[0] < ys[2]){vm = 0; vb = 2;}
				else{vm = 2; vb = 0;}
			}else{
				vt = 2; vm = 1; vb = 0;
			}
		}

	// Compute triangle orientation
		float d = (tri->xs[vb] - tri->xs[vt]) * (tri->ys[vm] - tri->ys[vt])
		        - (tri->xs[vm] - tri->xs[vt]) * (tri->ys[vb] - tri->ys[vt]);

	// Compute scan start & end
		if (d < 0.0f) {
			calcScans(vt, vb, 0);
			calcScans(vt, vm, 1);
			calcScans(vm, vb, 1);
		}else{
			calcScans(vt, vb, 1);
			calcScans(vt, vm, 0);
			calcScans(vm, vb, 0);
		}

	// Fill scan lines
		int st = ys[vt] >> 16;
		int sb = ys[vb] >> 16;
		if (slot->flags & LE_BMP_ALPHACHANNEL)
			fillScansFlatTexAlpha(st, sb);
		else fillScansFlatTex(st, sb);
	}
}

/*****************************************************************************/
void LeRasterizer::calcScans(int vt, int vb, int side)
{
	int s1 = ys[vt] >> 16;
	int s2 = (ys[vb] + 0xFFFF) >> 16;
	int d = s2 - s1;
	if (d == 0) return;

	int32_t	ac = ((xs[vb] - xs[vt]) << 4) / d;
	int32_t au = (us[vb] - us[vt]) / d;
	int32_t av = (vs[vb] - vs[vt]) / d;

	int32_t x = xs[vt] << 4;
	int32_t u = us[vt];
	int32_t v = vs[vt];
	for (int s = s1; s < s2; s++) {
		scans[s].x[side] = x>>4;
		scans[s].u[side] = u;
		scans[s].v[side] = v;
		x += ac;
		u += au;
		v += av;
	}
}

/*****************************************************************************/
#if LE_USE_MMX == 1
void LeRasterizer::fillScansFlatTex(int t, int b)
{
	__m64 zv = _mm_setzero_si64();
	__m64 cv = (__m64) (uint64_t) color;
	cv = _mm_unpacklo_pi8(cv, zv);

	uint32_t * fd = t * frame.tx + (uint32_t *) frame.data;
	for (int s = t; s < b; s++) {
		int x1 = scans[s].x[0] >> 16;
		int x2 = (scans[s].x[1]) >> 16;
		int d = x2 - x1;
		if (d != 0) {
			int32_t au = (scans[s].u[1] - scans[s].u[0]) / d;
			int32_t av = (scans[s].v[1] - scans[s].v[0]) / d;
			int32_t u = scans[s].u[0];
			int32_t v = scans[s].v[0];

			for (int x = x1; x < x2; x++) {
				uint32_t tu = (u & texMaskU) >> 16;
				uint32_t tv = (v & texMaskV) >> (16 - texSizeU);
				uint32_t t = texPixels[tu + tv];

				__m64 k =(__m64) (uint64_t) t;
				k = _mm_unpacklo_pi8(k, zv);
				k = _mm_mullo_pi16(k, cv);
				k = _mm_srli_pi16(k, 8);
				k = _mm_packs_pu16(k, zv);

				u += au;
				v += av;

				fd[x] = (int64_t) k;
			}
		}
		fd += frame.tx;
	}
	_mm_empty();
}
#else
void LeRasterizer::fillScansFlatTex(int t, int b)
{
	uint8_t * c = (uint8_t *) &color;
	uint32_t * fd = t * frame.tx + (uint32_t *) frame.data;
	for (int s = t; s < b; s++) {
		int x1 = scans[s].x[0] >> 16;
		int x2 = (scans[s].x[1]) >> 16;
		int d = x2 - x1;
		if (d != 0) {
			int32_t au = (scans[s].u[1] - scans[s].u[0]) / d;
			int32_t av = (scans[s].v[1] - scans[s].v[0]) / d;
			int32_t u = scans[s].u[0];
			int32_t v = scans[s].v[0];

			for (int x = x1; x < x2; x++) {
				uint32_t tu = (u & texMaskU) >> 16;
				uint32_t tv = (v & texMaskV) >> (16 - texSizeU);
				uint8_t * t = (uint8_t *) &texPixels[tu + tv];
				uint8_t * p = (uint8_t *) &fd[x];
				p[0] = (t[0] * c[0]) >> 8;
				p[1] = (t[1] * c[1]) >> 8;
				p[2] = (t[2] * c[2]) >> 8;

				u += au;
				v += av;
			}
		}
		fd += frame.tx;
	}
}
#endif

/*****************************************************************************/
#if LE_USE_MMX == 1
void LeRasterizer::fillScansFlatTexAlpha(int t, int b)
{
	__m64 zv = _mm_setzero_si64();
	__m64 cv = (__m64) (uint64_t) color;
	cv = _mm_unpacklo_pi8(cv, zv);

	uint32_t * fd = t * frame.tx + (uint32_t *) frame.data;
	for (int s = t; s < b; s++) {
		int x1 = scans[s].x[0] >> 16;
		int x2 = (scans[s].x[1]) >> 16;
		int d = x2 - x1;
		if (d != 0) {
			int32_t au = (scans[s].u[1] - scans[s].u[0]) / d;
			int32_t av = (scans[s].v[1] - scans[s].v[0]) / d;

			int32_t u = scans[s].u[0];
			int32_t v = scans[s].v[0];

			for (int x = x1; x < x2; x++) {
				uint32_t tu = (u & texMaskU) >> 16;
				uint32_t tv = (v & texMaskV) >> (16 - texSizeU);
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

				u += au;
				v += av;

				fd[x] = (int64_t) k;
			}
		}
		fd += frame.tx;
	}

	_mm_empty();
}

#else
void LeRasterizer::fillScansFlatTexAlpha(int t, int b)
{
	uint8_t * c = (uint8_t *) &color;
	uint32_t * fd = t * frame.tx + (uint32_t *) frame.data;
	for (int s = t; s < b; s++) {
		int x1 = scans[s].x[0] >> 16;
		int x2 = (scans[s].x[1]) >> 16;
		int d = x2 - x1;
		if (d != 0) {
			int32_t au = (scans[s].u[1] - scans[s].u[0]) / d;
			int32_t av = (scans[s].v[1] - scans[s].v[0]) / d;

			int32_t u = scans[s].u[0];
			int32_t v = scans[s].v[0];

			for (int x = x1; x < x2; x++) {
				uint32_t tu = (u & texMaskU) >> 16;
				uint32_t tv = (v & texMaskV) >> (16 - texSizeU);
				uint8_t * t = (uint8_t *) &texPixels[tu + tv];
				uint8_t * p = (uint8_t *) &fd[x];

				uint8_t a = t[3] ^ 0xFF;
				p[0] = (p[0] * a + t[0] * c[0]) >> 8;
				p[1] = (p[1] * a + t[1] * c[1]) >> 8;
				p[2] = (p[2] * a + t[2] * c[2]) >> 8;

				u += au;
				v += av;
			}
		}
		fd += frame.tx;
	}
}
#endif
