/**
	\file light.cpp
	\brief LightEngine 3D: Simple light models (point / directional / ambient)
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

#include "light.h"

#include <stdlib.h>
#include <strings.h>

/*****************************************************************************/
void macColor(uint32_t color1, uint32_t color2, float factor, uint32_t &result);
/*****************************************************************************/
LeLight::LeLight() :
	type(LE_LIGHT_AMBIENT), color(0x00CCCCCC), rolloff(1.0f)
{
}

LeLight::LeLight(LE_LIGHT_TYPES type, uint32_t color) :
	type(type), color(color), rolloff(1.0f)
{
}

/*****************************************************************************/
void LeLight::setPosition(float x, float y, float z)
{
	pos.origin.x = x;
	pos.origin.y = y;
	pos.origin.z = z;
}

void LeLight::setDirection(float dx, float dy, float dz)
{
	pos.axis.x = dx;
	pos.axis.y = dy;
	pos.axis.z = dz;
	pos.axis.normalize();
}

void LeLight::setColor(uint32_t color)
{
	this->color = color;
}

/*****************************************************************************/
void LeLight::blackMesh(LeMesh * mesh)
{
	if (!mesh->shades) mesh->allocateShades();
	memset(mesh->shades, 0, sizeof(uint32_t) * mesh->noTriangles);
}

void LeLight::shineMesh(LeMesh * mesh)
{
	if (!mesh->normals) mesh->computeNormals();
	if (!mesh->shades) mesh->allocateShades();

	switch (type) {
	case LE_LIGHT_POINT:
		shinePoint(mesh);
		break;
	case LE_LIGHT_DIRECTIONAL :
		shineDirectional(mesh);
		break;
	case LE_LIGHT_AMBIENT :
		shineAmbient(mesh);
		break;
	}
}

/*****************************************************************************/
inline void LeLight::shinePoint(LeMesh * mesh)
{
// Compute the origin
	Vertex o = pos.origin - mesh->pos;
	for (int j = 0; j < mesh->noTriangles; j++) {
		const float third = 1.0f / 3.0f;
		Vertex v1 = mesh->vertexes[mesh->vertexList[j*3]];
		Vertex v2 = mesh->vertexes[mesh->vertexList[j*3+1]];
		Vertex v3 = mesh->vertexes[mesh->vertexList[j*3+2]];
		Vertex m = (v1 + v2 + v3) * third - o;
		Vertex n = mesh->normals[j];

		float p = n.dot(m);
		if (p > 0.0f) continue;
		float d = m.dot(m);
		float e = cmax((1.0f - d * rolloff), 0.0f);
		macColor(mesh->colors[j], color, e, mesh->shades[j]);
	}
}

inline void LeLight::shineDirectional(LeMesh * mesh)
{
// Compute light relative direction
	Matrix iv = mesh->view.inverse3x3();
	Vertex rp = iv * pos.axis;

// Calculate the light
	for (int j = 0; j < mesh->noTriangles; j++) {
		float p = mesh->normals[j].dot(rp);
		if (p > 0.0f) macColor(0xFFFFFF, color, p, mesh->shades[j]);
	}
}

inline void LeLight::shineAmbient(LeMesh * mesh)
{
	for (int j = 0; j <  mesh->noTriangles; j++)
		macColor(mesh->colors[j], color, 1.0f, mesh->shades[j]);
}

/*****************************************************************************/
inline void LeLight::macColor(uint32_t color1, uint32_t color2, float factor, uint32_t &result)
{
	uint8_t * c1 = (uint8_t *) &color1;
	uint8_t * c2 = (uint8_t *) &color2;
	uint8_t * r  = (uint8_t *) &result;
	uint32_t f = factor * 65536.0f;
	r[0] = cbound(r[0] + ((c1[0] * c2[0] * f) >> 24), 0, 255);
	r[1] = cbound(r[1] + ((c1[1] * c2[1] * f) >> 24), 0, 255);
	r[2] = cbound(r[2] + ((c1[2] * c2[2] * f) >> 24), 0, 255);
}

