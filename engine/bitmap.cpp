/**
	\file bitmap.cpp
	\brief LightEngine 3D: Bitmap image container & manipulator
	\brief All platforms implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2018
	\version 1.6

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

#include "bitmap.h"

#include "global.h"
#include "config.h"

#include "simd.h"

/*****************************************************************************/
LeBitmap::LeBitmap() :
	context(0), bitmap(0),
	tx(0), ty(0),
	txP2(0), tyP2(0),
	flags(LE_BITMAP_RGB),
	data(NULL), dataAllocated(false),
	mmLevels(0)
{
	for (int l = 0; l < LE_BMP_MIPMAPS; l++)
		mipmaps[l] = NULL;
}

LeBitmap::~LeBitmap()
{
	deallocate();
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
/**
	\fn void LeBitmap::clear(LeColor color)
	\brief Clear the image with the specified color
	\param[in] color RGBA 32bit color
*/
#if LE_USE_SIMD == 1 && LE_USE_SSE2 == 1
void LeBitmap::clear(LeColor color)
{
	int size = tx * ty;
	int b = size >> 2;
	int r = size & 0x3;

	__m128i color_4 = _mm_set_epi32(color, color, color, color);
	__m128i * p_4 = (__m128i *) data;
	for (int t = 0; t < b; t ++)
		*p_4++ = color_4;

	LeColor * p = (LeColor *) p_4;
	if (r == 0) return;
	*p++ = color;
	if (r == 1) return;
	*p++ = color;
	if (r == 2) return;
	*p++ = color;
}
#else

void LeBitmap::clear(LeColor color)
{
	size_t size = tx * ty;
	LeColor * p = (LeColor *) data;
	for (size_t t = 0; t < size; t ++)
		p[t] = color;
}
#endif	// LE_USE_SIMD && LE_USE_SSE2

/*****************************************************************************/
/**
	\fn void LeBitmap::rect(int32_t x, int32_t y, int32_t w, int32_t h, LeColor color)
	\brief Fill a rectangle with the specified color
	\param[in] x horizontal position of the rectangle (pixels)
	\param[in] y vertical position of the rectangle (pixels)
	\param[in] w width of the rectangle (pixels)
	\param[in] h height of the rectangle (pixels)
	\param[in] color RGBA 32bit color
*/
void LeBitmap::rect(int32_t x, int32_t y, int32_t w, int32_t h, LeColor color)
{
	if (x >= tx) return;
	if (y >= ty) return;
	int xe = x + w;
	if (xe <= 0) return;
	int ye = y + h;
	if (ye <= 0) return;

	if (x < 0) x = 0;
	if (y < 0) y = 0;
	if (xe > tx) xe = tx;
	if (ye > ty) ye = ty;

	LeColor * d = (LeColor *) data;
	d += x + y * tx;
	w = xe - x;
	h = ye - y;

	int step = tx - w;
	for (int j = 0; j < h; j++){
		for (int i = 0; i < w; i++)
			*d++ = color;
		d += step;
	}
}

/*****************************************************************************/
/**
	\fn void LeBitmap::blit(int32_t xDst, int32_t yDst, const LeBitmap * src, int32_t xSrc, int32_t ySrc, int32_t w, int32_t h)
	\brief Copy an image portion to the image
	\param[in] xDst horizontal destination position (pixels)
	\param[in] yDst vertical destination position (pixels)
	\param[in] src source bitmap image
	\param[in] xSrc horizontal source position (pixels)
	\param[in] ySrc vertical source position (pixels)
	\param[in] w portion width (pixels)
	\param[in] h portion height (pixels)
*/
void LeBitmap::blit(int32_t xDst, int32_t yDst, const LeBitmap * src, int32_t xSrc, int32_t ySrc, int32_t w, int32_t h)
{
	if (xDst >= tx) return;
	if (yDst >= ty) return;
	int xeDst = xDst + w;
	if (xeDst <= 0) return;
	int yeDst = yDst + h;
	if (yeDst <= 0) return;

	if (xDst < 0) {
		xDst = 0; xSrc -= xDst;
	}
	if (yDst < 0) {
		yDst = 0; ySrc -= yDst;
	}
	if (xeDst > tx) xeDst = tx;
	if (yeDst > ty) yeDst = ty;

	LeColor * d = (LeColor *) data;
	LeColor * s = (LeColor *) src->data;
	d += xDst + yDst * tx;
	s += xSrc + ySrc * src->tx;
	w = xeDst - xDst;
	h = yeDst - yDst;

	int stepDst = tx - w;
	int stepSrc = src->tx - w;

	for (int y = 0; y < h; y++){
		for (int x = 0; x < w; x++)
			*d++ = *s++;
		s += stepSrc;
		d += stepDst;
	}
}

/*****************************************************************************/
/**
	\fn void LeBitmap::alphaBlit(int32_t xDst, int32_t yDst, const LeBitmap * src, int32_t xSrc, int32_t ySrc, int32_t w, int32_t h)
	\brief Copy an image portion to the image (premultiplied alpha format)
	\param[in] xDst horizontal destination position (pixels)
	\param[in] yDst vertical destination position (pixels)
	\param[in] src source bitmap image
	\param[in] xSrc horizontal source position (pixels)
	\param[in] ySrc vertical source position (pixels)
	\param[in] w portion width (pixels)
	\param[in] h portion height (pixels)
*/
#if LE_USE_SIMD == 1 && LE_USE_SSE2 == 1
void LeBitmap::alphaBlit(int32_t xDst, int32_t yDst, const LeBitmap * src, int32_t xSrc, int32_t ySrc, int32_t w, int32_t h)
{
	if (xDst >= tx) return;
	if (yDst >= ty) return;
	int xeDst = xDst + w;
	if (xeDst <= 0) return;
	int yeDst = yDst + h;
	if (yeDst <= 0) return;

	if (xDst < 0) {
		xDst = 0; xSrc -= xDst;
	}
	if (yDst < 0) {
		yDst = 0; ySrc -= yDst;
	}
	if (xeDst > tx) xeDst = tx;
	if (yeDst > ty) yeDst = ty;

	LeColor * d = (LeColor *) data;
	LeColor * s = (LeColor *) src->data;
	d += xDst + yDst * tx;
	s += xSrc + ySrc * src->tx;
	w = xeDst - xDst;
	h = yeDst - yDst;

	int stepDst = tx - w;
	int stepSrc = src->tx - w;

	__m128i zv = _mm_set_epi32(0, 0, 0, 0);
	__m128i sc = _mm_set_epi32(0x01000100, 0x01000100, 0x01000100, 0x01000100);

	for (int y = 0; y < h; y++){
		for (int x = 0; x < w; x++){
			__m128i dp, sp;
			dp = _mm_loadl_epi64((__m128i *) d);
			sp = _mm_loadl_epi64((__m128i *) s++);
			dp = _mm_unpacklo_epi8(zv, dp);
			sp = _mm_unpacklo_epi8(sp, zv);

			__m128i ap;
			ap = _mm_shufflelo_epi16(sp, 0xFF);
			ap = _mm_sub_epi16(sc, ap);
			dp = _mm_mulhi_epu16(dp, ap);
			dp = _mm_adds_epu16(sp, dp);
			dp = _mm_packus_epi16(dp, zv);

			*d++ = _mm_cvtsi128_si32(dp);
		}

		s += stepSrc;
		d += stepDst;
	}
}

#else

void LeBitmap::alphaBlit(int32_t xDst, int32_t yDst, const LeBitmap * src, int32_t xSrc, int32_t ySrc, int32_t w, int32_t h)
{
	if (xDst >= tx) return;
	if (yDst >= ty) return;
	int xeDst = xDst + w;
	if (xeDst <= 0) return;
	int yeDst = yDst + h;
	if (yeDst <= 0) return;

	if (xDst < 0) {
		xDst = 0; xSrc -= xDst;
	}
	if (yDst < 0) {
		yDst = 0; ySrc -= yDst;
	}
	if (xeDst > tx) xeDst = tx;
	if (yeDst > ty) yeDst = ty;

	LeColor * d = (LeColor *) data;
	LeColor * s = (LeColor *) src->data;
	d += xDst + yDst * tx;
	s += xSrc + ySrc * src->tx;
	w = xeDst - xDst;
	h = yeDst - yDst;

	int stepDst = tx - w;
	int stepSrc = src->tx - w;

	for (int y = 0; y < h; y++){
		for (int x = 0; x < w; x++){
			uint8_t * dPix = (uint8_t *) d ++;
			uint8_t * sPix = (uint8_t *) s ++;
			uint16_t a = 256 - sPix[3];
			dPix[0] = ((dPix[0] * a) >> 8) + sPix[0];
			dPix[1] = ((dPix[1] * a) >> 8) + sPix[1];
			dPix[2] = ((dPix[2] * a) >> 8) + sPix[2];
			dPix[3] = ((dPix[3] * a) >> 8) + sPix[3];
		}

		s += stepSrc;
		d += stepDst;
	}
}
#endif // LE_USE_SIMD && LE_USE_SSE2

/*****************************************************************************/
/**
	\fn void LeBitmap::alphaScaleBlit(int32_t xDst, int32_t yDst, int32_t wDst, int32_t hDst, const LeBitmap * src, int32_t xSrc, int32_t ySrc, int32_t wSrc, int32_t hSrc)
	\brief Copy and scale an image portion to the image (premultiplied alpha format)
	\param[in] xDst horizontal destination position (pixels)
	\param[in] yDst vertical destination position (pixels)
	\param[in] wDst destination width (pixels)
	\param[in] hDst destination height (pixels)
	\param[in] src source bitmap image
	\param[in] xSrc horizontal source position (pixels)
	\param[in] ySrc vertical source position (pixels)
	\param[in] wSrc source width (pixels)
	\param[in] hSrc source height (pixels)
*/
#if LE_USE_SIMD == 1
void LeBitmap::alphaScaleBlit(int32_t xDst, int32_t yDst, int32_t wDst, int32_t hDst, const LeBitmap * src, int32_t xSrc, int32_t ySrc, int32_t wSrc, int32_t hSrc)
{
	if (wDst <= 0) return;
	if (hDst <= 0) return;

	int32_t us = (wSrc << 16) / wDst;
	int32_t vs = (hSrc << 16) / hDst;

	if (xDst >= tx) return;
	if (yDst >= ty) return;
	int xeDst = xDst + wDst;
	if (xeDst <= 0) return;
	int yeDst = yDst + hDst;
	if (yeDst <= 0) return;

	int32_t ub = 0;
	int32_t vb = 0;
	if (xDst < 0) {
		ub = ((int64_t)(-xDst * wSrc) << 16) / wDst;
		xDst = 0;
	}
	if (yDst < 0) {
		vb = ((int64_t)(-yDst * hSrc) << 16) / hDst;
		yDst = 0;
	}
	if (xeDst > tx) xeDst = tx;
	if (yeDst > ty) yeDst = ty;

	LeColor * d = (LeColor *) data;
	LeColor * s = (LeColor *) src->data;
	d += xDst + yDst * tx;

	wDst = xeDst - xDst;
	hDst = yeDst - yDst;
	int stepDst = tx - wDst;

	__m128i zv = _mm_set_epi32(0, 0, 0, 0);
	__m128i sc = _mm_set_epi32(0x01000100, 0x01000100, 0x01000100, 0x01000100);

	int32_t u = ub;
	int32_t v = vb;
	for (int y = 0; y < hDst; y++){
		for (int x = 0; x < wDst; x++){
			LeColor * p = &s[(u >> 16) + (v >> 16) * src->tx];
			u += us;

			__m128i dp, sp;
			dp = _mm_loadl_epi64((__m128i *) d);
			sp = _mm_loadl_epi64((__m128i *) p);
			dp = _mm_unpacklo_epi8(zv, dp);
			sp = _mm_unpacklo_epi8(sp, zv);

			__m128i ap;
			ap = _mm_shufflelo_epi16(sp, 0xFF);
			ap = _mm_sub_epi16(sc, ap);
			dp = _mm_mulhi_epu16(dp, ap);
			dp = _mm_adds_epu16(sp, dp);
			dp = _mm_packus_epi16(dp, zv);
			*d++ = _mm_cvtsi128_si32(dp);
		}

		u = ub;
		v += vs;
		d += stepDst;
	}
}

#else

void LeBitmap::alphaScaleBlit(int32_t xDst, int32_t yDst, int32_t wDst, int32_t hDst, const LeBitmap * src, int32_t xSrc, int32_t ySrc, int32_t wSrc, int32_t hSrc)
{
	if (wDst <= 0) return;
	if (hDst <= 0) return;

	int32_t us = (wSrc << 16) / wDst;
	int32_t vs = (hSrc << 16) / hDst;

	if (xDst >= tx) return;
	if (yDst >= ty) return;
	int xeDst = xDst + wDst;
	if (xeDst <= 0) return;
	int yeDst = yDst + hDst;
	if (yeDst <= 0) return;

	int32_t ub = 0;
	int32_t vb = 0;
	if (xDst < 0) {
		ub = ((int64_t)(-xDst * wSrc) << 16) / wDst;
		xDst = 0;
	}
	if (yDst < 0) {
		vb = ((int64_t)(-yDst * hSrc) << 16) / hDst;
		yDst = 0;
	}
	if (xeDst > tx) xeDst = tx;
	if (yeDst > ty) yeDst = ty;

	LeColor * d = (LeColor *) data;
	LeColor * s = (LeColor *) src->data;
	d += xDst + yDst * tx;

	wDst = xeDst - xDst;
	hDst = yeDst - yDst;
	int stepDst = tx - wDst;

	int32_t u = ub;
	int32_t v = vb;
	for (int y = 0; y < hDst; y++){
		for (int x = 0; x < wDst; x++){
			uint8_t * sPix = (uint8_t *) &s[(u >> 16) + (v >> 16) * src->tx];
			uint8_t * dPix = (uint8_t *) d ++;
			u += us;

			uint16_t a = 256 - sPix[3];
			dPix[0] = ((dPix[0] * a) >> 8) + sPix[0];
			dPix[1] = ((dPix[1] * a) >> 8) + sPix[1];
			dPix[2] = ((dPix[2] * a) >> 8) + sPix[2];
			dPix[3] = ((dPix[3] * a) >> 8) + sPix[3];
		}

		u = ub;
		v += vs;
		d += stepDst;
	}
}

#endif

/*****************************************************************************/
/**
	\fn void LeBitmap::text(int x, int y, const char * text, int length, const LeBmpFont * font)
	\brief Write a short text with the specified bitmap character set
	\param[in] x horizontal text position (pixels)
	\param[in] y vertical text position (pixels)
	\param[in] text ascii string
	\param[in] length string length
	\param[in] font monospace bitmap character set
*/
void LeBitmap::text(int x, int y, const char * text, int length, const LeBmpFont * font)
{
	int cx = font->charSizeX;
	int cy = font->charSizeY;
	int lx = 0, ly = 0;

	for (int i = 0; i < length; i++) {
		int c = text[i];
		if (c == '\n') {
			lx = 0; ly ++;
		}else{
			if (c >= font->charBegin && c < font->charEnd) {
				int px = x + lx * font->spaceX;
				int py = y + ly * font->spaceY;
				alphaBlit(px, py, font->font, 0, (c - font->charBegin) * cy, cx, cy);
			}
			lx ++;
		}
	}
}

/*****************************************************************************/
/**
	\fn void LeBitmap::allocate(int tx, int ty)
	\brief Allocate bitmap memory
	\param[in] tx image width (pixels)
	\param[in] ty image height (pixels)
*/
void LeBitmap::allocate(int tx, int ty)
{
	data = new LeColor[tx*ty];
	dataAllocated = true;

	this->tx = tx;
	this->ty = ty;
	txP2 = LeGlobal::log2i32(tx);
	tyP2 = LeGlobal::log2i32(ty);
	flags = LE_BITMAP_RGB;
}

/**
	\fn void LeBitmap::deallocate()
	\brief Deallocate bitmap memory
*/
void LeBitmap::deallocate()
{
	if (dataAllocated && data)
		delete[] (LeColor *) data;
	dataAllocated = false;

	for (int l = 1; l < mmLevels; l++)
		delete mipmaps[l];

	tx = ty = 0;
	txP2 = tyP2 = 0;
	flags = 0;
}

/*****************************************************************************/
/**
	\fn void LeBitmap::preMultiply()
	\brief Alpha pre-multiply an RGBA bitmap
*/
void LeBitmap::preMultiply()
{
	size_t noPixels = tx * ty;

	uint8_t * c = (uint8_t *) data;
	for (size_t i = 0; i < noPixels; i++) {
		c[0] = (c[0] * c[3]) >> 8;
		c[1] = (c[1] * c[3]) >> 8;
		c[2] = (c[2] * c[3]) >> 8;
		c += 4;
	}

	flags |= LE_BITMAP_RGBA | LE_BITMAP_PREMULTIPLIED;
	for (int l = 1; l < mmLevels; l++)
		mipmaps[l]->preMultiply();
}

/**
	\fn void LeBitmap::makeMipmaps()
	\brief Generate mipmaps from the bitmap
*/
void LeBitmap::makeMipmaps()
{
	if ((tx & (tx - 1)) != 0 || (ty & (ty - 1)) != 0)
		return;

	mmLevels = 0;
	mipmaps[mmLevels++] = this;

	int mtx = tx / 2;
	int mty = ty / 2;

	LeColor * o = (LeColor *) data;

	for (int l = 0; l < LE_BMP_MIPMAPS; l++) {
		if (mtx < 4 || mty < 4) break;

		LeBitmap * bmp = new LeBitmap();
		bmp->allocate(mtx, mty);

		LeColor * p = (LeColor *) bmp->data;

		for (int y = 0; y < mty; y++) {
			for (int x = 0; x < mtx; x++) {
				LeColor * s1 = o;
				LeColor * s2 = o+1;
				LeColor * s3 = o + mtx*2*4;
				LeColor * s4 = o + mtx*2*4+1;
				int r = (s1->r + s2->r + s3->r + s4->r) >> 2;
				int g = (s1->g + s2->g + s3->g + s4->g) >> 2;
				int b = (s1->b + s2->b + s3->b + s4->b) >> 2;
				int a = (s1->a + s2->a + s3->a + s4->a) >> 2;
				* p++ = LeColor(r, g, b, a);
				o += 2;
			}
			o += mtx * 2;
		}

		mtx /= 2;
		mty /= 2;
		o = (LeColor *) bmp->data;

		mipmaps[mmLevels++] = bmp;
	}
}
