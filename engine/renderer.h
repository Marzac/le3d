/**
	\file renderer.h
	\brief LightEngine 3D: Mesh renderer
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

/*****************************************************************************/
/**
	The renderer assume a standard, right handed coordinate system
	- X goes right
	- Y goes top
	- Z goes backward
*/

#ifndef LE_RENDERER_H
#define LE_RENDERER_H

/*****************************************************************************/
#include "global.h"
#include "geometry.h"
#include "mesh.h"
#include "rasterizer.h"
#include "trilist.h"
#include "verlist.h"

/*****************************************************************************/
class LeRenderer
{
public:
	LeRenderer(int width, int height);
	~LeRenderer();

	void render(LeMesh * mesh);
	void flush();

	void setViewPosition(float x, float y, float z);
	void setViewRotation(float ax, float ay, float az);
	void updateViewMatrix();
	void setViewMatrix(Matrix view);

	void setViewProjection(float fov);
	void setViewport(float left, float top, float right, float bottom);

	void setBackculling(bool enable);
	void setViewOffset(float offset);

	void setTriList(TriList * trilist);
	void setVerList(VerList * verlist);
	TriList * getTriList();
	VerList * getVerList();

private:
	int build(LeMesh * mesh, Vertex vertexes[], Triangle tris[], int indices[]);

	void transform(Matrix view, Vertex srcVertexes[], Vertex dstVertexes[], int nb);
	int project(Triangle tris[], int srcIndices[], int dstIndices[], int nb);
	int clip3D(Triangle tris[], int srcIndices[], int dstIndices[], int nb, Plan &plan);
	int clip2D(Triangle tris[], int srcIndices[], int dstIndices[], int nb, Axis &axis);
	int backculling(Triangle tris[], int srcIndices[], int dstIndices[], int nb);
	void zMergeSort(Triangle tris[], int indices[], int tmp[], int nb);

	VerList intVerlist;
	TriList intTrilist;
	VerList * usedVerlist;
	TriList * usedTrilist;

	int extra;
	int extraMax;

	uint32_t * colors;

	Vertex viewPosition;
	Vertex viewRotation;
	Matrix viewMatrix;

	Plan viewFrontPlan;
	Plan viewBackPlan;
	Axis viewLeftAxis;
	Axis viewRightAxis;
	Axis viewTopAxis;
	Axis viewBottomAxis;

	float width;
	float height;
	float ztx, zty;

	bool enableBack;
	float vOffset;

};

#endif
