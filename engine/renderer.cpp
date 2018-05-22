/**
	\file renderer.cpp
	\brief LightEngine 3D: Meshes and billboard sets renderer
	\brief All platforms implementation
	\author Frédéric Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frédéric Meslin 2015 - 2018
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

#include "renderer.h"

#include "global.h"
#include "config.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

/*****************************************************************************/
#if LE_RENDERER_3DFRUSTRUM == 0 && LE_RENDERER_2DFRAME == 0
	#error One of the clipping systems (3D frustrum or 2D frame) must be enabled (in config.h).
#endif

/*****************************************************************************/
LeRenderer::LeRenderer(int width, int height) :
	usedVerlist(&intVerlist),
	usedTrilist(&intTrilist),
	extra(0), extraMax(0),
	colors(NULL),
	enableBack(true), vOffset(0.0f)
{
// Configure viewport
	setViewport(0.0f, 0.0f, (float) width, (float) height);

// Configure default camera
	setViewPosition(LeVertex(0.0f, 0.0f, 0.0f));
	setViewAngle(LeVertex(0.0f, 0.0f, 0.0f));
	setViewProjection(LE_RENDERER_FOV);

// Flush the lists
	flush();
}

LeRenderer::~LeRenderer()
{
}

/*****************************************************************************/
/**
	\fn void LeRenderer::render(const LeMesh * mesh)
	\brief Render a 3D mesh
	\param[in] mesh pointer to a mesh
*/
void LeRenderer::render(const LeMesh * mesh)
{
// Check vertex memory space
	if (!checkMemory(mesh->noVertexes, mesh->noTriangles))
		return;

// Transform the geometry
	LeTriangle * triRender = &usedTrilist->triangles[usedTrilist->noUsed];
	int * id1 = &usedTrilist->srcIndices[usedTrilist->noValid];
	int * id2 = &usedTrilist->dstIndices[usedTrilist->noValid];

	transform(mesh->view, mesh->vertexes, usedVerlist->vertexes, mesh->noVertexes);
	int noTris = build(mesh, usedVerlist->vertexes, triRender, id1);
	extra = noTris;

// Clip and project
	noTris = clip3D(triRender, id1, id2, noTris, viewFrontPlan);
	noTris = clip3D(triRender, id2, id1, noTris, viewBackPlan);

#if LE_RENDERER_3DFRUSTRUM == 1
	noTris = clip3D(triRender, id1, id2, noTris, viewLeftPlan);
	noTris = clip3D(triRender, id2, id1, noTris, viewRightPlan);
	noTris = clip3D(triRender, id1, id2, noTris, viewTopPlan);
	noTris = clip3D(triRender, id2, id1, noTris, viewBotPlan);
#endif // LE_RENDERER_3DFRUSTRUM

	noTris = project(triRender, id1, id2, noTris);
	noTris = backculling(triRender, id2, id1, noTris);

#if LE_RENDERER_2DFRAME == 1
	noTris = clip2D(triRender, id1, id2, noTris, viewLeftAxis);
	noTris = clip2D(triRender, id2, id1, noTris, viewRightAxis);
	noTris = clip2D(triRender, id1, id2, noTris, viewTopAxis);
	noTris = clip2D(triRender, id2, id1, noTris, viewBottomAxis);
#endif // LE_RENDERER_2DFRAME

// Make render indices absolute
	for (int i = 0; i < noTris; i++)
		id1[i] += usedTrilist->noUsed;

// Modify the state
	usedTrilist->noUsed += extra;
	usedTrilist->noValid += noTris;
}

