/**
	\file le3d.h
	\brief LightEngine 3D: General include file
	\brief All platforms implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
	\copyright Frederic Meslin 2015 - 2018
	\version 1.6

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

#ifndef LE_LE3D_H
#define LE_LE3D_H

/*****************************************************************************/
/** Engine class includes */
/*****************************************************************************/
	#include "global.h"
	#include "config.h"

	#include "system.h"
	#include "window.h"
	#include "draw.h"
	#include "renderer.h"
	#include "rasterizer.h"
	#include "gamepad.h"

	#include "geometry.h"
	#include "trilist.h"
	#include "verlist.h"

	#include "light.h"
	#include "mesh.h"
	#include "bset.h"
	#include "bitmap.h"

	#include "bmpfile.h"
	#include "objfile.h"
	#include "bmpcache.h"
	#include "meshcache.h"

#endif // LE_LE3D_H

/*****************************************************************************/
/* Doxywizard specific */
/*****************************************************************************/
/**
* \mainpage le3d - LightEngine 3D
* \section intro_sec C++ 3D software engine
* **Version 1.6 - 09/05/2018**
*
* Currently supported platforms:
* - Microsoft Windows XP, 7, 8 and 10
* - Linux based operating systems
* - MacOS X
*
* **Source code / examples / documentation**<br>
* Copyright (c) 2015-2018 Fr&eacute;d&eacute;ric Meslin<br>
* **Twitter:**  marzacdev<br>
* **Email:**  fred@fredslab.net<br>
*
* **CMake scripts / MacOS support**<br>
* Andreas Streichard<br>
* **Twitter:** m0ppers<br>
* **Email:** andreas@mop.koeln<br>
*
* **Exceptions:**<br>
* __Mesh textures__<br>
* In the example folder, one or more textures on the 3D models
* have been created with photographs from Textures.com.
* These photographs may not be redistributed by default;
* Please visit http://www.textures.com for more information.
*
* __Skybox texture__<br>
* In the example folder, the meadow skybox has been created by
* Emil "Humus" Persson and it is licenced under the Creative Commons
* Attribution 3.0 Unported License.
* Please visit http://www.humus.name/index.php?page=Textures for more information.
*
* **Fred's Lab:**<br>
* A project maintained by Fred's Lab<br>
* **Website:** www.fredslab.net <br>
*
* This is open source software released under the MIT License, please refer to LICENCE file for more information.
*
* # Renderer (Portable C++ / ASM (SSE2))
* The renderer:
* - Renders meshes
* - Renders billboard sets
* - Does simple transformations (rotation, scaling, translation)
* - Performs 3D clipping
* - Performs 2D clipping
* - Performs projection
* - Does backculling
* - Performs Z-sorting
*
*
* The rasterizer:
* - Draws textured triangles
* - Handles mipmaping
* - Handles alpha blending
* - Performs perspective correction
* - Applies solid color per triangle
* - Does not perform texture filtering
* - Does not draw anti-aliased edges
* - Align vertex coordinates to nearest pixel coordinates
*
*
* The lighting system:
* - Handles point lights
* - Handles directionnal lights
* - Handles ambient lights
*
*
* # Backends (Native OS code)
* window / draw / gamepad / timing / system
*
* The backend - Windows version:
* - Handles windows creation / management
* - Handles mouse events
* - Handles keyboard events
* - Handles graphic contexts with GDI
* - Handles joysticks with rumble support
*   (with Microsoft XInput V1.3 drivers)
* - Supports Windows XP / 7 / 8 and 10 OS
* - Supports fullscreen mode
*
* The backend - Linux version:
* - Handles windows creation / management (with X11)
* - Handles mouse events (with X11)
* - Handles keyboard events (with X11)
* - Handles graphic contexts (with X11)
* - Handles joysticks with rumble support
*   (with evdev interface)
* - Supports virtually all Linux based OS
*
*
* The backend - MacOS version:
* - Similar to Linux version
* - Needs X11 (XQuartz) for MacOS
* - No native COCOA support
* - No joysticks support yet (soon)
*
* # File formats
* Supported bitmap / texture formats:
* - Uncompressed 24bit RGB windows bitmap
* - Uncompressed 32bit RGBA windows bitmap
* - Import & export functions
*
* Supported 3D model formats:
* - Wavefront OBJ
* - Import & export functions
*
* # Limitations
* - No Z-buffer
* - No multilayer texturing
* - No shaders
* - No fog
* - No antialiasing
* - Single core support
*
* # Comparison
* It renders graphics a bit better than a Sony Playstation one.
*
*/
