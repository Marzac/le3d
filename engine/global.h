/**
	\file global.h
	\brief LightEngine 3D: Global helpers and definitions
	\brief All platforms implementation
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

#ifndef LE_GLOBAL_H
#define LE_GLOBAL_H

/*****************************************************************************/
/** Standard declarations */
	#include <stdint.h>
	#include <stdlib.h>
	#include <stddef.h>
	typedef unsigned long long		LeHandle;												/** Native OS handle / pointer holder type */

/** Handful macros */
	#define cmmax(a,b)				((a)>(b)?(a):(b))										/** Return the maximum of two values */
	#define cmmin(a,b)				((a)<(b)?(a):(b))										/** Return the minimum of two values */
	#define cmabs(a)				((a)<0.0f?(-(a)):(a))									/** Return the absolute value */
	#define cmbound(v, vmin, vmax)	((v)>(vmax)?(vmax):((v)<(vmin)?(vmin):(v)))				/** Limit a value between two bounds (included) */
	#define cmsgn(a)				((a)<0.0f?-1.0f:1.0f)									/** Return the sign (+1.0 or -1.0) of a value */
	#define cmthr(a, t)				((a)<(-t)?-1.0f:((a)>(t)?1.0f:0.0f))					/** Compare a value to a threshold and return +1.0 (above), -1.0 (below) or 0.0f (inside) */
	#define cmmod(a, m)				(((a) % (m) + (m)) % (m))								/** Perform a non-signed modulo (integer) */
	#define cmmodf(a, m)			(fmodf((fmodf((a), (m)) + (m)), (m)))					/** Perform a non-signed modulo (float) */

	#define randf()					(((float) rand() / (float) (RAND_MAX >> 1)) - 1.0f)		/** Compute a float random pseudo number */
	#define d2r						(const float) (M_PI / 180.0f)							/** Mutiply constant to convert degrees in radiants */
	#define r2d						(const float) (180.0f / M_PI)							/** Mutiply constant to convert radiants in degrees */

/*****************************************************************************/
/** Global string & math functions */
	namespace LeGlobal {
		void toLower(char * txt);															/** Convert in place null terminated C string to lower case */
		void toUpper(char * txt);															/** Convert in place null terminated C string to upper case */

		void getFileExtention(char * ext, const int extSize, const char * path);			/** Return a file extension from a path */
		void getFileName(char * name, const int nameSize, const char * path);				/** Return a file name from a path */
		void getFileDirectory(char * dir, int dirSize, const char * path);					/** Return a directory name from a path */

		int log2i32(int n);																	/** Compute the log2 of a 32bit integer */
	};

/*****************************************************************************/
/** Compilers missing - Memory aligned allocation functions */
	#ifdef __MINGW32__
		#include <string.h>
		#include <malloc.h>
		#define _strdup	strdup
	#elif defined(__GNUC__)
		#include <string.h>
		#define _strdup	strdup
		#if defined(__APPLE__)
			extern "C" void * _aligned_malloc(size_t size, size_t alignment);
		#else
			#ifndef _aligned_malloc
				#define _aligned_malloc(s, a) aligned_alloc(a, s)
			#endif
		#endif
		#ifndef _aligned_free
			#define _aligned_free  free
		#endif
	#elif defined(_MSC_VER)
		#include <malloc.h>
		#ifndef alloca
			#define alloca(s) _malloca(s)
		#endif
	#endif

/*****************************************************************************/
/** Brute force 128bit memory alignement (for SIMD maths) */
	#if LE_USE_SIMD == 1
		void * operator new(size_t size) {
			return _aligned_malloc(size, 16);
		}
		void * operator new[](size_t size) {
			return _aligned_malloc(size, 16);
		}
		void operator delete(void * ptr) {
			_aligned_free(ptr);
		}
		void operator delete[](void * ptr) {
			_aligned_free(ptr);
		}
	#endif
	
/*****************************************************************************/
/** Compilers missing - intrinsics and maths functions */
#ifdef _MSC_VER
	#include <intrin.h>
	extern "C" int __builtin_ffs(int x);

#elif defined (__WATCOMC__)
	extern "C" int __builtin_ffs(int x);
	
// Watcom C++ is so outdated that it does not
// come with floating point version of math.h
// functions
	extern "C" float copysignf(float x, float y);
	
	#define sinf(n)		((float)std::sin(n))
	#define asinf(n)	((float)std::asin(n))
	#define cosf(n)		((float)std::cos(n))
	#define acosf(n)	((float)std::acos(n))
	#define tanf(n)		((float)std::tan(n))
	#define atanf(n)	((float)std::atan(n))
	#define floorf(n)	((float)std::floor(n))
	#define ceilf(n)	((float)std::ceil(n))
	#define sqrtf(n)	((float)std::sqrt(n))
	#define fabsf(n)	((float)std::fabs(n))
	#define atan2f(n,m)	((float)std::atan2(n,m))
		
	#define M_PI		3.14159265358979323846

#elif defined (AMIGA)
	extern "C" float copysignf(float x, float y);
#endif

/*****************************************************************************/
/** Endianness conversion macros */
#define FROM_LEU16(x) (x = ((uint8_t *) &(x))[0] + (((uint8_t *) &(x))[1] << 8))
#define FROM_LES16(x) FROM_LEU16(x)
#define FROM_LEU32(x) (x = ((uint8_t *) &x)[0] + (((uint8_t *) &x)[1] << 8) + (((uint8_t *) &x)[2] << 16) + (((uint8_t *) &x)[3] << 24))
#define FROM_LES32(x) FROM_LEU32(x)

#define TO_LEU16(x) (\
	((uint8_t *) &(x))[0] = ((uint16_t) (x)) & 0xFF, \
	((uint8_t *) &(x))[1] = ((uint16_t) (x)) >> 8 \
	)

#define TO_LES16(x) (\
	((uint8_t *) &(x))[0] = ((int16_t) (x)) & 0xFF, \
	((uint8_t *) &(x))[1] = ((int16_t) (x)) >> 8 \
	)
	
#define TO_LEU32(x) (\
	((uint8_t *) &(x))[0] = ((uint32_t) (x)) & 0xFF, \
	((uint8_t *) &(x))[1] = ((uint32_t) (x)) >> 8, \
	((uint8_t *) &(x))[2] = ((uint32_t) (x)) >> 16, \
	((uint8_t *) &(x))[3] = ((uint32_t) (x)) >> 24 \
	)

#define TO_LES32(x) (\
	((uint8_t *) &(x))[0] = ((int32_t) (x)) & 0xFF, \
	((uint8_t *) &(x))[1] = ((int32_t) (x)) >> 8, \
	((uint8_t *) &(x))[2] = ((int32_t) (x)) >> 16, \
	((uint8_t *) &(x))[3] = ((int32_t) (x)) >> 24 \
	)
#endif // LE_GLOBAL_H
