/**
	\file objfile.h
	\brief LightEngine 3D: Wavefront object file importer
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

#ifndef LE_OBJFILE_H
#define LE_OBJFILE_H

#include "global.h"
#include "config.h"

#include "geometry.h"
#include "mesh.h"

#include <stdio.h>
#include <string.h>

/*****************************************************************************/
/**
	\class LeObjMaterial
	\brief Contain and manage Wavefront object files materials
*/
struct LeObjMaterial
{
	char  name[LE_OBJ_MAX_NAME+1];		/**< Name of the material */
	float ambient[4];					/**< Ambient color of the material */
	float diffuse[4];					/**< Diffuse color of the material */
	float specular[4];					/**< Specular color of the material */
	float shininess;					/**< Shininess factor of the material */
	float transparency;					/**< Transparency factor of the material */
	char  texture[LE_OBJ_MAX_NAME+1];	/**< Name of first texture of the material */

	LeObjMaterial()
	{
		strcpy(name, "default");

		float color[4] = {1.0f, 1.0f, 1.0f, 0.0f};
		memcpy(ambient, color, sizeof(float) * 4);
		memcpy(diffuse, color, sizeof(float) * 4);
		memcpy(specular, color, sizeof(float) * 4);

		shininess = 0.0f;
		transparency = 0.0f;
		texture[0] = '\0';
	}
};

/*****************************************************************************/
/**
	\class LeObjFile
	\brief Load 3D meshes in Wavefront object format
*/
class LeObjFile
{
public:
	LeObjFile(const char * filename);
	~LeObjFile();

	int getNoMeshes();
	const char * getMeshName(int index);
	
	LeMesh * load(int index);
	void save(const LeMesh * mesh);

private:
	int countTokens(FILE * file, const char * token);

	void importVertexes(FILE * file, LeMesh * mesh);
	void importTexCoords(FILE * file, LeMesh * mesh);
	void importNormals(FILE * file, LeMesh * mesh);
	void importTriangles(FILE * file, LeMesh * mesh);

	void exportHeader(FILE * file, const LeMesh * mesh);
	void exportVertexes(FILE * file, const LeMesh * mesh);
	void exportTexCoords(FILE * file, const LeMesh * mesh);
	void exportTriangles(FILE * file, const LeMesh * mesh);
	void exportMaterials(FILE * file, const LeMesh * mesh);

	void importMatLib(FILE * file);
	int	 countMatLib(FILE * file);
	void readColor(const char * buffer, float * color);

	int	 getNoMaterials(FILE * file);
	void loadMaterials(FILE * file);
	LeObjMaterial * findMaterial(const char * name);

	int	 readLine(FILE * file, char * buffer, int len);
	void readVertex(const char * buffer, LeMesh * mesh, int index);
	void readTriangle(const char * buffer, LeMesh * mesh, int index);
	void readTexCoord(const char * buffer, LeMesh * mesh, int index);

	char * path;						/**< File path of the mesh */
	LeObjMaterial * materials;			/**< Materials of the mesh */
	LeObjMaterial * curMaterial;		/**< Currently selected material */
	int noMaterials;					/**< Number of materials */

	char line[LE_OBJ_MAX_LINE+1];		/**< Line buffer (for parsing the file) */
};

#endif // LE_OBJFILE_H
