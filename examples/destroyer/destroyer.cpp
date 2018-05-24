/**
	\file destroyer.cpp
	\brief LightEngine 3D (examples): destroyer example
	\brief All platforms implementation
	\author Frederic Meslin (fred@fredslab.net)
	\brief Destroyer example (code and assets)
	\brief All rights reserved to Frédéric Meslin
	
	\brief This specific software (= code and assets) is
	\brief provided for educational purposes only and is
	\brief not licensed under the MIT license.
	
	\brief Reuse of the code and assets without the explicit
	\brief content of the original author is prohibited.

	\twitter @marzacdev
	\website http://fredslab.net
*/

/*****************************************************************************/
#include "engine/le3d.h"
#include "tools/physics.h"
#include "tools/timing.h"

#include <stdlib.h>
#include <stdio.h>
#include <float.h>

/*****************************************************************************/
#define FPS_DESIRED					60
#define ORBITAL_SPEED_MAX			0.75f
#define DISTANCE_SPEED_MAX			1.0f
#define SHUTTLE_DISTANCE_MIN		50.0f
#define SHUTTLE_DISTANCE_MAX		200.0f
#define BULLETS_PER_ROUND			4
#define BULLETS_MAX					256
#define BULLETS_LIFE				12.0f
#define SPARKS_PER_EXPLOSION		64
#define EXPLOSIONS_LIFE				4.0f
#define EXPLOSIONS_MAX				16
#define SHAKE_TIME					0.5f
#define PLAYER_WEAPON_RELOAD		0.33f
#define LAUNCHER_WEAPON_RELOAD		0.33f

const int resoX = 1024;
const int resoY = 768;
#define uint unsigned int

/*****************************************************************************/
/** Launcher positions */
const LeVertex launchersPos[] = {
// Facing up
	LeVertex(-9.0f, 15.0f, 9.50f) + LeVertex(0.0f, -12.5f, 27.5f),
	LeVertex(-3.0f, 15.0f, 9.50f) + LeVertex(0.0f, -12.5f, 27.5f),
	LeVertex( 3.0f, 15.0f, 9.50f) + LeVertex(0.0f, -12.5f, 27.5f),
	LeVertex( 9.0f, 15.0f, 9.50f) + LeVertex(0.0f, -12.5f, 27.5f),

	LeVertex( 9.0f, 17.5f, -7.50f) + LeVertex(0.0f, -12.5f, 27.5f),
	LeVertex( 3.0f, 17.5f, -7.50f) + LeVertex(0.0f, -12.5f, 27.5f),
	LeVertex(-3.0f, 17.5f, -7.50f) + LeVertex(0.0f, -12.5f, 27.5f),
	LeVertex(-9.0f, 17.5f, -7.50f) + LeVertex(0.0f, -12.5f, 27.5f),

	LeVertex(15.5f, 30.0f, -13.25f) + LeVertex(0.0f, -12.5f, 27.5f),

	LeVertex(0.0f, 28.0f, -54.0f) + LeVertex(0.0f, -12.5f, 27.5f),
	LeVertex(0.0f, 28.0f, -60.0f) + LeVertex(0.0f, -12.5f, 27.5f),
	LeVertex(0.0f, 28.0f, -60.0f) + LeVertex(0.0f, -12.5f, 27.5f),

	LeVertex( 15.5f, 25.0f, -54.0f) + LeVertex(0.0f, -12.5f, 27.5f),
	LeVertex( 15.5f, 25.0f, -43.0f) + LeVertex(0.0f, -12.5f, 27.5f),
	LeVertex(-15.5f, 25.0f, -54.0f) + LeVertex(0.0f, -12.5f, 27.5f),
	LeVertex(-15.5f, 25.0f, -43.0f) + LeVertex(0.0f, -12.5f, 27.5f),

	LeVertex(-2.0f, 12.5f, -30.0f) + LeVertex(0.0f, -12.5f, 27.5f),
	LeVertex( 9.0f, 12.5f, -30.0f) + LeVertex(0.0f, -12.5f, 27.5f),

	LeVertex(-3.0f, 6.25f, -78.5f) + LeVertex(0.0f, -12.5f, 27.5f),
	LeVertex( 3.0f, 6.25f, -78.5f) + LeVertex(0.0f, -12.5f, 27.5f),

	LeVertex( 30.0f, 6.25f, -40.0f) + LeVertex(0.0f, -12.5f, 27.5f),
	LeVertex( 30.0f, 6.25f, -60.0f) + LeVertex(0.0f, -12.5f, 27.5f),
	LeVertex(-30.0f, 6.25f, -40.0f) + LeVertex(0.0f, -12.5f, 27.5f),
	LeVertex(-30.0f, 6.25f, -60.0f) + LeVertex(0.0f, -12.5f, 27.5f),
	
// Facing down
	LeVertex(0.0f, -6.25f, -16.0f) + LeVertex(0.0f, -12.5f, 27.5f),
	LeVertex(0.0f, -6.25f, -46.0f) + LeVertex(0.0f, -12.5f, 27.5f),
	LeVertex(0.0f, -6.25f, -56.0f) + LeVertex(0.0f, -12.5f, 27.5f),
};