/**
	\fn void LeRenderer::render(const LeBSet * bset)
	\brief Render a billboard set
	\param[in] bset pointer to a billboard set
*/
void LeRenderer::render(const LeBSet * bset)
{
// Check vertex memory space
	int noVertexes = bset->noBillboards * 4;
	int noTriangles = bset->noBillboards * 2;
	if (!checkMemory(noVertexes, noTriangles))
		return;

// Transform the geometry
	LeTriangle * triRender = &usedTrilist->triangles[usedTrilist->noUsed];
	int * id1 = &usedTrilist->srcIndices[usedTrilist->noValid];
	int * id2 = &usedTrilist->dstIndices[usedTrilist->noValid];

	transform(bset->view, bset->places, usedVerlist->vertexes, bset->noBillboards);
	int noTris = build(bset, usedVerlist->vertexes, triRender, id1);
	extra = noTris;

// Clip and project
	noTris = clip3D(triRender, id1, id2, noTris, viewFrontPlan);
	noTris = clip3D(triRender, id2, id1, noTris, viewBackPlan);

#if LE_RENDERER_3DFRUSTRUM == 1
	noTris = clip3D(triRender, id1, id2, noTris, viewLeftPlan);
	noTris = clip3D(triRender, id2, id1, noTris, viewRightPlan);
	noTris = clip3D(triRender, id1, id2, noTris, viewTopPlan);
	noTris = clip3D(triRender, id2, id1, noTris, viewBotPlan);
#endif // LE_RENDERER_3DFRUSTRUM

	noTris = project(triRender, id1, id2, noTris);
	noTris = backculling(triRender, id2, id1, noTris);

#if LE_RENDERER_2DFRAME == 1
	noTris = clip2D(triRender, id1, id2, noTris, viewLeftAxis);
	noTris = clip2D(triRender, id2, id1, noTris, viewRightAxis);
	noTris = clip2D(triRender, id1, id2, noTris, viewTopAxis);
	noTris = clip2D(triRender, id2, id1, noTris, viewBottomAxis);
#endif // LE_RENDERER_2DFRAME

// Make render indices absolute
	for (int i = 0; i < noTris; i++)
		id1[i] += usedTrilist->noUsed;

// Modify the state
	usedTrilist->noUsed += extra;
	usedTrilist->noValid += noTris;
}

/*****************************************************************************/
/**
	\fn int LeRenderer::getViewportCoordinates(const LeVertex & pos, float & x, float & y)
	\brief Return the viewport coordinates of a 3D vertex
	\param[in] pos vertex position in 3D space
	\param[in] viewCoords viewport coordinates (if within viewport)
	\return 1 if vertex inside viewport, 0 otherwise
*/
int LeRenderer::getViewportCoordinates(const LeVertex & pos, LeVertex & viewCoords)
{
	float centerX = (viewRightAxis.origin.x - viewLeftAxis.origin.x) * 0.5f;
	float centerY = (viewBottomAxis.origin.y - viewTopAxis.origin.y) * 0.5f;

	LeVertex p = viewMatrix * pos;
	if (p.z > viewFrontPlan.zAxis.origin.z) return 0;
	if (p.z <= viewBackPlan.zAxis.origin.z) return 0;

	float w = 1.0f / p.z;
	viewCoords.x = p.x * ztx * w + centerX;
	viewCoords.y = centerY - p.y * zty * w;
	viewCoords.z = p.z;

	if (viewCoords.x < viewLeftAxis.origin.x) return 0;
	if (viewCoords.x >= viewRightAxis.origin.x) return 0;
	if (viewCoords.y < viewTopAxis.origin.y) return 0;
	if (viewCoords.y >= viewBottomAxis.origin.y) return 0;

	return 1;
}

/*****************************************************************************/
/**
	\fn bool LeRenderer::checkMemory(int noVertexes, int noTriangles)
	\brief Check if there is enough memory to render
	\param[in] noVertexes number of vertexes
	\param[in] noTriangles number of triangles
	\return true if enough memory available, false else
*/
bool LeRenderer::checkMemory(int noVertexes, int noTriangles)
{
	if (noVertexes > usedVerlist->noAllocated) return false;
	usedVerlist->noUsed = 0;

	int freeTriangles = usedTrilist->noAllocated - usedTrilist->noUsed;
	if (noTriangles > freeTriangles) return false;
	extraMax = freeTriangles;

	return true;
}

/*****************************************************************************/
/**
	\fn void LeRenderer::flush()
	\brief Flush the currently selected triangle list
*/
void LeRenderer::flush()
{
	usedTrilist->noUsed = 0;
	usedTrilist->noValid = 0;
}

