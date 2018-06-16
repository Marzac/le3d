/**
	\file ammx.h
	\brief LightEngine 3D: Amiga ammx function declaration
	\brief Amiga+Vampire only
    \brief header files for asm functions (ammx.s)
	\author Andreas Streichardt (andreas@mop.koeln)
	\twitter @m0ppers
	\website https://mop.koeln/
	\copyright Andreas Streichardt 2018
	\version 1.5

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

#ifndef AMMX_H
#define AMMX_H

#include "engine/color.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void prepare_fill_texel(LeColor* c);

void fill_flat_texel(
    uint8_t* p, short d, int u1, int v1, int w1, int au, int av, int aw,
    uint32_t texMaskU, uint32_t texMaskV, uint32_t texSizeU, LeColor* texPixels,
	uint8_t* c
);
void set_ammx_pixels(void* data, size_t bytes, LeColor color);

#ifdef __cplusplus
}
#endif

#endif