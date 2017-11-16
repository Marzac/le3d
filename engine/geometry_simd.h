/**
	\file geometry_simd.h
	\brief LightEngine 3D: Vertex / axis / plan structures
	\brief All platforms implementation (with SIMD support)
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

#ifndef LE_GEOMETRY_SIMD_H
#define LE_GEOMETRY_SIMD_H

#include "global.h"
#include "config.h"

#if LE_USE_SIMD == 1

#include "simd.h"
#include <math.h>

/*****************************************************************************/
struct __attribute__ ((aligned (16))) LeVertex
{
	union {
		V4SF vf;
		struct {float x, y, z, w;};
	};

	LeVertex()
	{
		V4SF vi = {0.0f, 0.0f, 0.0f, 1.0f};
		vf = vi;
	}

	LeVertex(float px, float py, float pz)
	{
		V4SF vi = {px, py, pz, 1.0f};
		vf = vi;
	}

	LeVertex(float px, float py, float pz, float pw)
	{
		V4SF vi = {px, py, pz, pw};
		vf = vi;
	}

	static LeVertex spherical(float azi, float inc, float dist)
	{
		LeVertex r;
		r.x = cosf(azi) * cosf(inc) * dist;
		r.y = sinf(inc) * dist;
		r.z = -sinf(azi) * cosf(inc) * dist;
		return r;
	}

	LeVertex operator + (LeVertex v) const
	{
		LeVertex r;
		r.vf = vf + v.vf;
		return r;
	}

	LeVertex operator += (LeVertex v)
	{
		vf += v.vf;
		return *this;
	}

	LeVertex operator - (LeVertex v) const
	{
		LeVertex r;
		r.vf = vf - v.vf;
		return r;
	}

	LeVertex operator - () const
	{
		LeVertex r;
		r.vf = - vf;
		return r;
	}

	LeVertex operator -= (LeVertex v)
	{
		vf -= v.vf;
		return *this;
	}

	LeVertex operator * (LeVertex v) const
	{
		LeVertex r;
		r.vf = vf * v.vf;
		return r;
	}

	LeVertex operator / (LeVertex v) const
	{
		LeVertex r;
		r.vf = vf / v.vf;
		return r;
	}

	LeVertex operator * (float v) const
	{
		LeVertex r;
		V4SF vs = {v, v, v, v};
		r.vf = vf * vs;
		return r;
	}

	LeVertex operator *= (LeVertex v)
	{
		vf *= v.vf;
		return *this;
	}

	LeVertex operator *= (float v)
	{
		V4SF vs = {v, v, v, v};
		vf *= vs;
		return *this;
	}

	bool operator == (LeVertex v)
	{
		return x == v.x &&
			   y == v.y &&
			   z == v.z;
	}


	float dot(LeVertex v) const
	{
		LeVertex d;
		d.vf = vf * v.vf;
		return d.x + d.y + d.z;
	}

	LeVertex cross(LeVertex v) const
	{
		LeVertex c;
		c.x = y * v.z - v.y * z;
		c.y = -x * v.z + v.x * z;
		c.z = x * v.y - v.x * y;
		return c;
	}

	LeVertex sign() const
	{
		LeVertex c;
		c.x = copysignf(1.0f, x);
		c.y = copysignf(1.0f, y);
		c.z = copysignf(1.0f, z);
		c.w = copysignf(1.0f, w);
		return c;
	}

	LeVertex round() const
	{
		LeVertex c;
		c.x = floorf(x + 0.5f);
		c.y = floorf(y + 0.5f);
		c.z = floorf(z + 0.5f);
		c.w = floorf(w + 0.5f);
		return c;
	}

	LeVertex floor() const
	{
		LeVertex c;
		c.x = floorf(x);
		c.y = floorf(y);
		c.z = floorf(z);
		c.w = floorf(w);
		return c;
	}

	LeVertex normalize()
	{
		float d = norm();
		if (d == 0.0f) {
			V4SF vi = {0.0f, 0.0f, 0.0f, 1.0f};
			vf = vi;
		}else{
			d = 1.0f / d;
			V4SF vs = {d, d, d, d};
			vf *= vs;
		}
		return *this;
	}

	float norm() const
	{
		return sqrtf(dot(*this));
	}
};

/*****************************************************************************/
struct __attribute__ ((aligned (16))) LeAxis
{
	LeVertex origin;
	LeVertex axis;
	float norm;

	LeAxis()
	{
		origin = LeVertex(0.0f, 0.0f, 0.0f);
		axis = LeVertex(0.0f, 0.0f, 1.0f);
		norm = 1.0f;
	}

	LeAxis(LeVertex v1, LeVertex v2)
	{
		origin = v1;
		axis = (v2 - v1).normalize();
		norm = (v2 - v1).norm();
	}
};

