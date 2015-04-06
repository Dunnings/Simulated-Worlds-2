#ifndef _GAME_DATA_H_
#define _GAME_DATA_H_
#include "gamestate.h"
#include <dinput.h>
#include <map>
#include <d3d11.h>
#include <vector>

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

struct Type
{
	int id;
	int scale;
	float breedDelay;
	bool canBreed;
	float speed;
	float sight;
	float health;
	std::vector<int> prey;
};

struct SimulationParameters
{
	static float groupStrength; //How strong is the grouping
	static float groupDistance; //How far are boids attracted to group
	static float groupHeading; //How strong is the average heading of groups
	static float boidMaxSpeed; //Max speed of type 1 boid
	static float obstacleSize; //Sphere of influence for obstacles
	static float mapSize; //Map bounds
	static float spawnDelay; //How often do boids spawn
	static float breedDelay; //How often do boids spawn
	static bool respawnOnFinish; //Once the boid has reached it's finish should it respawn?
	static bool respawnOnDeath; //If killed, should the boid respawn?
	static bool cursorObstacle; //Is the cursor an obstacle
	static bool showDebugMenu; //Do I display debug menu
	static bool showDebugForces; //Do I display debug lines
	static bool showDebugSight; //Do I display debug sight range
	static bool showDebugWaypoints; //Do I display debug waypoints
	static float starvationTime; //How long after a kill do boids lose weight
	static std::map<int, int> boidCount; //Count of eac htype of boid
};

#endif