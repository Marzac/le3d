/**
	\file meshcache.h
	\brief LightEngine 3D: Mesh cache management
	\brief All platforms implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2017
	\version 1.3

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


#ifndef LE_MESHCACHE_H
#define LE_MESHCACHE_H

#include "global.h"
#include "config.h"

#include "mesh.h"

/*****************************************************************************/
class LeMeshCache
{
public:
	LeMeshCache();
	~LeMeshCache();

	void loadDirectory(const char * path);
	LeMesh * loadOBJ(const char * path);
	int getFromName(const char * name);

public:
	typedef struct{
		LeMesh * mesh;
		char path[LE_MAX_FILE_PATH];
		char name[LE_MAX_FILE_NAME];
		int flags;
	}Slot;

	Slot slots[LE_MESHCACHE_SLOTS];
	int noSlots;

private:
	int createSlot(LeMesh * mesh, const char * path);
	void deleteSlot(int slot);
};

extern LeMeshCache meshCache;

#endif // LE_MESHCACHE_H
