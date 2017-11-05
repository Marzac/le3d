/**
	\file global.h
	\brief LightEngine 3D: Global helpers and definitions
	\brief All platforms implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2017
	\version 1.3

	The MIT License (MIT)
	Copyright (c) 2017 Fr�d�ric Meslin

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

#ifndef LE_GLOBAL_H
#define LE_GLOBAL_H

/*****************************************************************************/
/** Standard declarations */
	#include <stdint.h>
	#include <stdlib.h>
	#include <stddef.h>
	typedef unsigned long long		LeHandle;												/** Native OS handle / pointer holder type */

/** Handful macros */
	#define cmax(a,b) 				((a)>(b)?(a):(b))										/** Return the maximum of two values */
	#define cmin(a,b) 				((a)<(b)?(a):(b))										/** Return the minimum of two values */
	#define cabs(a)   				((a)<0.0f?(-(a)):(a))									/** Return the absolute value */
	#define cbound(v, vmin, vmax)	((v)>(vmax)?(vmax):((v)<(vmin)?(vmin):(v)))				/** Limit a value between two bounds (included) */
	#define csgn(a)   				((a)<0.0f?-1.0f:1.0f)									/** Return the sign (+1.0 or -1.0) of a value */
	#define cthr(a, t) 				((a)<(-t)?-1.0f:((a)>(t)?1.0f:0.0f))					/** Compare a value to a threshold and return +1.0 (above), -1.0 (below) or 0.0f (inside) */
	#define cmod(a, m)				(((a) % (m) + (m)) % (m))								/** Perform a non-signed modulo (integer) */
	#define cmodf(a, m)				(fmodf((fmodf((a), (m)) + (m)), (m)))					/** Perform a non-signed modulo (float) */

	#define randf()					(((float) rand() / (float) (RAND_MAX >> 1)) - 1.0f)		/** Compute a float random pseudo number */
	#define d2r						(const float) (M_PI / 180.0f)							/** Mutiply constant to convert degrees in radiants */
	#define r2d						(const float) (180.0f / M_PI)							/** Mutiply constant to convert radiants in degrees */

/** Smoothing / filters macros */
	#define lp1factor(fs, dt)		((const float) (1.0f - (1.0f / (1.0f + (((fs) * (dt)) / (2.0f * M_PI))))))

/*****************************************************************************/
/** Global string & math functions */
	namespace LeGlobal {
		void toLower(char * txt);														/** Convert in place null terminated C string to lower case */
		void toUpper(char * txt);														/** Convert in place null terminated C string to upper case */
		void getFileExt(char * ext, const int maxExt, const char * path);				/** Return a file extension from a path */
		void getFileName(char * name, const int maxName, const char * path);			/** Return a file name from a path */
		int log2i32(int n);																		/** Compute the log2 of a 32bit integer */
	};


#endif
