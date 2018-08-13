/**
	\file geometry_scalar.h
	\brief LightEngine 3D: Vertex / axis / plane / matrix objects
	\brief All platforms implementation (without SIMD support)
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2018
	\version 1.7

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

#ifndef LE_GEOMETRY_SCALAR_H
#define LE_GEOMETRY_SCALAR_H

#include "global.h"
#include "config.h"

#include <math.h>

/*****************************************************************************/
/**
	\struct LeVertex
	\brief Represent a vertex in 3D space
**/
struct LeVertex
{
	float x, y, z, w;

	LeVertex()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
		w = 1.0f;
	}

	LeVertex(float px, float py, float pz)
	{
		x = px;
		y = py;
		z = pz;
		w = 1.0f;
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
		r.x = x + v.x;
		r.y = y + v.y;
		r.z = z + v.z;
		return r;
	}

	LeVertex operator += (LeVertex v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	LeVertex operator - (LeVertex v) const
	{
		LeVertex r;
		r.x = x - v.x;
		r.y = y - v.y;
		r.z = z - v.z;
		return r;
	}

	LeVertex operator - () const
	{
		LeVertex r;
		r.x = -x;
		r.y = -y;
		r.z = -z;
		return r;
	}

	LeVertex operator -= (LeVertex v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	LeVertex operator * (LeVertex v) const
	{
		LeVertex r;
		r.x = x * v.x;
		r.y = y * v.y;
		r.z = z * v.z;
		return r;
	}

	LeVertex operator / (LeVertex v) const
	{
		LeVertex r;
		r.x = x / v.x;
		r.y = y / v.y;
		r.z = z / v.z;
		return r;
	}

	LeVertex operator * (float v) const
	{
		LeVertex r;
		r.x = x * v;
		r.y = y * v;
		r.z = z * v;
		return r;
	}
	
	LeVertex operator / (float v) const
	{
		float iv = 1.0f / v;
		LeVertex r;
		r.x = x * iv;
		r.y = y * iv;
		r.z = z * iv;
		return r;
	}
	
	LeVertex operator *= (LeVertex v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}
	
	LeVertex operator /= (LeVertex v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}
	
	LeVertex operator *= (float v)
	{
		x *= v;
		y *= v;
		z *= v;
		return *this;
	}
	
	LeVertex operator /= (float v)
	{
		float iv = 1.0f / v;
		x *= iv;
		y *= iv;
		z *= iv;
		return *this;
	}
	
	bool operator == (LeVertex v)
	{
		return x == v.x && y == v.y && z == v.z;
	}

	float dot(LeVertex v) const
	{
		return x * v.x + y * v.y + z * v.z;
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
			x = y = z = 0.0f;
			w = 1.0f;
		}else{
			d = 1.0f / d;
			x *= d;
			y *= d;
			z *= d;
			w *= d;
		}
		return *this;
	}

	float norm() const
	{
		return sqrtf(dot(*this));
	}

};

/*****************************************************************************/
/**
	\struct LeAxis
	\brief Represent an axis in 3D space
**/
struct LeAxis
{
	LeVertex origin;	/**< Origin of the axis */
	LeVertex axis;		/**< Direction of the axis (normalized) */
	float norm;			/**< Length of the axis */

	LeAxis()
	{
		origin.x = 0.0f;
		origin.y = 0.0f;
		origin.z = 0.0f;
		axis.x = 0.0f;
		axis.y = 0.0f;
		axis.z = 1.0f;
		norm = 1.0f;
	}

	LeAxis(LeVertex v1, LeVertex v2)
	{
		origin.x = v1.x;
		origin.y = v1.y;
		origin.z = v1.z;
		float dx = v2.x - v1.x;
		float dy = v2.y - v1.y;
		float dz = v2.z - v1.z;
		norm = sqrtf(dx * dx + dy * dy + dz * dz);
		axis.x = dx / norm;
		axis.y = dy / norm;
		axis.z = dz / norm;
	}
};

