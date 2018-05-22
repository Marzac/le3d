/**
	\file bmpfile.cpp
	\brief LightEngine 3D: Windows bitmap file importer / exporter
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

#include "bmpfile.h"

#include "global.h"
#include "config.h"

#include <stdlib.h>
#include <string.h>

/*****************************************************************************/
#pragma pack(push, 1)
typedef struct {
	uint16_t bfType;
	uint32_t bfSize;
	int16_t  bfReserved1;
	int16_t  bfReserved2;
	uint32_t bfOffBits;
} BMPFILEHEADER;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
	uint32_t biSize;
	int32_t  biWidth;
	int32_t  biHeight;
	uint16_t biPlanes;
	uint16_t biBitCount;
	uint32_t biCompression;
	uint32_t biSizeImage;
	int32_t  biXPelsPerMeter;
	int32_t  biYPelsPerMeter;
	uint32_t biClrUsed;
	uint32_t biClrImportant;
} BMPINFOHEADER;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
	uint32_t CSType;
	uint32_t Endpoints[9];
	uint32_t GammaRed;
	uint32_t GammaGreen;
	uint32_t GammaBlue;
} BMPCOLORSPACE;

#pragma pack(push, 1)
typedef struct {
	uint32_t mR;
	uint32_t mG;
	uint32_t mB;
	uint32_t mA;
} BMPCOLORMASK;
#pragma pack(pop)

/*****************************************************************************/
#define BI_RGB				0
#define BI_BITFIELDS		3
#define BI_ALPHABITFIELDS	6

/*****************************************************************************/
LeBmpFile::LeBmpFile(const char * filename) :
	path(NULL)
{
	if (filename) path = _strdup(filename);
}

LeBmpFile::~LeBmpFile()
{
	if (path) free(path);
}

/*****************************************************************************/
/**
	\fn LeBitmap * LeBmpFile::load()
	\brief Load bitmap data from the file
	\return pointer to a new loaded bitmap, else NULL (error)
*/
LeBitmap * LeBmpFile::load()
{
	FILE * file = fopen(path, "rb");
	if (!file) {
		printf("bmpFile: file not found %s!\n", path);
		return NULL;
	}

	LeBitmap * bitmap = new LeBitmap();
	if (!bitmap) {
		fclose(file);
		return NULL;
	}

	readBitmap(file, bitmap);
	fclose(file);
	return bitmap;
}

/**
	\fn void LeBmpFile::save(LeBitmap * bitmap)
	\brief Save bitmap data into the file
	\param[in] bitmap pointer a valid bitmap
*/
void LeBmpFile::save(const LeBitmap * bitmap)
{
	FILE * file = fopen(path, "wb");
	if (!file) {
		printf("bmpFile: file cannot be opened %s!\n", path);
		return;
	}

	writeBitmap(file, bitmap);
	fclose(file);
}

/*****************************************************************************/
int LeBmpFile::readBitmap(FILE * file, LeBitmap * bitmap)
{
	BMPFILEHEADER fileHeader;
	BMPINFOHEADER infoHeader;
	BMPCOLORMASK  mask = {
		0x00FF0000,
		0x0000FF00,
		0x000000FF,
		0xFF000000
	};

// Read the headers
	fread(&fileHeader, sizeof(BMPFILEHEADER), 1, file);
	fread(&infoHeader, sizeof(BMPINFOHEADER), 1, file);
	
// Format the data (little-endianness)
	FROM_LEU32(fileHeader.bfSize);
	FROM_LEU32(fileHeader.bfOffBits);
	FROM_LEU32(infoHeader.biSize);
	FROM_LES32(infoHeader.biWidth);
	FROM_LES32(infoHeader.biHeight);
	FROM_LEU16(infoHeader.biPlanes);
	FROM_LEU16(infoHeader.biBitCount);
	FROM_LEU32(infoHeader.biCompression);
	FROM_LEU32(infoHeader.biSizeImage);
	FROM_LES32(infoHeader.biXPelsPerMeter);
	FROM_LES32(infoHeader.biYPelsPerMeter);
	FROM_LEU32(infoHeader.biClrUsed);
	FROM_LEU32(infoHeader.biClrImportant);
	
// Check the headers
	if (strncmp((char *) &fileHeader.bfType, "BM", 2)){
		printf("bmpFile: file not a bitmap!\n");
		return 0;
	}
	if (infoHeader.biBitCount != 24 && infoHeader.biBitCount != 32) {
		printf("bmpFile: only 24bit or 32bit bitmaps are supported %d!\n", infoHeader.biBitCount);
		return 0;
	}
	if (infoHeader.biCompression != BI_RGB && infoHeader.biCompression != BI_BITFIELDS){
		printf("bmpFile: only uncompressed formats are supported!\n");
		return 0;
	}

// Load the bitmasks
	int shiftA = 24;
	int shiftR = 16;
	int shiftG = 8;
	int shiftB = 0;
	if (infoHeader.biCompression == BI_BITFIELDS) {
		fread(&mask, sizeof(BMPCOLORMASK), 1, file);
		FROM_LEU32(mask.mR);
		FROM_LEU32(mask.mG);
		FROM_LEU32(mask.mB);
		FROM_LEU32(mask.mA);
		shiftR = __builtin_ffs(mask.mR) - 1;
		shiftG = __builtin_ffs(mask.mG) - 1;
		shiftB = __builtin_ffs(mask.mB) - 1;
		shiftA = __builtin_ffs(mask.mA) - 1;
	}

// Retrieve bitmap size
	bitmap->tx = infoHeader.biWidth;
	bitmap->ty = infoHeader.biHeight;
	bitmap->txP2 = LeGlobal::log2i32(bitmap->tx);
	bitmap->tyP2 = LeGlobal::log2i32(bitmap->ty);

	int upsidedown = 1;
	if (bitmap->ty < 0) {
		bitmap->ty = -bitmap->ty;
		upsidedown = 0;
	}

// Set bitmap flags
	bitmap->flags = LE_BITMAP_RGB;
	if (infoHeader.biBitCount == 32)
		bitmap->flags |= LE_BITMAP_RGBA;

// Allocate bitmap memory
	int srcScan;
	srcScan = bitmap->tx * (infoHeader.biBitCount >> 3);
	srcScan = (srcScan + 0x3) & ~0x3;

	bitmap->data = new LeColor[bitmap->tx * bitmap->ty];
	bitmap->dataAllocated = true;

// Load bitmap data
	uint8_t * buffer = (uint8_t *) alloca(srcScan);
	uint8_t * data = (uint8_t *) bitmap->data;

	int dstScan = bitmap->tx * sizeof(uint32_t);
	fseek(file, fileHeader.bfOffBits, SEEK_SET);
	if (upsidedown)
		data += dstScan * (bitmap->ty - 1);

	if (infoHeader.biBitCount == 32) {
	// Parse a 32 bits image
		for (int y = 0; y < bitmap->ty; y ++) {
			fread(buffer, srcScan, 1, file);
			uint32_t * d = (uint32_t *) data;
			uint32_t * s = (uint32_t *) buffer;
			for (int i = 0; i < bitmap->tx; i ++) {
				uint32_t c = *s++;
				uint32_t a, r, g, b;
				a = (c & mask.mA) >> shiftA;
				r = (c & mask.mR) >> shiftR;
				g = (c & mask.mG) >> shiftG;
				b = (c & mask.mB) >> shiftB;
				*d++ = (a << 24) | (r << 16) | (g << 8) | b;
			}
			if (upsidedown) data -= dstScan;
			else data += dstScan;
		}
	}else{
	// Parse a 24 bits image
		for (int y = 0; y < bitmap->ty; y ++) {
			fread(buffer, srcScan, 1, file);
			LeColor * d = (LeColor *) data;
			uint8_t	 * s = buffer;
			for (int i = 0; i < bitmap->tx; i ++) {
				d->b = * s++;
				d->g = * s++;
				d->r = * s++;
				d++;
			}
			if (upsidedown) data -= dstScan;
			else data += dstScan;
		}
	}

	return 1;
}

