/**
	\file collisions.cpp
	\brief LightEngine 3D (tools): Collision routines
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

#include "collisions.h"

#include "../engine/le3d.h"

#include <math.h>
#include <float.h>

/*****************************************************************************/
/**
	\fn int LeCollisions::collideRectRect(float &ansX, float &ansY, float srcX, float srcY, float srcW, float srcH, float dstX, float dstY, float dstW, float dstH)
	\brief Compute the intersection between two rectangles
	\param[out] ansX x coordinate of answer / correction vector
	\param[out] ansY y coordinate of answer / correction vector
	\param[in] srcX x coordinate of source rectangle
	\param[in] srcY y coordinate of source rectangle
	\param[in] srcW width of source rectangle
	\param[in] srcH height of source rectangle
	\param[in] dstX x coordinate of destination rectangle
	\param[in] dstY y coordinate of destination rectangle
	\param[in] dstW width of destination rectangle
	\param[in] dstH height of destination rectangle
	\return intersection type (see COLLISIONS_RECT_RESULT)
*/
int LeCollisions::collideRectRect(float &ansX, float &ansY, float srcX, float srcY, float srcW, float srcH, float dstX, float dstY, float dstW, float dstH)
{
	float mx = (srcW + dstW) * 0.5f;
	float my = (srcH + dstH) * 0.5f;
	float dx = fabsf(dstX - srcX);
	float dy = fabsf(dstY - srcY);

	if (dx < mx && dy < my) {
		float px = mx - dx;
		float py = my - dy;
		if (px < py) {
			if (dstX > srcX) {
				ansX -= px;
				return COLLISIONS_RECT_LEFT_COL;
			}else{
				ansX += px;
				return COLLISIONS_RECT_RIGHT_COL;
			}
		}else{
			if (dstY > srcY) {
				ansY -= py;
				return COLLISIONS_RECT_TOP_COL;
			}else{
				ansY += py;
				return COLLISIONS_RECT_BOTTOM_COL;
			}
		}
	}
	return COLLISIONS_RECT_NO_COL;
}

/*****************************************************************************/
/**
	\fn int LeCollisions::collideSphereSphere(LeVertex &ans, LeVertex &contact, const LeVertex &pos, float srcRadius, float dstRadius)
	\brief Compute the intersection between two spheres
	\param[out] ans answer / correction vector
	\param[out] contact place of contact vector
	\param[in] pos relative position of spheres
	\param[in] srcRadius radius of source sphere
	\param[in] dstRadius radius of destination sphere
	\return place of collision (see COLLISIONS_SPHERE_RESULT)
*/
int LeCollisions::collideSphereSphere(LeVertex &ans, LeVertex &contact, const LeVertex &pos, float srcRadius, float dstRadius)
{
	float rt = srcRadius + dstRadius;

	ans = LePrimitives::zero;
	contact = LePrimitives::zero;

	float n2 = pos.dot(pos);
	if (n2 > rt * rt) return COLLISIONS_SPHERE_NO_COL;
	if (n2 == 0.0f) return COLLISIONS_SPHERE_SURFACE;

	float n = sqrtf(n2);
	LeVertex dir = pos * (1.0f / n);
	ans = dir * (rt - n);
	contact = dir * srcRadius;

	return COLLISIONS_SPHERE_SURFACE;
}

int LeCollisions::traceSphere(const LeVertex &pos, float radius, const LeVertex &axis, float &distance)
{
	return 0;
}

