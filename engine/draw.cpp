/**
	\file draw.cpp
	\brief LightEngine 3D: Native OS graphic context
	\brief Windows OS implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2017
	\version 1.2

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

#include "draw.h"

#include "global.h"
#include "config.h"

#define WINVER	0x0600
#include <windef.h>
#include <windows.h>

/*****************************************************************************/
LeDraw::LeDraw(LeHandle context, int width, int height) :
	width(width), height(height),
	backColor(0),
	frontContext(context)
{
	if (!frontContext)
		frontContext = (LeHandle) GetDC(NULL);

	prepareContext(backContext, backBitmap, width, height);
	prepareContext(overlayContext[0], overlayBitmap[0], width, height);
	prepareContext(overlayContext[1], overlayBitmap[1], width, height);
	prepareContext(draftContext, draftBitmap, width, height);

	contexts[LE_LAYER_BACK] = backContext;
	contexts[LE_LAYER_OVERLAY_1] = overlayContext[0];
	contexts[LE_LAYER_OVERLAY_2] = overlayContext[1];
	contexts[LE_LAYER_DRAFT] = draftContext;
}

LeDraw::~LeDraw()
{
	unPrepareContext(backContext, backBitmap);
	unPrepareContext(overlayContext[0], overlayBitmap[0]);
	unPrepareContext(overlayContext[1], overlayBitmap[1]);
	unPrepareContext(draftContext, draftBitmap);
}

/*****************************************************************************/
void LeDraw::prepareContext(LeHandle &context, LeHandle &bitmap, int width, int height)
{
	context = (LeHandle) CreateCompatibleDC((HDC) frontContext);
	bitmap = (LeHandle) CreateCompatibleBitmap((HDC) frontContext, width, height);

	SelectObject((HDC) context, (HANDLE) bitmap);

	SelectObject((HDC) context, GetStockObject(DC_BRUSH));
	SelectObject((HDC) context, GetStockObject(DC_PEN));
	SetTextAlign((HDC) context, TA_BASELINE | TA_LEFT);

	SetStretchBltMode((HDC) context, STRETCH_HALFTONE);
	SetBkMode((HDC) context, TRANSPARENT);
}

void LeDraw::unPrepareContext(LeHandle &context, LeHandle &bitmap)
{
	if (context) DeleteDC((HDC) context);
	if (bitmap) DeleteObject((HANDLE) bitmap);
	context = 0;
	bitmap = 0;
}

/*****************************************************************************/
void LeDraw::setCustomLayer(LeHandle context)
{
	contexts[LE_LAYER_CUSTOM] = context;
}

/*****************************************************************************/
void LeDraw::setBackground(uint32_t color)
{
	backColor = color;
}

void LeDraw::setPixels(void * data)
{
	BITMAPV4HEADER info;
	memset(&info, 0, sizeof(BITMAPV4HEADER));
	info.bV4Size = sizeof(BITMAPV4HEADER);
	GetDIBits((HDC) backContext, (HBITMAP) backBitmap, 0, 1, NULL, (BITMAPINFO *) &info, DIB_RGB_COLORS);
	info.bV4RedMask   = 0x00FF0000;
	info.bV4GreenMask = 0x0000FF00;
	info.bV4BlueMask  = 0x000000FF;
	info.bV4AlphaMask = 0xFF000000;
	info.bV4Height = - info.bV4Height;
	SetDIBits((HDC) backContext, (HBITMAP) backBitmap, 0, -info.bV4Height, data, (BITMAPINFO *) &info, DIB_RGB_COLORS);
}

/*****************************************************************************/
void LeDraw::flip(float zoom)
{
	BLENDFUNCTION blend;
	blend.BlendOp = AC_SRC_OVER;
	blend.BlendFlags = 0;
	blend.AlphaFormat = 0;

	blend.SourceConstantAlpha = 255;
	AlphaBlend((HDC) frontContext, 0, 0, width * zoom, height * zoom, (HDC) backContext, 0, 0, width, height, blend);

	clear(backColor, LE_LAYER_BACK);
}

/*****************************************************************************/
void LeDraw::line(float x1, float y1, float x2, float y2, uint32_t color, LE_DRAW_LAYER layer)
{
	POINT points[2];
	HDC context = (HDC) contexts[layer];
	points[0].x = (long) x1;
	points[0].y = (long) y1;
	points[1].x = (long) x2;
	points[1].y = (long) y2;
	SetDCPenColor((HDC) context, color);
	Polyline((HDC) context, points, 2);
}

void LeDraw::lines(float coords[], int nb, uint32_t color, LE_DRAW_LAYER layer)
{
	POINT points[nb];
	HDC context = (HDC) contexts[layer];
	for (int i = 0; i < nb; i++) {
		points[i].x = (long) coords[0];
		points[i].y = (long) coords[1];
		coords += 2;
	}
	SetDCPenColor((HDC) context, color);
	Polyline((HDC) context, points, nb);
}

/*****************************************************************************/
void LeDraw::rect(float x1, float y1, float x2, float y2, uint32_t color, LE_DRAW_LAYER layer)
{
	RECT rect;
	rect.left   = (long) x1;
	rect.top    = (long) y1;
	rect.right  = (long) x2;
	rect.bottom = (long) y2;

	HDC context = (HDC) contexts[layer];
	SetDCBrushColor(context, color);
	FrameRect(context, &rect, (HBRUSH) GetStockObject(DC_BRUSH));
}

