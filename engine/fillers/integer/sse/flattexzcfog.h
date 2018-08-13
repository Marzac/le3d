/**
	\file flattexzcfog.inc
	\brief LightEngine 3D: Filler (sse/integer) - flat textured z-corrected scans with fog
	\brief Intel x86 CPU (with MMX-SSE-SSE2) implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2018
	\version 1.75

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

inline void LeRasterizer::fillFlatTexZCFog(int y, int x1, int x2, int w1, int w2, int u1, int u2, int v1, int v2)
{
	// TODO: SSE Implementation
	uint8_t * sc = (uint8_t *)&curTriangle->solidColor;
	uint8_t * fc = (uint8_t *)&curTrilist->fog.color;

	short d = x2 - x1;
	if (d == 0) return;

	int au = (u2 - u1) / d;
	int av = (v2 - v1) / d;
	int aw = (w2 - w1) / d;

	const float sw = 0x1p8;
	int32_t znear = (int32_t)(curTrilist->fog.near * sw);
	int32_t zfar = (int32_t)(curTrilist->fog.far * sw);
	int32_t zscale = (1 << 30) / (zfar - znear);

	uint8_t * p = (uint8_t *)(x1 + y * frame.tx + pixels);

	for (int x = x1; x <= x2; x++) {
		int32_t z = (1 << 30) / (w1 >> 8);
		uint32_t tu = (((int64_t)u1 * z) >> 24) & texMaskU;
		uint32_t tv = (((int64_t)v1 * z) >> 24) & texMaskV;
		uint8_t * t = (uint8_t *)&texDiffusePixels[tu + (tv << texSizeU)];

		int r = (t[0] * sc[0]) >> 8;
		int g = (t[1] * sc[1]) >> 8;
		int b = (t[2] * sc[2]) >> 8;

		int32_t ff = ((int64_t)(z - znear) * zscale) >> 15;
		ff = cmmax(0, ff);
		ff = cmmin((1 << 15), ff);
		int fb = (ff * ff) >> (14 + 8);

		p[0] = r + (((fc[0] - r) * fb) >> 8);
		p[1] = g + (((fc[1] - g) * fb) >> 8);
		p[2] = b + (((fc[2] - b) * fb) >> 8);
		p += 4;

		u1 += au;
		v1 += av;
		w1 += aw;
	}
}
