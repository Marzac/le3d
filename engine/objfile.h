/**
	\file objfile.h
	\brief LightEngine 3D: Wavefront object file importer
	\brief All platforms implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2018
	\version 1.4

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
#include <strings.h>

/*****************************************************************************/
struct LeObjMaterial
{
	char  name[LE_OBJ_MAX_NAME+1];
	float ambient[4];
	float diffuse[4];
	float specular[4];
	float shininess;
	float transparency;
	char  texture[LE_OBJ_MAX_NAME+1];

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
class LeObjFile
{
public:
	LeObjFile(const char * filename);
	~LeObjFile();

	int getNoMeshes();
	const char * getMeshName(int index);
	LeMesh * loadMesh(int index);

private:
	int countTokens(FILE * file, const char * token);

	void importVertexes(FILE * file, LeMesh * mesh);
	void importTexCoords(FILE * file, LeMesh * mesh);
	void importNormals(FILE * file, LeMesh * mesh);
	void importTriangles(FILE * file, LeMesh * mesh);

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

	char * path;
	LeObjMaterial * materials;
	LeObjMaterial * curMaterial;
	int noMaterials;

	char line[LE_OBJ_MAX_LINE+1];
};

#endif // LE_OBJFILE_H
