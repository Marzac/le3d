/**
	\file flattexalphazc.inc
	\brief LightEngine 3D: Filler (ref/float) - flat textured & alpha blended z-corrected scans
	\brief All platforms implementation
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

inline void LeRasterizer::fillFlatTexAlphaZC(int y, float x1, float x2, float w1, float w2, float u1, float u2, float v1, float v2)
{
	uint8_t * sc = (uint8_t *) &curTriangle->solidColor;

	float d = x2 - x1;
	if (d == 0.0f) return;

	float au = (u2 - u1) / d;
	float av = (v2 - v1) / d;
	float aw = (w2 - w1) / d;

	int xb = (int)(x1);
	int xe = (int)(x2 + 1.9999f);
	if (xe > frame.tx) xe = frame.tx;

	uint8_t * p = (uint8_t *) (xb + ((int) y) * frame.tx + pixels);

	for (int x = xb; x < xe; x++) {

		float z = 1.0f / w1;
		uint32_t tu = ((int32_t) (u1 * z)) & texMaskU;
		uint32_t tv = ((int32_t) (v1 * z)) & texMaskV;
		uint8_t * t = (uint8_t *) &texDiffusePixels[tu + (tv << texSizeU)];

		int a = 256 - t[3];
		p[0] = (p[0] * a + t[0] * sc[0]) >> 8;
		p[1] = (p[1] * a + t[1] * sc[1]) >> 8;
		p[2] = (p[2] * a + t[2] * sc[2]) >> 8;
		p += 4;

		u1 += au;
		v1 += av;
		w1 += aw;
	}
}