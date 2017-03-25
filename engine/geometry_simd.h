/**
	\file geometry_simd.h
	\brief LightEngine 3D: Vertex / axis / plan structures
	\brief All platforms implementation (with SIMD support)
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

#ifndef LE_GEOMETRY_SIMD_H
#define LE_GEOMETRY_SIMD_H

#include "global.h"

#include <math.h>

/*****************************************************************************/
typedef float V4SF __attribute__ ((vector_size (16)));
struct __attribute__ ((aligned (16))) Vertex
{
	union {
		V4SF vf;
		struct {float x, y, z, w;};
	};

	Vertex()
	{
		V4SF vi = {0.0f, 0.0f, 0.0f, 1.0f};
		vf = vi;
	}

	Vertex(float px, float py, float pz)
	{
		V4SF vi = {px, py, pz, 1.0f};
		vf = vi;
	}

	Vertex(float px, float py, float pz, float pw)
	{
		V4SF vi = {px, py, pz, pw};
		vf = vi;
	}

	static Vertex spherical(float azi, float inc, float dist)
	{
		Vertex r;
		r.x = cosf(azi) * cosf(inc) * dist;
		r.y = sinf(inc) * dist;
		r.z = -sinf(azi) * cosf(inc) * dist;
		return r;
	}

	Vertex operator + (Vertex v) const
	{
		Vertex r;
		r.vf = vf + v.vf;
		return r;
	}

	Vertex operator += (Vertex v)
	{
		vf += v.vf;
		return *this;
	}

	Vertex operator - (Vertex v) const
	{
		Vertex r;
		r.vf = vf - v.vf;
		return r;
	}

	Vertex operator - () const
	{
		Vertex r;
		r.vf = - vf;
		return r;
	}

	Vertex operator -= (Vertex v)
	{
		vf -= v.vf;
		return *this;
	}

	Vertex operator * (Vertex v) const
	{
		Vertex r;
		r.vf = vf * v.vf;
		return r;
	}

	Vertex operator / (Vertex v) const
	{
		Vertex r;
		r.vf = vf / v.vf;
		return r;
	}

	Vertex operator * (float v) const
	{
		Vertex r;
		V4SF vs = {v, v, v, v};
		r.vf = vf * vs;
		return r;
	}

	Vertex operator *= (Vertex v)
	{
		vf *= v.vf;
		return *this;
	}

	Vertex operator *= (float v)
	{
		V4SF vs = {v, v, v, v};
		vf *= vs;
		return *this;
	}

	bool operator == (Vertex v)
	{
		return x == v.x &&
			   y == v.y &&
			   z == v.z;
	}


	float dot(Vertex v) const
	{
		Vertex d;
		d.vf = vf * v.vf;
		return d.x + d.y + d.z;
	}

	Vertex cross(Vertex v) const
	{
		Vertex c;
		c.x = y * v.z - v.y * z;
		c.y = -x * v.z + v.x * z;
		c.z = x * v.y - v.x * y;
		return c;
	}

	Vertex sign() const
	{
		Vertex c;
		c.x = copysignf(1.0f, x);
		c.y = copysignf(1.0f, y);
		c.z = copysignf(1.0f, z);
		c.w = copysignf(1.0f, w);
		return c;
	}

	Vertex round() const
	{
		Vertex c;
		c.x = floorf(x + 0.5f);
		c.y = floorf(y + 0.5f);
		c.z = floorf(z + 0.5f);
		c.w = floorf(w + 0.5f);
		return c;
	}

	Vertex floor() const
	{
		Vertex c;
		c.x = floorf(x);
		c.y = floorf(y);
		c.z = floorf(z);
		c.w = floorf(w);
		return c;
	}

	Vertex normalize()
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

	float norm()
	{
		return sqrtf(dot(*this));
	}
};

/*****************************************************************************/
struct __attribute__ ((aligned (16))) Axis
{
	Vertex origin;
	Vertex axis;
	float norm;

