/**
	\file global.h
	\brief LightEngine 3D: Global helpers and definitions
	\brief All platforms implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2017
	\version 1.0

	The MIT License (MIT)
	Copyright (c) 2017 Frédéric Meslin

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
/** Light engine configuration */
	#define LE_MAX_FILE_EXTENSION		8						/** Maximum file extension string length */
	#define LE_MAX_FILE_NAME			128						/** Maximum file name string length */
	#define LE_MAX_FILE_PATH			256						/** Maximum file path string length */

/** Internal caches */
	#define LE_BMPCACHE_SLOTS			1024					/** Maximum number of bitmaps in cache */
	#define LE_MESHCACHE_SLOTS			1024					/** Maximum number of meshes in cache */

/** Bitmaps */
	#define LE_BMP_CHECKCOORDS 			1						/** Check coordinated on bitmap operations */

/** Wavefront objects */
	#define LE_OBJ_MAX_NAME				256						/** Wavefront object maximum name string length */
	#define LE_OBJ_MAX_LINE				1024					/** Wavefront object maximum file line length */
	#define LE_OBJ_MAX_PATH				256						/** Wavefront object maximum path string length */

/** LeRenderer configuration */
	#define LE_RENDERER_FRONT			-0.5f					/** Front clipping plane */
	#define LE_RENDERER_BACK			-100000.0f				/** Back clipping plane */
	#define LE_RENDERER_FOV				65.0f					/** Default field of view */
	#define LE_TRILIST_MAX				10000					/** Maximum number of triangles in display list */
	#define LE_VERLIST_MAX				(3 * LE_TRILIST_MAX)	/** Maximum number of vertexes in transformation buffer */

/** Performance optimizations */
	#define LE_USE_SIMD					1						/** Use SIMD (SSE / Neon) vectors */
	#define LE_USE_MMX					0						/** Use MMX acceleration for integer math */
	#define LE_FORCE_16B_ALIGN			1						/** Force 16bit alignment (needed by SSE) */

/*****************************************************************************/
/** Types and standard declarations */
	#include <stdlib.h>
	#include <stdint.h>
	#include <stddef.h>

	typedef unsigned long long	Handle;		/** Native OS object handle type */
	typedef unsigned long  		ulong;
	typedef unsigned short 		ushort;
	typedef unsigned char  		uchar;

/** Handful macros */
	#define cmax(a,b) 				((a)>(b)?(a):(b))
	#define cmin(a,b) 				((a)<(b)?(a):(b))
	#define cabs(a)   				((a)<0.0f?(-(a)):(a))
	#define cbound(v, vmin, vmax)	((v)>(vmax)?(vmax):((v)<(vmin)?(vmin):(v)))
	#define csgn(a)   				((a)<0.0f?-1.0f:1.0f)
	#define cthr(a, t) 				((a)<(-t)?-1.0f:((a)>(t)?1.0f:0.0f))

	#define randf()					(((float) rand() / (float) (RAND_MAX >> 1)) - 1.0f)
	#define d2r						(const float) (M_PI / 180.0f)
	#define r2d						(const float) (180.0f / M_PI)

/*****************************************************************************/
/** Global functions */
	namespace LeGlobal{
		void toLower(char * txt);
		void toUpper(char * txt);
		void getFileExt(char * ext, const int maxExt, const char * path);
		void getFileName(char * name, const int maxName, const char * path);
	};

#endif
