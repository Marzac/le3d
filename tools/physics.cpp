/**
	\file physics.cpp
	\brief LightEngine 3D (tools): Collision routines
	\brief All platforms implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2017
	\version 1.3

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


#include "physics.h"

#include "../engine/le3d.h"

#include <math.h>
#include <float.h>

/*****************************************************************************/
int collideCircleSegment(LeVertex v1, LeVertex v2, LeVertex c, float radius2);


/*****************************************************************************/
int collideRectRect(float &ansX, float &ansY, float srcX, float srcY, float srcW, float srcH, float dstX, float dstY, float dstW, float dstH)
{
	float mx = (srcW + dstW) * 0.5f;
	float my = (srcH + dstH) * 0.5f;
	float dx = fabs(dstX - srcX);
	float dy = fabs(dstY - srcY);

	if (dx < mx && dy < my) {
		float px = mx - dx;
		float py = my - dy;
		if (px < py) {
			if (dstX > srcX) {
				ansX -= px;
				return PHYSICS_RECT_LEFT_COL;
			}else{
				ansX += px;
				return PHYSICS_RECT_RIGHT_COL;
			}
		}else{
			if (dstY > srcY) {
				ansY -= py;
				return PHYSICS_RECT_TOP_COL;
			}else{
				ansY += py;
				return PHYSICS_RECT_BOTTOM_COL;
			}
		}
	}
	return PHYSICS_RECT_NO_COL;
}

/*****************************************************************************/
int collideSphereBox(LeVertex &ans, LeVertex pos, float radius, LeVertex size)
{
	const float lx = radius + size.x;
	const float ly = radius + size.y;
	const float lz = radius + size.z;

// Outside of the area
	ans = LePrimitives::zero;
    if (pos.x > lx)  return 0;
    if (pos.x < -lx) return 0;
    if (pos.y > ly)  return 0;
    if (pos.y < -ly) return 0;
    if (pos.z > lz)  return 0;
    if (pos.z < -lz) return 0;

// Touching a side
	float dx = fabs(pos.x);
	float dy = fabs(pos.y);
	float dz = fabs(pos.z);

	if (dx > size.x) {
		if (dy <= size.y && dz <= size.z) {
			ans.x = (lx - dx) * csgn(pos.x);
			return 1;
		}
	}

	if (dy > size.y) {
		if (dx <= size.x && dz <= size.z) {
			ans.y = (ly - dy) * csgn(pos.y);
			return 2;
		}
	}

	if (dz > size.z) {
		if (dx <= size.x && dy <= size.y) {
			ans.z = (lz - dz) * csgn(pos.z);
			return 4;
		}
	}

// Touching a corner
	if (dx > dy) {
		if (dx > dz)
			ans.x = (lx - dx) * csgn(pos.x);
		else
			ans.z = (lz - dz) * csgn(pos.z);
	}else{
		if (dy > dz)
			ans.y = (ly - dy) * csgn(pos.y);
		else
			ans.z = (lz - dz) * csgn(pos.z);
	}
	return 16;
}

/*****************************************************************************/
int traceBox(LeVertex pos, LeVertex size, LeVertex axis, float &distance)
{
	int result = -1;
	float dMin = FLT_MAX;
	pos = -pos;

// X sides
	if (axis.x != 0.0f) {
		float d = (pos.x + size.x) / axis.x;
		LeVertex h = pos + axis * d;
		if (d >= 0.0f) {
			if (fabs(h.y) < size.y && fabs(h.z) < size.z) {
				if (d < dMin) {
					dMin = d; result = 0;
				}
			}
		}
		d = (pos.x - size.x) / axis.x;
		h = pos + axis * d;
		if (d >= 0.0f) {
			if (fabs(h.y) < size.y && fabs(h.z) < size.z) {
				if (d < dMin) {
					dMin = d; result = 1;
				}
			}
		}
	}

// Y sides
	if (axis.y != 0.0f) {
		float d = (pos.y + size.y) / axis.y;
		LeVertex h = pos + axis * d;
		if (d >= 0.0f) {
			if (fabs(h.x) < size.x && fabs(h.z) < size.z) {
				if (d < dMin) {
					dMin = d; result = 2;
				}
			}
		}
		d = (pos.y - size.y) / axis.y;
		h = pos + axis * d;
		if (d >= 0.0f) {
			if (fabs(h.x) < size.x && fabs(h.z) < size.z) {
				if (d < dMin) {
					dMin = d; result = 3;
				}
			}
		}
	}

// Z sides
	if (axis.z != 0.0f) {
		float d = (pos.z + size.z) / axis.z;
		LeVertex h = pos + axis * d;
		if (d >= 0.0f) {
			if (fabs(h.x) < size.x && fabs(h.y) < size.y) {
				if (d < dMin) {
					dMin = d; result = 4;
				}
			}
		}
		d = (pos.z - size.z) / axis.z;
		h = pos + axis * d;
		if (d >= 0.0f) {
			if (fabs(h.x) < size.x && fabs(h.y) < size.y) {
				if (d < dMin) {
					dMin = d; result = 5;
				}
			}
		}
	}
	distance = dMin;
	return result;
}

