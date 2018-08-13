/**
	\file bmpcache.h
	\brief LightEngine 3D: Bitmap cache manager
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

#ifndef LE_BMPCACHE_H
#define LE_BMPCACHE_H

#include "global.h"
#include "config.h"

#include "bitmap.h"

/*****************************************************************************/
/**
	\enum LE_BMPCACHE_FLAGS
	\brief Bitmap cache slot flags
*/
typedef enum{
	LE_BMPCACHE_RGB				= 0x00,		/**< Bitmap in 32bit RGB color format */
	LE_BMPCACHE_RGBA				= 0x01,		/**< Bitmap in 32bit RGBA (alpha pre-multiplied) format */
	LE_BMPCACHE_ANIMATION		= 0x02,		/**< Bitmap with animation (uses cursor & extra bitmaps) */
	LE_BMPCACHE_MIPMAPPED		= 0x04,		/**< Bitmap with computed mipmaps */
}LE_BMPCACHE_FLAGS;

/*****************************************************************************/
/**
	\class LeBmpCache
	\brief Cache and inventory all bitmaps loaded
*/
class LeBmpCache
{
public:
	LeBmpCache();
	~LeBmpCache();

	void clean();
	
	void loadDirectory(const char * path);
	LeBitmap * loadBMP(const char * path);

	int getSlotFromName(const char * name);
	LeBitmap * getBitmapFromName(const char * name);

public:
	/**
		\struct Slot
		\brief represents a bitmap cache slot
	**/
	typedef struct{
		LeBitmap * bitmap;					/**< Bitmap associated to the slot */
		char path[LE_MAX_FILE_PATH+1];		/**< Bitmap file full path */
		char name[LE_MAX_FILE_NAME+1];		/**< Bitmap file name */
		int flags;							/**< Bitmap format and attributes */

		LeBitmap * extras;					/**< Extra bitmaps (for animation) */
		int noExtras;						/**< Number of extra bitmaps */
		int cursor;							/**< Cursor for animation playback */ 
	}Slot;

	Slot cacheSlots[LE_BMPCACHE_SLOTS];			/**< Slots in cache */
	int noSlots;							/**< Number of cacheSlots in cache */

private:
	int createSlot(LeBitmap * bitmap, const char * path);
	void deleteSlot(int slot);
};

extern LeBmpCache bmpCache;

#endif // LE_BMPCACHE_H
