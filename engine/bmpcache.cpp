/**
	\file bmpcache.cpp
	\brief LightEngine 3D: Bitmap cache management
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

#include "bmpcache.h"
#include "bmpfile.h"

#include "global.h"
#include "config.h"

#include <string.h>
#include <dirent.h>

/*****************************************************************************/
LeBmpCache bmpCache;

/*****************************************************************************/
LeBmpCache::LeBmpCache() :
	noSlots(0)
{
	memset(slots, 0, sizeof(Slot) * LE_BMPCACHE_SLOTS);

// Create a default bitmap
	LeBitmap * defBitmap = new LeBitmap();
	defBitmap->allocate(32, 32);

// Register the default bitmap
	Slot * defSlot = &slots[0];
	defSlot->bitmap = defBitmap;
	strcpy(defSlot->path, "none");
	strcpy(defSlot->name, "default");
	defSlot->extras = NULL;
	defSlot->noExtras = 0;
	defSlot->cursor = 0;
	defSlot->flags = 0;
	noSlots = 1;
}

LeBmpCache::~LeBmpCache()
{
	for (int i = 0; i < LE_BMPCACHE_SLOTS; i++)
		deleteSlot(i);
}


/*****************************************************************************/
LeBitmap * LeBmpCache::loadBMP(const char * path)
{
	if (noSlots >= LE_BMPCACHE_SLOTS) return NULL;

	LeBmpFile bmpFile = LeBmpFile(path);
	LeBitmap * bitmap = bmpFile.load();
	if (!bitmap) return NULL;

	int slot = createSlot(bitmap, path);
	if (slot < 0) {
		delete bitmap;
		return NULL;
	}

	if (bitmap->flags & LE_BMP_ALPHACHANNEL) {
		bitmap->alphaPreMult();
		slots[slot].flags |= LE_BMPCACHE_ALPHACHANNEL;
	}
	return bitmap;
}

/*****************************************************************************/
void LeBmpCache::loadDirectory(const char * path)
{
	char ext[LE_MAX_FILE_EXTENSION];
	char filePath[LE_MAX_FILE_PATH];

    DIR * dir = opendir(path);
    struct dirent * dd;

    while ((dd = readdir(dir))) {
    	if (dd->d_name[0] == '.') continue;
		LeGlobal::getFileExt(ext, LE_MAX_FILE_EXTENSION, dd->d_name);

	// Load a windows bitmap file
    	if (strcmp(ext, "bmp") == 0) {
			snprintf(filePath, LE_MAX_FILE_PATH, "%s/%s", path, dd->d_name);
			filePath[LE_MAX_FILE_PATH-1] = '\0';
			loadBMP(filePath);
			printf("Load bitmap: %s\n", filePath);
    	}

    }
    closedir(dir);
}

/*****************************************************************************/
int LeBmpCache::createSlot(LeBitmap * bitmap, const char * path)
{
	for (int i = 0; i < LE_BMPCACHE_SLOTS; i++) {
		Slot * slot = &slots[i];
		if (slot->bitmap) continue;

	// Register the bitmap
		slot->bitmap = bitmap;
		strncpy(slot->path, path, LE_MAX_FILE_PATH - 1);
		slot->path[LE_MAX_FILE_PATH-1] = '\0';
		LeGlobal::getFileName(slot->name, LE_MAX_FILE_NAME, path);

	// Initialise the extra flags
		slot->extras = NULL;
		slot->noExtras = 0;
		slot->cursor = 0;
		slot->flags = 0;

		noSlots++;
		return i;
	}
	return -1;
}

void LeBmpCache::deleteSlot(int index)
{
	Slot * slot = &slots[index];
	if (!slot->bitmap) return;

	delete slot->bitmap;
	slot->bitmap = NULL;

	slot->path[0] = '\0';
	slot->name[0] = '\0';
	slot->cursor = 0;
	slot->flags = 0;
}

/*****************************************************************************/
int LeBmpCache::getFromName(const char * path)
{
	char name[LE_MAX_FILE_NAME];
	LeGlobal::getFileName(name, LE_MAX_FILE_NAME, path);

// Search for resource
	for (int i = 0; i < noSlots; i++) {
		Slot * slot = &slots[i];
		if (!slot->bitmap) continue;
		if (strcmp(slot->name, name) == 0)
			return i;
	}

// Resource not found
	return 0;
}

