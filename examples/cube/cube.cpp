/**
	\file cube.cpp
	\brief LightEngine 3D (examples): cube example
	\brief All platforms implementation
	\author Frederic Meslin (fred@fredslab.net)
	\twitter @marzacdev
	\website http://fredslab.net
*/

#include "engine/le3d.h"
#include "tools/timing.h"

#include <stdlib.h>
#include <stdio.h>

/*****************************************************************************/
int main()
{
	sys.initialize();
	LeGamePad::setup();

/** Create application objects */
	LeWindow	 window		= LeWindow("Le3d: cube example");
	LeDraw		 draw		= LeDraw(window.getContext());
	LeRenderer	 renderer	= LeRenderer();
	LeRasterizer rasterizer = LeRasterizer();

/** Load the assets (textures then 3D models) */
	bmpCache.loadDirectory("assets");
	meshCache.loadDirectory("assets");

/** Retrieve the 3D model */
	LeMesh * crate = meshCache.getMeshFromName("crate.obj");

/** Create three lights */
	LeLight light1(LE_LIGHT_DIRECTIONAL, LeColor::rgb(0xFF4040));
	LeLight light2(LE_LIGHT_DIRECTIONAL, LeColor::rgb(0x4040FF));
	LeLight light3(LE_LIGHT_AMBIENT, LeColor::rgb(0x404040));

	light1.axis = LeAxis(LeVertex(), LeVertex(1.0f, 0.0f, -1.0f));
	light2.axis = LeAxis(LeVertex(), LeVertex(-1.0f, 0.0f, -1.0f));

/** Set the renderer properties */
	renderer.setViewPosition(LeVertex(0.0f, 0.0f, 3.0f));
	renderer.updateViewMatrix();
	rasterizer.background = LeColor::rgb(0xC0C0FF);

/** Initialize the timing */
	timing.setup(60);
	timing.firstFrame();

/** Program main loop */
	
	while (sys.running && window.visible) {
	/** Process OS messages */
		sys.update();
		window.update();

	/** Wait for next frame */
		timing.waitNextFrame();

	/** Copy render frame to window context */
		draw.setPixels(rasterizer.getPixels());

	/** Update model transforms */
		crate->angle += LeVertex(0.1f, 2.0f, 0.0f);
		crate->updateMatrix();

	/** Light model */
		LeLight::black(crate);
		light1.shine(crate);
		light2.shine(crate);
		light3.shine(crate);

	/** Render the 3D model */
		renderer.render(crate);

	/** Draw the triangles */
		rasterizer.flush();
		rasterizer.rasterList(renderer.getTriangleList());
		renderer.flush();
	}

/** Make a screenshot */
	LeBmpFile screenshot("screenshot.bmp");
	screenshot.save(&rasterizer.frame);

	timing.lastFrame();
	LeGamePad::release();
	sys.terminate();
	return 0;
}