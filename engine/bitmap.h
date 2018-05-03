/**
	\file bitmap.h
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

#ifndef LE_BITMAP_H
#define LE_BITMAP_H

#include "global.h"
#include "color.h"
#include "config.h"

/*****************************************************************************/
/**
	\enum LE_BITMAP_FLAGS
	\brief Bitmap format flags
*/
typedef enum{
	LE_BITMAP_RGB				= 0,	/**< Bitmap in 32bit RGB color format */
	LE_BITMAP_RGBA				= 1,	/**< Bitmap in 32bit RGBA format */
	LE_BITMAP_PREMULTIPLIED		= 2		/**< Bitmap in 32bit RGBA (alpha pre-multiplied) format */
}LE_BITMAP_FLAGS;

/*****************************************************************************/
class LeBmpFont;

/**
	\class LeBitmap
	\brief Contain and manage a RGB or RGBA 32bit bitmap image
*/
class LeBitmap
{
public:
	LeBitmap();
	~LeBitmap();

	void clear(LeColor color);
	void rect(int32_t x, int32_t y, int32_t w, int32_t h, LeColor color);
	void blit(int32_t xDst, int32_t yDst, const LeBitmap * src, int32_t xSrc, int32_t ySrc, int32_t w, int32_t h);
	void alphaBlit(int32_t xDst, int32_t yDst, const LeBitmap * src, int32_t xSrc, int32_t ySrc, int32_t w, int32_t h);
	void alphaScaleBlit(int32_t xDst, int32_t yDst, int32_t wDst, int32_t hDst, const LeBitmap * src, int32_t xSrc, int32_t ySrc, int32_t wSrc, int32_t hSrc);

	void text(int x, int y, const char * text, int length, const LeBmpFont * font);

	void allocate(int tx, int ty);
	void deallocate();

	void preMultiply();
	void makeMipmaps();

	LeHandle context;		/**< Handle available for graphic contexts */
	LeHandle bitmap;		/**< Handle available for bitmap */

	int tx;					/**< Horizontal size of image in pixels */
	int ty;					/**< Vertical size of image in pixels */
	int txP2;				/**< Horizontal size (power of 2) */
	int tyP2;				/**< Vertical size (power of 2) */
	int flags;				/**< Image format and attributes */

	void * data;			/**< Pointer to raw data */
	bool dataAllocated;		/**< Has data been allocated? */

	LeBitmap * mipmaps[LE_BMP_MIPMAPS];		/**< Table of mipmaps (bitmap pointers) */ 
	int mmLevels;							/**< No of mipmaps */
};

/*****************************************************************************/
/**
	\class LeBmpFont
	\brief Contain and manage a monospace bitmap font
*/
class LeBmpFont
{
public:
	LeBmpFont();
	~LeBmpFont();

	LeBitmap * font;	/**< Bitmap with character set */

	int charSizeX;		/**< Horizontal size of character */
	int charSizeY;		/**< Vertical size of character */
	int charBegin;		/**< First character in set (ascii code) */
	int charEnd;		/**< Last character in set (ascii code) */
	int spaceX;			/**< Horizontal space between characters */
	int spaceY;			/**< Vertical space between characters */
};

#endif // LE_BITMAP_H