/*****************************************************************************/
int collideSphereMesh(LeVertex &ans, LeVertex pos, float radius, LeMesh * mesh)
{
	ans = LePrimitives::zero;
	for (int t = 0; t < mesh->noTriangles; t++) {

	// 1: Compute distance to plan
		LeVertex n = mesh->normals[t];
		LeVertex v1 = mesh->vertexes[mesh->vertexList[t*3]];
		float d = n.dot(pos - v1);
		if (d > radius || d < 0.0f) continue;

	// 2: Check if intersection in triangle
		LeVertex h = pos - n * d;
		LeVertex v2 = mesh->vertexes[mesh->vertexList[t*3+1]];
		LeVertex v3 = mesh->vertexes[mesh->vertexList[t*3+2]];
		do {
			float n1 = n.dot((v1 - h).cross(v2 - h));
			if (n1 < 0.0f) break;
			float n2 = n.dot((v2 - h).cross(v3 - h));
			if (n2 < 0.0f) break;
			float n3 = n.dot((v3 - h).cross(v1 - h));
			if (n3 < 0.0f) break;
			ans = n * (radius - d);
			return 1;
		} while(0);

	// 3: Compute intersection between segment and radius
		float r2 = radius * radius - d * d;
		if (!collideCircleSegment(v1, v2, h, r2) &&
			!collideCircleSegment(v2, v3, h, r2) &&
			!collideCircleSegment(v3, v1, h, r2))
			continue;
		ans = n * (radius - d);
		return 2;
	}

	return 0;
}

int collideCircleSegment(LeVertex v1, LeVertex v2, LeVertex c, float radius2)
{
	LeVertex d = v2 - v1;
	LeVertex r = v1 - c;

	float a = d.dot(d);
	float b = 2.0f * d.dot(r);
	float g = r.dot(r) - radius2;

	float delta = b * b - 4 * a * g;
	if (delta >= 0.0f) {
		float t1 = 0.5f * (b - sqrtf(delta)) / a;
		if (t1 >= 0.0f && t1 <= 1.0f) return 1;
		float t2 = 0.5f * (b + sqrtf(delta)) / a;
		if (t2 >= 0.0f && t2 <= 1.0f) return 1;
	}
	return 0;
}


/*****************************************************************************/
int traceMesh(LeMesh * mesh, LeAxis axis, float &distance)
{
	int result = -1;
	float dMin = FLT_MAX;

	for (int t = 0; t < mesh->noTriangles; t++) {
	// 1: Check if intersection with plan
		LeVertex n = mesh->normals[t];
		float a = n.dot(axis.axis);
		if (a >= 0.0f) continue;

	// 2: Check distance to plan
		LeVertex v1 = mesh->vertexes[mesh->vertexList[t*3]];
		float d = n.dot(v1 - axis.origin) / a;
		if (d >= dMin) continue;

	// 3: Check if intersection in triangle
		LeVertex h = axis.origin + axis.axis * d;
		LeVertex v2 = mesh->vertexes[mesh->vertexList[t*3+1]];
		LeVertex v3 = mesh->vertexes[mesh->vertexList[t*3+2]];

		float n1 = n.dot((v1 - h).cross(v2 - h));
		if (n1 < 0.0f) continue;
		float n2 = n.dot((v2 - h).cross(v3 - h));
		if (n2 < 0.0f) continue;
		float n3 = n.dot((v3 - h).cross(v1 - h));
		if (n3 < 0.0f) continue;

		dMin = d;
		result = t;
	}

	distance = dMin;
	return result;
}
