/**
	\file renderer.cpp
	\brief LightEngine 3D: LeMesh renderer
	\brief All platforms implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2017
	\version 1.0

	The MIT License (MIT)
	Copyright (c) 2017 Fr�d�ric Meslin

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

#include "renderer.h"

#include <stdint.h>
#include <stdlib.h>
#include <strings.h>
#include <math.h>

/*****************************************************************************/
LeRenderer::LeRenderer(int width, int height) :
	extra(0), colors(NULL),
	usedTrilist(&intTrilist),
	usedVerlist(&intVerlist),
	enableBack(true), vOffset(0.0f)
{
// Configure default viewport
	viewFrontPlan.zAxis.origin.z = LE_RENDERER_FRONT;
	viewBackPlan.zAxis.origin.z  = LE_RENDERER_BACK;
	viewFrontPlan.zAxis.axis.z = -1.0f;
	viewBackPlan.zAxis.axis.z = -1.0f;

	float minX = 0.0f;
	float maxX = width - 1.0f;
	float minY = 0.0f;
	float maxY = height - 1.0f;
	setViewport(minX, minY, maxX, maxY);

	this->width = width;
	this->height = height;

// Configure default camera
	setViewPosition(0.0f, 0.0f, 0.0f);
	setViewRotation(0.0f, 0.0f, 0.0f);
	setViewProjection(LE_RENDERER_FOV);

// Flush the lists
	flush();
}

LeRenderer::~LeRenderer()
{
}

/*****************************************************************************/
void LeRenderer::render(LeMesh * mesh)
{
// Check memory space
	if (mesh->noVertexes > usedVerlist->noAllocated) return;
	int remTriangles = usedTrilist->noAllocated - usedTrilist->noUsed;
	if (mesh->noTriangles > remTriangles) return;

// Assign the color source
	if (mesh->shades) colors = mesh->shades;
	else colors = mesh->colors;

// Transform the geometry
	Triangle * triRender = &usedTrilist->triangles[usedTrilist->noUsed];
	int * id1 = &usedTrilist->srcIndices[usedTrilist->noValid];
	int * id2 = &usedTrilist->dstIndices[usedTrilist->noValid];

	transform(mesh->view, mesh->vertexes, usedVerlist->vertexes, mesh->noVertexes);
	int noTris = build(mesh, usedVerlist->vertexes, triRender, id2);

// Pointer for extra triangles
	extra = noTris;
	extraMax = usedTrilist->noAllocated - usedTrilist->noUsed;

// Clip and project
	noTris = clip3D(triRender, id2, id1, noTris, viewFrontPlan);
	noTris = project(triRender, id1, id2, noTris);
	noTris = backculling(triRender, id2, id1, noTris);
	noTris = clip2D(triRender, id1, id2, noTris, viewLeftAxis);
	noTris = clip2D(triRender, id2, id1, noTris, viewRightAxis);
	noTris = clip2D(triRender, id1, id2, noTris, viewTopAxis);
	noTris = clip2D(triRender, id2, id1, noTris, viewBottomAxis);

// Make render indices absolute
	for (int i = 0; i < noTris; i++)
		id1[i] += usedTrilist->noUsed;

// Modify the state
	usedTrilist->noUsed += extra;
	usedTrilist->noValid += noTris;
}

void LeRenderer::flush()
{
	usedVerlist->noUsed = 0;
	usedTrilist->noUsed = 0;
	usedTrilist->noValid = 0;
}

/*****************************************************************************/
void LeRenderer::setTriList(TriList * trilist)
{
	if (!trilist) usedTrilist = &intTrilist;
	else usedTrilist = trilist;
}

TriList * LeRenderer::getTriList()
{
	return usedTrilist;
}

/*****************************************************************************/
void LeRenderer::setViewPosition(float x, float y, float z)
{
	viewPosition.x = x;
	viewPosition.y = y;
	viewPosition.z = z;
}

void LeRenderer::setViewRotation(float ax, float ay, float az)
{
	viewRotation.x = ax * d2r;
	viewRotation.y = ay * d2r;
	viewRotation.z = az * d2r;
}

void LeRenderer::updateViewMatrix()
{
	viewMatrix.identity();
	viewMatrix.translate(-viewPosition.x, -viewPosition.y, -viewPosition.z);
	viewMatrix.rotate(-viewRotation.x, -viewRotation.y, -viewRotation.z);
}

