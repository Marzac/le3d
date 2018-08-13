/**
	\file flattexzc.inc
	\brief LightEngine 3D: Filler (ammx/float) - flat textured z-corrected scans
	\brief Amiga+Vampire only
	\author Andreas Streichardt (andreas@mop.koeln)
	\twitter @m0ppers
	\website https://mop.koeln/
	\copyright Andreas Streichardt 2018
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
inline void LeRasterizer::fillFlatTexZC(int y, float x1, float x2, float w1, float w2, float u1, float u2, float v1, float v2)
{
	float floatd = x2 - x1;
	if (floatd == 0.0f) return;

	int xb = (int)(x1);
	int xe = (int)(x2 + 1.9999f);
	if (xe > frame.tx) xe = frame.tx;

	uint8_t * p = (uint8_t *) (xb + y * frame.tx + pixels);
	short shortd = xe - xb;

	fill_flat_texel_float(p, shortd, floatd, u1, v1, w1, u2, v2, w2, texMaskU, texMaskV, texSizeU, texDiffusePixels);
}
