/**
	\file light.cpp
	\brief LightEngine 3D: Simple light models (point / directional / ambient)
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

#include "light.h"

#include "global.h"
#include "config.h"

#include <stdlib.h>
#include <string.h>

/*****************************************************************************/
LeLight::LeLight() :
	type(LE_LIGHT_AMBIENT),
	axis(), color(LeColor::rgb(0xCCCCCC)),
	rolloff(1.0f)
{
}

LeLight::LeLight(LE_LIGHT_TYPES type, LeColor color) :
	type(type),
	axis(), color(color),
	rolloff(1.0f)
{
}

/*****************************************************************************/
/**
	\fn void LeLight::black(LeMesh * mesh)
	\brief Clear a mesh light information (shades)
	\param[in] mesh mesh pointer
*/
void LeLight::black(LeMesh * mesh)
{
	if (!mesh->shades) mesh->allocateShades();
	memset(mesh->shades, 0, sizeof(LeColor) * mesh->noTriangles);
}

/**
	\fn void LeLight::shine(LeMesh * mesh)
	\brief Shine a mesh with the light
	\param[in] mesh mesh pointer
*/
void LeLight::shine(LeMesh * mesh)
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
	LeVertex o = axis.origin - mesh->pos;
	for (int j = 0; j < mesh->noTriangles; j++) {
		const float third = 1.0f / 3.0f;
		LeVertex v1 = mesh->vertexes[mesh->vertexesList[j*3]];
		LeVertex v2 = mesh->vertexes[mesh->vertexesList[j*3+1]];
		LeVertex v3 = mesh->vertexes[mesh->vertexesList[j*3+2]];
		LeVertex m = (v1 + v2 + v3) * third - o;
		LeVertex n = mesh->normals[j];

		float p = n.dot(m);
		if (p > 0.0f) continue;
		float d = m.dot(m);
		float e = cmmax((1.0f - d * rolloff), 0.0f);
		blendColors(mesh->colors[j], color, e, mesh->shades[j]);
	}
}

inline void LeLight::shineDirectional(LeMesh * mesh)
{
	LeMatrix iv = mesh->view.inverse3x3();
	LeVertex rp = iv * axis.axis;
	rp.normalize();

	for (int j = 0; j < mesh->noTriangles; j++) {
		float p = -rp.dot(mesh->normals[j]);
		if (p > 0.0f) blendColors(LeColor::rgb(0xFFFFFF), color, p, mesh->shades[j]);
	}
}

inline void LeLight::shineAmbient(LeMesh * mesh)
{
	for (int j = 0; j <	 mesh->noTriangles; j++)
		blendColors(mesh->colors[j], color, 1.0f, mesh->shades[j]);
}

/*****************************************************************************/
/**
	\fn void LeLight::blendColors(uint32_t color1, uint32_t color2, float factor, uint32_t &result)
	\brief Blend two colors together and accumulate the result
	\param[in] color1 first RGBA 32bit color
	\param[in] color2 second RGBA 32bit color
	\param[in] factor blend factor (0.0 - 1.0)
	\param[in] result accumulated color result
*/
void LeLight::blendColors(LeColor color1, LeColor color2, float factor, LeColor &result)
{
	uint32_t f = (uint32_t) (factor * 65536.0f);
	result.r = cmbound(result.r + ((color1.r * color2.r * f) >> 24), 0, 255);
	result.g = cmbound(result.g + ((color1.g * color2.g * f) >> 24), 0, 255);
	result.b = cmbound(result.b + ((color1.b * color2.b * f) >> 24), 0, 255);
	result.a = 0;
}
