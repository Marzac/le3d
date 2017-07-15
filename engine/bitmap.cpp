/**
	\file bitmap.cpp
	\brief LightEngine 3D: Bitmap image container / manipulator
	\brief All platforms implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2017
	\version 1.1

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

#include "bitmap.h"

#include "global.h"
#include "config.h"

#include "emmintrin.h"
#include "mmintrin.h"

/*****************************************************************************/
LeBitmap::LeBitmap() :
	context(0), bitmap(0),
	tx(0), ty(0),
	txP2(0), tyP2(0),
	flags(LE_BMP_DEFAULT),
	data(NULL), allocated(false)
{
}

LeBitmap::~LeBitmap()
{
	if (allocated) deallocate();
}


/*****************************************************************************/
LeBmpFont::LeBmpFont() :
	font(NULL),
	charSizeX(20), charSizeY(30),
	charBegin(32), charEnd(128),
	spaceX(16), spaceY(charSizeY)
{
}

LeBmpFont::~LeBmpFont()
{
}

/*****************************************************************************/
void LeBitmap::clear(uint32_t color)
{
	size_t size = tx * ty;
	uint32_t * p = (uint32_t *) data;
	for (size_t t = 0; t < size; t += 4) {
		p[t]   = color;
		p[t+1] = color;
		p[t+2] = color;
		p[t+3] = color;
	}
}

void LeBitmap::rect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color)
{
#if LE_BMP_CHECKCOORDS == 1
	if (x < 0 || y < 0 || x + w > tx || y + h > ty) return;
#endif
	uint32_t * d = (uint32_t *) data;
	d += x + y * tx;
	int step = tx - w;
	for (int j = 0; j < h; j++){
		for (int i = 0; i < w; i++)
			*d++ = color;
		d += step;
	}
}

/*****************************************************************************/
void LeBitmap::text(int x, int y, const char * text, int length, const LeBmpFont * font)
{
#if LE_BMP_CHECKCOORDS == 1
	if (x < 0 || y < 0) return;
#endif
	int cx = font->charSizeX;
	int cy = font->charSizeY;
	int lx = 0;
	int ly = 0;
	for (int i = 0; i < length; i++) {
		int c = text[i];
		if (c == '\n') {
			lx = 0;
			ly ++;
		}else{
			if (c >= font->charBegin && c < font->charEnd)
				alphaBlit(x + lx * font->spaceX, y + ly * font->spaceY, font->font, 0, (c - font->charBegin) * cy, cx, cy);
			lx ++;
		}
	}
}

/*****************************************************************************/
void LeBitmap::blit(int32_t xDst, int32_t yDst, const LeBitmap * src, int32_t xSrc, int32_t ySrc, int32_t w, int32_t h)
{
#if LE_BMP_CHECKCOORDS == 1
	if (xSrc < 0 || ySrc < 0 || xSrc + w > src->tx || ySrc + h > src->ty) return;
	if (xDst < 0 || yDst < 0 || xDst + w > tx || yDst + h > ty) return;
#endif
	uint32_t * d = (uint32_t *) data;
	uint32_t * s = (uint32_t *) src->data;

	d += xDst + yDst * tx;
	s += xSrc + ySrc * src->tx;

	int stepDst = tx - w;
	int stepSrc = src->tx - w;

	for (int y = 0; y < h; y++){
		for (int x = 0; x < w; x++)
			*d++ = *s++;
		s += stepSrc;
		d += stepDst;
	}
}

