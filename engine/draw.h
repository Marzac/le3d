/**
	\file draw.h
	\brief LightEngine 3D: Native OS graphic context
	\brief Windows implementation
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

#ifndef LE_DRAW_H
#define LE_DRAW_H

#include "global.h"
#include "bitmap.h"

/*****************************************************************************/
typedef enum{
	LE_LAYER_BACK = 0,		/** Back ground layer */
	LE_LAYER_OVERLAY_1,		/** First overlay */
	LE_LAYER_OVERLAY_2,		/** Second overlay */
	LE_LAYER_DRAFT,			/** Draft layer */
	LE_LAYER_CUSTOM,		/** User defined layer */
	LE_LAYER_NB,
} LE_DRAW_LAYER;

/*****************************************************************************/
struct LeFont
{
	Handle font;

	LeFont()
	{
		font = 0;
	}
};

/*****************************************************************************/
class LeDraw
{
public:
	LeDraw(Handle context, int width, int heigth);
	~LeDraw();

	void prepareContext(Handle &context, Handle &bitmap, int width, int height);
	void unPrepareContext(Handle &context, Handle &bitmap);
	void setCustomLayer(Handle context);

	void flip(float zoom = 1.0f);

	void setBackground(uint32_t color);
	void setPixels(void * data);

	void line(float x1, float y1, float x2, float y2, uint32_t color, LE_DRAW_LAYER layer = LE_LAYER_BACK);
	void lines(float coords[], int nb, uint32_t color, LE_DRAW_LAYER layer = LE_LAYER_BACK);
	void rect(float x1, float y1, float x2, float y2, uint32_t color, LE_DRAW_LAYER layer = LE_LAYER_BACK);
	void fill(float x1, float y1, float x2, float y2, uint32_t color, LE_DRAW_LAYER layer = LE_LAYER_BACK);
	void clear(uint32_t color, LE_DRAW_LAYER layer);

	void prepareBitmap(LeBitmap * bitmap, bool alpha);
	void unprepareBitmap(LeBitmap * bitmap);

	void prepareFont(LeFont * font, const char * family, int height, int weight);
	void unprepareFont(LeFont * font);

	void layerBlit(LE_DRAW_LAYER layerDst, LE_DRAW_LAYER layerSrc, uint8_t alpha);
	void bitmapBlit(LE_DRAW_LAYER layerDst, const LeBitmap * bitmap, float x, float y, float sx, float sy, float sw, float sh, float alpha);
	void bitmapAlphaBlit(LE_DRAW_LAYER layerDst, const LeBitmap * bitmap, float x, float y, float sx, float sy, float sw, float sh, float alpha);

	void text(float x, float y, const char * text, const LeFont * font, uint32_t color, LE_DRAW_LAYER layer = LE_LAYER_BACK);
	void textBitmap(float x, float y, const char * text, uint32_t color);

	int getWidth() {return width;}
	int getHeight() {return height;}

private:
	int width;
	int height;
	uint32_t backColor;

	Handle frontContext;
	Handle backContext;
	Handle backBitmap;

	Handle overlayContext[2];
	Handle overlayBitmap[2];

	Handle draftContext;
	Handle draftBitmap;

	Handle contexts[LE_LAYER_NB];

};

#endif	//LE_DRAW_H
