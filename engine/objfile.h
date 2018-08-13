/**
	\file objfile.h
	\brief LightEngine 3D: Wavefront object file importer
	\brief All platforms implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2018
	\version 1.75

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
#include "color.h"
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
	char name[LE_OBJ_MAX_NAME+1];		/**< Name of the material */
	LeColor ambient;					/**< Ambient color of the material */
	LeColor diffuse;					/**< Diffuse color of the material */
	LeColor specular;					/**< Specular color of the material */
	float shininess;					/**< Shininess factor of the material */
	float transparency;					/**< Transparency factor of the material */
	char texture[LE_OBJ_MAX_NAME+1];	/**< Name of first texture of the material */

	LeObjMaterial()
	{
		strcpy(name, "default");
		ambient = LeColor(255, 255, 255, 0);
		diffuse = LeColor(255, 255, 255, 0);
		specular = LeColor(255, 255, 255, 0);
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
	void importMeshAllocate(FILE * file, LeMesh * mesh);
	void importMeshData(FILE * file, LeMesh * mesh);

	void exportHeader(FILE * file, const LeMesh * mesh);
	void exportVertexes(FILE * file, const LeMesh * mesh);
	void exportNormals(FILE * file, const LeMesh * mesh);
	void exportTexCoords(FILE * file, const LeMesh * mesh);
	void exportTriangles(FILE * file, const LeMesh * mesh);
	void exportMaterials(FILE * file, const LeMesh * mesh);

	void loadMaterialLibraries(FILE * file);
	void importMaterialAllocate(FILE * file);
	void importMaterialData(FILE * file);
	LeObjMaterial * getMaterialFromName(const char * name);

	void readVertex(LeMesh * mesh, int index);
	void readNormal(LeMesh * mesh, int index);
	void readTriangle(LeMesh * mesh, int index);
	void readTexCoord(LeMesh * mesh, int index);
	void readColor(const char * buffer, LeColor & color);

	char * path;						/**< File path of the mesh */
	
	LeObjMaterial * materials;			/**< Internal - file materials */
	LeObjMaterial * curMaterial;		/**< Currently selected material */
	int noMaterials;					/**< Internal - number of materials */
	
	LeVertex * normals;					/**< Internal - object normals */
	int noNormals;						/**< Internal - number of normals */ 

	int	readLine(FILE * file);	
	char line[LE_OBJ_MAX_LINE+1];		/**< Line buffer (for parsing the file) */
};

#endif // LE_OBJFILE_H