/*****************************************************************************/
/**
	\struct LePlane
	\brief Represent a plane in 3D space
**/
struct LePlane
{
	LeAxis xAxis;
	LeAxis yAxis;
	LeAxis zAxis;

	LePlane()
	{
		xAxis.axis.x = 1.0f;
		xAxis.axis.y = 0.0f;
		xAxis.axis.z = 0.0f;

		yAxis.axis.x = 0.0f;
		yAxis.axis.y = 1.0f;
		yAxis.axis.z = 0.0f;

		zAxis.axis.x = 0.0f;
		zAxis.axis.y = 0.0f;
		zAxis.axis.z = 1.0f;
	}

	LePlane(LeVertex v1, LeVertex v2, LeVertex v3) :
		xAxis(LeAxis(v1, v2)), yAxis(LeAxis(v1, v3))
	{

		LeVertex tv1, tv2;
		tv1.x = xAxis.axis.z * yAxis.axis.y;
		tv1.y = xAxis.axis.x * yAxis.axis.z;
		tv1.z = xAxis.axis.y * yAxis.axis.x;
		tv2.x = xAxis.axis.y * yAxis.axis.z;
		tv2.y = xAxis.axis.z * yAxis.axis.x;
		tv2.z = xAxis.axis.x * yAxis.axis.y;
		zAxis = LeAxis(tv1, tv2);
		zAxis.origin.x = v1.x;
		zAxis.origin.y = v1.y;
		zAxis.origin.z = v1.z;
	}
};

/*****************************************************************************/
/**
	\struct LeMatrix
	\brief Represent a 4x4 matrix to handle 3D transforms
**/
struct LeMatrix
{
	float mat[4][4];

	LeMatrix()
	{
		identity();
	}

	void identity()
	{
		mat[0][0] = 1.0f;
		mat[0][1] = 0.0f;
		mat[0][2] = 0.0f;
		mat[0][3] = 0.0f;

		mat[1][0] = 0.0f;
		mat[1][1] = 1.0f;
		mat[1][2] = 0.0f;
		mat[1][3] = 0.0f;

		mat[2][0] = 0.0f;
		mat[2][1] = 0.0f;
		mat[2][2] = 1.0f;
		mat[2][3] = 0.0f;

		mat[3][0] = 0.0f;
		mat[3][1] = 0.0f;
		mat[3][2] = 0.0f;
		mat[3][3] = 1.0f;
	}

	void zero()
	{
		for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			mat[i][j] = 0;
	}

	void transpose()
	{
		LeMatrix m;
		for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			m.mat[j][i] = mat[i][j];
		*this = m;
	}

	void translate(LeVertex d)
	{
		mat[0][3] += d.x;
		mat[1][3] += d.y;
		mat[2][3] += d.z;
	}

	void scale(LeVertex s)
	{
		LeMatrix m;
		m.mat[0][0] *= s.x;
		m.mat[1][1] *= s.y;
		m.mat[2][2] *= s.z;
		*this = m * *this;
	}

/*****************************************************************************/
	void rotateEulerXYZ(LeVertex a)
	{
		rotateX(a.x);
		rotateY(a.y);
		rotateZ(a.z);
	}

	void rotateEulerXZY(LeVertex a)
	{
		rotateX(a.x);
		rotateZ(a.z);
		rotateY(a.y);
	}
	
	void rotateEulerYZX(LeVertex a)
	{
		rotateY(a.y);
		rotateZ(a.z);
		rotateX(a.x);
	}

	void rotateEulerYXZ(LeVertex a)
	{
		rotateY(a.y);
		rotateX(a.x);
		rotateZ(a.z);
	}

	void rotateEulerZXY(LeVertex a)
	{
		rotateZ(a.z);
		rotateX(a.x);
		rotateY(a.y);
	}

	void rotateEulerZYX(LeVertex a)
	{
		rotateZ(a.z);
		rotateY(a.y);
		rotateX(a.x);
	}