#if LE_USE_MMX == 1
void LeBitmap::alphaBlit(int32_t xDst, int32_t yDst, const LeBitmap * src, int32_t xSrc, int32_t ySrc, int32_t w, int32_t h)
{
#if LE_BMP_CHECKCOORDS == 1
	if (xSrc < 0 || ySrc < 0 || xSrc + w > src->tx || ySrc + h > src->ty) return;
	if (xDst < 0 || yDst < 0 || xDst + w > tx || yDst + h > ty) return;
#endif
	uint32_t * d = (uint32_t *) data;
	uint32_t * s = (uint32_t *) src->data;

	d += xDst + yDst * tx;
	s += xSrc + ySrc * src->tx;

	int stepDst = tx - w;
	int stepSrc = src->tx - w;

	__m64 zv = _mm_setzero_si64();
	__m64 bv = _mm_set1_pi16(0xFF);

	for (int y = 0; y < h; y++){
		for (int x = 0; x < w; x++){

			uint32_t dPix = *d;
			uint32_t sPix = *s++;
			uint32_t a = (sPix >> 24) & 0xFF;

			__m64 a1v = _mm_set1_pi16(a);
			__m64 a2v = _mm_xor_si64(a1v, bv);

			__m64 sPixCom = (__m64) (uint64_t) sPix;
			__m64 dPixCom = (__m64) (uint64_t) dPix;

			sPixCom = _mm_unpacklo_pi8(sPixCom, zv);
			dPixCom = _mm_unpacklo_pi8(dPixCom, zv);

			dPixCom = _mm_mullo_pi16(dPixCom, a2v);
			dPixCom = _mm_srli_pi16(dPixCom, 8);
			dPixCom = _mm_adds_pu16(sPixCom, dPixCom);
			dPixCom = _mm_packs_pu16(dPixCom, zv);

			*d++ = (uint64_t) dPixCom;
		}

		s += stepSrc;
		d += stepDst;
	}

	_mm_empty();
}

#else
void LeBitmap::alphaBlit(int32_t xDst, int32_t yDst, const LeBitmap * src, int32_t xSrc, int32_t ySrc, int32_t w, int32_t h)
{
#if LE_BMP_CHECKCOORDS == 1
	if (xSrc < 0 || ySrc < 0 || xSrc + w > src->tx || ySrc + h > src->ty) return;
	if (xDst < 0 || yDst < 0 || xDst + w > tx || yDst + h > ty) return;
#endif
	uint32_t * d = (uint32_t *) data;
	uint32_t * s = (uint32_t *) src->data;

	d += xDst + yDst * tx;
	s += xSrc + ySrc * src->tx;

	int stepDst = tx - w;
	int stepSrc = src->tx - w;

	for (int y = 0; y < h; y++){
		for (int x = 0; x < w; x++){
			uint8_t * dPix = (uint8_t *) d ++;
			uint8_t * sPix = (uint8_t *) s ++;
			uint8_t a = sPix[3] ^ 0xFF;
			dPix[0] = ((dPix[0] * a) >> 8) + sPix[0];
			dPix[1] = ((dPix[1] * a) >> 8) + sPix[1];
			dPix[2] = ((dPix[2] * a) >> 8) + sPix[2];
			dPix[3] = ((dPix[3] * a) >> 8) + sPix[3];
		}

		s += stepSrc;
		d += stepDst;
	}
}
#endif

/*****************************************************************************/
void LeBitmap::allocate(int tx, int ty)
{
	this->tx = tx;
	this->ty = ty;
	txP2 = tyP2 = 0;
	flags = LE_BMP_DEFAULT;
	data = new uint32_t[tx*ty];
	allocated = true;
}

void LeBitmap::deallocate()
{
	if (data) delete[] (uint32_t *)data;
	tx = ty = 0;
	txP2 = tyP2 = 0;
	flags = 0;
	allocated = false;
}

/*****************************************************************************/
void LeBitmap::alphaPreMult()
{
	size_t noPixels = tx * ty;
	uint8_t * c = (uint8_t *) data;
	for (size_t i = 0; i < noPixels; i++) {
		c[0] = (c[0] * c[3]) >> 8;
		c[1] = (c[1] * c[3]) >> 8;
		c[2] = (c[2] * c[3]) >> 8;
		c += 4;
	}
	flags |= LE_BMP_PREMULTIPLIED;
}