/*****************************************************************************/
/**
	\fn int LeCollisions::collideSphereBox(LeVertex &ans, LeVertex &contact, const LeVertex &pos, float srcRadius, const LeVertex &dstSize)
	\brief Compute the intersection between a sphere and a box
	\param[out] ans answer / correction vector
	\param[out] contact place of contact vector
	\param[in] pos relative position of the sphere to the box center
	\param[in] srcRadius radius of source sphere
	\param[in] dstSize size of destination box
	\return place of collision (see COLLISIONS_BOX_RESULT)
*/
int LeCollisions::collideSphereBox(LeVertex &ans, LeVertex &contact, const LeVertex &pos, float srcRadius, const LeVertex &dstSize)
{
	const float lx = srcRadius + dstSize.x;
	const float ly = srcRadius + dstSize.y;
	const float lz = srcRadius + dstSize.z;

// Outside of the area
	ans = LePrimitives::zero;
	contact = LePrimitives::zero;

	if (pos.x > lx)	 return COLLISIONS_BOX_NO_COL;
	if (pos.x < -lx) return COLLISIONS_BOX_NO_COL;
	if (pos.y > ly)	 return COLLISIONS_BOX_NO_COL;
	if (pos.y < -ly) return COLLISIONS_BOX_NO_COL;
	if (pos.z > lz)	 return COLLISIONS_BOX_NO_COL;
	if (pos.z < -lz) return COLLISIONS_BOX_NO_COL;

	float dx = fabsf(pos.x);
	float dy = fabsf(pos.y);
	float dz = fabsf(pos.z);

// Touching a side
	if (dx > dstSize.x) {
		if (dy <= dstSize.y && dz <= dstSize.z) {
			float sx = cmsgn(pos.x);
			ans.x = (lx - dx) * sx;
			contact = pos;
			contact.x = dstSize.x * sx;
			return COLLISIONS_BOX_SIDE;
		}
	}

	if (dy > dstSize.y) {
		if (dx <= dstSize.x && dz <= dstSize.z) {
			float sy = cmsgn(pos.y);
			ans.y = (ly - dy) * sy;
			contact = pos;
			contact.y = dstSize.y * sy;
			return COLLISIONS_BOX_SIDE;
		}
	}

	if (dz > dstSize.z) {
		if (dx <= dstSize.x && dy <= dstSize.y) {
			float sz = cmsgn(pos.z);
			ans.z = (lz - dz) * sz;
			contact.z = dstSize.y * sz;
			return COLLISIONS_BOX_SIDE;
		}
	}

// Touching an edge
	int res = COLLISIONS_BOX_CORNER;
	LeVertex corner = dstSize * pos.sign();
	if (dx < dstSize.x) {
		corner.x = pos.x;
		res = COLLISIONS_BOX_EDGE;
	}
	if (dy < dstSize.y) {
		corner.y = pos.y;
		res = COLLISIONS_BOX_EDGE;
	}
	if (dz < dstSize.z) {
		corner.z = pos.z;
		res = COLLISIONS_BOX_EDGE;
	}

// Touching a corner
	LeVertex delta = pos - corner;
	float r = delta.dot(delta);
	if (r > srcRadius * srcRadius)
		return COLLISIONS_BOX_NO_COL;

	delta.normalize();
	ans = delta * (srcRadius - sqrtf(r));
	contact = corner + delta * srcRadius;
	return res;
}

/*****************************************************************************/
/**
	\fn int LeCollisions::traceBox(const LeVertex &pos, const LeVertex &size, const LeVertex &axis, float &distance)
	\brief Compute the intersection between a casted ray and a box
	\param[in] pos position of the center of the box
	\param[in] size size of the box
	\param[in] axis casted ray axis
	\param[out] distance distance from axis origin to box surface
	\return mesh box side or -1 (no intersection) (see)
*/
int LeCollisions::traceBox(const LeVertex &pos, const LeVertex &size, const LeVertex &axis, float &distance)
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
					dMin = d;
					result = COLLISIONS_BOX_TRACE_RIGHT;
				}
			}
		}
		d = (pos.x - size.x) / axis.x;
		h = pos + axis * d;
		if (d >= 0.0f) {
			if (fabs(h.y) < size.y && fabs(h.z) < size.z) {
				if (d < dMin) {
					dMin = d;
					result = COLLISIONS_BOX_TRACE_LEFT;
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
					dMin = d;
					result = COLLISIONS_BOX_TRACE_TOP;
				}
			}
		}
		d = (pos.y - size.y) / axis.y;
		h = pos + axis * d;
		if (d >= 0.0f) {
			if (fabs(h.x) < size.x && fabs(h.z) < size.z) {
				if (d < dMin) {
					dMin = d;
					result = COLLISIONS_BOX_TRACE_BOTTOM;
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
					dMin = d;
					result = COLLISIONS_BOX_TRACE_FRONT;
				}
			}
		}
		d = (pos.z - size.z) / axis.z;
		h = pos + axis * d;
		if (d >= 0.0f) {
			if (fabs(h.x) < size.x && fabs(h.y) < size.y) {
				if (d < dMin) {
					dMin = d;
					result = COLLISIONS_BOX_TRACE_BACK;
				}
			}
		}
	}

	distance = dMin;
	return result;
}