/*****************************************************************************/
/**
	\fn void LeRenderer::setTriangleList(LeTriList * trilist)
	\brief Associate an external triangle list (for storage)
	\param[in] trilist pointer to an allocated triangle list or NULL (for internal triangle list)
*/
void LeRenderer::setTriangleList(LeTriList * trilist)
{
	if (!trilist) usedTrilist = &intTrilist;
	else usedTrilist = trilist;
}

/**
	\fn LeTriList * LeRenderer::getTriangleList()
	\brief Retrieve the associated triangle list
	\return pointer to a triangle list
*/
LeTriList * LeRenderer::getTriangleList()
{
	return usedTrilist;
}

/*****************************************************************************/
/**
	\fn void LeRenderer::setViewPosition(const LeVertex &pos)
	\brief Set the rendering view position
*/
void LeRenderer::setViewPosition(const LeVertex &pos)
{
	viewPosition = pos;
}

/**
	\fn void LeRenderer::setViewAngle(const LeVertex &angle)
	\brief Set the rendering view angle (in degrees)
*/
void LeRenderer::setViewAngle(const LeVertex &angle)
{
	viewAngle = angle * d2r;
}

/**
	\fn void LeRenderer::updateViewMatrix()
	\brief Update the rendering view matrix with position, scaling and angle vectors
*/
void LeRenderer::updateViewMatrix()
{
	viewMatrix.identity();
	viewMatrix.translate(-viewPosition);
	viewMatrix.rotate(-viewAngle);
}

/**
	\fn void LeRenderer::setViewMatrix(const LeMatrix &view)
	\brief Set the rendering view matrix
*/
void LeRenderer::setViewMatrix(const LeMatrix &view)
{
	viewMatrix = view;
}

/*****************************************************************************/
/**
	\fn void LeRenderer::setViewport(float left, float top, float right, float bottom)
	\brief Set the rendering viewport
	\param[in] left left viewport position (in pixels)
	\param[in] top top viewport position (in pixels)
	\param[in] right right viewport position (in pixels)
	\param[in] bottom bottom viewport position (in pixels)
*/
void LeRenderer::setViewport(float left, float top, float right, float bottom)
{
	viewLeftAxis   = LeAxis(LeVertex(left,	top,	0.0f), LeVertex(left,  bottom, 0.0f));
	viewRightAxis  = LeAxis(LeVertex(right, bottom, 0.0f), LeVertex(right, top,	   0.0f));
	viewTopAxis	   = LeAxis(LeVertex(right, top,	0.0f), LeVertex(left,  top,	   0.0f));
	viewBottomAxis = LeAxis(LeVertex(left,	bottom, 0.0f), LeVertex(right, bottom ,0.0f));
}

/**
	\fn void LeRenderer::setViewProjection(float fov)
	\brief Set the rendering field of view angle (in degrees)
*/
void LeRenderer::setViewProjection(float fov)
{
	float width = viewRightAxis.origin.x - viewLeftAxis.origin.x;
	float ratio = tanf(fov * d2r);
	ztx = -width / ratio;
	zty = -width / ratio;
	updateFrustrum();
}

/**
	\fn void LeRenderer::setViewOffset(float offset)
	\brief Set the renderer view offset (for triangle sorting)
	\param[in] offset view offset
*/
void LeRenderer::setViewOffset(float offset)
{
	vOffset = offset * offset * cmsgn(offset);
}

/*****************************************************************************/
/**
	\fn void LeRenderer::setBackculling(bool enable)
	\brief Enable or disable the backculling
	\param[in] enable backculling enable state
*/
void LeRenderer::setBackculling(bool enable)
{
	enableBack = enable;
}

