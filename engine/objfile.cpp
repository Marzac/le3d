/**
	\file objfile.cpp
	\brief LightEngine 3D: Wavefront object file importer
	\brief All platforms implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2017
	\version 1.1

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

#include "objfile.h"
#include "bmpcache.h"

#include "global.h"
#include "config.h"

#include <stdlib.h>
#include <strings.h>

/*****************************************************************************/
const char * objMaterialLib		= "mtllib ";
const char * objMaterialSet		= "usemtl ";
const char * objGroup			= "g ";
const char * objSmoothingGroup	= "s ";
const char * objObject			= "o ";

const char * objGeoVertex		= "v ";
const char * objTexCoords		= "vt ";
const char * objVertexNormals	= "vn ";
const char * objFace			= "f ";

const char * matMaterial		= "newmtl ";
const char * matAmbient			= "Ka ";
const char * matDiffuse			= "Kd ";
const char * matSpecular		= "Ks ";
const char * matShininess		= "Ns ";
const char * matTransparency	= "d ";
const char * matTexture			= "map_Kd ";

LeObjMaterial defMaterial;

/*****************************************************************************/
LeObjFile::LeObjFile(const char * filename) :
	path(NULL),
	materials(NULL), curMaterial(&defMaterial), noMaterials(0)
{
	if (filename) path = strdup(filename);
}

LeObjFile::~LeObjFile()
{
	if (path) free(path);
	//if (materials) delete[] materials;
}

/*****************************************************************************/
LeMesh * LeObjFile::loadMesh(int index)
{
	FILE * file = fopen(path, "rb");
	if (!file) return NULL;

	LeMesh * mesh = new LeMesh();
	if (!mesh) {
		fclose(file);
		return NULL;
	}

	mesh->allocated = true;
	if (!materials)	loadMaterials(file);
	curMaterial = &defMaterial;

	fseek(file, 0, SEEK_SET);
	int object = 0;
	int len = readLine(file, line, LE_OBJ_MAX_LINE);
	while (len) {
		if (strncmp(line, objObject, 2) == 0) {
			if (object == index) {
				importVertexes(file, mesh);
				importTriangles(file, mesh);
				importTexCoords(file, mesh);
				break;
			}else object ++;
		}
		len = readLine(file, line, LE_OBJ_MAX_LINE);
	}
	fclose(file);
	return mesh;
}

int LeObjFile::getNoMeshes()
{
	FILE * file = fopen(path, "rb");
	if (!file) return 0;
	int nbObjects = 0;
	int len = readLine(file, line, LE_OBJ_MAX_LINE);
	while (len) {
		if (strncmp(line, objObject, 2) == 0) nbObjects ++;
		len = readLine(file, line, LE_OBJ_MAX_LINE);
	}
	fclose(file);
	return nbObjects;
}

const char * LeObjFile::getMeshName(int index)
{
	FILE * file = fopen(path, "rb");
	if (!file) return NULL;
	int object = 0;
	int readLen = readLine(file, line, LE_OBJ_MAX_LINE);
	while (readLen) {
		if (strncmp(line, objObject, 2) == 0) {
			if (object == index) {
				line[LE_OBJ_MAX_LINE] = '\0';
				fclose(file);
				return &line[2];
			}else object ++;
		}
		readLen = readLine(file, line, LE_OBJ_MAX_LINE);
	}
	fclose(file);
	return NULL;
}

/*****************************************************************************/
void LeObjFile::loadMaterials(FILE * file)
{
	FILE * libFile;
	char filename[LE_OBJ_MAX_PATH+1];
	char libName[LE_OBJ_MAX_NAME+1];
	noMaterials	= 0;
	int nb = getNoMaterials(file);
	materials = new LeObjMaterial[nb];
	if (!materials) return;
	fseek(file, 0, SEEK_SET);
	int len = readLine(file, line, LE_OBJ_MAX_LINE);
	while (len) {
		if (strncmp(line, objMaterialLib, 7) == 0) {
			strncpy(libName, &line[7], LE_OBJ_MAX_NAME);
			libName[LE_OBJ_MAX_NAME] = '\0';
			getDir(filename, LE_OBJ_MAX_PATH);
			strcat(filename, libName);	//TODO: secure this
			libFile = fopen(filename, "rb");
			if (libFile) {
				importMatLib(libFile);
				fclose(libFile);
			}
		}
		len = readLine(file, line, LE_OBJ_MAX_LINE);
	}
}

int LeObjFile::getNoMaterials(FILE * file)
{
	FILE * libFile;
	char filename[LE_OBJ_MAX_PATH+1];
	char libName[LE_OBJ_MAX_NAME+1];
	int nbMats = 0;
	fseek(file, 0, SEEK_SET);
	int len = readLine(file, line, LE_OBJ_MAX_LINE);
	while (len) {
		if (strncmp(line, objMaterialLib, 7) == 0) {
			strncpy(libName, &line[7], LE_OBJ_MAX_NAME);
			libName[LE_OBJ_MAX_NAME] = '\0';
			getDir(filename, LE_OBJ_MAX_PATH);
			strcat(filename, libName);	//TODO: secure this
			libFile = fopen(filename, "rb");
			if (libFile) {
				nbMats += countMatLib(libFile);
				fclose(libFile);
			}
		}
		len = readLine(file, line, LE_OBJ_MAX_LINE);
	}
	return nbMats;
}

