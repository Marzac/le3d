/**
	\file bitmap.h
	\brief LightEngine 3D: Bitmap image container / manipulator
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

#ifndef LE_BITMAP_H
#define LE_BITMAP_H

#include "global.h"
#include <stdint.h>

/*****************************************************************************/
typedef enum{
	LE_BMP_DEFAULT = 0,
	LE_BMP_ALPHACHANNEL = 1,
	LE_BMP_PREMULTIPLIED = 2
}BITMAP_FLAGS;

/*****************************************************************************/
class LeBmpFont;
class LeBitmap
{
public:
	LeBitmap();
	~LeBitmap();

	void clear(uint32_t color);
	void rect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
	void blit(int32_t xDst, int32_t yDst, const LeBitmap * src, int32_t xSrc, int32_t ySrc, int32_t w, int32_t h);
	void alphaBlit(int32_t xDst, int32_t yDst, const LeBitmap * src, int32_t xSrc, int32_t ySrc, int32_t w, int32_t h);
	void text(int x, int y, const char * text, int length, const LeBmpFont * font);

	void allocate(int tx, int ty);
	void deallocate();
	void alphaPreMult();

	Handle context;
	Handle bitmap;

	int tx, ty;
	int txP2, tyP2;
	int flags;

	void * data;
	bool allocated;
};

/*****************************************************************************/
class LeBmpFont
{
public:
	LeBmpFont();
	~LeBmpFont();

	LeBitmap * font;

	int charSizeX;
	int charSizeY;
	int charBegin;
	int charEnd;
	int spaceX;
	int spaceY;
};

#endif // LE_BITMAP_H