void LeRenderer::setViewMatrix(Matrix view)
{
	viewMatrix = view;
}

void LeRenderer::setViewProjection(float fov)
{
	float ratio = tanf(fov * d2r);
	ztx = -width / ratio;
	zty = -width / ratio;
}

void LeRenderer::setViewport(float left, float top, float right, float bottom)
{
	viewLeftAxis   = Axis(Vertex(left,  top,    0.0f), Vertex(left,  bottom, 0.0f));
	viewRightAxis  = Axis(Vertex(right, bottom, 0.0f), Vertex(right, top,    0.0f));
	viewTopAxis    = Axis(Vertex(right, top,    0.0f), Vertex(left,  top,    0.0f));
	viewBottomAxis = Axis(Vertex(left,  bottom, 0.0f), Vertex(right, bottom ,0.0f));
}

/*****************************************************************************/
void LeRenderer::setBackculling(bool enable)
{
	enableBack = enable;
}

/*****************************************************************************/
void LeRenderer::transform(Matrix view, Vertex srcVertexes[], Vertex dstVertexes[], int nb)
{
	view = viewMatrix * view;
	for (int i = 0; i < nb; i++)
		dstVertexes[i] = view * srcVertexes[i];
}

/*****************************************************************************/
int LeRenderer::build(LeMesh * mesh, Vertex vertexes[], Triangle tris[], int indices[])
{
	int k = 0;
	float fontZ = viewFrontPlan.zAxis.origin.z;
	float backZ = viewBackPlan.zAxis.origin.z;

	for (int i = 0; i < mesh->noTriangles; i++) {
		Vertex * v1 = &vertexes[mesh->vertexList[i*3]];
		Vertex * v2 = &vertexes[mesh->vertexList[i*3+1]];
		Vertex * v3 = &vertexes[mesh->vertexList[i*3+2]];

	// Hard clip
		if (v1->z > fontZ && v2->z > fontZ && v3->z > fontZ) continue;
		if (v1->z < backZ && v2->z < backZ && v3->z < backZ) continue;

	// Copy coordinates (for clipping)
		Triangle * tri = &tris[k];
		tri->xs[0] = v1->x;
		tri->ys[0] = v1->y;
		tri->zs[0] = v1->z;
		tri->xs[1] = v2->x;
		tri->ys[1] = v2->y;
		tri->zs[1] = v2->z;
		tri->xs[2] = v3->x;
		tri->ys[2] = v3->y;
		tri->zs[2] = v3->z;
		int m1 = 2 * mesh->texCoordsList[i*3];
		tri->us[0] = mesh->texCoords[m1];
		tri->vs[0] = mesh->texCoords[m1+1];
		int m2 = 2 * mesh->texCoordsList[i*3+1];
		tri->us[1] = mesh->texCoords[m2];
		tri->vs[1] = mesh->texCoords[m2+1];
		int m3 = 2 * mesh->texCoordsList[i*3+2];
		tri->us[2] = mesh->texCoords[m3];
		tri->vs[2] = mesh->texCoords[m3+1];

	// Compute view distance
		float d1 = tri->zs[0] + tri->zs[1] + tri->zs[2];
		float d2 = tri->ys[0] + tri->ys[1] + tri->ys[2];
		float d3 = tri->xs[0] + tri->xs[1] + tri->xs[2];
		tri->vd = d1 * d1 + d2 * d2 + d3 * d3 - vOffset;

	// Set the material
		tri->color = colors[i];
		tri->tex = mesh->texSlotList[i];

		indices[k] = k;
		k++;
	}
	return k;
}