/*****************************************************************************/
void LeObjFile::importMatLib(FILE * file)
{
	int mat = -1;
	int len = readLine(file, line, LE_OBJ_MAX_LINE);
	while (len) {
		if (strncmp(line, matMaterial, 7) == 0) {
			mat = noMaterials++;
			strncpy(materials[mat].name, &line[7], LE_OBJ_MAX_NAME);
			materials[mat].name[LE_OBJ_MAX_NAME] = '\0';
		}else if (strncmp(line, matAmbient, 3) == 0) {
			readColor(&line[3], materials[mat].ambient);
		}else if (strncmp(line, matDiffuse, 3) == 0) {
			readColor(&line[3], materials[mat].diffuse);
		}else if (strncmp(line, matSpecular, 3) == 0) {
			readColor(&line[3], materials[mat].specular);
		}else if (strncmp(line, matShininess, 3) == 0) {
			sscanf(&line[3], "%f", &materials[mat].shininess);
		}else if (strncmp(line, matTransparency, 2) == 0) {
			sscanf(&line[2], "%f", &materials[mat].transparency);
		}else if (strncmp(line, matTexture, 7) == 0) {
			strncpy(materials[mat].texture, &line[7], LE_OBJ_MAX_NAME);
			materials[mat].texture[LE_OBJ_MAX_NAME] = '\0';
		}
		len = readLine(file, line, LE_OBJ_MAX_LINE);
	}
}

int LeObjFile::countMatLib(FILE * file)
{
	int nb = 0;
	int len = readLine(file, line, LE_OBJ_MAX_LINE);
	while (len) {
		if (strncmp(line, matMaterial, 7) == 0) nb ++;
		len = readLine(file, line, LE_OBJ_MAX_LINE);
	}
	return nb;
}

/*****************************************************************************/
LeObjMaterial * LeObjFile::findMaterial(const char * name)
{
	int i;
	for (i = 0; i < noMaterials; i++)
        if (strcmp(name, materials[i].name) == 0) {
			//printf("Found %s", objMaterials[i].name);
			return &materials[i];
		}
	return &defMaterial;
}

/*****************************************************************************/
void LeObjFile::importVertexes(FILE * file, LeMesh * mesh)
{
// Count number of vertexes
	int nb = countTokens(file, objGeoVertex);
	if (!nb) return;
// Allocate memory
	mesh->vertexes = (LeVertex *) new LeVertex[nb];
	if (!mesh->vertexes) return;
	mesh->noVertexes = nb;
// Import vertexes
	int start = ftell(file);
	int len = readLine(file, line, LE_OBJ_MAX_LINE);
	int index = 0;
	while (len) {
		if (strncmp(line, objGeoVertex, 2) == 0) {
			readVertex(line, mesh, index);
			index ++;
		}else if (strncmp(line, objObject, 2) == 0) break;
		len = readLine(file, line, LE_OBJ_MAX_LINE);
	}
	fseek(file, start, SEEK_SET);
}


void LeObjFile::importTexCoords(FILE * file, LeMesh * mesh)
{
// Count number of tex coords
	int nb = countTokens(file, objTexCoords);
	if (!nb) return;
// Allocate memory
	mesh->texCoords = (float *) new float[nb * 2];
	if (!mesh->texCoords) return;
	mesh->noTexCoords = nb;
// Import tex coords
	int start = ftell(file);
	int len = readLine(file, line, LE_OBJ_MAX_LINE);
	int index = 0;
	while (len) {
		if (strncmp(line, objTexCoords, 3) == 0) {
			readTexCoord(line, mesh, index);
			index ++;
		}else if (strncmp(line, objObject, 2) == 0) break;
		len = readLine(file, line, LE_OBJ_MAX_LINE);
	}
	fseek(file, start, SEEK_SET);
}

