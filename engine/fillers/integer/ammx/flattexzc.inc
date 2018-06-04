/**
	\file flattexzc.inc
	\brief LightEngine 3D: Filler (ammx/integer) - flat textured z-corrected scans
	\brief Amiga+Vampire only
	\author Andreas Streichardt (andreas@mop.koeln)
	\twitter @m0ppers
	\website https://mop.koeln/
	\copyright Andreas Streichardt 2018
	\version 1.7

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

inline void LeRasterizer::fillFlatTexZC(int y, int x1, int x2, int w1, int w2, int u1, int u2, int v1, int v2)
{
	uint8_t * sc = (uint8_t *) &curTriangle->solidColor;

	short d = x2 - x1;
	if (d == 0) return;

	int au = (u2 - u1) / d;
	int av = (v2 - v1) / d;
	int aw = (w2 - w1) / d;

	uint8_t * p = (uint8_t *) (x1 + y * frame.tx + pixels);
	fill_flat_texel(p, d, u1, v1, w1, au, av, aw, texMaskU, texMaskV, texSizeU, (uint32_t *) texPixels, sc);
}
