/**
	\file solid.h
	\brief LightEngine 3D (tools): Basic solid point physics simulation
	\brief All platforms implementation
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

#ifndef SOLID_H
#define SOLID_H

#include "../engine/le3d.h"
#include <stdint.h>

/*****************************************************************************/
// Physical constants
#define VISCOSITY_OIL		1500fe-3
#define VISCOSITY_WATER		1.0fe-3
#define VISCOSITY_AIR		0.02fe-3

/*****************************************************************************/
/**
	\enum LE_SOLID_SYSTEM
	\brief System coordinate use for calculation
*/
enum LE_SOLID_SYSTEM {
	LE_SOLID_SYSTEM_ABSOLUTE		= 0,		/**< Absolute coordinates (world coordinates) */
	LE_SOLID_SYSTEM_RELATIVE		= 1,		/**< Relative coordinates (object coordinates) */
};

/*****************************************************************************/
/**
	\class Solid
	\brief Basic solid point physics object
*/
class LeSolid {
public:
	LeSolid();
	~LeSolid();

	void init(const LeVertex & pos);
	void stop();

	void configure(float mass, float radius);

	void glueMoveAxis(LeVertex axis);
	void glueRotateAxis(LeVertex axis);

    void collideSolid(LeVertex contact, LeSolid & collider);
    void collideHard(LeVertex contact, LeVertex normal, float rigidity);

	void applyFriction(LeVertex normal, float friction);

    void applyForce(LeVertex force);
	void applyForceRight(float force);
	void applyForceUp(float force);
	void applyForceBack(float force);
	
	void applyTorque(LeVertex torque);
	void applyTorqueRight(float torque);
	void applyTorqueUp(float torque);
	void applyTorqueBack(float torque);

	void applyForcePoint(LeVertex point, LeVertex force);
	void applyTorquePoint(LeVertex point, LeVertex torque);
	
	void update(float dt);
	
private:
    void applyGravity();
    void applyGround();

	void computeVectors();
	void computeFriction(float dt);

public:
    float adhesion;
    float rigidity;
    float gravity;

    float groundLevel;
	float groundFriction;
	
	LE_SOLID_SYSTEM fluidSystem;
	LeVertex fluid1Move;
	LeVertex fluid1Rotate;
	LeVertex fluid2Move;
	LeVertex fluid2Rotate;

	LeVertex glueMove;
	LeVertex glueRotate;

    LeVertex pos;
    LeVertex posSpeed;
    LeVertex posAccel;

    LeVertex rot;
    LeVertex rotSpeed;
    LeVertex rotAccel;

    LeVertex rightVector;
    LeVertex upVector;
    LeVertex backVector;
		
private:
	float lastdt;
	LeVertex frictionAccel;

	float mass, imass;
    float inertia, iinertia;
    float radius;
};

#endif