/*****************************************************************************/
/**
	\fn int LeCollisions::collideSphereMesh(LeVertex &ans, LeVertex &contact, const LeVertex &pos, float srcRadius, const LeMesh * dstMesh)
	\brief Compute the intersection between a sphere and a mesh
	\param[out] ans answer / correction vector
	\param[out] contact place of contact vector
	\param[in] pos relative position of the sphere to the mesh center
	\param[in] srcRadius radius of source sphere
	\param[in] dstMesh pointer to a destination mesh
	\return place of collision (see COLLISIONS_MESH_RESULT)
*/
int collideCircleSegment(LeVertex &localAns, LeVertex &localContact, const LeVertex &v1, const LeVertex &v2, const LeVertex &c, float radius2);
int LeCollisions::collideSphereMesh(LeVertex &ans, LeVertex &contact, const LeVertex &pos, float srcRadius, const LeMesh * dstMesh)
{
	ans = LePrimitives::zero;
	contact = LePrimitives::zero;

	LeMatrix mt;
	mt.scale(dstMesh->scale);
	mt.rotateEulerYZX(dstMesh->angle * d2r);

	if (!dstMesh->normals) return COLLISIONS_MESH_NO_COL;
	for (int t = 0; t < dstMesh->noTriangles; t++) {
	// 1: Compute distance to plan
		LeVertex n = dstMesh->normals[t];
		LeVertex v1 = mt * dstMesh->vertexes[dstMesh->vertexesList[t*3]];
		float d = n.dot(pos - v1);
		if (d > srcRadius || d < 0.0f) continue;

	// 2: Check intersection in triangle
		LeVertex h = pos - n * d;
		LeVertex v2 = mt * dstMesh->vertexes[dstMesh->vertexesList[t*3+1]];
		LeVertex v3 = mt * dstMesh->vertexes[dstMesh->vertexesList[t*3+2]];
		float n1 = n.dot((v1 - h).cross(v2 - h));
		if (n1 < 0.0f) continue;
		float n2 = n.dot((v2 - h).cross(v3 - h));
		if (n2 < 0.0f) continue;
		float n3 = n.dot((v3 - h).cross(v1 - h));
		if (n3 < 0.0f) continue;

		ans = n * (srcRadius - d);
		contact = h;
		return COLLISIONS_MESH_SIDE;
	}

	for (int t = 0; t < dstMesh->noTriangles; t++) {
	// 3: Compute intersection with edges
		LeVertex v1 = mt * dstMesh->vertexes[dstMesh->vertexesList[t*3]];
		LeVertex v2 = mt * dstMesh->vertexes[dstMesh->vertexesList[t*3+1]];
		LeVertex v3 = mt * dstMesh->vertexes[dstMesh->vertexesList[t*3+2]];

		LeVertex localContact, localAns;
		int n = collideCircleSegment(localAns, localContact, v1, v2, pos, srcRadius);
		n += collideCircleSegment(localAns, localContact, v2, v3, pos, srcRadius);
		n += collideCircleSegment(localAns, localContact, v3, v1, pos, srcRadius);
		if (!n) continue;

		float in = 1.0f / n;
		ans = localAns * in;
		contact = localContact * in;
		return COLLISIONS_MESH_EDGE;
	}

	return COLLISIONS_MESH_NO_COL;
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
	t1 = cmbound(t1, 0.0f, 1.0f);
	float t2 = 0.5f * (-b + sd) / a;
	t2 = cmbound(t2, 0.0f, 1.0f);
	if (t1 == t2) return 0;

	LeVertex h = v1 + d * ((t1 + t2) * 0.5f);
	LeVertex n = c - h;
	float t = n.norm();

	ans += n * ((radius - t) / t);
	contact += h;
	return 1;
}

/*****************************************************************************/
/**
	\fn int LeCollisions::traceMesh(const LeMesh * mesh, const LeAxis &axis, float &distance)
	\brief Compute the intersection between a casted ray and a mesh
	\param[in] mesh pointer to a mesh
	\param[in] axis casted ray axis
	\param[out] distance distance from axis origin to mesh surface
	\return mesh triangle index or -1 (no intersection)
*/
int LeCollisions::traceMesh(const LeMesh * mesh, const LeAxis &axis, float &distance)
{
	int result = -1;
	float dMin = FLT_MAX;

	if (!mesh->normals) return -1;
	for (int t = 0; t < mesh->noTriangles; t++) {
	// 1: Check if intersection with plan
		LeVertex n = mesh->normals[t];
		float a = n.dot(axis.axis);
		if (a >= 0.0f) continue;

	// 2: Check distance to plan
		LeVertex v1 = mesh->vertexes[mesh->vertexesList[t*3]];
		float d = n.dot(v1 - axis.origin) / a;
		if (d >= dMin) continue;

	// 3: Check if intersection in triangle
		LeVertex h = axis.origin + axis.axis * d;
		LeVertex v2 = mesh->vertexes[mesh->vertexesList[t*3+1]];
		LeVertex v3 = mesh->vertexes[mesh->vertexesList[t*3+2]];

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
