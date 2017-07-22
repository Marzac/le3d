/**
	\file meshcache.cpp
	\brief LightEngine 3D: Mesh cache management
	\brief All platforms implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2017
	\version 1.2

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

#include "meshcache.h"
#include "objfile.h"

#include "global.h"
#include "config.h"

#include <string.h>
#include <dirent.h>

/*****************************************************************************/
LeMeshCache meshCache;

/*****************************************************************************/
LeMeshCache::LeMeshCache() :
	noSlots(0)
{
	memset(slots, 0, sizeof(Slot) * LE_MESHCACHE_SLOTS);

// Create a default mesh
	LeMesh * defMesh = new LeMesh();

// Register the default mesh
	Slot * defSlot = &slots[0];
	defSlot->mesh = defMesh;
	strcpy(defSlot->path, "none");
	strcpy(defSlot->name, "default");
	defSlot->flags = 0;
	noSlots = 1;
}

LeMeshCache::~LeMeshCache()
{
	for (int i = 0; i < LE_MESHCACHE_SLOTS; i++)
		deleteSlot(i);
}


/*****************************************************************************/
LeMesh * LeMeshCache::loadOBJ(const char * path)
{
	if (noSlots >= LE_MESHCACHE_SLOTS) return NULL;

	LeObjFile objFile = LeObjFile(path);
	LeMesh * mesh = objFile.loadMesh(0);
	if (!mesh) return NULL;

	int slot = createSlot(mesh, path);
	if (slot < 0) {
		delete mesh;
		return NULL;
	}
	return mesh;
}

/*****************************************************************************/
void LeMeshCache::loadDirectory(const char * path)
{
	char ext[LE_MAX_FILE_EXTENSION];
	char filePath[LE_MAX_FILE_PATH];

    DIR * dir = opendir(path);
    struct dirent * dd;

    while ((dd = readdir(dir))) {
    	if (dd->d_name[0] == '.') continue;
		LeGlobal::getFileExt(ext, LE_MAX_FILE_EXTENSION, dd->d_name);

	// Load a wavefront obj file
    	if (strcmp(ext, "obj") == 0) {
			snprintf(filePath, LE_MAX_FILE_PATH, "%s/%s", path, dd->d_name);
			filePath[LE_MAX_FILE_PATH-1] = '\0';
			loadOBJ(filePath);
			printf("Load mesh: %s\n", filePath);
    	}

    }
    closedir(dir);
}

/*****************************************************************************/
int LeMeshCache::createSlot(LeMesh * mesh, const char * path)
{
	for (int i = 0; i < LE_MESHCACHE_SLOTS; i++) {
		Slot * slot = &slots[i];
		if (slot->mesh) continue;

	// Register the mesh
		slot->mesh = mesh;
		strncpy(slot->path, path, LE_MAX_FILE_PATH - 1);
		slot->path[LE_MAX_FILE_PATH-1] = '\0';
		LeGlobal::getFileName(slot->name, LE_MAX_FILE_NAME, path);

	// Initialise the extra flags
		slot->flags = 0;

		noSlots++;
		return i;
	}
	return -1;
}

void LeMeshCache::deleteSlot(int index)
{
	Slot * slot = &slots[index];
	if (!slot->mesh) return;

	delete slot->mesh;
	slot->mesh = NULL;

	slot->path[0] = '\0';
	slot->name[0] = '\0';
	slot->flags = 0;
}

/*****************************************************************************/
int LeMeshCache::getFromName(const char * path)
{
	char name[LE_MAX_FILE_NAME];
	LeGlobal::getFileName(name, LE_MAX_FILE_NAME, path);

// Search for resource
	for (int i = 0; i < noSlots; i++) {
		Slot * slot = &slots[i];
		if (!slot->mesh) continue;
		if (strcmp(slot->name, name) == 0)
			return i;
	}

// Resource not found
	return 0;
}

