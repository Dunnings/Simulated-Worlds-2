#ifndef _GAME_H_
#define _GAME_H_

#include <windows.h>
#include "Effects.h"
#include <list>
#include "MyEffectsFactory.h"
#include "CommonStates.h"
#include <dinput.h> 

using namespace std;

using namespace DirectX;

class GameObject;
class GameObject2D;
class Camera;
class Light;
class PredCamera;
class Boid;
class boidManager;
struct GameData;
struct DrawData;
struct DrawData2D;

class Game
{
public:
	Game(ID3D11Device* pd3dDevice, HINSTANCE hInstance);
	~Game();
	//Tick the game state
	bool update(); 
	//Render everything
	void render(ID3D11DeviceContext* pd3dImmediateContext); //render the current game state

protected:
	//Read in keyboard state
	bool ReadKeyboard();
	//Read in mouse state
	bool ReadMouse();
	//Amount of time since the game started
	DWORD m_playTime; 
	//Main camera
	Camera* m_mainCam; 
	//Predator camera
	PredCamera* m_predCamera;

	//Base light
	Light* m_Light;

	boidManager* boidMan;
	//Data structure storing all GameObjects of this Game
	list<GameObject *> m_GameObjects;
	//Data structure storing all 2D GameObjects
	list<GameObject2D *> m_GameObject2Ds;

	//Required for the CMO model rendering system
	MyEffectFactory* m_myEF; 
	CommonStates* m_States;

	//Input stuff
	IDirectInput8*			m_pDirectInput;
	IDirectInputDevice8*	m_pKeyboard;
	unsigned char			m_keyboardState[256];
	unsigned char			m_prevKeyboardState[256];
	IDirectInputDevice8*	m_pMouse;
	DIMOUSESTATE			m_mouse_state;
	DIMOUSESTATE			m_prev_mouse_state;

	//My GameData
	GameData* m_GD;
	DrawData* m_DD;
	DrawData2D* m_DD2D;

	//Parameters
	bool loadedFile = false;
	void LoadParameters();
	string fileName = "SimulationParameters.dat";
	int spawnPerPress = 1;
	bool spawn = true;
};


#endif
