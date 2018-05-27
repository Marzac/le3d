/**
	\file physics.h
	\brief LightEngine 3D (tools): Collision routines
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

#ifndef PHYSICS_H
#define PHYSICS_H

	#include "../engine/le3d.h"

	namespace LePhysics {
		
		/**
			\enum PHYSICS_RECT_RESULT
			\brief Return values of the collideRectRect routine
		*/
		typedef enum{
			PHYSICS_RECT_NO_COL		= 0x00,		/**< No collision has occured */
			PHYSICS_RECT_LEFT_COL	= 0x01,		/**< Left side (negative x) collision has occured */
			PHYSICS_RECT_RIGHT_COL	= 0x02,		/**< Right side (positive x) collision has occured */
			PHYSICS_RECT_TOP_COL	= 0x03,		/**< Top side (negative y) collision has occured */
			PHYSICS_RECT_BOTTOM_COL = 0x04,		/**< Bottom side (positive y) collision has occured */
		}PHYSICS_RECT_RESULT;

		/**
			\enum PHYSICS_SPHERE_RESULT
			\brief Return values of the collideSphereSphere routine
		*/
		typedef enum{
			PHYSICS_SPHERE_NO_COL	= 0x00,		/**< No collision has occured */
			PHYSICS_SPHERE_SURFACE	= 0x01,		/**< Surface collision has occured */
		}PHYSICS_SPHERE_RESULT;

		/**
			\enum PHYSICS_BOX_RESULT
			\brief Return values of the collideSphereBox routine
		*/
		typedef enum{
			PHYSICS_BOX_NO_COL		= 0x00,		/**< No collision has occured */
			PHYSICS_BOX_SIDE		= 0x01,		/**< Side collision has occured */
			PHYSICS_BOX_EDGE		= 0x02,		/**< Edge collision has occured */
			PHYSICS_BOX_CORNER		= 0x03,		/**< Corner collision has occured */
		}PHYSICS_BOX_RESULT;
		
		/**
			\enum PHYSICS_MESH_RESULT
			\brief Return values of the collideSphereMesh routine
		*/
		typedef enum{
			PHYSICS_MESH_NO_COL		= 0x00,		/**< No collision has occured */
			PHYSICS_MESH_SIDE		= 0x01,		/**< Side collision has occured */
			PHYSICS_MESH_EDGE		= 0x02,		/**< Edge collision has occured */
		}PHYSICS_MESH_RESULT;

		/*****************************************************************************/
		/**
			\enum PHYSICS_BOX_TRACE_RESULT
			\brief Return values of the traceBox routine
		*/
		typedef enum{
			PHYSICS_BOX_TRACE_NO_INTER	= 0x00,			/**< No intersection has occured */
			PHYSICS_BOX_TRACE_LEFT		= 0x01,			/**< Intersection with left side (negative x) */
			PHYSICS_BOX_TRACE_RIGHT		= 0x02,			/**< Intersection with right side (positive x) */
			PHYSICS_BOX_TRACE_BOTTOM	= 0x03,			/**< Intersection with bottom side (negative y) */
			PHYSICS_BOX_TRACE_TOP		= 0x04,			/**< Intersection with top side (positive y) */
			PHYSICS_BOX_TRACE_BACK		= 0x05,			/**< Intersection with back side (negative z) */
			PHYSICS_BOX_TRACE_FRONT		= 0x06,			/**< Intersection with front side (positive z) */
		}PHYSICS_BOX_TRACE_RESULT;

		/*****************************************************************************/
		int collideRectRect(float &ansX, float &ansY, float srcX, float srcY, float srcW, float srcH, float dstX, float dstY, float dstW, float dstH);
		int collideSphereSphere(LeVertex &ans, LeVertex &contact, const LeVertex &pos, float srcRadius, float dstRadius);
		int collideSphereBox(LeVertex &ans, LeVertex &contact, const LeVertex &pos, float srcRadius, const LeVertex &dstSize);
		int collideSphereMesh(LeVertex &ans, LeVertex &contact, const LeVertex &pos, float srcRadius, const LeMesh * dstMesh);

		int traceSphere(const LeVertex &pos, float radius, const LeVertex &axis, float &distance);
		int traceBox(const LeVertex &pos, const LeVertex &size, const LeVertex &axis, float &distance);
		int traceMesh(const LeMesh * mesh, const LeAxis &axis, float &distance);
	
	}
	
#endif // PHYSICS_H