/*****************************************************************************/
/**
	\fn void LeRenderer::updateFrustrum()
	\brief Update the frustrum representation
*/
void LeRenderer::updateFrustrum()
{
	viewFrontPlan.zAxis.origin.z = LE_RENDERER_FRONT;
	viewBackPlan.zAxis.origin.z	 = LE_RENDERER_BACK;
	viewFrontPlan.zAxis.axis.z = -1.0f;
	viewBackPlan.zAxis.axis.z = 1.0f;

	float height = viewBottomAxis.origin.y - viewTopAxis.origin.y;
	float hf = height * 0.5f * LE_RENDERER_FRONT / zty;
	float hb = height * 0.5f * LE_RENDERER_BACK / zty;
	viewTopPlan = LePlan(LeVertex(0.0f,	 hf, LE_RENDERER_FRONT), LeVertex(1.0f,	 hb, LE_RENDERER_BACK), LeVertex(1.0f, hf, LE_RENDERER_FRONT));
	viewBotPlan = LePlan(LeVertex(0.0f, -hf, LE_RENDERER_FRONT), LeVertex(1.0f, -hb, LE_RENDERER_BACK), LeVertex(-1.0f, -hf, LE_RENDERER_FRONT));

	float width = viewRightAxis.origin.x - viewLeftAxis.origin.x;
	float wf = width * 0.5f * LE_RENDERER_FRONT / ztx;
	float wb = width * 0.5f * LE_RENDERER_BACK / ztx;
	viewLeftPlan  = LePlan(LeVertex(-wf, 0.0f, LE_RENDERER_FRONT), LeVertex(-wb, 0.0f, LE_RENDERER_BACK), LeVertex(-wf,	 1.0f, LE_RENDERER_FRONT));
	viewRightPlan = LePlan(LeVertex( wf, 0.0f, LE_RENDERER_FRONT), LeVertex( wb, 0.0f, LE_RENDERER_BACK), LeVertex( wf, -1.0f, LE_RENDERER_FRONT));
}

/*****************************************************************************/
/**
	\fn void LeRenderer::transform(LeMatrix view, const LeVertex srcVertexes[], LeVertex dstVertexes[], int nb)
	\brief Transform the vertexes by the specified matrix and the view matrix
	\param[in] matrix transform matrix
	\param[in] srcVertexes source vertex buffer
	\param[out] dstVertexes destination vertex buffer
	\param[in] nb number of vertexes
*/
void LeRenderer::transform(const LeMatrix & matrix, const LeVertex srcVertexes[], LeVertex dstVertexes[], int nb)
{
	LeMatrix view = viewMatrix * matrix;
	for (int i = 0; i < nb; i++)
		dstVertexes[i] = view * srcVertexes[i];
}

