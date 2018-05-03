/**
	\file color.h
	\brief LightEngine 3D: Color implementation
	\brief All platforms implementation
	\author Andreas Streichardt <andreas@mop.koeln>
	\twitter @m0ppers
	\website https://mop.koeln
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

#ifndef LE_COLOR_H
#define LE_COLOR_H

#include <stdint.h>

struct LeColor {
    LeColor(): r(0), g(0), b(0), a(0) {}
    LeColor(uint32_t color) :
		r(color >> 24),
		g(color >> 16),
		b(color >> 8),
		a(color) {
    }

	LeColor& operator=(LeColor color) {
        r = color.r;
        g = color.g;
        b = color.b;
        a = color.a;
        return *this;
    }

	operator int() {
		return (r << 24) | (g << 16) | (b << 8) << a;
	}

    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};


#endif