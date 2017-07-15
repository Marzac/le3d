/**
	\file global.cpp
	\brief LightEngine 3D: Global helpers and definitions
	\brief All platforms implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2017
	\version 1.1

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

#include "global.h"
#include "config.h"

#include <strings.h>
#include <malloc.h>

/*****************************************************************************/
/** Hack: force 16 byte aligned dynamic allocations */
#if LE_FORCE_16B_ALIGN == 1
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
#endif // LE_FORCE_16B_ALIGN

/*****************************************************************************/
void LeGlobal::toLower(char * txt)
{
	int len = strlen(txt);
	for (int i = 0; i < len; i++){
		char c = txt[i];
		if (c >= 'A' && c <= 'Z') c += 'a' - 'A';
		txt[i] = c;
	}
}

void LeGlobal::toUpper(char * txt)
{
	int len = strlen(txt);
	for (int i = 0; i < len; i++){
		char c = txt[i];
		if (c >= 'a' && c <= 'z') c += 'A' - 'a';
		txt[i] = c;
	}
}

/*****************************************************************************/
void LeGlobal::getFileExt(char * ext, const int maxExt, const char * path)
{
	ext[0] = '\0';
	int len = strlen(path);
	while(len) {
		if (path[--len] == '.') {
			strncpy(ext, &path[len+1], maxExt-1);
			ext[maxExt-1] = '\0';
			toLower(ext);
			return;
		}
	}
}

void LeGlobal::getFileName(char * name, const int maxName, const char * path)
{
	name[0] = '\0';
	int len = strlen(path);
	while(len) {
		char c = path[--len];
		if (c == '\\' || c == '/') {
			strncpy(name, &path[len+1], maxName-1);
			name[maxName-1] = '\0';
			return;
		}
	}
	strncpy(name, path, maxName-1);
}