	void rotateX(float a)
	{
		float c = cosf(a);
		float s = sinf(a);

		LeMatrix m;
		m.mat[1][1] = c;
		m.mat[1][2] = -s;
		m.mat[2][1] = s;
		m.mat[2][2] = c;

		*this = m * *this;
	}

	void rotateY(float a)
	{
		float c = cosf(a);
		float s = sinf(a);
		
		LeMatrix m;
		m.mat[0][0] = c;
		m.mat[0][2] = s;
		m.mat[2][0] = -s;
		m.mat[2][2] = c;

		*this = m * *this;
	}

	void rotateZ(float a)
	{
		float c = cosf(a);
		float s = sinf(a);

		LeMatrix m;
		m.mat[0][0] = c;
		m.mat[0][1] = -s;
		m.mat[1][0] = s;
		m.mat[1][1] = c;

		*this = m * *this;
	}

	void rotate(LeVertex axis, float angle)
	{
		float c = cosf(angle);
		float s = sinf(angle);
		float d = 1.0f - c;

		LeMatrix m;
		m.mat[0][0] = c + axis.x * axis.x * d;
		m.mat[1][0] = axis.y * axis.x * d + axis.z * s;
		m.mat[2][0] = axis.z * axis.x * d - axis.y * s;
		m.mat[3][0] = 0.0f;

		m.mat[0][1] = axis.x * axis.y * d - axis.z * s;
		m.mat[1][1] = c + axis.y * axis.y * d;
		m.mat[2][1] = axis.z * axis.y * d + axis.x * s;
		m.mat[3][1] = 0.0f;

		m.mat[0][2] = axis.x * axis.z * d + axis.y * s;
		m.mat[1][2] = axis.y * axis.z * d - axis.x * s;
		m.mat[2][2] = c + axis.z * axis.z * d;
		m.mat[3][2] = 0.0f;

		m.mat[0][3] = 0.0f;
		m.mat[1][3] = 0.0f;
		m.mat[2][3] = 0.0f;
		m.mat[3][3] = 1.0f;
		*this = m * *this;
	}

/*****************************************************************************/
	void toEulerZYX(LeVertex & angle)
	{
		float l = sqrtf(mat[0][0] * mat[0][0] + mat[1][0] * mat[1][0]);
		if (l > 1e-6) {
			angle.x = atan2f(mat[2][1], mat[2][2]);
			angle.y = atan2f(-mat[2][0], l);
			angle.z = atan2f(mat[1][0], mat[0][0]);
		}else{
			angle.x = atan2f(-mat[1][2], mat[1][1]);
			angle.y = atan2f(-mat[2][0], l);
			angle.z = 0.0f;
		}
	}

/*****************************************************************************/
	void alignBackUp(LeVertex back, LeVertex up)
	{
		back.normalize();
		up.normalize();
		LeVertex right = up.cross(back);

		LeMatrix m;
		m.mat[0][0] = right.x;
		m.mat[1][0] = right.y;
		m.mat[2][0] = right.z;
		m.mat[3][0] = 0.0f;

		m.mat[0][1] = up.x;
		m.mat[1][1] = up.y;
		m.mat[2][1] = up.z;
		m.mat[3][1] = 0.0f;

		m.mat[0][2] = back.x;
		m.mat[1][2] = back.y;
		m.mat[2][2] = back.z;
		m.mat[3][2] = 0.0f;

		m.mat[0][3] = 0.0f;
		m.mat[1][3] = 0.0f;
		m.mat[2][3] = 0.0f;
		m.mat[3][3] = 1.0f;
		*this = m * *this;
	}

