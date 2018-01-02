/**
	\file meshcache.cpp
	\brief LightEngine 3D: Mesh cache management
	\brief All platforms implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2018
	\version 1.4

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
	if (noSlots >= LE_MESHCACHE_SLOTS) {
		printf("meshCache: no free slots!\n");
		return NULL;
	}

	LeObjFile objFile = LeObjFile(path);
	LeMesh * mesh = objFile.loadMesh(0);
	if (!mesh) return NULL;

	if (createSlot(mesh, path) < 0) {
		delete mesh;
		return NULL;
	}
	return mesh;
}

/*****************************************************************************/
void LeMeshCache::loadDirectory(const char * path)
{
	char ext[LE_MAX_FILE_EXTENSION+1];
	char filePath[LE_MAX_FILE_PATH+1];

	DIR * dir = opendir(path);
	struct dirent * dd;

	while ((dd = readdir(dir))) {
		if (dd->d_name[0] == '.') continue;
		LeGlobal::getFileExtention(ext, LE_MAX_FILE_EXTENSION, dd->d_name);

		if (strcmp(ext, "obj") == 0) {
		// Load a Wavefront obj file
			snprintf(filePath, LE_MAX_FILE_PATH, "%s/%s", path, dd->d_name);
			filePath[LE_MAX_FILE_PATH] = '\0';
			printf("meshCache: loading mesh: %s\n", filePath);
			loadOBJ(filePath);
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
		strncpy(slot->path, path, LE_MAX_FILE_PATH);
		slot->path[LE_MAX_FILE_PATH] = '\0';
		LeGlobal::getFileName(slot->name, LE_MAX_FILE_NAME, path);

	// Initialize the flags
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

	noSlots--;
}

/*****************************************************************************/
int LeMeshCache::getFromName(const char * path)
{
	char name[LE_MAX_FILE_NAME+1];
	LeGlobal::getFileName(name, LE_MAX_FILE_NAME, path);

// Search for resource
	for (int i = 0; i < noSlots; i++) {
		Slot * slot = &slots[i];
		if (!slot->mesh) continue;
		if (strcmp(slot->name, name) == 0)
			return i;
	}

// Resource not found
	printf("meshCache: %s not found!\n", path);
	return 0;
}
