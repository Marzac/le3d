/**
	\file mesh.h
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

#ifndef LE_MESH_H
#define LE_MESH_H

#include "global.h"
#include "config.h"

#include "color.h"
#include "geometry.h"

/*****************************************************************************/
/**
	\class LeMesh
	\brief Contain and manage a 3D mesh
*/
class LeMesh
{
public:
	LeMesh();
	LeMesh(LeVertex vertexes[], int noVertexes, float texCoords[], int noTexCoords,
		 int vertexesList[], int texCoordsList[],
		 LeColor colors[], int noTriangles);
	virtual ~LeMesh();

	void shadowCopy(LeMesh * copy) const;
	void copy(LeMesh * copy) const;

	void allocate(int noVertexes, int noTexCoords, int noTriangles);
	void deallocate();

	void transform(const LeMatrix &matrix);
	void setMatrix(const LeMatrix &matrix);
	void updateMatrix();

	void computeNormals();
	void allocateNormals();
	void allocateShades();
	
// Overall positioning
	LeMatrix view;						/**< View matrix of the mesh */
	LeVertex pos;						/**< Position of the mesh */
	LeVertex scale;						/**< Scaling of the mesh */
	LeVertex angle;						/**< Absolute angle of the mesh (in degrees) */

// Static mesh data
	char name[LE_OBJ_MAX_NAME+1];		/**< Mesh name */
	LeVertex * vertexes;				/**< Vertex positions (x, y, z) of the mesh */
	int noVertexes;						/**< Number of vertexes in the mesh */
	float * texCoords;					/**< Texture coordinates (u, v) of the mesh */
	int noTexCoords;					/**< Number of texture coordinates in the mesh */
	int * vertexesList;					/**< Triangles - vertex indexes tripplets */
	int * texCoordsList;				/**< Triangles - texture coordinate indexes tripplets */
	int * texSlotList;					/**< Triangles - texture slots */
	LeColor * colors;					/**< Triangles - colors */
	int noTriangles;					/**< Number of triangles in the mesh */

// Computed mesh data
	LeVertex * normals;					/** Normal vector per triangle */
	LeColor * shades;					/** Shade color per triangle (lighting) */

	bool allocated;						/** Has data been allocated */
};

#endif // LE_MESH_H