	void alignBackRight(LeVertex back, LeVertex right)
	{
		back.normalize();
		right.normalize();
		LeVertex up = back.cross(right);

		LeMatrix m;
		m.mat[0][0] = right.x;
		m.mat[1][0] = right.y;
		m.mat[2][0] = right.z;
		m.mat[3][0] = 0.0f;

		m.mat[0][1] = up.x;
		m.mat[1][1] = up.y;
		m.mat[2][1] = up.z;
		m.mat[3][1] = 0.0f;

		m.mat[0][2] = back.x;
		m.mat[1][2] = back.y;
		m.mat[2][2] = back.z;
		m.mat[3][2] = 0.0f;

		m.mat[0][3] = 0.0f;
		m.mat[1][3] = 0.0f;
		m.mat[2][3] = 0.0f;
		m.mat[3][3] = 1.0f;

		*this = m * *this;
	}

/*****************************************************************************/
	LeMatrix inverse3x3()
	{
		float d = mat[0][0]*(mat[1][1]*mat[2][2]-mat[2][1]*mat[1][2])
				- mat[0][1]*(mat[1][0]*mat[2][2]-mat[1][2]*mat[2][0])
				+ mat[0][2]*(mat[1][0]*mat[2][1]-mat[1][1]*mat[2][0]);

		LeMatrix m;
		if (d == 0.0f) {m.zero(); return m;}
		d = 1.0f / d;

		m.mat[0][0] =  (mat[1][1] * mat[2][2] - mat[2][1] * mat[1][2]) * d;
		m.mat[0][1] = -(mat[0][1] * mat[2][2] - mat[2][1] * mat[0][2]) * d;
		m.mat[0][2] =  (mat[0][1] * mat[1][2] - mat[1][1] * mat[0][2]) * d;

		m.mat[1][0] = -(mat[1][0] * mat[2][2] - mat[2][0] * mat[1][2]) * d;
		m.mat[1][1] =  (mat[0][0] * mat[2][2] - mat[2][0] * mat[0][2]) * d;
		m.mat[1][2] = -(mat[0][0] * mat[1][2] - mat[1][0] * mat[0][2]) * d;

		m.mat[2][0] =  (mat[1][0] * mat[2][1] - mat[2][0] * mat[1][1]) * d;
		m.mat[2][1] = -(mat[0][0] * mat[2][1] - mat[2][0] * mat[0][1]) * d;
		m.mat[2][2] =  (mat[0][0] * mat[1][1] - mat[1][0] * mat[0][1]) * d;

		return m;
	}

/*****************************************************************************/
	LeVertex operator * (LeVertex v) const
	{
		float x = v.x * mat[0][0] + v.y * mat[0][1] + v.z * mat[0][2] + mat[0][3];
		float y = v.x * mat[1][0] + v.y * mat[1][1] + v.z * mat[1][2] + mat[1][3];
		float z = v.x * mat[2][0] + v.y * mat[2][1] + v.z * mat[2][2] + mat[2][3];
		return LeVertex(x, y, z);
	}

	LeMatrix operator + (LeMatrix m) const
	{
		LeMatrix r;
		for (int i = 0; i < 4; i++) {
			r.mat[i][0] = mat[i][0] + m.mat[i][0];
			r.mat[i][1] = mat[i][1] + m.mat[i][1];
			r.mat[i][2] = mat[i][2] + m.mat[i][2];
			r.mat[i][3] = mat[i][3] + m.mat[i][3];
		}
		return r;
	}

	LeMatrix operator * (LeMatrix m) const
	{
		LeMatrix r;
		for (int i = 0; i < 4; i++) {
			r.mat[i][0] = mat[i][0] * m.mat[0][0] + mat[i][1] * m.mat[1][0] + mat[i][2] * m.mat[2][0] + mat[i][3] * m.mat[3][0];
			r.mat[i][1] = mat[i][0] * m.mat[0][1] + mat[i][1] * m.mat[1][1] + mat[i][2] * m.mat[2][1] + mat[i][3] * m.mat[3][1];
			r.mat[i][2] = mat[i][0] * m.mat[0][2] + mat[i][1] * m.mat[1][2] + mat[i][2] * m.mat[2][2] + mat[i][3] * m.mat[3][2];
			r.mat[i][3] = mat[i][0] * m.mat[0][3] + mat[i][1] * m.mat[1][3] + mat[i][2] * m.mat[2][3] + mat[i][3] * m.mat[3][3];
		}
		return r;
	}
};