/*****************************************************************************/
int LeRenderer::project(Triangle tris[], int srcIndices[], int dstIndices[], int nb)
{
	int k = 0;
	float centerX = width * 0.5f;
	float centerY = height * 0.5f;

	for (int i = 0; i < nb; i++) {
		int j = srcIndices[i];

	// Project on frame
		Triangle * tri = &tris[j];
		tri->xs[0] = tri->xs[0] * ztx / tri->zs[0] + centerX;
		tri->xs[1] = tri->xs[1] * ztx / tri->zs[1] + centerX;
		tri->xs[2] = tri->xs[2] * ztx / tri->zs[2] + centerX;
		tri->ys[0] = centerY - tri->ys[0] * zty / tri->zs[0];
		tri->ys[1] = centerY - tri->ys[1] * zty / tri->zs[1];
		tri->ys[2] = centerY - tri->ys[2] * zty / tri->zs[2];

	// Hard clip
		if (tri->xs[0] <  viewLeftAxis.origin.x   && tri->xs[1] <  viewLeftAxis.origin.x   && tri->xs[2]  < viewLeftAxis.origin.x)   continue;
		if (tri->xs[0] >= viewRightAxis.origin.x  && tri->xs[1] >= viewRightAxis.origin.x  && tri->xs[2] >= viewRightAxis.origin.x)  continue;
		if (tri->ys[0] <  viewTopAxis.origin.y    && tri->ys[1] <  viewTopAxis.origin.y    && tri->ys[2] <  viewTopAxis.origin.y)    continue;
		if (tri->ys[0] >= viewBottomAxis.origin.y && tri->ys[1] >= viewBottomAxis.origin.y && tri->ys[2] >= viewBottomAxis.origin.y) continue;
		dstIndices[k++] = j;
	}
	return k;
}

