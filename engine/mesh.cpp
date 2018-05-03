/**
	\file mesh.cpp
	\brief LightEngine 3D: Mesh container and manipulator
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

#include "mesh.h"

#include "global.h"
#include "config.h"

#include <stdlib.h>
#include <string.h>

/*****************************************************************************/
LeMesh::LeMesh() :
	view(),
	pos(), scale(1.0f, 1.0f, 1.0f), angle(),
	vertexes(NULL), noVertexes(0),
	texCoords(NULL), noTexCoords(0),
	vertexList(NULL), texCoordsList(NULL), texSlotList(NULL),
	colors(NULL), noTriangles(0),
	normals(NULL), shades(NULL),
	allocated(false)
{
	memset(name, 0, LE_OBJ_MAX_NAME+1);
	updateMatrix();
}

LeMesh::LeMesh(LeVertex vertexes[], int noVertexes,
			float texCoords[], int noTexCoords,
			int vertexList[], int texCoordsList[],
			uint32_t colors[], int noTriangles) :
	view(),
	pos(), scale(1.0f, 1.0f, 1.0f), angle(),
	vertexes(vertexes), noVertexes(noVertexes),
	texCoords(texCoords), noTexCoords(noTexCoords),
	vertexList(NULL), texCoordsList(NULL), texSlotList(NULL),
	colors(colors), noTriangles(noTriangles),
	normals(NULL), shades(NULL),
	allocated(false)
{
	memset(name, 0, LE_OBJ_MAX_NAME+1);
	updateMatrix();
}

LeMesh::~LeMesh()
{
	deallocate();
}

/*****************************************************************************/
/**
	\fn void LeMesh::allocate(int noVertexes, int noTexCoords, int noTriangles)
	\brief Allocate mesh memory
	\param[in] noVertexes number of vertexes
	\param[in] noTexCoords number of texture coordinates
	\param[in] noTriangles number of triangles
*/
void LeMesh::allocate(int noVertexes, int noTexCoords, int noTriangles)
{
	if (allocated) deallocate();

	vertexes = new LeVertex[noVertexes];
	this->noVertexes = noVertexes;

	texCoords = new float[noTexCoords * 2];
	this->noTexCoords = noTexCoords;

	vertexList = new int[noTriangles * 3];
	texCoordsList = new int[noTriangles * 3];
	texSlotList = new int[noTriangles];

	colors = new uint32_t[noTriangles];
	memset(colors, 0xFF, sizeof(uint32_t) * noTriangles);
	
	this->noTriangles = noTriangles;
	allocated = true;
}

/**
	\fn void LeMesh::deallocate()
	\brief Deallocate mesh memory
*/
void LeMesh::deallocate()
{
// Deallocate static data
	if (allocated) {
		if (vertexes) delete vertexes;
		vertexes = NULL;
		noVertexes = 0;
		if (texCoords) delete texCoords;
		texCoords = NULL;
		noTexCoords = 0;
		if (vertexList) delete vertexList;
		vertexList = NULL;
		if (texCoordsList) delete texCoordsList;
		texCoordsList = NULL;
		if (texSlotList) delete texSlotList;
		texSlotList = NULL;
		if (colors) delete colors;
		colors = NULL;

		noTriangles = 0;
		allocated = false;
	}

// Deallocate temporary data
	if (normals) delete normals;
	normals = NULL;
	if (shades) delete shades;
	shades = NULL;
}

