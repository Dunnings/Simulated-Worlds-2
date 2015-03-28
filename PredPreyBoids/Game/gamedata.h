#ifndef _GAME_DATA_H_
#define _GAME_DATA_H_
#include "gamestate.h"
#include <dinput.h>
#include <map>

class UserInterfaceManager;
class MyEffectFactory;

struct GameData
{
	float dt; //duration of previous frame
	unsigned char* keyboard; //current keyboard state
	unsigned char* prevKeyboard; //previous frame's keyboard state
	DIMOUSESTATE* mouse; //current mouse state
	DIMOUSESTATE* prevMouse; //previous mouse state
	GameState GS; //current game state
	static ID3D11Device* p3d;
	static MyEffectFactory* EF;
	UserInterfaceManager* UIManager;
};

struct SimulationParameters
{
	static float groupStrength;
	static float groupDistance;
	static float groupHeading;
	static float boidMaxSpeed;
	static float restTime;
	static float mapSize;
	static bool cursorObstacle;
	static bool showDebug;
	static float starvationTime;
	static std::map<int, int> boidCount;
};

#endif