/*****************************************************************************/
int LeRenderer::clip3D(Triangle tris[], int srcIndices[], int dstIndices[], int nb, Plan &plan)
{
	int k = 0;
	for (int i = 0; i < nb; i++) {
		int s = 0;
		int j = srcIndices[i];

	// Project against clipping plan
		Triangle * tri = &tris[j];
		float pj1 = (tri->xs[0] - plan.zAxis.origin.x) * plan.zAxis.axis.x + (tri->ys[0] - plan.zAxis.origin.y) * plan.zAxis.axis.y + (tri->zs[0] - plan.zAxis.origin.z) * plan.zAxis.axis.z;
		float pj2 = (tri->xs[1] - plan.zAxis.origin.x) * plan.zAxis.axis.x + (tri->ys[1] - plan.zAxis.origin.y) * plan.zAxis.axis.y + (tri->zs[1] - plan.zAxis.origin.z) * plan.zAxis.axis.z;
		float pj3 = (tri->xs[2] - plan.zAxis.origin.x) * plan.zAxis.axis.x + (tri->ys[2] - plan.zAxis.origin.y) * plan.zAxis.axis.y + (tri->zs[2] - plan.zAxis.origin.z) * plan.zAxis.axis.z;

	// Compute triangle intersections
		float nx[4], ny[4], nz[4];
		float nu[4], nv[4];
		if (pj1 > 0.0f) {
			nx[s]   = tri->xs[0];
			ny[s]   = tri->ys[0];
			nz[s]   = tri->zs[0];
			nu[s]   = tri->us[0];
			nv[s++] = tri->vs[0];
		}
		if (pj1 * pj2 < 0.0f) {
			float ratio = cabs(pj1 / (pj1 - pj2));
			nx[s]   = tri->xs[0] + ratio * (tri->xs[1] - tri->xs[0]);
			ny[s]   = tri->ys[0] + ratio * (tri->ys[1] - tri->ys[0]);
			nz[s]   = tri->zs[0] + ratio * (tri->zs[1] - tri->zs[0]);
			nu[s]   = tri->us[0] + ratio * (tri->us[1] - tri->us[0]);
			nv[s++] = tri->vs[0] + ratio * (tri->vs[1] - tri->vs[0]);
		}
		if (pj2 >= 0.0f) {
			nx[s]   = tri->xs[1];
			ny[s]   = tri->ys[1];
			nz[s]   = tri->zs[1];
			nu[s]   = tri->us[1];
			nv[s++] = tri->vs[1];
		}
		if (pj2 * pj3 < 0.0f) {
			float ratio = cabs(pj2 / (pj2 - pj3));
			nx[s]   = tri->xs[1] + ratio * (tri->xs[2] - tri->xs[1]);
			ny[s]   = tri->ys[1] + ratio * (tri->ys[2] - tri->ys[1]);
			nz[s]   = tri->zs[1] + ratio * (tri->zs[2] - tri->zs[1]);
			nu[s]   = tri->us[1] + ratio * (tri->us[2] - tri->us[1]);
			nv[s++] = tri->vs[1] + ratio * (tri->vs[2] - tri->vs[1]);
		}
		if (pj3 >= 0.0f) {
			nx[s]   = tri->xs[2];
			ny[s]   = tri->ys[2];
			nz[s]   = tri->zs[2];
			nu[s]   = tri->us[2];
			nv[s++] = tri->vs[2];
		}
		if (pj3 * pj1 < 0.0f) {
			float ratio = cabs(pj3 / (pj3 - pj1));
			nx[s]   = tri->xs[2] + ratio * (tri->xs[0] - tri->xs[2]);
			ny[s]   = tri->ys[2] + ratio * (tri->ys[0] - tri->ys[2]);
			nz[s]   = tri->zs[2] + ratio * (tri->zs[0] - tri->zs[2]);
			nu[s]   = tri->us[2] + ratio * (tri->us[0] - tri->us[2]);
			nv[s++] = tri->vs[2] + ratio * (tri->vs[0] - tri->vs[2]);
		}
	// Build triangle list
		if (s >= 3) {
		// Correct triangle coordinates
			tri->xs[0] = nx[0];
			tri->xs[1] = nx[1];
			tri->xs[2] = nx[2];
			tri->ys[0] = ny[0];
			tri->ys[1] = ny[1];
			tri->ys[2] = ny[2];
			tri->zs[0] = nz[0];
			tri->zs[1] = nz[1];
			tri->zs[2] = nz[2];
			tri->us[0] = nu[0];
			tri->us[1] = nu[1];
			tri->us[2] = nu[2];
			tri->vs[0] = nv[0];
			tri->vs[1] = nv[1];
			tri->vs[2] = nv[2];

		// Correct view distance
			float dx = nx[0] + nx[1] + nx[2];
			float dy = ny[0] + ny[1] + ny[2];
			float dz = nz[0] + nz[1] + nz[2];
			tri->vd = dx * dx + dy * dy + dz * dz - vOffset;
			dstIndices[k++] = j;
		}
		if (s >= 4 && extra < extraMax) {
		// Copy triangle coordinates
			Triangle * ntri = &tris[extra];
			ntri->xs[0] = nx[0];
			ntri->xs[1] = nx[2];
			ntri->xs[2] = nx[3];
			ntri->ys[0] = ny[0];
			ntri->ys[1] = ny[2];
			ntri->ys[2] = ny[3];
			ntri->zs[0] = nz[0];
			ntri->zs[1] = nz[2];
			ntri->zs[2] = nz[3];
			ntri->us[0] = nu[0];
			ntri->us[1] = nu[2];
			ntri->us[2] = nu[3];
			ntri->vs[0] = nv[0];
			ntri->vs[1] = nv[2];
			ntri->vs[2] = nv[3];
			ntri->tex = tri->tex;

		// Compute view distance
			float dx = nx[0] + nx[2] + nx[3];
			float dy = ny[0] + ny[2] + ny[3];
			float dz = nz[0] + nz[2] + nz[3];
			ntri->vd = dx * dx + dy * dy + dz * dz - vOffset;
			ntri->color = tri->color;
			dstIndices[k++] = extra++;
		}
	}
	return k;
}