const int launchersNb = sizeof(launchersPos) / sizeof(LeVertex);

/*****************************************************************************/
struct Player {
	LeVertex orbital;
	LeVertex orbitalSpeed;
	LeVertex orbitalAcc;

	float distance;
	float distanceSpeed;
	float distanceAcc;
	
	LeVertex shuttlePos;
	LeVertex cagePos[4];

	uint timeToFire;
};

/*****************************************************************************/
enum BULLET_SRC {
	BULLET_SRC_SHUTTLE		= 0,
	BULLET_SRC_LAUNCHER		= 1
};

struct Bullet {
	LeVertex speed;
	int source;
	int life;
};

/*****************************************************************************/
struct Explosion {
	LeVertex center;
	int frame;
	LeBSet sparks;
	LeVertex speeds[SPARKS_PER_EXPLOSION];
};

/*****************************************************************************/
struct Launcher {
	int life;
};

/*****************************************************************************/
struct Camera {
	LeVertex orbital;
	float distance;
	int shake;
};

/*****************************************************************************/
/** Light setup */
LeLight lightNebula1(LE_LIGHT_DIRECTIONAL, LeColor::rgb(0xFF4040));
LeLight lightNebula2(LE_LIGHT_DIRECTIONAL, LeColor::rgb(0x4040FF));
LeLight lightAmbient(LE_LIGHT_AMBIENT, LeColor::rgb(0x404040));

/** Global references to assets */
int bulletSlot = 0;
int targetSlot = 0;
int sparkSlots[4];

LeMesh * destroyerMesh = NULL;
LeMesh * skyMesh = NULL;
LeMesh * shuttleMesh = NULL;
LeMesh * shuttlePropCageMesh = NULL;
LeMesh * shuttlePropEnergyMesh = NULL;
LeMesh * launcherMesh = NULL;

LeBitmap * targetBmp = NULL;
LeBitmap * titleBmp = NULL;
LeBitmap * creditBmp = NULL;
LeBitmap * startBmp = NULL;
LeBitmap * tuto1Bmp = NULL;
LeBitmap * tuto2Bmp = NULL;
LeBitmap * missionAccomplishedBmp = NULL;
LeBitmap * missionFailedBmp = NULL;

/** Game objects */
LeBSet bulletsBSet(BULLETS_MAX);
Bullet bullets[BULLETS_MAX];

Explosion explosions[EXPLOSIONS_MAX];

Launcher launchers[launchersNb];
uint launchersTimeToFire;
uint launchersIndex;
uint launchersCount;

Player player;
Camera camera;

/** Auto-target stuff */
LeVertex targetPos;
float targetX;
float targetY;
int targetID;
bool targetValid;

/*****************************************************************************/
/** Game state */
enum GAME_STATE {
	GAME_STATE_TITLE,
	GAME_STATE_TUTO1,
	GAME_STATE_TUTO2,
	GAME_STATE_RUN,
	GAME_STATE_WIN,
	GAME_STATE_LOSE
};
GAME_STATE gameState = GAME_STATE_TITLE;
int gameStateCounter = 0;

/*****************************************************************************/
/** Gamepad emulator = keyboard mode */
void keyHandler(int key, int state);
void keyPadEmulate(LeGamePad & pad);
int keyButtons = 0;
float keyAxisX = 0;
float keyAxisY = 0;

/*****************************************************************************/
/** Prototypes */
void sceneInit(LeGamePad & pad);
void sceneRender(LeRenderer & renderer, LeRasterizer & rasterizer);
void sceneLightMesh(LeRenderer & renderer, LeMesh * mesh);

void gameUpdate(LeGamePad & pad);

void playerKill();
void playerUpdate(LeGamePad & pad);
void playerRender(LeRenderer & renderer);

void launchersKill(int launcher);
void launchersUpdate();
void launchersRender(LeRenderer & renderer);

void autotargetUpdate(LeRenderer & renderer, LeGamePad & pad);
void cameraUpdate(LeRenderer & renderer, LeGamePad & pad);

void bulletsKill(int bullet);
void bulletsUpdate();
void bulletsFire(BULLET_SRC source, const LeVertex & pos, const LeVertex & dir);

void explosionsUpdate();
void explosionsBoom(const LeVertex & pos, int strength);