/*****************************************************************************/
void LeObjFile::importTriangles(FILE * file, LeMesh * mesh)
{
// Count number of triangles
	int nb = countTokens(file, objFace);
	if (!nb) return;
// Allocate memory
	mesh->vertexList = new int[3 * nb];
	memset(mesh->vertexList, 0, 3 * nb * sizeof(int));
	mesh->texCoordsList = new int[3 * nb];
	memset(mesh->texCoordsList, 0, 3 * nb * sizeof(int));
	mesh->texSlotList = new int[nb];
	memset(mesh->texSlotList, 0, nb * sizeof(int));
	mesh->colors = new uint32_t[nb];
	memset(mesh->colors, 0, nb * sizeof(uint32_t));
	mesh->noTriangles = nb;
// Import the triangles
	int start = ftell(file);
	int len = readLine(file, line, LE_OBJ_MAX_LINE);
	int index = 0;
	while (len) {
		if (strncmp(line, objFace, 2) == 0) {
			readTriangle(line, mesh, index);
			uint32_t r = cbound(curMaterial->diffuse[0] * 255.0f, 0.0f, 255.0f);
			uint32_t g = cbound(curMaterial->diffuse[1] * 255.0f, 0.0f, 255.0f);
			uint32_t b = cbound(curMaterial->diffuse[2] * 255.0f, 0.0f, 255.0f);
			mesh->colors[index] = r | (g << 8) | (b << 16);
			int slot = bmpCache.getFromName(curMaterial->texture);
			if (slot < 0) {
				printf("objFile: Taking default texture!\n");
				slot = 0;
			}
			mesh->texSlotList[index] = slot;
			index ++;
		}else if (strncmp(line, objMaterialSet, 7) == 0) {
			curMaterial = findMaterial(&line[7]);
		}else if (strncmp(line, objObject, 2) == 0) break;
		len = readLine(file, line, LE_OBJ_MAX_LINE);
	}
	fseek(file, start, SEEK_SET);
}

/*****************************************************************************/
int LeObjFile::countTokens(FILE * file, const char * token)
{
	int start = ftell(file);
	int nb = 0;
	int tl = strlen(token);
	int len = readLine(file, line, LE_OBJ_MAX_LINE);
	while (len) {
		if (strncmp(line, token, tl) == 0) nb ++;
		else if (strncmp(line, objObject, 2) == 0) break;
		len = readLine(file, line, LE_OBJ_MAX_LINE);
	}
	fseek(file, start, SEEK_SET);
	return nb;
}

/*****************************************************************************/
void LeObjFile::readVertex(const char * buffer, LeMesh * mesh, int index)
{
	sscanf(&line[2], "%f %f %f", &mesh->vertexes[index].x, &mesh->vertexes[index].y, &mesh->vertexes[index].z);
}

void LeObjFile::readTexCoord(const char * buffer, LeMesh * mesh, int index)
{
	float u, v;
	sscanf(&line[2], "%f %f", &u, &v);
	mesh->texCoords[index*2]   = u;
	mesh->texCoords[index*2+1] = 1.0f - v;
}

void LeObjFile::readTriangle(const char * buffer, LeMesh * mesh, int index)
{
	buffer = &buffer[2];
	int * vList = &mesh->vertexList[index * 3];
	int * tList = &mesh->texCoordsList[index * 3];
	for (int i = 0; i < 3; i++) {
		int v, t, n;
		sscanf(buffer, "%i/%i/%i", &v, &t, &n);
		vList[i] = v - 1;
		tList[i] = t - 1;
		buffer = strchr(buffer, ' ');
		if (!buffer) break;
		buffer ++;
	}
}

/*****************************************************************************/
void LeObjFile::readColor(const char * buffer, float * color)
{
	sscanf(buffer, "%f %f %f", &color[0], &color[1], &color[2]);
}

/*****************************************************************************/
int LeObjFile::readLine(FILE * file, char * buffer, int len)
{
	bool skipLine = false;
	int readLen = 0;
	while(!feof(file)) {
		int c = fgetc(file);
		if (c =='\n' || c == '\r') {
			if (readLen != 0) {
				buffer[readLen] = '\0';
				return readLen;
			}else {
				skipLine = false;
				continue;
			}
		}
		if (skipLine) continue;
		if (c =='#') {
			skipLine = true;
			continue;
		}
		if (readLen < len - 1) {
			buffer[readLen ++] = c;
		}else{
			buffer[len-1] = '\0';
			return readLen;
		}
	}
	buffer[readLen] = '\0';
	return readLen;
}

/*****************************************************************************/
void LeObjFile::getDir(char * dir, int size)
{
	*dir = 0;
	int len = strlen(path);
	if (len == 0) return;
	const char * p = &path[len - 1];
	while (p >= path) {
		if (*p == '/' || *p == '\\') {
			int n = p - path + 1;
			if (n > size - 1) return;
			strncpy(dir, path, n);
			dir[n] = 0;
			return;
		} p --;
	}
}

/*****************************************************************************/
/*
int main()
{
	char name[32];
	FILE * file;
	file = fopen("autruche.obj", "rb");
	printf("OBJ file %s information\n", "autruche.obj");
	printf("\tNum of meshes : %d", embOBJGetNoObjects(file, 0));
	embOBJGetObjectName(file, 0, name, 32);
	printf("\tObj1 name : %s", name);
	embOBJGetObjectName(file, 1, name, 32);
	printf("\tObj2 name : %s", name);
	embOBJGetObjectName(file, 2, name, 32);
	printf("\tObj3 name : %s", name);

	printf("\tNum of materials : %d", embOBJGetNoMaterials(file));
	embOBJImportMaterials(file);

	LeMesh mesh;
	embOBJImportMesh(file, 0, &mesh);


	fclose(file);
}
*/