	Axis()
	{
		origin = Vertex(0.0f, 0.0f, 0.0f);
		axis = Vertex(0.0f, 0.0f, 1.0f);
		norm = 1.0f;
	}

	Axis(Vertex v1, Vertex v2)
	{
		origin = v1;
		axis = (v2 - v1).normalize();
		norm = (v2 - v1).norm();
	}
};

/*****************************************************************************/
struct __attribute__ ((aligned (16))) Plan
{
	Axis xAxis;
	Axis yAxis;
	Axis zAxis;

	Plan()
	{
		xAxis.axis = Vertex(1.0f, 0.0f, 0.0f);
		yAxis.axis = Vertex(0.0f, 1.0f, 0.0f);
		zAxis.axis = Vertex(0.0f, 0.0f, 1.0f);
	}

	Plan(Vertex v1, Vertex v2, Vertex v3) :
		xAxis(Axis(v1, v2)), yAxis(Axis(v1, v3))
	{
		zAxis = Axis(v1, v1 + xAxis.axis.cross(yAxis.axis));
		zAxis.origin = v1;
	}
};

/*****************************************************************************/
struct __attribute__ ((aligned (16))) Matrix
{
	Vertex lines[4];

	Matrix()
	{
		identity();
	}

	void identity()
	{
		lines[0] = Vertex(1.0f, 0.0f, 0.0f, 0.0f);
		lines[1] = Vertex(0.0f, 1.0f, 0.0f, 0.0f);
		lines[2] = Vertex(0.0f, 0.0f, 1.0f, 0.0f);
		lines[3] = Vertex(0.0f, 0.0f, 0.0f, 1.0f);
	}

	void zero()
	{
		Vertex zv = Vertex(0.0f, 0.0f, 0.0f, 0.0f);
		lines[0] = zv;
		lines[1] = zv;
		lines[2] = zv;
		lines[3] = zv;
	}

	void translate(float dx, float dy, float dz)
	{
		lines[0].w += dx;
		lines[1].w += dy;
		lines[2].w += dz;
	}

	void scale(float sx, float sy, float sz)
	{
		Matrix m;
		m.lines[0].x *= sx;
		m.lines[1].y *= sy;
		m.lines[2].z *= sz;
		*this = m * *this;
	}

	void rotate(float ax, float ay, float az)
	{
		rotateY(ay);
		rotateZ(az);
		rotateX(ax);
	}

	void rotateBackUp(Vertex back, Vertex up, float a)
	{
		back.normalize();
		up.normalize();
		Vertex right = up.cross(back);

		Matrix m1;
		m1.lines[0] = right;
		m1.lines[1] = up;
		m1.lines[2] = back;

		Matrix m2;
		m2.rotateY(a);

		Matrix m3;
		m3.lines[0] = Vertex(right.x, up.x, back.x, 0.0f);
		m3.lines[1] = Vertex(right.y, up.y, back.y, 0.0f);
		m3.lines[2] = Vertex(right.z, up.z, back.z, 0.0f);
		m3.lines[3] = Vertex(0.0f, 0.0f, 0.0f, 1.0f);

		*this = m3 * m2 * m1 * *this;
	}

	void rotateBackRight(Vertex back, Vertex right, float a)
	{
		back.normalize();
		right.normalize();
		Vertex up = back.cross(right);

		Matrix m1;
		m1.lines[0] = right;
		m1.lines[1] = up;
		m1.lines[2] = back;

		Matrix m2;
		m2.rotateY(a);

		Matrix m3;
		m3.lines[0] = Vertex(right.x, up.x, back.x, 0.0f);
		m3.lines[1] = Vertex(right.y, up.y, back.y, 0.0f);
		m3.lines[2] = Vertex(right.z, up.z, back.z, 0.0f);
		m3.lines[3] = Vertex(0.0f, 0.0f, 0.0f, 1.0f);

		*this = m3 * m2 * m1 * *this;
	}