/*****************************************************************************/
#define HEAD_LEN sizeof(BMPFILEHEADER) + sizeof(BMPINFOHEADER) + sizeof(BMPCOLORMASK)
int LeBmpFile::writeBitmap(FILE * file, const LeBitmap * bitmap)
{
// Prepare the headers
	size_t size = bitmap->tx * bitmap->ty * sizeof(uint32_t);
	BMPFILEHEADER fileHeader;
	fileHeader.bfType = 0x4D42;
	fileHeader.bfSize = HEAD_LEN + size;
	fileHeader.bfReserved1 = 0;
	fileHeader.bfReserved2 = 0;
	fileHeader.bfOffBits = HEAD_LEN;

	BMPINFOHEADER infoHeader;
	infoHeader.biSize = sizeof(BMPINFOHEADER) + sizeof(BMPCOLORMASK);
	infoHeader.biWidth = bitmap->tx;
	infoHeader.biHeight = bitmap->ty;
	infoHeader.biPlanes = 1;
	infoHeader.biBitCount = 32;
	infoHeader.biCompression = BI_BITFIELDS;
	infoHeader.biSizeImage = 0;
	infoHeader.biXPelsPerMeter = 96;
	infoHeader.biYPelsPerMeter = 96;
	infoHeader.biClrUsed = 0;
	infoHeader.biClrImportant = 0;

	BMPCOLORMASK mask = {
		0x00FF0000,
		0x0000FF00,
		0x000000FF,
		0xFF000000
	};
	
// Format the data (little-endianness)
	TO_LEU16(fileHeader.bfType);
	TO_LEU32(fileHeader.bfSize);
	TO_LEU32(fileHeader.bfOffBits);
	TO_LEU32(infoHeader.biSize);
	TO_LES32(infoHeader.biWidth);
	TO_LES32(infoHeader.biHeight);
	TO_LEU16(infoHeader.biPlanes);
	TO_LEU16(infoHeader.biBitCount);
	TO_LEU32(infoHeader.biCompression);
	TO_LEU32(infoHeader.biSizeImage);
	TO_LES32(infoHeader.biXPelsPerMeter);
	TO_LES32(infoHeader.biYPelsPerMeter);
	TO_LEU32(infoHeader.biClrUsed);
	TO_LEU32(infoHeader.biClrImportant);
	//TO_LEU32(mask[0]);
	//TO_LEU32(mask[1]);
	//TO_LEU32(mask[2]);
	//TO_LEU32(mask[3]);
	
// Write the headers
	fwrite(&fileHeader, sizeof(BMPFILEHEADER), 1, file);
	fwrite(&infoHeader, sizeof(BMPINFOHEADER), 1, file);
	fwrite(&mask, sizeof(BMPCOLORMASK), 1, file);

// Save the picture
	uint32_t scan = bitmap->tx * sizeof(uint32_t);
	uint8_t * data = (uint8_t *) bitmap->data;
	for (int y = 0; y < bitmap->ty; y ++) {
		fwrite(data, scan, 1, file);
		data += scan;
	}
	return 0;
}
