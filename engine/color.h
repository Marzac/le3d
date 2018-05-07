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

class LeColor {
public:
    LeColor(): r(0), g(0), b(0), a(0) {}
	LeColor(uint8_t const& _r, uint8_t const& _g, uint8_t const& _b, uint8_t const& _a): r(_r), g(_g), b(_b), a(_a) {} 
	LeColor& operator=(LeColor const& color) {
        r = color.r;
        g = color.g;
        b = color.b;
        a = color.a;
        return *this;
    }
	explicit LeColor(int const& color): r(color >> 16), g(color >> 8), b(color), a(color >> 24) {};
	LeColor& operator=(int const& color) {
		r = color >> 16;
		g = color >> 8;
		b = color;
		a = color >> 24;

		return *this;
	}
    operator int() {
	    return (r << 16) | (g << 8) | b | (a << 24);
	}

public:
    static LeColor rgba(uint32_t const& rgba) {
		return LeColor(rgba >> 16, rgba >> 8, rgba, rgba >> 24);
    }
    
	static LeColor rgb(uint32_t const& rgb) {
		return LeColor(rgb >> 16, rgb >> 8, rgb, 1);
    }

public:
    uint8_t b;
    uint8_t g;
    uint8_t r;
    uint8_t a;
};


#endif