/**
	\file bset.h
	\brief LightEngine 3D: Billboard set container and manipulator
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

#ifndef LE_BSET_H
#define LE_BSET_H

#include "global.h"
#include "config.h"

#include "color.h"
#include "geometry.h"

/*****************************************************************************/
typedef enum {
	LE_BSET_EXIST 		= 0x01,		/**< Billboard exist */
} LE_BSET_FLAGS;

/*****************************************************************************/
/**
	\class LeBSet
	\brief Contain and manage a billboard set
*/
class LeBSet
{
public:
	LeBSet();
	LeBSet(int noBillboards);
	~LeBSet();

	void shadowCopy(LeBSet * copy) const;
	void copy(LeBSet * copy) const;
	
	void basic();
	void clear();
	
	void allocate(int noBillboards);
	void deallocate();

	void transform(const LeMatrix &matrix);
	void setMatrix(const LeMatrix &matrix);
	void updateMatrix();

// Overall positioning
	LeMatrix view;			/**< View matrix of billboard set */
	LeVertex pos;			/**< Position of billboard set */
	LeVertex scale;			/**< Scaling of billboard set */
	LeVertex angle;			/**< Absolute angle of billboard set (in degrees) */

// Static billboards data
	LeVertex * places;		/**< Position per billboard */ 
	float * sizes;			/**< Size (x, y) per billboard */
	LeColor * colors;		/**< Color per billboard */
	int * texSlots;			/**< Texture slot per billboard */
	int * flags;			/**< Flag per billboard */
	
	int noBillboards;		/**< Number of allocated billboards */

// Computed billboards data
	LeColor * shades;		/**< Shade color per billboard (lighting) */
	bool allocated;			/**< Has data been allocated */
};

#endif // LE_BSET_H
