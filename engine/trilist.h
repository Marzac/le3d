/**
	\file trilist.h
	\brief LightEngine 3D: Triangle lists
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

#ifndef LE_TRILIST_H
#define LE_TRILIST_H

#include "global.h"
#include "config.h"

#include "color.h"

/*****************************************************************************/
	/**
	\struct LeFog
	\brief Represent a quadratic fog
*/
struct LeFog {
	LeFog() :
		color(),
		near(-LE_RENDERER_NEAR_DEFAULT),
		far(-100.0f)
	{}

	LeColor color;					/**< fog basic color */
	float near;						/**< fog start distance */
	float far;						/**< fog end distance */
};

/*****************************************************************************/
/**
	\enum LE_TRIANGLE_FLAGS
	\brief Rendering flags per triangle
*/
typedef enum {
	LE_TRIANGLE_DEFAULT		= 0,	/**< default triangle type */ 
	LE_TRIANGLE_TEXTURED	= 1,	/**< apply single layer texturing */ 
	LE_TRIANGLE_MIPMAPPED	= 2,	/**< apply mipmap filtering */ 
	LE_TRIANGLE_FOGGED		= 4,	/**< apply per-fragment quadratic fog */
	LE_TRIANGLE_BLENDED		= 8,	/**< apply alpha blending (for textures with alpha channel) */
}LE_TRIANGLE_FLAGS;

/**
	\class LeTriangle
	\brief Represent a rasterizable triangle 
*/
typedef struct {
	float xs[4];					/**< x coordinate of vertexes */
	float ys[4];					/**< y coordinate of vertexes */
	float zs[4];					/**< z coordinate of vertexes */
	float us[4];					/**< u texture coordinate of vertexes */
	float vs[4];					/**< v texture coordinate of vertexes */
	float vd;						/**< average view distance */
	
	LeColor solidColor;				/**< solid color */
	int diffuseTexture;				/**< diffuse texture slot */
	int flags;						/**< extra flags */
} LeTriangle;

/*****************************************************************************/
/**
	\class LeTriList
	\brief Contain and manage triangle lists
*/
class LeTriList
{
public:
	LeTriList();
	LeTriList(int noTrangles);
	~LeTriList();

	void allocate(int noTriangles);
	void zSort();

	LeFog fog;						/**< associated quadratic fog model */

public:
	int * srcIndices;				/**< array of triangle source indexes */
	int * dstIndices;				/**< array of triangle destination indexes */
	LeTriangle * triangles;			/**< array of triangles */

	int noAllocated;				/**< number of allocated triangles */
	int noUsed;						/**< number of used triangles */
	int noValid;					/**< number of valid triangles */

private:
	void zMergeSort(int indices[], int tmp[], int nb);
};

#endif // LE_TRILIST_H