/*****************************************************************************/
int main()
{
	sys.initialize();
	LeGamePad::setup();

// Create application objects
	LeWindow window	= LeWindow("Destroy The Destroyer", resoX, resoY, true);
	LeDraw draw	= LeDraw(window.getContext(), resoX, resoY);
	LeRenderer renderer	= LeRenderer(resoX, resoY);
	LeRasterizer rasterizer = LeRasterizer(resoX, resoY);
	LeGamePad pad = LeGamePad(0);

	rasterizer.background = LeColor::rgb(0x000000);

// Register keyboard handler
	window.registerKeyCallback(keyHandler);

// Load the assets (textures then 3D models)
	bmpCache.loadDirectory("assets");
	meshCache.loadDirectory("assets");

// Retrieve the 2D assets
	bulletSlot = bmpCache.getSlotFromName("bullet.bmp");
	targetSlot = bmpCache.getSlotFromName("target.bmp");
	sparkSlots[0] = bmpCache.getSlotFromName("spark1.bmp");
	sparkSlots[1] = bmpCache.getSlotFromName("spark2.bmp");
	sparkSlots[2] = bmpCache.getSlotFromName("spark3.bmp");
	sparkSlots[3] = bmpCache.getSlotFromName("spark4.bmp");

	targetBmp = bmpCache.getBitmapFromName("target.bmp");
	titleBmp = bmpCache.getBitmapFromName("title.bmp");
	creditBmp = bmpCache.getBitmapFromName("credit.bmp");
	tuto1Bmp = bmpCache.getBitmapFromName("tuto1.bmp");
	tuto2Bmp = bmpCache.getBitmapFromName("tuto2.bmp");
	startBmp = bmpCache.getBitmapFromName("start.bmp");
	missionAccomplishedBmp = bmpCache.getBitmapFromName("mission-accomplished.bmp");
	missionFailedBmp = bmpCache.getBitmapFromName("mission-failed.bmp");

// Retrieve the 3D models
	destroyerMesh = meshCache.getMeshFromName("destroyer.obj");
	skyMesh = meshCache.getMeshFromName("skybox.obj");
	shuttleMesh = meshCache.getMeshFromName("shuttle.obj");
	shuttlePropCageMesh = meshCache.getMeshFromName("shuttle-propcage.obj");
	shuttlePropEnergyMesh = meshCache.getMeshFromName("shuttle-propenergy.obj");
	launcherMesh = meshCache.getMeshFromName("launcher.obj");
	
// Allocate some memory (for explosions)
	for (int e = 0; e < EXPLOSIONS_MAX; e++) {
		explosions[e].sparks.allocate(SPARKS_PER_EXPLOSION);
		explosions[e].sparks.basic();
	}

// Configure lights + static lighting
	lightNebula1.axis = LeAxis(LeVertex(), LeVertex(1.0f, 0.0f, -1.0f));
	lightNebula2.axis = LeAxis(LeVertex(), LeVertex(-1.0f, 0.0f, -1.0f));
	sceneLightMesh(renderer, destroyerMesh);
	sceneLightMesh(renderer, launcherMesh);

// Initialize the timing
	timing.setup(FPS_DESIRED);
	timing.firstFrame();

// Program main loop
	gameState = GAME_STATE_TITLE;
	gameStateCounter = 3 * FPS_DESIRED;
	pad.init();
	sceneInit(pad);

	while (sys.running && window.visible) {
	// Process OS messages
		sys.update();
		window.update();

	// Wait for next frame
		timing.waitNextFrame();
		draw.setPixels(rasterizer.getPixels());

	// Update scene
		int tmpButtons = pad.buttons;
		pad.update();
		if (!pad.detected) {
			pad.buttons = tmpButtons;
			keyPadEmulate(pad);
		}
		
	// Subsystems / depending on game state
		if (gameState > GAME_STATE_TUTO2) {
			playerUpdate(pad);
			launchersUpdate();
			bulletsUpdate();
			explosionsUpdate();
			autotargetUpdate(renderer, pad);
		} else {
			player.orbital.x = -20.0f + sinf(player.orbital.y * d2r) * 20.0f;
			player.orbital.y = fmodf(player.orbital.y + 1.0f / FPS_DESIRED, 360.0f);
			player.distance = 80.0f;
		}
		gameUpdate(pad);

	// Render the scene
		cameraUpdate(renderer, pad);
		sceneRender(renderer, rasterizer);
	}

	timing.lastFrame();
	LeGamePad::release();
	sys.terminate();
	return 0;
}

