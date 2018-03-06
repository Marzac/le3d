/**
	\file geometry.cpp
	\brief LightEngine 3D: Vertex / axis / plan / matrix objects
	\brief All platforms implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2018
	\version 1.5

	The MIT License (MIT)
	Copyright (c) 2015-2018 Fr�d�ric Meslin

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

#include "geometry.h"

#if LE_USE_SIMD == 1

/******************************************************************************/
#include <stdlib.h>

#if defined(__unix__) || defined(__unix)

	#ifndef _aligned_malloc
		#define _aligned_malloc(s, a) aligned_alloc(a, s)
	#endif
	#ifndef _aligned_free
		#define _aligned_free(p) free(p)
	#endif

#endif

#ifdef __APPLE__
	void * _aligned_malloc(size_t size, size_t alignment) {
	void *buffer;
	posix_memalign(&buffer, alignment, size);
	return buffer;
}
#define _aligned_free  free
#endif

/******************************************************************************/
/** 128Bit aligned allocators / deallocators */
void * operator new(size_t size)
{
	return _aligned_malloc(size, 16);
}

void * operator new[](size_t size)
{
	return _aligned_malloc(size, 16);
}

void operator delete(void * ptr)
{
	_aligned_free(ptr);
}

void operator delete[](void * ptr)
{
	_aligned_free(ptr);
}

#endif // LE_USE_SIMD
