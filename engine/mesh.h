/**
	\file mesh.h
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

#ifndef LE_MESH_H
#define LE_MESH_H

#include "global.h"
#include "geometry.h"

/*****************************************************************************/
class LeMesh
{
public:
	LeMesh();
	LeMesh(Vertex vertexes[], int noVertexes, float texCoords[], int noTexCoords,
		 int vertexList[], int texCoordsList[],
		 uint32_t colors[], int noTriangles);
	virtual ~LeMesh();

	LeMesh * shadowCopy();
	LeMesh * copy();

	void setPosition(float x, float y, float z);
	void setScale(float sx, float sy, float sz);
	void setRotation(float ax, float ay, float az);
	void transform(const Matrix &matrix);
	void updateMatrix();

	void setMatrix(const Matrix &matrix);

	void computeNormals();
	void allocateShades();

	Matrix view;
	Vertex pos;
	Vertex size;
	Vertex angle;

	Vertex * vertexes;
	int noVertexes;
	float * texCoords;
	int noTexCoords;

	int * vertexList;
	int * texCoordsList;
	int * texSlotList;
	uint32_t * colors;
	int noTriangles;

	Vertex * normals;
	uint32_t * shades;

	bool allocated;

};

#endif // LE_MESH_H