/*****************************************************************************/
int LeRenderer::build(const LeMesh * mesh, LeVertex vertexes[], LeTriangle tris[], int indices[])
{
	int k = 0;
	float frontZ = viewFrontPlan.zAxis.origin.z;
	float backZ = viewBackPlan.zAxis.origin.z;

	if (mesh->shades) colors = mesh->shades;
	else colors = mesh->colors;

	for (int i = 0; i < mesh->noTriangles; i++) {
		LeVertex * v1 = &vertexes[mesh->vertexesList[i*3]];
		LeVertex * v2 = &vertexes[mesh->vertexesList[i*3+1]];
		LeVertex * v3 = &vertexes[mesh->vertexesList[i*3+2]];

	// Hard clip
		if (v1->z > frontZ && v2->z > frontZ && v3->z > frontZ) continue;
		if (v1->z < backZ && v2->z < backZ && v3->z < backZ) continue;

	// Copy coordinates (for clipping)
		LeTriangle * tri = &tris[k];
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

int LeRenderer::build(const LeBSet * bset, LeVertex vertexes[], LeTriangle tris[], int indices[])
{
	int k = 0;
	float frontZ = viewFrontPlan.zAxis.origin.z;
	float backZ = viewBackPlan.zAxis.origin.z;

	if (bset->shades) colors = bset->shades;
	else colors = bset->colors;

	for (int i = 0; i < bset->noBillboards; i++) {
		if (!bset->flags[i]) continue;
		LeVertex * v = &vertexes[i];
		if (v->z > frontZ && v->z < backZ) continue;

		float sx = bset->sizes[i*2+0] * 0.5f;
		float sy = bset->sizes[i*2+1] * 0.5f;

	// Construct billboard
		LeTriangle * tri = &tris[k];
		tri->xs[0] = v->x + sx;
		tri->ys[0] = v->y + sy;
		tri->zs[0] = v->z;
		tri->xs[1] = v->x - sx;
		tri->ys[1] = v->y + sy;
		tri->zs[1] = v->z;
		tri->xs[2] = v->x - sx;
		tri->ys[2] = v->y - sy;
		tri->zs[2] = v->z;

		tri->us[0] = 1.0f;
		tri->vs[0] = 0.0f;
		tri->us[1] = 0.0f;
		tri->vs[1] = 0.0f;
		tri->us[2] = 0.0f;
		tri->vs[2] = 1.0f;

		tri->vd = v->x * v->x + v->y * v->y + v->z * v->z - vOffset;
		tri->color = colors[i];
		tri->tex = bset->texSlots[i];
		indices[k] = k;
		k++;

		tri = &tris[k];
		tri->xs[0] = v->x + sx;
		tri->ys[0] = v->y + sy;
		tri->zs[0] = v->z;
		tri->xs[1] = v->x - sx;
		tri->ys[1] = v->y - sy;
		tri->zs[1] = v->z;
		tri->xs[2] = v->x + sx;
		tri->ys[2] = v->y - sy;
		tri->zs[2] = v->z;

		tri->us[0] = 1.0f;
		tri->vs[0] = 0.0f;
		tri->us[1] = 0.0f;
		tri->vs[1] = 1.0f;
		tri->us[2] = 1.0f;
		tri->vs[2] = 1.0f;

		tri->vd = v->x * v->x + v->y * v->y + v->z * v->z - vOffset;
		tri->color = colors[i];
		tri->tex = bset->texSlots[i];
		indices[k] = k;
		k++;
	}
	return k;
}

/*****************************************************************************/
int LeRenderer::project(LeTriangle tris[], const int srcIndices[], int dstIndices[], int nb)
{
	int k = 0;

	float width = viewRightAxis.origin.x - viewLeftAxis.origin.x;
	float height = viewBottomAxis.origin.y - viewTopAxis.origin.y;
	float centerX = width * 0.5f;
	float centerY = height * 0.5f;

	for (int i = 0; i < nb; i++) {
		int j = srcIndices[i];

		LeTriangle * tri = &tris[j];
		float w0 = 1.0f / tri->zs[0];
		float w1 = 1.0f / tri->zs[1];
		float w2 = 1.0f / tri->zs[2];

		tri->xs[0] = tri->xs[0] * ztx * w0 + centerX;
		tri->xs[1] = tri->xs[1] * ztx * w1 + centerX;
		tri->xs[2] = tri->xs[2] * ztx * w2 + centerX;

		if (tri->xs[0] <  viewLeftAxis.origin.x	  && tri->xs[1] <  viewLeftAxis.origin.x   && tri->xs[2]  < viewLeftAxis.origin.x)	 continue;
		if (tri->xs[0] >= viewRightAxis.origin.x  && tri->xs[1] >= viewRightAxis.origin.x  && tri->xs[2] >= viewRightAxis.origin.x)	 continue;

		tri->ys[0] = centerY - tri->ys[0] * zty * w0;
		tri->ys[1] = centerY - tri->ys[1] * zty * w1;
		tri->ys[2] = centerY - tri->ys[2] * zty * w2;

		if (tri->ys[0] <  viewTopAxis.origin.y	  && tri->ys[1] <  viewTopAxis.origin.y	   && tri->ys[2] <	viewTopAxis.origin.y)	 continue;
		if (tri->ys[0] >= viewBottomAxis.origin.y && tri->ys[1] >= viewBottomAxis.origin.y && tri->ys[2] >= viewBottomAxis.origin.y) continue;

		tri->zs[0] = w0;
		tri->zs[1] = w1;
		tri->zs[2] = w2;
		tri->us[0] *= w0;
		tri->us[1] *= w1;
		tri->us[2] *= w2;
		tri->vs[0] *= w0;
		tri->vs[1] *= w1;
		tri->vs[2] *= w2;

		dstIndices[k++] = j;
	}
	return k;
}

/*****************************************************************************/
int LeRenderer::clip3D(LeTriangle tris[], const int srcIndices[], int dstIndices[], int nb, LePlan &plan)
{
	int k = 0;
	for (int i = 0; i < nb; i++) {
		int s = 0;
		int j = srcIndices[i];

	// Project against clipping plan
		LeTriangle * tri = &tris[j];
		float pj1 = (tri->xs[0] - plan.zAxis.origin.x) * plan.zAxis.axis.x + (tri->ys[0] - plan.zAxis.origin.y) * plan.zAxis.axis.y + (tri->zs[0] - plan.zAxis.origin.z) * plan.zAxis.axis.z;
		float pj2 = (tri->xs[1] - plan.zAxis.origin.x) * plan.zAxis.axis.x + (tri->ys[1] - plan.zAxis.origin.y) * plan.zAxis.axis.y + (tri->zs[1] - plan.zAxis.origin.z) * plan.zAxis.axis.z;
		float pj3 = (tri->xs[2] - plan.zAxis.origin.x) * plan.zAxis.axis.x + (tri->ys[2] - plan.zAxis.origin.y) * plan.zAxis.axis.y + (tri->zs[2] - plan.zAxis.origin.z) * plan.zAxis.axis.z;

	// Compute triangle intersections
		float nx[4], ny[4], nz[4];
		float nu[4], nv[4];
		if (pj1 > 0.0f) {
			nx[s]	= tri->xs[0];
			ny[s]	= tri->ys[0];
			nz[s]	= tri->zs[0];
			nu[s]	= tri->us[0];
			nv[s++] = tri->vs[0];
		}
		if (pj1 * pj2 < 0.0f) {
			float ratio = cmabs(pj1 / (pj1 - pj2));
			nx[s]	= tri->xs[0] + ratio * (tri->xs[1] - tri->xs[0]);
			ny[s]	= tri->ys[0] + ratio * (tri->ys[1] - tri->ys[0]);
			nz[s]	= tri->zs[0] + ratio * (tri->zs[1] - tri->zs[0]);
			nu[s]	= tri->us[0] + ratio * (tri->us[1] - tri->us[0]);
			nv[s++] = tri->vs[0] + ratio * (tri->vs[1] - tri->vs[0]);
		}
		if (pj2 >= 0.0f) {
			nx[s]	= tri->xs[1];
			ny[s]	= tri->ys[1];
			nz[s]	= tri->zs[1];
			nu[s]	= tri->us[1];
			nv[s++] = tri->vs[1];
		}
		if (pj2 * pj3 < 0.0f) {
			float ratio = cmabs(pj2 / (pj2 - pj3));
			nx[s]	= tri->xs[1] + ratio * (tri->xs[2] - tri->xs[1]);
			ny[s]	= tri->ys[1] + ratio * (tri->ys[2] - tri->ys[1]);
			nz[s]	= tri->zs[1] + ratio * (tri->zs[2] - tri->zs[1]);
			nu[s]	= tri->us[1] + ratio * (tri->us[2] - tri->us[1]);
			nv[s++] = tri->vs[1] + ratio * (tri->vs[2] - tri->vs[1]);
		}
		if (pj3 >= 0.0f) {
			nx[s]	= tri->xs[2];
			ny[s]	= tri->ys[2];
			nz[s]	= tri->zs[2];
			nu[s]	= tri->us[2];
			nv[s++] = tri->vs[2];
		}
		if (pj3 * pj1 < 0.0f) {
			float ratio = cmabs(pj3 / (pj3 - pj1));
			nx[s]	= tri->xs[2] + ratio * (tri->xs[0] - tri->xs[2]);
			ny[s]	= tri->ys[2] + ratio * (tri->ys[0] - tri->ys[2]);
			nz[s]	= tri->zs[2] + ratio * (tri->zs[0] - tri->zs[2]);
			nu[s]	= tri->us[2] + ratio * (tri->us[0] - tri->us[2]);
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
			LeTriangle * ntri = &tris[extra];
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
int LeRenderer::clip2D(LeTriangle tris[], const int srcIndices[], int dstIndices[], int nb, LeAxis &axis)
{
	int k = 0;
	for (int i = 0; i < nb; i++) {
		int s = 0;
		int j = srcIndices[i];

	// Project against clipping line
		LeTriangle * tri = &tris[j];
		float pj1 = (tri->xs[0] - axis.origin.x) * axis.axis.y - (tri->ys[0] - axis.origin.y) * axis.axis.x;
		float pj2 = (tri->xs[1] - axis.origin.x) * axis.axis.y - (tri->ys[1] - axis.origin.y) * axis.axis.x;
		float pj3 = (tri->xs[2] - axis.origin.x) * axis.axis.y - (tri->ys[2] - axis.origin.y) * axis.axis.x;

	// Compute intersections
		float nx[4], ny[4], nz[4];
		float nu[4], nv[4];
		if (pj1 >= 0.0f) {
			nx[s]	= tri->xs[0];
			ny[s]	= tri->ys[0];
			nz[s]	= tri->zs[0];
			nu[s]	= tri->us[0];
			nv[s++] = tri->vs[0];
		}
		if (pj1 * pj2 < 0.0f) {
			float ratio = cmabs(pj1 / (pj1 - pj2));
			nx[s]	= tri->xs[0] + ratio * (tri->xs[1] - tri->xs[0]);
			ny[s]	= tri->ys[0] + ratio * (tri->ys[1] - tri->ys[0]);
			nz[s]	= tri->zs[0] + ratio * (tri->zs[1] - tri->zs[0]);
			nu[s]	= tri->us[0] + ratio * (tri->us[1] - tri->us[0]);
			nv[s++] = tri->vs[0] + ratio * (tri->vs[1] - tri->vs[0]);
		}
		if (pj2 >= 0.0f) {
			nx[s]	= tri->xs[1];
			ny[s]	= tri->ys[1];
			nz[s]	= tri->zs[1];
			nu[s]	= tri->us[1];
			nv[s++] = tri->vs[1];
		}
		if (pj2 * pj3 < 0.0f) {
			float ratio = cmabs(pj2 / (pj2 - pj3));
			nx[s]	= tri->xs[1] + ratio * (tri->xs[2] - tri->xs[1]);
			ny[s]	= tri->ys[1] + ratio * (tri->ys[2] - tri->ys[1]);
			nz[s]	= tri->zs[1] + ratio * (tri->zs[2] - tri->zs[1]);
			nu[s]	= tri->us[1] + ratio * (tri->us[2] - tri->us[1]);
			nv[s++] = tri->vs[1] + ratio * (tri->vs[2] - tri->vs[1]);
		}
		if (pj3 >= 0.0f) {
			nx[s]	= tri->xs[2];
			ny[s]	= tri->ys[2];
			nz[s]	= tri->zs[2];
			nu[s]	= tri->us[2];
			nv[s++] = tri->vs[2];
		}
		if (pj3 * pj1 < 0.0f) {
			float ratio = cmabs(pj3 / (pj3 - pj1));
			nx[s]	= tri->xs[2] + ratio * (tri->xs[0] - tri->xs[2]);
			ny[s]	= tri->ys[2] + ratio * (tri->ys[0] - tri->ys[2]);
			nz[s]	= tri->zs[2] + ratio * (tri->zs[0] - tri->zs[2]);
			nu[s]	= tri->us[2] + ratio * (tri->us[0] - tri->us[2]);
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
			tri->zs[0] = nz[0];
			tri->zs[1] = nz[1];
			tri->zs[2] = nz[2];
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
			LeTriangle * ntri = &tris[extra];
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

			ntri->vd = tri->vd;
			ntri->color = tri->color;
			ntri->tex = tri->tex;

			dstIndices[k++] = extra++;
		}
	}
	return k;
}

/*****************************************************************************/
int LeRenderer::backculling(LeTriangle tris[], const int srcIndices[], int dstIndices[], int nb)
{
	int k = 0;
	if (!enableBack) {
		memcpy(dstIndices, srcIndices, nb * (sizeof(int)));
		return nb;
	}
	for (int i = 0; i < nb; i++) {
		int j = srcIndices[i];
		LeTriangle * tri = &tris[j];
		float back = (tri->xs[1] - tri->xs[0]) * (tri->ys[2] - tri->ys[0]);
		back -= (tri->ys[1] - tri->ys[0]) * (tri->xs[2] - tri->xs[0]);
		if (back <= 0.0f) dstIndices[k++] = j;
	}
	return k;
}

