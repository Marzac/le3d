/**
	\file light.h
	\brief LightEngine 3D: Simple light models (point / directional / ambient)
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

#ifndef LE_LIGHT_H
#define LE_LIGHT_H

#include "global.h"
#include "config.h"

#include "geometry.h"
#include "mesh.h"

/*****************************************************************************/
/**
	\enum LE_LIGHT_TYPES
	\brief Available light models
*/
typedef enum {
	LE_LIGHT_POINT			= 0,	/**< A point light (intensity decrease with distance) */
	LE_LIGHT_DIRECTIONAL,			/**< A directional light (light intensity depends of incidence) */
	LE_LIGHT_AMBIENT,				/**< An ambient light (light influence all triangles) */
}LE_LIGHT_TYPES;

/*****************************************************************************/
/**
	\class LeLight
	\brief Contain and manage a light object
*/
class LeLight
{
public:
	LeLight();
	LeLight(LE_LIGHT_TYPES type, uint32_t color);

	static void black(LeMesh * mesh);
	void shine(LeMesh * mesh);

	LE_LIGHT_TYPES type;		/**< Model of the light */
	LeAxis axis;				/**< Axis (source and direction) of the light */
	uint32_t color;				/**< Diffuse color of the light */
	float rolloff;				/**< Roll off factor (point model) of the light */

private:
	void shinePoint(LeMesh * mesh);
	void shineDirectional(LeMesh * mesh);
	void shineAmbient(LeMesh * mesh);

public:
	static void blendColors(uint32_t color1, uint32_t color2, float factor, uint32_t &result);

};

#endif // LE_LIGHT_H