/*****************************************************************************/
struct __attribute__ ((aligned (16))) LePlan
{
	LeAxis xAxis;
	LeAxis yAxis;
	LeAxis zAxis;

	LePlan()
	{
		xAxis.axis = LeVertex(1.0f, 0.0f, 0.0f);
		yAxis.axis = LeVertex(0.0f, 1.0f, 0.0f);
		zAxis.axis = LeVertex(0.0f, 0.0f, 1.0f);
	}

	LePlan(LeVertex v1, LeVertex v2, LeVertex v3) :
		xAxis(LeAxis(v1, v2)), yAxis(LeAxis(v1, v3))
	{
		zAxis = LeAxis(v1, v1 + xAxis.axis.cross(yAxis.axis));
	}
};

/*****************************************************************************/
struct __attribute__ ((aligned (16))) LeMatrix
{
	LeVertex lines[4];

	LeMatrix()
	{
		identity();
	}

	void identity()
	{
		lines[0] = LeVertex(1.0f, 0.0f, 0.0f, 0.0f);
		lines[1] = LeVertex(0.0f, 1.0f, 0.0f, 0.0f);
		lines[2] = LeVertex(0.0f, 0.0f, 1.0f, 0.0f);
		lines[3] = LeVertex(0.0f, 0.0f, 0.0f, 1.0f);
	}

	void zero()
	{
		LeVertex zv = LeVertex(0.0f, 0.0f, 0.0f, 0.0f);
		lines[0] = zv;
		lines[1] = zv;
		lines[2] = zv;
		lines[3] = zv;
	}

	void translate(LeVertex d)
	{
		lines[0].w += d.x;
		lines[1].w += d.y;
		lines[2].w += d.z;
	}

	void scale(LeVertex s)
	{
		LeMatrix m;
		m.lines[0].x *= s.x;
		m.lines[1].y *= s.y;
		m.lines[2].z *= s.z;
		*this = m * *this;
	}

	void rotate(LeVertex a)
	{
		rotateY(a.y);
		rotateZ(a.z);
		rotateX(a.x);
	}

	void rotateBackUp(LeVertex back, LeVertex up, float a)
	{
		back.normalize();
		up.normalize();
		LeVertex right = up.cross(back);

		LeMatrix m1;
		m1.lines[0] = right;
		m1.lines[1] = up;
		m1.lines[2] = back;

		LeMatrix m2;
		m2.rotateY(a);

		LeMatrix m3;
		m3.lines[0] = LeVertex(right.x, up.x, back.x, 0.0f);
		m3.lines[1] = LeVertex(right.y, up.y, back.y, 0.0f);
		m3.lines[2] = LeVertex(right.z, up.z, back.z, 0.0f);
		m3.lines[3] = LeVertex(0.0f, 0.0f, 0.0f, 1.0f);

		*this = m3 * m2 * m1 * *this;
	}

	void rotateBackRight(LeVertex back, LeVertex right, float a)
	{
		back.normalize();
		right.normalize();
		LeVertex up = back.cross(right);

		LeMatrix m1;
		m1.lines[0] = right;
		m1.lines[1] = up;
		m1.lines[2] = back;

		LeMatrix m2;
		m2.rotateY(a);

		LeMatrix m3;
		m3.lines[0] = LeVertex(right.x, up.x, back.x, 0.0f);
		m3.lines[1] = LeVertex(right.y, up.y, back.y, 0.0f);
		m3.lines[2] = LeVertex(right.z, up.z, back.z, 0.0f);
		m3.lines[3] = LeVertex(0.0f, 0.0f, 0.0f, 1.0f);

		*this = m3 * m2 * m1 * *this;
	}

	void alignBackUp(LeVertex back, LeVertex up)
	{
		back.normalize();
		up.normalize();
		LeVertex right = up.cross(back);

		LeMatrix m;
		m.lines[0] = LeVertex(right.x, up.x, back.x, 0.0f);
		m.lines[1] = LeVertex(right.y, up.y, back.y, 0.0f);
		m.lines[2] = LeVertex(right.z, up.z, back.z, 0.0f);
		m.lines[3] = LeVertex(0.0f, 0.0f, 0.0f, 1.0f);

		*this = m * *this;
	}

	void alignBackRight(LeVertex back, LeVertex right)
	{
		back.normalize();
		right.normalize();
		LeVertex up = back.cross(right);

		LeMatrix m;
		m.lines[0] = LeVertex(right.x, up.x, back.x, 0.0f);
		m.lines[1] = LeVertex(right.y, up.y, back.y, 0.0f);
		m.lines[2] = LeVertex(right.z, up.z, back.z, 0.0f);
		m.lines[3] = LeVertex(0.0f, 0.0f, 0.0f, 1.0f);

		*this = m * *this;
	}