/*****************************************************************************/
void gameUpdate(LeGamePad & pad)
{
// State transitions
	if (gameState == GAME_STATE_TITLE) {
		if (pad.pressed & LE_GAMEPAD_A)
			gameState = GAME_STATE_TUTO1;
	}else if (gameState == GAME_STATE_TUTO1) {
		if (pad.pressed & LE_GAMEPAD_A)
			gameState = GAME_STATE_TUTO2;
	}else if (gameState == GAME_STATE_TUTO2) {
		if (pad.pressed & LE_GAMEPAD_A) {
			sceneInit(pad);
			gameStateCounter = 2 * FPS_DESIRED;
			gameState = GAME_STATE_RUN;
		}
	}else if (gameState == GAME_STATE_RUN) {
		if (launchersCount == 0) {
			gameState = GAME_STATE_WIN;
			gameStateCounter = 10 * FPS_DESIRED;
		}
	}else if (gameState == GAME_STATE_WIN) {
		if (pad.pressed & LE_GAMEPAD_A)
			if (gameStateCounter < 8 * FPS_DESIRED)
				gameStateCounter = 1;
		int l = rand() % launchersNb;
		explosionsBoom(launchersPos[l], 0);

	}else if (gameState == GAME_STATE_LOSE) {
		if (pad.pressed & LE_GAMEPAD_A)
			if (gameStateCounter < 8 * FPS_DESIRED)
				gameStateCounter = 1;
	}
	
	if (!gameStateCounter) return;
	gameStateCounter--;
	if (gameStateCounter) return;

// Back to main menu
	if (gameState == GAME_STATE_WIN ||
		gameState == GAME_STATE_LOSE) {
		sceneInit(pad);
		gameState = GAME_STATE_TITLE;
		gameStateCounter = 3 * FPS_DESIRED;
	}
	
}

/*****************************************************************************/
void sceneInit(LeGamePad & pad)
{
	player.orbital = LeVertex();
	player.orbitalSpeed = LeVertex();
	player.orbitalAcc = LeVertex();
	player.distance = 100.0f;
	player.distanceSpeed = 0.0f;
	player.distanceAcc = 0.0f;
	player.orbital.y = 45.0f;

	LeMatrix mr;
	mr.translate(LePrimitives::back * player.distance);
	mr.rotateX(player.orbital.x * d2r);
	mr.rotateY(player.orbital.y * d2r);
	player.shuttlePos = mr * LeVertex();

	player.cagePos[0] = LeVertex();
	player.cagePos[1] = LeVertex();
	player.cagePos[2] = LeVertex();
	player.cagePos[3] = LeVertex();

	camera.orbital = LeVertex();
	camera.distance = 200.0f;
	camera.shake = 0;
	pad.feedback(0.0f, 0.0f);

	memset(bullets, 0, sizeof(Bullet) * BULLETS_MAX);

	memset(launchers, 0, sizeof(Launcher) * launchersNb);
	for (int l = 0; l < launchersNb; l++) {
		launchers[l].life = 1;
	}
	launchersTimeToFire = (int) (2.5f * FPS_DESIRED);
	launchersIndex = 0;
	launchersCount = launchersNb;

	bulletsBSet.clear();
	for (int b = 0; b < BULLETS_MAX; b++) {
		bulletsBSet.sizes[b * 2 + 0] = 3.0f;
		bulletsBSet.sizes[b * 2 + 1] = 3.0f;
		bulletsBSet.texSlots[b] = bulletSlot;
	}

	for (int e = 0; e < EXPLOSIONS_MAX; e++) {
		explosions[e].frame = 0;
		LeBSet & bs = explosions[e].sparks;
		for (int s = 0; s < SPARKS_PER_EXPLOSION; s++) {
			bs.texSlots[s] = sparkSlots[rand() % 4];
			float a = 2.0f * ((float) M_PI) * rand() / RAND_MAX;
			float b = 2.0f * ((float) M_PI) * rand() / RAND_MAX;
			float t = 0.5f + 0.5f * rand() / RAND_MAX;
			float n = t * 5.0f / FPS_DESIRED;
			explosions[e].speeds[s] = LeVertex(cosf(a) * cosf(b), sinf(b), sinf(a) * cosf(b)) * n;
		}
	}

	targetPos = LeVertex();
	targetID = -1;
	targetValid = false;
}

