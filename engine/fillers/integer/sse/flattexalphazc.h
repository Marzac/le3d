/**
	\file flattexalphazc.inc
	\brief LightEngine 3D: Filler (sse/integer) - flat textured & alpha blended z-corrected scans
	\brief Intel x86 CPU (with MMX-SSE-SSE2) implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2018
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

inline void LeRasterizer::fillFlatTexAlphaZC(int y, int x1, int x2, int w1, int w2, int u1, int u2, int v1, int v2)
{
	int d = x2 - x1;
	if (d == 0) return;

	int au = (u2 - u1) / d;
	int av = (v2 - v1) / d;
	int aw = (w2 - w1) / d;

	if (++x2 > frame.tx) x2 = frame.tx;
	LeColor * p = x1 + y * frame.tx + pixels;

	__m128i sc = _mm_set1_epi32(0x01000100);
	for (int x = x1; x < x2; x ++) {
		int32_t z = (1 << 30) / (w1 >> 8);
		uint32_t tu = (((int64_t) u1 * z) >> 24) & texMaskU;
		uint32_t tv = (((int64_t) v1 * z) >> 24) & texMaskV;

		__m128i zv = _mm_set1_epi32(0);
		__m128i tp, fp;
		fp = _mm_loadl_epi64((__m128i *) p);
		tp = _mm_loadl_epi64((__m128i *) &texDiffusePixels[tu + (tv << texSizeU)]);
		fp = _mm_unpacklo_epi8(fp, zv);
		tp = _mm_unpacklo_epi8(tp, zv);

		__m128i ap;
		ap = _mm_shufflelo_epi16(tp, 0xFF);
		ap = _mm_sub_epi16(sc, ap);

		tp = _mm_mullo_epi16(tp, color_4);
		fp = _mm_mullo_epi16(fp, ap);
		tp = _mm_adds_epu16(tp, fp);
		tp = _mm_srli_epi16(tp, 8);
		tp = _mm_packus_epi16(tp, zv);
		*p++ = _mm_cvtsi128_si32(tp);

		u1 += au;
		v1 += av;
		w1 += aw;
	}
}