	void rotateX(float a)
	{
		LeMatrix m;
		float c = cosf(a);
		float s = sinf(a);
		m.lines[1].y = c;
		m.lines[1].z = -s;
		m.lines[2].y = s;
		m.lines[2].z = c;
		*this = m * *this;
	}

	void rotateY(float a)
	{
		LeMatrix m;
		float c = cosf(a);
		float s = sinf(a);
		m.lines[0].x = c;
		m.lines[0].z = s;
		m.lines[2].x = -s;
		m.lines[2].z = c;
		*this = m * *this;
	}

	void rotateZ(float a)
	{
		LeMatrix m;
		float c = cosf(a);
		float s = sinf(a);
		m.lines[0].x = c;
		m.lines[0].y = -s;
		m.lines[1].x = s;
		m.lines[1].y = c;
		*this = m * *this;
	}

	LeMatrix inverse3x3()
	{
		float d = lines[0].x*(lines[1].y*lines[2].z-lines[2].y*lines[1].z)
				- lines[0].y*(lines[1].x*lines[2].z-lines[1].z*lines[2].x)
                + lines[0].z*(lines[1].x*lines[2].y-lines[1].y*lines[2].x);

		LeMatrix m;
		if (d == 0.0f) {m.zero(); return m;}
		d = 1.0f / d;

		m.lines[0].x =  (lines[1].y * lines[2].z - lines[2].y * lines[1].z) * d;
		m.lines[0].y = -(lines[0].y * lines[2].z - lines[2].y * lines[0].z) * d;
		m.lines[0].z =  (lines[0].y * lines[1].z - lines[1].y * lines[0].z) * d;

		m.lines[1].x = -(lines[1].x * lines[2].z - lines[2].x * lines[1].z) * d;
		m.lines[1].y =  (lines[0].x * lines[2].z - lines[2].x * lines[0].z) * d;
		m.lines[1].z = -(lines[0].x * lines[1].z - lines[1].x * lines[0].z) * d;

		m.lines[2].x =  (lines[1].x * lines[2].y - lines[2].x * lines[1].y) * d;
		m.lines[2].y = -(lines[0].x * lines[2].y - lines[2].x * lines[0].y) * d;
		m.lines[2].z =  (lines[0].x * lines[1].y - lines[1].x * lines[0].y) * d;

		return m;
	}

	LeVertex operator * (LeVertex v) const
	{
		v.w = 1.0f;
		LeVertex l1 = lines[0] * v;
		LeVertex l2 = lines[1] * v;
		LeVertex l3 = lines[2] * v;

		return LeVertex(l1.x + l1.y + l1.z + l1.w,
					  l2.x + l2.y + l2.z + l2.w,
					  l3.x + l3.y + l3.z + l3.w);
	}

	LeMatrix operator + (LeMatrix m) const
	{
		LeMatrix r;
		r.lines[0] = lines[0] + m.lines[0];
		r.lines[1] = lines[1] + m.lines[1];
		r.lines[2] = lines[2] + m.lines[2];
		r.lines[3] = lines[3] + m.lines[3];

		return r;
	}

	LeMatrix operator * (LeMatrix m) const
	{
		LeMatrix r;
		for (int i = 0; i < 4; i++) {
			LeVertex vx = LeVertex(lines[i].x, lines[i].x, lines[i].x, lines[i].x);
			LeVertex vy = LeVertex(lines[i].y, lines[i].y, lines[i].y, lines[i].y);
			LeVertex vz = LeVertex(lines[i].z, lines[i].z, lines[i].z, lines[i].z);
			LeVertex vw = LeVertex(lines[i].w, lines[i].w, lines[i].w, lines[i].w);
			r.lines[i] = vx * m.lines[0] + vy * m.lines[1] + vz * m.lines[2] + vw * m.lines[3];
		}

		return r;
	}
};

/*****************************************************************************/
namespace LePrimitives {
	const LeVertex up    = LeVertex(0.0f, 1.0f, 0.0f);
	const LeVertex down  = LeVertex(0.0f, -1.0f, 0.0f);
	const LeVertex front = LeVertex(0.0f, 0.0f, -1.0f);
	const LeVertex back  = LeVertex(0.0f, 0.0f, 1.0f);
	const LeVertex left  = LeVertex(-1.0f, 0.0f, 0.0f);
	const LeVertex right = LeVertex(1.0f, 0.0f, 0.0f);
	const LeVertex zero  = LeVertex(0.0f, 0.0f, 0.0f);
}

#endif // LE_USE_SIMD

#endif	//LE_GEOMETRY_SIMD_H