/*****************************************************************************/
/**
	\fn void LeMesh::shadowCopy(LeMesh * copy) const
	\brief Duplicate the mesh without copying its static data
	\param[out] copy pointer to the copy mesh
*/
void LeMesh::shadowCopy(LeMesh * copy) const
{
	if (copy->allocated) copy->deallocate();

	copy->vertexes = vertexes;
	copy->noVertexes = noVertexes;
	copy->texCoords = texCoords;
	copy->noTexCoords = noTexCoords;
	copy->vertexList = vertexList;
	copy->texCoordsList = texCoordsList;
	copy->texSlotList = texSlotList;
	copy->colors = colors;
	copy->noTriangles = noTriangles;
	copy->allocated = false;

	if (normals) {
		copy->normals = new LeVertex[noTriangles];
		memcpy(copy->normals, normals, noTriangles * sizeof(LeVertex));
	}
	if (shades) {
		copy->shades = new uint32_t[noTriangles];
		memcpy(copy->shades, shades, noTriangles * sizeof(uint32_t));
	}
}

/**
	\fn void LeMesh::copy(LeMesh * copy) const
	\brief Duplicate the mesh
	\param[out] copy pointer to the copy mesh
*/
void LeMesh::copy(LeMesh * copy) const
{
	if (copy->allocated) copy->deallocate();

	copy->allocate(noVertexes, noTexCoords, noTriangles);

	memcpy(copy->vertexes, vertexes, noVertexes * sizeof(LeVertex));
	memcpy(copy->texCoords, texCoords, noTexCoords * sizeof(float) * 2);
	memcpy(copy->vertexList, vertexList, noTriangles * sizeof(int) * 3);
	memcpy(copy->texCoordsList, texCoordsList, noTriangles * sizeof(int) * 3);
	memcpy(copy->texSlotList, texSlotList, noTriangles * sizeof(int));
	memcpy(copy->colors, colors, noTriangles * sizeof(uint32_t));

	if (normals) {
		copy->normals = new LeVertex[noTriangles];
		memcpy(copy->normals, normals, noTriangles * sizeof(LeVertex));
	}
	if (shades) {
		copy->shades = new uint32_t[noTriangles];
		memcpy(copy->shades, shades, noTriangles * sizeof(uint32_t));
	}
}

/*****************************************************************************/
/**
	\fn void LeMesh::transform(const LeMatrix &matrix)
	\brief Apply a transformation matrix to the mesh view matrix
	\param[in] matrix transformation matrix
*/
void LeMesh::transform(const LeMatrix &matrix)
{
	updateMatrix();
	view = matrix * view;
}

/**
	\fn void LeMesh::setMatrix(const LeMatrix &matrix)
	\brief Set the mesh view matrix
	\param[in] matrix view matrix
*/
void LeMesh::setMatrix(const LeMatrix &matrix)
{
	view = matrix;
}

/**
	\fn void LeMesh::updateMatrix()
	\brief Update the mesh view matrix with position, scaling and angle vectors
*/
void LeMesh::updateMatrix()
{
	view.identity();
	view.scale(scale);
	view.rotate(angle * d2r);
	view.translate(pos);
}

/*****************************************************************************/
/**
	\fn void LeMesh::computeNormals()
	\brief Compute mesh normals
*/
void LeMesh::computeNormals()
{
	if (!normals) allocateNormals();
	for (int i = 0; i < noTriangles; i++) {
		LeVertex v1 = vertexes[vertexList[i*3]];
		LeVertex v2 = vertexes[vertexList[i*3+1]];
		LeVertex v3 = vertexes[vertexList[i*3+2]];
		LeVertex a = v2 - v1;
		LeVertex b = v3 - v1;
		LeVertex c = a.cross(b);
		c.normalize();
		normals[i] = c;
	}
}

/*****************************************************************************/
/**
	\fn void LeMesh::allocateNormals()
	\brief Allocate mesh normals memory
*/
void LeMesh::allocateNormals()
{
	if (!normals) {
		normals = new LeVertex[noTriangles];
		memset(normals, 0x00, noTriangles * sizeof(LeVertex));
	}
}

/**
	\fn void LeMesh::allocateShades()
	\brief Allocate mesh shades memory
*/
void LeMesh::allocateShades()
{
	if (!shades){
		shades = new uint32_t[noTriangles];
		memset(shades, 0xFF, noTriangles * sizeof(uint32_t));
	}
}
