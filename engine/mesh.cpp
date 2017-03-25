/**
	\file mesh.cpp
	\brief LightEngine 3D: Mesh container and manipulator
	\brief All platforms implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2017
	\version 1.0

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

#include "mesh.h"

#include <stdlib.h>
#include <strings.h>

/*****************************************************************************/
LeMesh::LeMesh() :
	view(), pos(), size(1.0f, 1.0f, 1.0f), angle(),
	vertexes(NULL), noVertexes(0),
	texCoords(NULL), noTexCoords(0),
	vertexList(NULL), texCoordsList(NULL), texSlotList(NULL),
	colors(NULL), noTriangles(0),
	normals(NULL), shades(NULL),
	allocated(false)
{
	updateMatrix();
}

LeMesh::LeMesh(Vertex vertexes[], int noVertexes, float texCoords[], int noTexCoords,
		   int vertexList[], int texCoordsList[],
		   uint32_t colors[], int noTriangles) :
	view(), pos(), size(1.0f, 1.0f, 1.0f), angle(),
	vertexes(vertexes), noVertexes(noVertexes),
	texCoords(texCoords), noTexCoords(noTexCoords),
	vertexList(NULL), texCoordsList(NULL), texSlotList(NULL),
	colors(colors), noTriangles(noTriangles),
	normals(NULL), shades(NULL),
	allocated(false)
{
	updateMatrix();
}

LeMesh::~LeMesh()
{
	if (allocated) {
		if (vertexes) delete vertexes;
		if (texCoords) delete texCoords;
		if (vertexList) delete vertexList;
		if (texCoordsList) delete texCoordsList;
		if (texSlotList) delete texSlotList;
		if (colors) delete colors;
	}
	if (shades) delete shades;
	if (normals) delete normals;
}

/*****************************************************************************/
LeMesh * LeMesh::shadowCopy()
{
	LeMesh * copy = new LeMesh();
	copy->vertexes = vertexes;
	copy->noVertexes = noVertexes;
	copy->texCoords = texCoords;
	copy->noTexCoords = noTexCoords;
	copy->vertexList = vertexList;
	copy->texCoordsList = texCoordsList;
	copy->texSlotList = texSlotList;
	copy->colors = colors;
	copy->noTriangles = noTriangles;

	copy->normals = normals;
	copy->shades = new uint32_t[noTriangles];

	copy->allocated = false;
	return copy;
}

LeMesh * LeMesh::copy()
{
	LeMesh * copy = new LeMesh();

	copy->vertexes = new Vertex[noVertexes];
	memcpy(copy->vertexes, vertexes, noVertexes * sizeof(Vertex));
	copy->noVertexes = noVertexes;
	copy->texCoords = new float[noTexCoords * 2];
	memcpy(copy->texCoords, texCoords, noTexCoords * sizeof(float) * 2);
	copy->noTexCoords = noTexCoords;
	copy->vertexList = new int[noTriangles * 3];
	memcpy(copy->vertexList, vertexList, noTriangles * sizeof(int) * 3);
	copy->texCoordsList = new int[noTriangles * 3];
	memcpy(copy->texCoordsList, texCoordsList, noTriangles * sizeof(int) * 3);
	copy->texSlotList = new int[noTriangles];
	memcpy(copy->texSlotList, texSlotList, noTriangles * sizeof(int));
	copy->colors = new uint32_t[noTriangles];
	memcpy(copy->colors, colors, noTriangles * sizeof(uint32_t));
	copy->noTriangles = noTriangles;

	copy->normals = new Vertex[noTriangles];
	memcpy(copy->normals, normals, noTriangles * sizeof(Vertex));
	copy->shades = new uint32_t[noTriangles];
	memcpy(copy->shades, shades, noTriangles * sizeof(uint32_t));

	copy->allocated = true;
	return copy;
}

/*****************************************************************************/
void LeMesh::setPosition(float x, float y, float z)
{
	this->pos.x = x;
	this->pos.y = y;
	this->pos.z = z;
}

void LeMesh::setScale(float sx, float sy, float sz)
{
	this->size.x = sx;
	this->size.y = sy;
	this->size.z = sz;
}

void LeMesh::setRotation(float ax, float ay, float az)
{
	this->angle.x = ax;
	this->angle.y = ay;
	this->angle.z = az;
}

void LeMesh::transform(const Matrix &matrix)
{
	updateMatrix();
	view = matrix * view;
}

void LeMesh::updateMatrix()
{
	view.identity();
	view.scale(size.x, size.y, size.z);
	view.rotate(angle.x * d2r, angle.y * d2r, angle.z * d2r);
	view.translate(pos.x, pos.y, pos.z);
}

/*****************************************************************************/
void LeMesh::setMatrix(const Matrix &matrix)
{
	view = matrix;
}

/*****************************************************************************/
void LeMesh::computeNormals()
{
	if (!normals)
		normals = new Vertex[noTriangles];

	for (int i = 0; i < noTriangles; i++) {
		Vertex v1 = vertexes[vertexList[i*3]];
		Vertex v2 = vertexes[vertexList[i*3+1]];
		Vertex v3 = vertexes[vertexList[i*3+2]];
		Vertex a = v2 - v1;
		Vertex b = v3 - v1;
		Vertex c = a.cross(b);
		c.normalize();
		normals[i] = c;
	}
}

void LeMesh::allocateShades()
{
	if (!shades){
		shades = new uint32_t[noTriangles];
		memset(shades, 0xFF, noTriangles * sizeof(uint32_t));
	}
}