void sceneRender(LeRenderer & renderer, LeRasterizer & rasterizer)
{
	rasterizer.flush();

// Layer 0: sky / destroyer / leuncher / bullets
	renderer.flush();
	renderer.render(skyMesh);
	renderer.render(destroyerMesh);
	launchersRender(renderer);
	
	if (gameState > GAME_STATE_TUTO2) {
		renderer.render(&bulletsBSet);
		for (int e = 0; e < EXPLOSIONS_MAX; e++) {
			if (!explosions[e].frame) continue;
			renderer.render(&explosions[e].sparks);
		}
		renderer.render(&bulletsBSet);
	}

	rasterizer.rasterList(renderer.getTriangleList());

// Layer 1: auto-target / UI / title
	if (targetValid && gameState == GAME_STATE_RUN) {
		int px = (int) (targetX - targetBmp->tx * 0.5f);
		int py = (int) (targetY - targetBmp->ty * 0.5f);
		rasterizer.frame.alphaBlit(px, py, targetBmp, 0, 0, targetBmp->tx, targetBmp->ty);
	}

	if (gameState == GAME_STATE_TITLE || 
		gameState == GAME_STATE_TUTO1 ||
		gameState == GAME_STATE_TUTO2) {
		int px = (int) ((resoX - titleBmp->tx) * 0.5f);
		int py = (int) (resoY / 5.0f - titleBmp->ty * 0.5f);
		rasterizer.frame.alphaBlit(px, py, titleBmp, 0, 0, titleBmp->tx, titleBmp->ty);
		px = (int)(resoX - creditBmp->tx - 16.0f);
		py = (int)(resoY - creditBmp->ty - 16.0f);
		rasterizer.frame.alphaBlit(px, py, creditBmp, 0, 0, creditBmp->tx, creditBmp->ty);
	}

	if (gameState == GAME_STATE_TITLE && !gameStateCounter) {
		int px = (int) ((resoX - startBmp->tx) * 0.5f);
		int py = (int) (resoY * 0.70f - startBmp->ty * 0.5f);
		rasterizer.frame.alphaBlit(px, py, startBmp, 0, 0, startBmp->tx, startBmp->ty);
	}

	if (gameState == GAME_STATE_TUTO1) {
		int px = (int) ((resoX - tuto1Bmp->tx) * 0.5f);
		int py = (int) (resoY * 0.62f - tuto1Bmp->ty * 0.5f);
		rasterizer.frame.alphaBlit(px, py, tuto1Bmp, 0, 0, tuto1Bmp->tx, tuto1Bmp->ty);
	}
	
	if (gameState == GAME_STATE_TUTO2) {
		int px = (int) ((resoX - tuto2Bmp->tx) * 0.5f);
		int py = (int) (resoY * 0.62f - tuto2Bmp->ty * 0.5f);
		rasterizer.frame.alphaBlit(px, py, tuto2Bmp, 0, 0, tuto2Bmp->tx, tuto2Bmp->ty);
	}

	if (gameState == GAME_STATE_WIN) {
		int px = (int)((resoX - missionAccomplishedBmp->tx) * 0.5f);
		int py = (int)((resoY - missionAccomplishedBmp->ty) * 0.5f);
		rasterizer.frame.alphaBlit(px, py, missionAccomplishedBmp, 0, 0, missionAccomplishedBmp->tx, missionAccomplishedBmp->ty);
	}

	if (gameState == GAME_STATE_LOSE) {
		int px = (int)((resoX - missionFailedBmp->tx) * 0.5f);
		int py = (int)((resoY - missionFailedBmp->ty) * 0.5f);
		rasterizer.frame.alphaBlit(px, py, missionFailedBmp, 0, 0, missionFailedBmp->tx, missionFailedBmp->ty);
	}

// Layer 2: shuttle / UI
	if (gameState > GAME_STATE_TUTO2 &&
		gameState < GAME_STATE_WIN) {
		renderer.flush();
		playerRender(renderer);
		rasterizer.rasterList(renderer.getTriangleList());
	}
}

void sceneLightMesh(LeRenderer & renderer, LeMesh * mesh)
{
	LeLight::black(mesh);
	lightNebula1.shine(mesh);
	lightNebula2.shine(mesh);
	lightAmbient.shine(mesh);
}

/*****************************************************************************/
void cameraUpdate(LeRenderer & renderer, LeGamePad & pad)
{
// Smooth the camera tracking
	float smooth = 0.75f;
	if (gameState != GAME_STATE_RUN)
		smooth = 0.98f;
	else {
		float t = (float) gameStateCounter / (2 * FPS_DESIRED);
		smooth = 0.75f + (0.99f - 0.75f) * t;
	}
	camera.orbital = player.orbital + (camera.orbital - player.orbital) * smooth;
	camera.distance = player.distance + (camera.distance - player.distance) * smooth;

// Shake that!
	LeVertex shake;
	if (camera.shake) {
		float a = camera.shake * 8.0f * 2.0f * ((float) M_PI) / FPS_DESIRED;
		float s = camera.shake / 30.0f;
		float b = s * s;
		shake.x = sinf(a) * b * 0.75f;
		shake.y = cosf(0.5f * a) * b * 0.40f;
		camera.shake--;

		if (!camera.shake) pad.feedback(0.0f, 0.0f);
		else pad.feedback(1.0f, 1.0f);
	}

// A little offset, for title screens
	LeMatrix m;
	if (gameState <= GAME_STATE_TUTO2)
		m.translate(LeVertex(0.0f, -15.0f, 0.0f));

// Position the camera
	m.translate(shake);
	m.rotateY(-camera.orbital.y * d2r);
	m.rotateX(-camera.orbital.x * d2r);
	m.translate(-LePrimitives::back * (camera.distance + 20.0f));
	renderer.setViewMatrix(m);
}

/*****************************************************************************/
void bulletsKill(int bullet)
{
	bullets[bullet].life = 0;
	bulletsBSet.flags[bullet] = 0;
}

