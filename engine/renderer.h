/**
	\file renderer.h
	\brief LightEngine 3D: Meshes and billboard sets renderer
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

/*****************************************************************************
	The renderer assumes a right handed coordinate system:
	- X goes right
	- Y goes top
	- Z goes backward
******************************************************************************/

#ifndef LE_RENDERER_H
#define LE_RENDERER_H

#include "global.h"
#include "config.h"

#include "color.h"
#include "geometry.h"
#include "mesh.h"
#include "bset.h"
#include "rasterizer.h"
#include "trilist.h"
#include "verlist.h"

/*****************************************************************************/
/**
	\class LeRenderer
	\brief Render meshes and billboard set to 2D triangle list
*/
class LeRenderer
{
public:
	LeRenderer(int width = LE_RESOX_DEFAULT, int height = LE_RESOY_DEFAULT);
	~LeRenderer();

	void render(const LeMesh * mesh);
	void render(const LeBSet * bset);
	void flush();

	int getViewportCoordinates(const LeVertex & pos, LeVertex & viewCoords);

	void setViewPosition(const LeVertex & pos);
	void setViewAngle(const LeVertex & angle);
	void updateViewMatrix();

	void setViewMatrix(const LeMatrix & view);

	void setViewport(float left, float top, float right, float bottom);
	void setViewClipping(float near, float far);
	void setViewProjection(float fov);
	void setViewOffset(float offset);

	void setBackculling(bool enable);

	void setFog(bool enable);
	void setFogProperties(LeColor color, float near, float far);

	void setMipmapping(bool enable);

	void setTriangleList(LeTriList * trilist);
	LeTriList * getTriangleList();

private:
	bool checkMemory(int noVertexes, int noTriangles);

	int build(const LeMesh * mesh, LeVertex vertexes[], LeTriangle tris[], int indices[]);
	int build(const LeBSet * bset, LeVertex vertexes[], LeTriangle tris[], int indices[]);

	void updateFrustrum();

	void transform(const LeMatrix &matrix, const LeVertex srcVertexes[], LeVertex dstVertexes[], int nb);
	int project(LeTriangle tris[], const int srcIndices[], int dstIndices[], int nb);
	int clip3D(LeTriangle tris[], const int srcIndices[], int dstIndices[], int nb, LePlan &plan);
	int clip2D(LeTriangle tris[], const int srcIndices[], int dstIndices[], int nb, LeAxis &axis);
	int backculling(LeTriangle tris[], const int srcIndices[], int dstIndices[], int nb);

	LeVerList intVerlist;				/**< Internal vertex list */
	LeTriList intTrilist;				/**< Internal triangle list */
	LeVerList * usedVerlist;			/**< Current vertex list in use */
	LeTriList * usedTrilist;			/**< Current triangle list in use */

	int extra;							/**< Index of extra triangles */
	int extraMax;						/**< Maximum number of extra triangles */

	LeColor * colors;					/**< Color table for triangles */

	LeVertex viewPosition;				/**< View position of renderer */
	LeVertex viewAngle;					/**< View angle of renderer (in degrees) */
	LeMatrix viewMatrix;				/**< View matrix of renderer */

	LePlan viewFrontPlan;				/**< Front clipping plane */
	LePlan viewBackPlan;				/**< Back clipping plane */

	LePlan viewLeftPlan;				/**< 3D frustrum left clipping plane */
	LePlan viewRightPlan;				/**< 3D frustrum right clipping plane */
	LePlan viewTopPlan;					/**< 3D frustrum top clipping plane */
	LePlan viewBotPlan;					/**< 3D frustrum bot clipping plane */

	LeAxis viewLeftAxis;				/**< 2D left clipping axis */
	LeAxis viewRightAxis;				/**< 2D right clipping axis */
	LeAxis viewTopAxis;					/**< 2D top clipping axis */
	LeAxis viewBottomAxis;				/**< 2D bottom clipping axis */

	float ztx;							/**< Horizontal projection factor */
	float zty;							/**< Vertical projection factor */

	float vOffset;						/**< Distance view offset */

	bool backEnable;					/**< Backculling enable state */
	bool mipmappingEnable;				/**< Mipmapping enable state */
	bool fogEnable;						/**< Fog enable state */
};

#endif // LE_RENDERER_H
