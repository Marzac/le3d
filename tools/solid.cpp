/**
	\file solid.cpp
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

#include "solid.h"

#include "../engine/le3d.h"

#include <math.h>
#include <float.h>

/*****************************************************************************/
LeSolid::LeSolid() :
	adhesion(0.5f), rigidity(1.0f),
	gravity(9.8f),
	groundLevel(0.0f), groundFriction(0.0f),
	fluidSystem(LE_SOLID_SYSTEM_RELATIVE),
	fluid1Move(), fluid1Rotate(), fluid2Move(), fluid2Rotate(),
	glueMove(1.0f, 1.0f, 1.0f),
	glueRotate(1.0f, 1.0f, 1.0f),
    pos(), posSpeed(), posAccel(),
    rot(), rotSpeed(), rotAccel(),
	rightVector(), upVector(), backVector()
{
    init(LeVertex());
    configure(1.0f, 1.0f);
}

LeSolid::~LeSolid()
{

}

/*****************************************************************************/
void LeSolid::init(const LeVertex & pos)
{
	stop();
	this->pos = pos;
	this->rot = LeVertex();
	rightVector = LePrimitives::right;
	upVector = LePrimitives::up;
	backVector = LePrimitives::back;
}

void LeSolid::stop()
{
	posSpeed = LeVertex();
	posAccel = LeVertex();
	rotSpeed = LeVertex();
	rotAccel = LeVertex();
}

void LeSolid::configure(float mass, float radius)
{
	if (mass <= 0.0f) return;
	if (radius <= 0.0f) return;

	this->radius = radius;
	this->mass = mass;
		
	inertia = 2.0f / 5.0f * mass * radius * radius;
	imass = 1.0f / mass;
	iinertia = 1.0f / inertia;
}

/*****************************************************************************/
void LeSolid::applyFriction(LeVertex normal, float coefficient)
{
	LeVertex sn = posSpeed;
	sn.normalize();
	frictionAccel -= sn * (sn.dot(LePrimitives::down) * mass * coefficient);
}

/*****************************************************************************/
void LeSolid::applyForce(LeVertex force)
{
    posAccel += force * imass;
}

void LeSolid::applyForceRight(float force)
{
	posAccel += rightVector * (force * imass);
}

void LeSolid::applyForceUp(float force)
{
	posAccel += upVector * (force * imass);
}

void LeSolid::applyForceBack(float force)
{
	posAccel += backVector * (force * imass);
}

void LeSolid::applyForcePoint(LeVertex point, LeVertex force)
{
	posAccel += force * imass;
	//LeVertex cv = point - pos;
	//rotAccel += cv.cross(force) * (r2d * iinertia);
}

/*****************************************************************************/
void LeSolid::applyTorque(LeVertex torque)
{
    rotAccel += torque * iinertia;
}

void LeSolid::applyTorqueRight(float torque)
{
	rotAccel += rightVector * (torque * iinertia);
}

void LeSolid::applyTorqueUp(float torque)
{
	rotAccel += upVector * (torque * iinertia);
}

void LeSolid::applyTorqueBack(float torque)
{
	rotAccel += backVector * (torque * iinertia);
}

void LeSolid::applyTorquePoint(LeVertex point, LeVertex torque)
{
	rotAccel += torque * iinertia;
	LeVertex cv = point - pos;
	posAccel += cv.cross(torque) * (d2r * iinertia);
}

/*****************************************************************************/
void LeSolid::computeVectors()
{
	LeMatrix m;
	m.rotateEulerYXZ(rot * d2r);
	rightVector = m * LePrimitives::right;
	upVector = m * LePrimitives::up;
	backVector = m * LePrimitives::back;
}