/*****************************************************************************/
int LeRenderer::clip2D(Triangle tris[], int srcIndices[], int dstIndices[], int nb, Axis &axis)
{
	int k = 0;
	for (int i = 0; i < nb; i++) {
		int s = 0;
		int j = srcIndices[i];

	// Project against clipping line
		Triangle * tri = &tris[j];
		float pj1 = (tri->xs[0] - axis.origin.x) * axis.axis.y - (tri->ys[0] - axis.origin.y) * axis.axis.x;
		float pj2 = (tri->xs[1] - axis.origin.x) * axis.axis.y - (tri->ys[1] - axis.origin.y) * axis.axis.x;
		float pj3 = (tri->xs[2] - axis.origin.x) * axis.axis.y - (tri->ys[2] - axis.origin.y) * axis.axis.x;

	// Compute intersections
		float nx[4], ny[4];
		float nu[4], nv[4];
		if (pj1 >= 0.0f) {
			nx[s]   = tri->xs[0];
			ny[s]   = tri->ys[0];
			nu[s]   = tri->us[0];
			nv[s++] = tri->vs[0];
		}
		if (pj1 * pj2 < 0.0f) {
			float ratio = cabs(pj1 / (pj1 - pj2));
			nx[s]   = tri->xs[0] + ratio * (tri->xs[1] - tri->xs[0]);
			ny[s]   = tri->ys[0] + ratio * (tri->ys[1] - tri->ys[0]);
			nu[s]   = tri->us[0] + ratio * (tri->us[1] - tri->us[0]);
			nv[s++] = tri->vs[0] + ratio * (tri->vs[1] - tri->vs[0]);
		}
		if (pj2 >= 0.0f) {
			nx[s]   = tri->xs[1];
			ny[s]   = tri->ys[1];
			nu[s]   = tri->us[1];
			nv[s++] = tri->vs[1];
		}
		if (pj2 * pj3 < 0.0f) {
			float ratio = cabs(pj2 / (pj2 - pj3));
			nx[s]   = tri->xs[1] + ratio * (tri->xs[2] - tri->xs[1]);
			ny[s]   = tri->ys[1] + ratio * (tri->ys[2] - tri->ys[1]);
			nu[s]   = tri->us[1] + ratio * (tri->us[2] - tri->us[1]);
			nv[s++] = tri->vs[1] + ratio * (tri->vs[2] - tri->vs[1]);
		}
		if (pj3 >= 0.0f) {
			nx[s]   = tri->xs[2];
			ny[s]   = tri->ys[2];
			nu[s]   = tri->us[2];
			nv[s++] = tri->vs[2];
		}
		if (pj3 * pj1 < 0.0f) {
			float ratio = cabs(pj3 / (pj3 - pj1));
			nx[s]   = tri->xs[2] + ratio * (tri->xs[0] - tri->xs[2]);
			ny[s]   = tri->ys[2] + ratio * (tri->ys[0] - tri->ys[2]);
			nu[s]   = tri->us[2] + ratio * (tri->us[0] - tri->us[2]);
			nv[s++] = tri->vs[2] + ratio * (tri->vs[0] - tri->vs[2]);
		}
	// Build triangle list
		if (s >= 3) {
		// Correct the triangle
			tri->xs[0] = nx[0];
			tri->xs[1] = nx[1];
			tri->xs[2] = nx[2];
			tri->ys[0] = ny[0];
			tri->ys[1] = ny[1];
			tri->ys[2] = ny[2];
			tri->us[0] = nu[0];
			tri->us[1] = nu[1];
			tri->us[2] = nu[2];
			tri->vs[0] = nv[0];
			tri->vs[1] = nv[1];
			tri->vs[2] = nv[2];

			dstIndices[k++] = j;
		}
		if (s >= 4 && extra < extraMax) {
		// Copy triangle coordinates
			Triangle * ntri = &tris[extra];
			ntri->xs[0] = nx[0];
			ntri->xs[1] = nx[2];
			ntri->xs[2] = nx[3];
			ntri->ys[0] = ny[0];
			ntri->ys[1] = ny[2];
			ntri->ys[2] = ny[3];
			ntri->us[0] = nu[0];
			ntri->us[1] = nu[2];
			ntri->us[2] = nu[3];
			ntri->vs[0] = nv[0];
			ntri->vs[1] = nv[2];
			ntri->vs[2] = nv[3];

			ntri->vd = tri->vd;
			ntri->color = tri->color;
			ntri->tex = tri->tex;

			dstIndices[k++] = extra++;
		}
	}
	return k;
}

/*****************************************************************************/
int LeRenderer::backculling(Triangle tris[], int srcIndices[], int dstIndices[], int nb)
{
	int k = 0;
	if (!enableBack) {
		memcpy(dstIndices, srcIndices, nb * (sizeof(int)));
		return nb;
	}
	for (int i = 0; i < nb; i++) {
		int j = srcIndices[i];
		Triangle * tri = &tris[j];
		float back = (tri->xs[1] - tri->xs[0]) * (tri->ys[2] - tri->ys[0]);
		back -= (tri->ys[1] - tri->ys[0]) * (tri->xs[2] - tri->xs[0]);
		if (back <= 0.0f) dstIndices[k++] = j;
	}
	return k;
}

void LeRenderer::setViewOffset(float offset)
{
	vOffset = offset * offset;
}
