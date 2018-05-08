/**
	\file global.cpp
	\brief LightEngine 3D: Global helpers and definitions
	\brief All platforms implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2018
	\version 1.6

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

#include "global.h"
#include "config.h"

#include <stdlib.h>
#include <string.h>

/*****************************************************************************/

#if __APPLE__
void * _aligned_malloc(size_t size, size_t alignment) {
	void * buffer;
	posix_memalign(&buffer, alignment, size);
	return buffer;
}
#endif

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
void LeGlobal::getFileExtention(char * ext, const int extSize, const char * path)
{
	ext[0] = '\0';
	int len = strlen(path);
	while(len) {
		if (path[--len] == '.') {
			strncpy(ext, &path[len+1], extSize-1);
			ext[extSize-1] = '\0';
			toLower(ext);
			return;
		}
	}
}

void LeGlobal::getFileName(char * name, const int nameSize, const char * path)
{
	name[0] = '\0';
	int len = strlen(path);
	while(len) {
		char c = path[--len];
		if (c == '\\' || c == '/') {
			strncpy(name, &path[len+1], nameSize-1);
			name[nameSize-1] = '\0';
			return;
		}
	}
	strncpy(name, path, nameSize-1);
}

void LeGlobal::getFileDirectory(char * dir, int dirSize, const char * path)
{
	dir[0] = '\0';
	int len = strlen(path);
	while(len) {
		char c = path[--len];
		if (c == '/' || c == '\\') {
			if (len > dirSize - 2) return;
			strncpy(dir, path, len+1);
			dir[len+1] = 0;
			return;
		}
	}
}

/*****************************************************************************/
int LeGlobal::log2i32(int n)
{
	int r = 0;
	if (n >= 0x10000) {n >>= 16; r |= 0x10;}
	if (n >= 0x100) {n >>= 8; r |= 0x8;}
	if (n >= 0x10) {n >>= 4; r |= 0x4;}
	if (n >= 0x4) {n >>= 2;	r |= 0x2;}
	if (n >= 0x2) {r |= 0x1;}
	return r;
}

/*****************************************************************************/
#ifdef _MSC_VER
int __builtin_ffs(int x) {
	unsigned long index;
	_BitScanForward(&index, x);
	return (int) index + 1;
}
#endif