void bulletsUpdate()
{
	for (int i = 0; i < BULLETS_MAX; i++) {
		if (!bulletsBSet.flags[i]) continue;
		bulletsBSet.places[i] += bullets[i].speed;

		LeVertex ans, contact;
		int res = LePhysics::collideSphereMesh(ans, contact, bulletsBSet.places[i], 0.5f, destroyerMesh);
		if (res) bulletsKill(i);

		int src = bullets[i].source;
		if (src == BULLET_SRC_SHUTTLE) {
		// Collide with a launcher
			LeVertex pos = bulletsBSet.places[i];
			for (int l = 0; l < launchersNb; l++) {
				if (!launchers[l].life) continue;
				LeVertex d = pos - launchersPos[l];
				if (d.dot(d) < 9.0f) {
					bulletsKill(i);
					launchersKill(l);
					break;
				}
			}
		} else if (src == BULLET_SRC_LAUNCHER) {
		// Collide with the shuttle
			LeVertex pos = bulletsBSet.places[i];
			LeVertex d = pos - player.shuttlePos;
			if (d.dot(d) < 16.0f) {
				bulletsKill(i);
				playerKill();
			}
		}
		
	// Too late
		if (bullets[i].life-- == 0)	bulletsKill(i);
	}
}

void bulletsFire(BULLET_SRC source, const LeVertex & pos, const LeVertex & dir)
{
	for (int b = 0; b < BULLETS_MAX; b++) {
		if (bulletsBSet.flags[b]) continue;

		bulletsBSet.places[b] = pos;
		bullets[b].source = source;
		bullets[b].speed = dir;
		bullets[b].life = (int)(BULLETS_LIFE * FPS_DESIRED);
		bulletsBSet.flags[b] = LE_BSET_EXIST;
		return;
	}
}

/*****************************************************************************/
void explosionsBoom(const LeVertex & pos, int strength)
{
	for (int e = 0; e < EXPLOSIONS_MAX; e++) {
		if (explosions[e].frame) continue;
		LeBSet & bs = explosions[e].sparks;
		for (int s = 0; s < SPARKS_PER_EXPLOSION; s++) {
			bs.places[s] = pos;
			bs.flags[s] = LE_BSET_EXIST;
		}
		explosions[e].frame = (int)(EXPLOSIONS_LIFE * FPS_DESIRED);
		camera.shake = (int)(SHAKE_TIME * FPS_DESIRED);
		return;
	}
}

void explosionsUpdate()
{
	for (int e = 0; e < EXPLOSIONS_MAX; e++) {
		if (!explosions[e].frame) continue;
		LeBSet & bs = explosions[e].sparks;
		float t = 2.0f * ((float) explosions[e].frame / (EXPLOSIONS_LIFE * FPS_DESIRED));
		for (int s = 0; s < SPARKS_PER_EXPLOSION; s++) {
			bs.places[s] += explosions[e].speeds[s];
			bs.sizes[s * 2 + 0] = t;
			bs.sizes[s * 2 + 1] = t;
		}
		
		if (explosions[e].frame-- == 0) {}
	}
}

/*****************************************************************************/
void launchersKill(int launcher)
{
	launchers[launcher].life = 0;
	explosionsBoom(launchersPos[launcher], 0);
	launchersCount--;
}

void launchersUpdate()
{
	if (!launchersTimeToFire) return;
	launchersTimeToFire--;
	if (launchersTimeToFire) return;
	
	if (!launchersCount) return;

	for (int i = 0; i < BULLETS_PER_ROUND; i++) {
		int l = (launchersIndex ++) % launchersNb;
		if (!launchers[l].life) continue;

	// Target the player
		LeVertex pos = launchersPos[l];
		LeVertex dir = player.shuttlePos - pos;
		dir.normalize();
		dir *= 1.0f;

		LeVertex offset;
		if (l < launchersNb - 3) offset = LeVertex(0.0f, 2.0f, 0.0f);
		else offset = LeVertex(0.0f, -2.0f, 0.0f);
		
		bulletsFire(BULLET_SRC_LAUNCHER, pos + offset, dir);
	}

	launchersTimeToFire = (int)(cmmax(1, LAUNCHER_WEAPON_RELOAD * FPS_DESIRED * (float) launchersCount / launchersNb));
	launchersIndex++;
}

void launchersRender(LeRenderer & renderer)
{
	renderer.setViewOffset(50.0f);
	for (int l = 0; l < launchersNb; l++) {
		if (!launchers[l].life) continue;
		launcherMesh->pos = launchersPos[l];

		if (l < launchersNb - 3) launcherMesh->angle.x = 0.0f;
		else launcherMesh->angle.x = 180.0f;
		
		launcherMesh->updateMatrix();
		renderer.render(launcherMesh);
	}
	renderer.setViewOffset(0.0f);
}