/*****************************************************************************/
void LeSolid::computeFriction(float dt)
{
	LeVertex posSpeedRelative;
	if (fluidSystem == LE_SOLID_SYSTEM_RELATIVE) {
		posSpeedRelative.x = posSpeed.dot(rightVector);
		posSpeedRelative.y = posSpeed.dot(upVector);
		posSpeedRelative.z = posSpeed.dot(backVector);
	}else posSpeedRelative = posSpeed;
	LeVertex psn = posSpeedRelative;
	psn.normalize();

	LeVertex rotSpeedRelative;
	if (fluidSystem == LE_SOLID_SYSTEM_RELATIVE) {
		rotSpeedRelative.x = rotSpeed.dot(rightVector);
		rotSpeedRelative.y = rotSpeed.dot(upVector);
		rotSpeedRelative.z = rotSpeed.dot(backVector);
	}else rotSpeedRelative = rotSpeed;

	printf("Speed %f %f %f\n", posSpeedRelative.x, posSpeedRelative.y, posSpeedRelative.z);

	LeVertex rsn = rotSpeedRelative;
	rsn.normalize();

// Movement
	LeVertex moveAccel = (psn * fluid1Move + posSpeedRelative * fluid2Move + frictionAccel) * imass * dt;
	if (cmabs(moveAccel.x) > cmabs(posSpeedRelative.x)) moveAccel.x = posSpeedRelative.x;
	if (cmabs(moveAccel.y) > cmabs(posSpeedRelative.y)) moveAccel.y = posSpeedRelative.y;
	if (cmabs(moveAccel.z) > cmabs(posSpeedRelative.z)) moveAccel.z = posSpeedRelative.z;
	if (fluidSystem == LE_SOLID_SYSTEM_ABSOLUTE) posSpeed -= moveAccel;
	else posSpeed -= rightVector * moveAccel.x + upVector * moveAccel.y + backVector * moveAccel.z;

// Rotation
	LeVertex rotateAccel = (rsn * fluid1Rotate + rotSpeedRelative * fluid2Rotate) * iinertia * dt;
	if (cmabs(rotateAccel.x) > cmabs(rotSpeedRelative.x)) rotateAccel.x = rotSpeedRelative.x;
	if (cmabs(rotateAccel.y) > cmabs(rotSpeedRelative.y)) rotateAccel.y = rotSpeedRelative.y;
	if (cmabs(rotateAccel.z) > cmabs(rotSpeedRelative.z)) rotateAccel.z = rotSpeedRelative.z;
	if (fluidSystem == LE_SOLID_SYSTEM_ABSOLUTE) rotSpeed -= rotateAccel;
	else rotSpeed -= rightVector * rotateAccel.x + upVector * rotateAccel.y + backVector * rotateAccel.z;
}

/*****************************************************************************/
void LeSolid::applyGravity()
{
    posAccel.y -= gravity;
}

void LeSolid::applyGround()
{
	float rl = groundLevel + radius;
	if (pos.y > rl) return;
    pos.y = rl;
	posSpeed.y = 0.0f;
	posAccel.y = 0.0f;

    //posAccel.y -= posAccel.y * (1.0f + rigidity);
	//applyFriction(LePrimitives::up, groundFriction);
}

/*****************************************************************************/
void LeSolid::update(float dt)
{
	computeVectors();

	applyGravity();
	applyGround();

	pos += posSpeed * dt;
	rot += rotSpeed * dt;
	posSpeed += posAccel * dt;
	posSpeed *= glueMove;
	rotSpeed += rotAccel * dt;
	rotSpeed *= glueRotate;

	computeFriction(dt);

	posAccel = LeVertex();
	rotAccel = LeVertex();
	frictionAccel = LeVertex();

	lastdt = dt;
}

/*****************************************************************************/
void LeSolid::collideSolid(LeVertex contact, LeSolid & collider)
{
    LeVertex contactForce = posSpeed * mass;
    posAccel -= contactForce * (imass * rigidity);
    collider.posAccel += contactForce * (collider.imass * collider.rigidity);

    LeVertex cv = contact - collider.pos;
    if (cv.dot(contactForce) < 0.0f)
    {
        float mc = r2d * collider.iinertia * collider.adhesion;
        collider.rotAccel += cv.cross(contactForce) * mc;
    }
}

/******************************************** *********************************/
void LeSolid::collideHard(LeVertex contact, LeVertex normal, float rigidity)
{
	LeVertex cv = contact - pos;
    LeVertex pointForce = (posSpeed * mass + cv.cross(rotSpeed) * d2r * inertia) / lastdt;
	LeVertex normalForce = normal * normal.dot(pointForce);
    
	normalForce = normalForce * (1.0f + rigidity);

/** Apply force on self */
   // rotSpeed -= cv.cross(normalForce) * (r2d * iinertia * lastdt);
    posSpeed -= normalForce * (imass * lastdt);
}