/*****************************************************************************/
namespace LePrimitives {
	const LeVertex up	 = LeVertex(0.0f, 1.0f, 0.0f);
	const LeVertex down	 = LeVertex(0.0f, -1.0f, 0.0f);
	const LeVertex front = LeVertex(0.0f, 0.0f, -1.0f);
	const LeVertex back	 = LeVertex(0.0f, 0.0f, 1.0f);
	const LeVertex left	 = LeVertex(-1.0f, 0.0f, 0.0f);
	const LeVertex right = LeVertex(1.0f, 0.0f, 0.0f);
	const LeVertex zero	 = LeVertex(0.0f, 0.0f, 0.0f);
}

#endif	//LE_GEOMETRY_SCALAR_H


/*

void rotateBackUp(LeVertex back, LeVertex up, float a)
{
back.normalize();
up.normalize();
LeVertex right = up.cross(back);

LeMatrix m1;
m1.mat[0][0] = right.x;
m1.mat[0][1] = right.y;
m1.mat[0][2] = right.z;
m1.mat[0][3] = right.w;

m1.mat[1][0] = up.x;
m1.mat[1][1] = up.y;
m1.mat[1][2] = up.z;
m1.mat[1][3] = up.w;

m1.mat[2][0] = back.x;
m1.mat[2][1] = back.y;
m1.mat[2][2] = back.z;
m1.mat[2][3] = back.w;

LeMatrix m2;
m2.rotateY(a);

LeMatrix m3;
m3.mat[0][0] = right.x;
m3.mat[1][0] = right.y;
m3.mat[2][0] = right.z;
m3.mat[3][0] = 0.0f;

m3.mat[0][1] = up.x;
m3.mat[1][1] = up.y;
m3.mat[2][1] = up.z;
m3.mat[3][1] = 0.0f;

m3.mat[0][2] = back.x;
m3.mat[1][2] = back.y;
m3.mat[2][2] = back.z;
m3.mat[3][2] = 0.0f;

m3.mat[0][3] = 0.0f;
m3.mat[1][3] = 0.0f;
m3.mat[2][3] = 0.0f;
m3.mat[3][3] = 1.0f;

*this = m3 * m2 * m1 * *this;
}

void rotateBackRight(LeVertex back, LeVertex right, float a)
{
back.normalize();
right.normalize();
LeVertex up = back.cross(right);

LeMatrix m1;
m1.mat[0][0] = right.x;
m1.mat[0][1] = right.y;
m1.mat[0][2] = right.z;
m1.mat[0][3] = right.w;

m1.mat[1][0] = up.x;
m1.mat[1][1] = up.y;
m1.mat[1][2] = up.z;
m1.mat[1][3] = up.w;

m1.mat[2][0] = back.x;
m1.mat[2][1] = back.y;
m1.mat[2][2] = back.z;
m1.mat[2][3] = back.w;

LeMatrix m2;
m2.rotateY(a);

LeMatrix m3;
m3.mat[0][0] = right.x;
m3.mat[1][0] = right.y;
m3.mat[2][0] = right.z;
m3.mat[3][0] = 0.0f;

m3.mat[0][1] = up.x;
m3.mat[1][1] = up.y;
m3.mat[2][1] = up.z;
m3.mat[3][1] = 0.0f;

m3.mat[0][2] = back.x;
m3.mat[1][2] = back.y;
m3.mat[2][2] = back.z;
m3.mat[3][2] = 1.0f;

m3.mat[3][3] = 0.0f;
m3.mat[3][3] = 0.0f;
m3.mat[3][3] = 0.0f;
m3.mat[3][3] = 1.0f;

*this = m3 * m2 * m1 * *this;
}

*/