void LeDraw::fill(float x1, float y1, float x2, float y2, uint32_t color, LE_DRAW_LAYER layer)
{
	RECT rect;
	rect.left   = (long) x1;
	rect.top    = (long) y1;
	rect.right  = (long) x2;
	rect.bottom = (long) y2;

	HDC context = (HDC) contexts[layer];
	SetDCBrushColor(context, color);
	FillRect(context, &rect, (HBRUSH) GetStockObject(DC_BRUSH));
}

void LeDraw::clear(uint32_t color, LE_DRAW_LAYER layer)
{
	fill(0.0f, 0.0f, width, height, color, layer);
}

/*****************************************************************************/
void LeDraw::prepareBitmap(LeBitmap * bitmap, bool alpha)
{
// Create information structure
	if (!bitmap) return;
	BITMAPV4HEADER info;
	memset(&info, 0, sizeof(BITMAPV4HEADER));
	info.bV4Size = sizeof(BITMAPV4HEADER);
	info.bV4Width = bitmap->tx;
	info.bV4Height = -bitmap->ty;
	info.bV4Planes = 1;
	info.bV4BitCount = 32;
	info.bV4V4Compression = BI_BITFIELDS;
	info.bV4RedMask   = 0x00FF0000;
	info.bV4GreenMask = 0x0000FF00;
	info.bV4BlueMask  = 0x000000FF;

	if (alpha) info.bV4AlphaMask = 0xFF000000;
	else info.bV4AlphaMask = 0x00000000;

// Create the bitmap resources
	bitmap->context = (LeHandle) CreateCompatibleDC((HDC) frontContext);
	bitmap->bitmap = (LeHandle) CreateCompatibleBitmap((HDC) frontContext, bitmap->tx, bitmap->ty);
	SelectObject((HDC) bitmap->context, (HBITMAP) bitmap->bitmap);
	SetDIBits((HDC) bitmap->context, (HBITMAP) bitmap->bitmap, 0, bitmap->ty, bitmap->data, (BITMAPINFO *) &info, DIB_RGB_COLORS);

}

void LeDraw::unprepareBitmap(LeBitmap * bitmap)
{
	if (bitmap->context) DeleteDC((HDC) bitmap->context);
	bitmap->context = 0;
	if (bitmap->bitmap) DeleteObject((HBITMAP) bitmap->bitmap);
	bitmap->bitmap = 0;
}

/*****************************************************************************/
void LeDraw::layerBlit(LE_DRAW_LAYER layerDst, LE_DRAW_LAYER layerSrc, uint8_t alpha)
{
	BLENDFUNCTION blend;
	blend.BlendOp = AC_SRC_OVER;
	blend.BlendFlags = 0;
	blend.AlphaFormat = 0;

	blend.SourceConstantAlpha = alpha;
	AlphaBlend((HDC) contexts[layerDst], 0, 0, width, height, (HDC) contexts[layerSrc], 0, 0, width, height, blend);
}

/*****************************************************************************/
void LeDraw::bitmapBlit(LE_DRAW_LAYER layerDst, const LeBitmap * bitmap, float x, float y, float sx, float sy, float sw, float sh, float alpha)
{
	if (!bitmap->bitmap) return;

	BLENDFUNCTION blend;
	blend.BlendOp = AC_SRC_OVER;
	blend.BlendFlags = 0;
	blend.AlphaFormat = 0;
	blend.SourceConstantAlpha = cbound((int)(alpha * 255.0f), 0.0f, 255.0f);

	AlphaBlend((HDC) contexts[layerDst], x, y, sw, sh, (HDC) bitmap->context, sx, sy, sw, sh, blend);
}

void LeDraw::bitmapAlphaBlit(LE_DRAW_LAYER layerDst, const LeBitmap * bitmap, float x, float y, float sx, float sy, float sw, float sh, float alpha)
{
	if (!bitmap->bitmap) return;

	BLENDFUNCTION blend;
	blend.BlendOp = AC_SRC_OVER;
	blend.BlendFlags = 0;
	blend.AlphaFormat = AC_SRC_ALPHA;
	blend.SourceConstantAlpha = cbound((int)(alpha * 255.0f), 0.0f, 255.0f);

	AlphaBlend((HDC) contexts[layerDst], x, y, sw, sh, (HDC) bitmap->context, sx, sy, sw, sh, blend);
}

/*****************************************************************************/
void LeDraw::text(float x, float y, const char * text, const LeFont * font, uint32_t color, LE_DRAW_LAYER layer)
{
	HDC context = (HDC) contexts[layer];
	SelectObject(context, (HGDIOBJ) font->font);
	SetTextColor(context, color);
	TextOut(context, (long) x, (long) y, text, strlen(text));
}

/*****************************************************************************/
void LeDraw::prepareFont(LeFont * font, const char * family, int height, int weight)
{
	LOGFONT logFont;
	memset(&logFont, 0, sizeof(LOGFONT));
	logFont.lfHeight = height;
	logFont.lfWeight = weight;
	logFont.lfQuality = NONANTIALIASED_QUALITY;
	strcpy(logFont.lfFaceName, family);
	font->font = (LeHandle) CreateFontIndirect(&logFont);
}

void LeDraw::unprepareFont(LeFont * font)
{
	if (!font->font) return;
	DeleteObject((HGDIOBJ)font->font);
	font->font = 0;
}