/*****************************************************************************/
void autotargetUpdate(LeRenderer & renderer, LeGamePad & pad)
{
	float targetDMin = FLT_MAX;
	LeVertex shuttleCoords;
	renderer.getViewportCoordinates(player.shuttlePos, shuttleCoords);

	if (pad.pressed & LE_GAMEPAD_B) {
	// Get a new target
		targetValid = false;
		for (int l = 0; l < launchersNb; l++) {
			if (!launchers[l].life) continue;

			LeVertex viewCoords;
			int r = renderer.getViewportCoordinates(launchersPos[l], viewCoords);
			if (!r) continue;

		// Is target too far away?
			viewCoords.z = -viewCoords.z;
			if (viewCoords.z > 150.0f) continue;

		// Is target not in sight?
			float dx = shuttleCoords.x - viewCoords.x;
			float dy = shuttleCoords.y - viewCoords.y;
			float radius = dx * dx + dy * dy;
			const float radiusMax = (resoX * 0.125f) * (resoX * 0.125f);
			if (radius > radiusMax) continue;

		// Sort the targets by depth
			float d = viewCoords.z + radius * 0.25f;
			if (d >= targetDMin) continue;
			targetDMin = d;
			targetX = viewCoords.x;
			targetY = viewCoords.y;
			targetID = l;
		}

	} else {
	// Update current target
		if (!targetValid) return;
		targetValid = false;
		if (!launchers[targetID].life) return;

		LeVertex viewCoords;
		int r = renderer.getViewportCoordinates(launchersPos[targetID], viewCoords);
		if (!r) return;

		targetX = viewCoords.x;
		targetY = viewCoords.y;
	}

	targetPos = launchersPos[targetID];
	targetValid = true;
}

/*****************************************************************************/
void playerKill()
{
	if (gameState != GAME_STATE_RUN) return;

	gameState = GAME_STATE_LOSE;
	gameStateCounter = 10 * FPS_DESIRED;
	explosionsBoom(player.shuttlePos, 0);
	explosionsBoom(player.cagePos[0], 0);
	explosionsBoom(player.cagePos[1], 0);
	explosionsBoom(player.cagePos[2], 0);
	explosionsBoom(player.cagePos[3], 0);
}

void playerUpdate(LeGamePad & pad)
{
	if (player.timeToFire) 
		player.timeToFire--;

// Fire a bullet
	if (pad.pressed & LE_GAMEPAD_A &&
		gameState == GAME_STATE_RUN) {
		if (!player.timeToFire) {
			LeVertex dir = -player.shuttlePos;
			if (targetValid) dir += targetPos;
			dir.normalize();
			dir *= 1.0f;
			bulletsFire(BULLET_SRC_SHUTTLE, player.shuttlePos, dir);
			player.timeToFire = (int)(PLAYER_WEAPON_RELOAD * FPS_DESIRED);
		}
	}

// Get closer or farer
	if (pad.buttons & LE_GAMEPAD_X)
		player.distanceAcc = -1.0f;
	else if (pad.buttons & LE_GAMEPAD_Y)
		player.distanceAcc = 1.0f;
	else player.distanceAcc = 0.0f;
	player.distanceAcc *= 8.0f / FPS_DESIRED;

// Move around the destroyer
	player.orbitalAcc.x = -pad.stickLeftY * 8.0f * ORBITAL_SPEED_MAX / FPS_DESIRED;
	player.orbitalAcc.y = pad.stickLeftX * 8.0f * ORBITAL_SPEED_MAX / FPS_DESIRED;

	player.orbitalSpeed += player.orbitalAcc;
	player.distanceSpeed += player.distanceAcc;

// Apply some basic physics
	float oa = player.orbitalSpeed.norm();
	if (oa > ORBITAL_SPEED_MAX) {
		player.orbitalSpeed *= ORBITAL_SPEED_MAX / oa;
		oa = ORBITAL_SPEED_MAX;
	}

	float dn = player.distanceSpeed;
	float da = fabsf(dn);
	if (da > DISTANCE_SPEED_MAX) {
		player.distanceSpeed *= DISTANCE_SPEED_MAX / da;
		dn = player.distanceSpeed;
		da = DISTANCE_SPEED_MAX;
	}

	player.orbital += player.orbitalSpeed;
	player.distance += player.distanceSpeed;

	if (player.orbital.x > 65.0f) {
		player.orbital.x = 65.0f;
		player.orbitalSpeed.x = 0.0f;
	}

	if (player.orbital.x < -65.0f) {
		player.orbital.x = -65.0f;
		player.orbitalSpeed.x = 0.0f;
	}

	if (player.distance <= SHUTTLE_DISTANCE_MIN) {
		player.distance = SHUTTLE_DISTANCE_MIN;
		player.distanceSpeed = 0.0f;
	}
	else if (player.distance >= SHUTTLE_DISTANCE_MAX) {
		player.distance = SHUTTLE_DISTANCE_MAX;
		player.distanceSpeed = 0.0f;
	}

	if (oa > 0.00625f)
		player.orbitalSpeed *= 1.0f - 0.00625f / oa;
	else player.orbitalSpeed = LeVertex();

	if (da > 0.025f)
		player.distanceSpeed *= 1.0f - 0.025f / da;
	else player.distanceSpeed = 0.0f;
}

