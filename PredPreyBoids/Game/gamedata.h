#ifndef _GAME_DATA_H_
#define _GAME_DATA_H_
#include "gamestate.h"
#include <dinput.h>
#include <map>
#include <d3d11.h>

class MyEffectFactory;

struct GameData
{
	float dt; //Duration of previous frame
	unsigned char* keyboard; //Current keyboard state
	unsigned char* prevKeyboard; //Previous frame's keyboard state
	DIMOUSESTATE* mouse; //Current mouse state
	DIMOUSESTATE* prevMouse; //Previous mouse state
	GameState GS; //Current game state
	GameState prePauseGS; //Gamestate prior to pausing
	static ID3D11Device* p3d;
	static MyEffectFactory* EF;
};

struct SimulationParameters
{
	static float groupStrength; //How strong is the grouping
	static float groupDistance; //How far are BOIDs attracted to group
	static float groupHeading; //How strong is the average heading of groups
	static float boidMaxSpeed; //Max speed of type 1 BOID
	static float obstacleSize; //Sphere of influence for obstacles
	static float mapSize; //Map bounds
	static bool respawnOnFinish; //Once the BOID has reached it's finish should it respawn?
	static bool respawnOnDeath; //If killed, should the BOID respawn?
	static bool cursorObstacle; //Is the cursor an obstacle
	static bool showDebug; //Do I display debug menu and lines
	static float starvationTime; //How long after a kill do BOIDs lose weight
	static std::map<int, int> boidCount; //Count of eac htype of BOID
};

#endif