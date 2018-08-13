/**
	\file flattexalphazc.inc
	\brief LightEngine 3D: Filler (sse/float) - flat textured & alpha blended z-corrected scans
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

inline void LeRasterizer::fillFlatTexAlphaZC(int y, float x1, float x2, float w1, float w2, float u1, float u2, float v1, float v2)
{
	float d = x2 - x1;
	if (d == 0.0f) return;

	float id = 1.0f / d;
	float au = (u2 - u1) * id;
	float av = (v2 - v1) * id;
	float aw = (w2 - w1) * id;

	__m128 u_4 = _mm_set_ps(u1 + 3.0f * au, u1 + 2.0f * au, u1 + au, u1);
	__m128 v_4 = _mm_set_ps(v1 + 3.0f * av, v1 + 2.0f * av, v1 + av, v1);
	__m128 w_4 = _mm_set_ps(w1 + 3.0f * aw, w1 + 2.0f * aw, w1 + aw, w1);

	__m128 au_4 = _mm_set1_ps(au * 4.0f);
	__m128 av_4 = _mm_set1_ps(av * 4.0f);
	__m128 aw_4 = _mm_set1_ps(aw * 4.0f);
	
	int xb = (int)(x1);
	int xe = (int)(x2 + 1.9999f);
	if (xe > frame.tx) xe = frame.tx;

	LeColor * p = xb + ((int) y) * frame.tx + pixels;
	int b = (xe - xb) >> 2;
	int r = (xe - xb) & 0x3;

	__m128i sc = _mm_set1_epi32(0x01000100);
	for (int x = 0; x < b; x ++) {
		__m128 z_4 = _mm_rcp_ps(w_4);

		__m128 mu_4, mv_4;
		mu_4 = _mm_mul_ps(u_4, z_4);
		mv_4 = _mm_mul_ps(v_4, z_4);
		mv_4 = _mm_mul_ps(mv_4, texScale_4);

		__m128i mui_4, mvi_4;
		mui_4 = _mm_cvtps_epi32(mu_4);
		mvi_4 = _mm_cvtps_epi32(mv_4);
		mui_4 = _mm_and_si128(mui_4, texMaskU_4);
		mvi_4 = _mm_and_si128(mvi_4, texMaskV_4);
		mui_4 = _mm_add_epi32(mui_4, mvi_4);

		__m128i zv = _mm_set1_epi32(0);
		__m128i tp, tq, fp, t1, t2;
		__m128i ap, apl, aph;

		fp = _mm_loadl_epi64((__m128i *) p);
		tp = _mm_loadl_epi64((__m128i *) &texDiffusePixels[((uint32_t *)&mui_4)[0]]);
		tq = _mm_loadl_epi64((__m128i *) &texDiffusePixels[((uint32_t *)&mui_4)[1]]);
		t1 = _mm_unpacklo_epi32(tp, tq);
		fp = _mm_unpacklo_epi8(fp, zv);
		t1 = _mm_unpacklo_epi8(t1, zv);

		apl = _mm_shufflelo_epi16(t1, 0xFF);
		aph = _mm_shufflehi_epi16(t1, 0xFF);
		ap = _mm_castpd_si128(_mm_move_sd(_mm_castsi128_pd(aph), _mm_castsi128_pd(apl)));
		ap = _mm_sub_epi16(sc, ap);

		t1 = _mm_mullo_epi16(t1, color_4);
		fp = _mm_mullo_epi16(fp, ap);
		t1 = _mm_adds_epu16(t1, fp);
		t1 = _mm_srli_epi16(t1, 8);

		fp = _mm_loadl_epi64((__m128i *) (p+2));
		tp = _mm_loadl_epi64((__m128i *) &texDiffusePixels[((uint32_t *)&mui_4)[2]]);
		tq = _mm_loadl_epi64((__m128i *) &texDiffusePixels[((uint32_t *)&mui_4)[3]]);
		t2 = _mm_unpacklo_epi32(tp, tq);
		fp = _mm_unpacklo_epi8(fp, zv);
		t2 = _mm_unpacklo_epi8(t2, zv);

		apl = _mm_shufflelo_epi16(t2, 0xFF);
		aph = _mm_shufflehi_epi16(t2, 0xFF);
		ap = _mm_castpd_si128(_mm_move_sd(_mm_castsi128_pd(aph), _mm_castsi128_pd(apl)));
		ap = _mm_sub_epi16(sc, ap);

		t2 = _mm_mullo_epi16(t2, color_4);
		fp = _mm_mullo_epi16(fp, ap);
		t2 = _mm_adds_epu16(t2, fp);
		t2 = _mm_srli_epi16(t2, 8);
		tp = _mm_packus_epi16(t1, t2);
		_mm_storeu_si128((__m128i *) p,  tp);
		p += 4;

		w_4 = _mm_add_ps(w_4, aw_4);
		u_4 = _mm_add_ps(u_4, au_4);
		v_4 = _mm_add_ps(v_4, av_4);
	}

	if (r == 0) return;

	__m128 z_4 = _mm_rcp_ps(w_4);
	
	__m128 mu_4, mv_4;
	mu_4 = _mm_mul_ps(u_4, z_4);
	mv_4 = _mm_mul_ps(v_4, z_4);
	mv_4 = _mm_mul_ps(mv_4, texScale_4);

	__m128i mui_4, mvi_4;
	mui_4 = _mm_cvtps_epi32(mu_4);
	mvi_4 = _mm_cvtps_epi32(mv_4);
	mui_4 = _mm_and_si128( mui_4, texMaskU_4);
	mvi_4 = _mm_and_si128( mvi_4, texMaskV_4);
	mui_4 = _mm_add_epi32(mui_4, mvi_4);

	__m128i zv = _mm_set1_epi32(0);
	__m128i tp, fp;
	fp = _mm_loadl_epi64((__m128i *) p);
	tp = _mm_loadl_epi64((__m128i *) &texDiffusePixels[((uint32_t *)&mui_4)[0]]);
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

	if (r == 1) return;
	fp = _mm_loadl_epi64((__m128i *) p);
	tp = _mm_loadl_epi64((__m128i *) &texDiffusePixels[((uint32_t *)&mui_4)[1]]);
	fp = _mm_unpacklo_epi8(fp, zv);
	tp = _mm_unpacklo_epi8(tp, zv);

	ap = _mm_shufflelo_epi16(tp, 0xFF);
	ap = _mm_sub_epi16(sc, ap);

	tp = _mm_mullo_epi16(tp, color_4);
	fp = _mm_mullo_epi16(fp, ap);
	tp = _mm_adds_epu16(tp, fp);
	tp = _mm_srli_epi16(tp, 8);

	tp = _mm_packus_epi16(tp, zv);
	*p++ = _mm_cvtsi128_si32(tp);

	if (r == 2) return;
	fp = _mm_loadl_epi64((__m128i *) p);
	tp = _mm_loadl_epi64((__m128i *) &texDiffusePixels[((uint32_t *)&mui_4)[2]]);
	fp = _mm_unpacklo_epi8(fp, zv);
	tp = _mm_unpacklo_epi8(tp, zv);

	ap = _mm_shufflelo_epi16(tp, 0xFF);
	ap = _mm_sub_epi16(sc, ap);

	tp = _mm_mullo_epi16(tp, color_4);
	fp = _mm_mullo_epi16(fp, ap);
	tp = _mm_adds_epu16(tp, fp);
	tp = _mm_srli_epi16(tp, 8);

	tp = _mm_packus_epi16(tp, zv);
	*p++ = _mm_cvtsi128_si32(tp);
}