void playerRender(LeRenderer & renderer)
{
	static float anim = 0.0f;
	anim = fmodf(anim + 0.1f, 2.0f * ((float) M_PI));

	LeMatrix mr;
	mr.translate(LePrimitives::back * player.distance);
	mr.rotateX(player.orbital.x * d2r);
	mr.rotateY(player.orbital.y * d2r);
	shuttleMesh->setMatrix(mr);
	player.shuttlePos = mr * LeVertex();

	float vx = sinf(anim) * 0.20f;
	float vy = cosf(anim) * 0.20f;
	LeMatrix mc;

	mc.identity();
	mc.translate(LeVertex(4.5f + vx, 0.0f, 0.0f));
	shuttlePropCageMesh->setMatrix(mr * mc);
	player.cagePos[0] = shuttlePropCageMesh->view * LeVertex();
	sceneLightMesh(renderer, shuttlePropCageMesh);
	renderer.render(shuttlePropCageMesh);

	mc.identity();
	mc.rotateZ(anim);
	mc.translate(LeVertex(4.5f + vx, 0.0f, 0.0f));
	shuttlePropEnergyMesh->setMatrix(mr * mc);
	sceneLightMesh(renderer, shuttlePropEnergyMesh);
	renderer.render(shuttlePropEnergyMesh);

	mc.identity();
	mc.translate(LeVertex(-4.5f - vx, 0.0f, 0.0f));
	shuttlePropCageMesh->setMatrix(mr * mc);
	player.cagePos[1] = shuttlePropCageMesh->view * LeVertex();
	sceneLightMesh(renderer, shuttlePropCageMesh);
	renderer.render(shuttlePropCageMesh);

	mc.identity();
	mc.rotateZ(anim);
	mc.translate(LeVertex(-4.5f - vx, 0.0f, 0.0f));
	shuttlePropEnergyMesh->setMatrix(mr * mc);
	sceneLightMesh(renderer, shuttlePropEnergyMesh);
	renderer.render(shuttlePropEnergyMesh);

	mc.identity();
	mc.rotateZ(90.0f * d2r);
	mc.translate(LeVertex(0.0f, 4.5f + vy, 0.0f));
	shuttlePropCageMesh->setMatrix(mr * mc);
	player.cagePos[2] = shuttlePropCageMesh->view * LeVertex();
	sceneLightMesh(renderer, shuttlePropCageMesh);
	renderer.render(shuttlePropCageMesh);

	mc.identity();
	mc.rotateZ(anim);
	mc.translate(LeVertex(0.0f, 4.5f + vy, 0.0f));
	shuttlePropEnergyMesh->setMatrix(mr * mc);
	sceneLightMesh(renderer, shuttlePropEnergyMesh);
	renderer.render(shuttlePropEnergyMesh);

	mc.identity();
	mc.rotateZ(90.0f * d2r);
	mc.translate(LeVertex(0.0f, -4.5f - vy, 0.0f));
	shuttlePropCageMesh->setMatrix(mr * mc);
	player.cagePos[3] = shuttlePropCageMesh->view * LeVertex();
	sceneLightMesh(renderer, shuttlePropCageMesh);
	renderer.render(shuttlePropCageMesh);

	mc.identity();
	mc.rotateZ(anim);
	mc.translate(LeVertex(0.0f, -4.5f - vy, 0.0f));
	shuttlePropEnergyMesh->setMatrix(mr * mc);
	sceneLightMesh(renderer, shuttlePropEnergyMesh);
	renderer.render(shuttlePropEnergyMesh);

	sceneLightMesh(renderer, shuttleMesh);
	renderer.render(shuttleMesh);
}

/*****************************************************************************/
void keyHandler(int key, int state)
{
	int mask = 0;
	if (key == 'X') mask |= LE_GAMEPAD_A;
	else if (key == 'C') mask |= LE_GAMEPAD_B;
	else if (key == 'V') mask |= LE_GAMEPAD_X;
	else if (key == 'B') mask |= LE_GAMEPAD_Y;

	if (state & LE_WINDOW_KEYSTATE_PRESSED)
		keyButtons |= mask;
	else keyButtons &= ~mask;

	float force = 1.0f;
	if (state & LE_WINDOW_KEYSTATE_RELEASED) force = 0.0f;
	
	if (key == LE_WINDOW_KEYCODE_UP)
		keyAxisY = force;
	else if (key == LE_WINDOW_KEYCODE_DOWN)
		keyAxisY = -force;

	if (key == LE_WINDOW_KEYCODE_LEFT)
		keyAxisX = -force;
	else if (key == LE_WINDOW_KEYCODE_RIGHT)
		keyAxisX = force;
}

void keyPadEmulate(LeGamePad & pad)
{
// Emulate the gamepad
	pad.toggled = keyButtons ^ pad.buttons;
	pad.buttons = keyButtons;
	pad.pressed = pad.toggled & pad.buttons;
	pad.released = pad.toggled & ~pad.buttons;

	pad.stickLeftX = keyAxisX;
	pad.stickLeftY = keyAxisY;
}
