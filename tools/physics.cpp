/**
	\file physics.cpp
	\brief LightEngine 3D (tools): Collision routines
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


#include "physics.h"

#include "../engine/le3d.h"

#include <math.h>
#include <float.h>

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
int collideSphereSphere(LeVertex &ans, LeVertex &contact, const LeVertex &pos, float radius1, float radius2)
{
    float rt = radius1 + radius2;

    ans = LePrimitives::zero;
    contact = LePrimitives::zero;

    float n2 = pos.dot(pos);
    if (n2 > rt * rt) return 0;
    if (n2 == 0.0f) return 1;

    float n = sqrtf(n2);
    LeVertex dir = pos * (1.0f / n);
    ans = dir * (rt - n);
    contact = dir * radius1;
    return 1;
}

int traceSphere(const LeVertex &pos, float radius, const LeVertex &axis, float &distance)
{
    return 0;
}

/*****************************************************************************/
int collideSphereBox(LeVertex &ans, LeVertex &contact, const LeVertex &pos, float radius, const LeVertex &size)
{
	const float lx = radius + size.x;
	const float ly = radius + size.y;
	const float lz = radius + size.z;

// Outside of the area
	ans = LePrimitives::zero;
    contact = LePrimitives::zero;

    if (pos.x > lx)  return 0;
    if (pos.x < -lx) return 0;
    if (pos.y > ly)  return 0;
    if (pos.y < -ly) return 0;
    if (pos.z > lz)  return 0;
    if (pos.z < -lz) return 0;

	float dx = fabs(pos.x);
	float dy = fabs(pos.y);
	float dz = fabs(pos.z);

// Touching a side
	if (dx > size.x) {
		if (dy <= size.y && dz <= size.z) {
            float sx = csgn(pos.x);
            ans.x = (lx - dx) * sx;
            contact = pos;
            contact.x = size.x * sx;
            return PHYSICS_BOX_SIDE;
		}
	}

	if (dy > size.y) {
		if (dx <= size.x && dz <= size.z) {
            float sy = csgn(pos.y);
            ans.y = (ly - dy) * sy;
            contact = pos;
            contact.y = size.y * sy;
            return PHYSICS_BOX_SIDE;
		}
	}

	if (dz > size.z) {
		if (dx <= size.x && dy <= size.y) {
            float sz = csgn(pos.z);
            ans.z = (lz - dz) * sz;
            contact.z = size.y * sz;
            return PHYSICS_BOX_SIDE;
		}
	}

// Touching an edge
    int res = PHYSICS_BOX_CORNER;
    LeVertex corner = size * pos.sign();
    if (dx < size.x) {
        corner.x = pos.x;
        res = PHYSICS_BOX_EDGE;
    }
    if (dy < size.y) {
        corner.y = pos.y;
        res = PHYSICS_BOX_EDGE;
    }
    if (dz < size.z) {
        corner.z = pos.z;
        res = PHYSICS_BOX_EDGE;
    }

// Touching a corner
    LeVertex delta = pos - corner;
    float r = delta.dot(delta);
    if (r > radius * radius) return 0;

    delta.normalize();
    ans = delta * (radius - sqrtf(r));
    contact = corner + delta * radius;
    return res;
}

/*****************************************************************************/
int traceBox(const LeVertex &pos, const LeVertex &size, const LeVertex &axis, float &distance)
{
	int result = -1;
	float dMin = FLT_MAX;

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
int collideCircleSegment(LeVertex &localAns, LeVertex &localContact, const LeVertex &v1, const LeVertex &v2, const LeVertex &c, float radius2);
int collideSphereMesh(LeVertex &ans, LeVertex &contact, const LeVertex &pos, float radius, const LeMesh * mesh)
{
	ans = LePrimitives::zero;
	contact = LePrimitives::zero;

	LeMatrix mt;
	mt.scale(mesh->scale);
	mt.rotate(mesh->angle * d2r);

	for (int t = 0; t < mesh->noTriangles; t++) {
	// 1: Compute distance to plan
		LeVertex n = mesh->normals[t];
		LeVertex v1 = mt * mesh->vertexes[mesh->vertexList[t*3]];
		float d = n.dot(pos - v1);
		if (d > radius || d < 0.0f) continue;

	// 2: Check intersection in triangle
		LeVertex h = pos - n * d;
		LeVertex v2 = mt * mesh->vertexes[mesh->vertexList[t*3+1]];
		LeVertex v3 = mt * mesh->vertexes[mesh->vertexList[t*3+2]];
        float n1 = n.dot((v1 - h).cross(v2 - h));
        if (n1 < 0.0f) continue;
        float n2 = n.dot((v2 - h).cross(v3 - h));
        if (n2 < 0.0f) continue;
        float n3 = n.dot((v3 - h).cross(v1 - h));
        if (n3 < 0.0f) continue;

        ans = n * (radius - d);
        contact = h;
        return 1;
	}

	for (int t = 0; t < mesh->noTriangles; t++) {
	// 3: Compute intersection with edges
		LeVertex v1 = mt * mesh->vertexes[mesh->vertexList[t*3]];
		LeVertex v2 = mt * mesh->vertexes[mesh->vertexList[t*3+1]];
		LeVertex v3 = mt * mesh->vertexes[mesh->vertexList[t*3+2]];

        LeVertex localContact, localAns;
		int n = collideCircleSegment(localAns, localContact, v1, v2, pos, radius);
        n += collideCircleSegment(localAns, localContact, v2, v3, pos, radius);
        n += collideCircleSegment(localAns, localContact, v3, v1, pos, radius);
        if (!n) continue;

        float in = 1.0f / n;
        ans = localAns * in;
        contact = localContact * in;
        return 2;
	}

	return 0;
}


int collideCircleSegment(LeVertex &ans, LeVertex &contact, const LeVertex &v1, const LeVertex &v2, const LeVertex &c, float radius)
{
	LeVertex d = v2 - v1;
	LeVertex r = v1 - c;

	float a = d.dot(d);
	float b = 2.0f * d.dot(r);
	float g = r.dot(r) - radius * radius;

	float delta = b * b - 4.0f * a * g;
	if (delta < 0.0f) return 0;
    float sd = sqrtf(delta);

    float t1 = 0.5f * (-b - sd) / a;
    t1 = cbound(t1, 0.0f, 1.0f);
    float t2 = 0.5f * (-b + sd) / a;
    t2 = cbound(t2, 0.0f, 1.0f);
    if (t1 == t2) return 0;

    LeVertex h = v1 + d * ((t1 + t2) * 0.5f);
    LeVertex n = c - h;
    float t = n.norm();

    ans += n * ((radius - t) / t);
	contact += h;
	return 1;
}

/*****************************************************************************/
int traceMesh(const LeMesh * mesh, const LeAxis &axis, float &distance)
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