	void alignBackUp(Vertex back, Vertex up)
	{
		back.normalize();
		up.normalize();
		Vertex right = up.cross(back);

		Matrix m;
		m.lines[0] = Vertex(right.x, up.x, back.x, 0.0f);
		m.lines[1] = Vertex(right.y, up.y, back.y, 0.0f);
		m.lines[2] = Vertex(right.z, up.z, back.z, 0.0f);
		m.lines[3] = Vertex(0.0f, 0.0f, 0.0f, 1.0f);

		*this = m * *this;
	}

	void alignBackRight(Vertex back, Vertex right)
	{
		back.normalize();
		right.normalize();
		Vertex up = back.cross(right);

		Matrix m;
		m.lines[0] = Vertex(right.x, up.x, back.x, 0.0f);
		m.lines[1] = Vertex(right.y, up.y, back.y, 0.0f);
		m.lines[2] = Vertex(right.z, up.z, back.z, 0.0f);
		m.lines[3] = Vertex(0.0f, 0.0f, 0.0f, 1.0f);

		*this = m * *this;
	}

	void rotateX(float a)
	{
		Matrix m;
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
		Matrix m;
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
		Matrix m;
		float c = cosf(a);
		float s = sinf(a);
		m.lines[0].x = c;
		m.lines[0].y = -s;
		m.lines[1].x = s;
		m.lines[1].y = c;
		*this = m * *this;
	}

	Matrix inverse3x3()
	{
		float d = lines[0].x*(lines[1].y*lines[2].z-lines[2].y*lines[1].z)
				- lines[0].y*(lines[1].x*lines[2].z-lines[1].z*lines[2].x)
                + lines[0].z*(lines[1].x*lines[2].y-lines[1].y*lines[2].x);

		Matrix m;
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

	Vertex operator * (Vertex v) const
	{
		v.w = 1.0f;
		Vertex l1 = lines[0] * v;
		Vertex l2 = lines[1] * v;
		Vertex l3 = lines[2] * v;

		return Vertex(l1.x + l1.y + l1.z + l1.w,
					  l2.x + l2.y + l2.z + l2.w,
					  l3.x + l3.y + l3.z + l3.w);
	}

	Matrix operator + (Matrix m) const
	{
		Matrix r;
		r.lines[0] = lines[0] + m.lines[0];
		r.lines[1] = lines[1] + m.lines[1];
		r.lines[2] = lines[2] + m.lines[2];
		r.lines[3] = lines[3] + m.lines[3];

		return r;
	}

	Matrix operator * (Matrix m) const
	{
		Matrix r;
		for (int i = 0; i < 4; i++) {
			Vertex vx = Vertex(lines[i].x, lines[i].x, lines[i].x, lines[i].x);
			Vertex vy = Vertex(lines[i].y, lines[i].y, lines[i].y, lines[i].y);
			Vertex vz = Vertex(lines[i].z, lines[i].z, lines[i].z, lines[i].z);
			Vertex vw = Vertex(lines[i].w, lines[i].w, lines[i].w, lines[i].w);
			r.lines[i] = vx * m.lines[0] + vy * m.lines[1] + vz * m.lines[2] + vw * m.lines[3];
		}

		return r;
	}
};

/*****************************************************************************/
namespace Geo{
	const Vertex up    = Vertex(0.0f, 1.0f, 0.0f);
	const Vertex down  = Vertex(0.0f, -1.0f, 0.0f);
	const Vertex front = Vertex(0.0f, 0.0f, -1.0f);
	const Vertex back  = Vertex(0.0f, 0.0f, 1.0f);
	const Vertex left  = Vertex(-1.0f, 0.0f, 0.0f);
	const Vertex right = Vertex(1.0f, 0.0f, 0.0f);
	const Vertex zero  = Vertex(0.0f, 0.0f, 0.0f);
}

#endif	//LE_GEOMETRY_SIMD_H
