#include "game.h"
#include "gameobject.h"
#include "GameObject2D.h"
#include "ObjectList.h"
#include <windows.h>
#include "CommonStates.h"
#include "gamedata.h"
#include <time.h>
#include "helper.h"
#include "DDSTextureLoader.h"
#include "drawdata.h"
#include "DrawData2D.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace DirectX;

extern HWND g_hWnd;

float SimulationParameters::boidMaxSpeed;
float SimulationParameters::groupDistance;
float SimulationParameters::groupHeading;
float SimulationParameters::groupStrength;
map<int, int> SimulationParameters::boidCount;
float SimulationParameters::obstacleSize;
float SimulationParameters::mapSize;
float SimulationParameters::spawnDelay;
bool SimulationParameters::canBreed;
bool SimulationParameters::respawnOnFinish;
bool SimulationParameters::respawnOnDeath;
bool SimulationParameters::cursorObstacle;
float SimulationParameters::starvationTime;
bool SimulationParameters::showDebugMenu;
bool SimulationParameters::showDebugForces;
bool SimulationParameters::showDebugSight;
bool SimulationParameters::showDebugWaypoints;

void Game::loadParameters()
{
	SimulationParameters para;
	//To make the compiler happy
	para;
	//Default values
	para.groupStrength = 0.0f;
	para.groupDistance = 200.0f;
	para.groupHeading = 0.7f;
	para.boidMaxSpeed = 20.0f;
	para.mapSize = 500.0f;
	para.starvationTime = 5000.0f;
	para.spawnDelay = 0.0f;
	para.canBreed = true;
	para.showDebugMenu = true;
	para.showDebugForces = true;
	para.showDebugSight = true;
	para.showDebugWaypoints = true;
	para.cursorObstacle = false;
	para.respawnOnFinish = true;
	para.respawnOnDeath = false;
	para.obstacleSize = 40.0f;
	//Create an input file stream using the parameter file name
	ifstream file;
	//Open the file
	file.open(fileName);
	//If the file successfully opened
	if (file.is_open())
	{
		//Update boolean to track if the file has loaded
		loadedFile = true;
		string currentLine;
		//Loop through each line in the file
		while (!file.eof())
		{
			//Get the current line of the file
			getline(file, currentLine);
			if (currentLine == "<parameters>")
			{
				while (currentLine != "</parameters>" && !file.eof())
				{
					getline(file, currentLine);
					//If the lane contains an equals
					if (currentLine.find(" = ") != string::npos)
					{
						//Get the parameter (before the =)
						string parameter;
						parameter = currentLine.substr(0, currentLine.find(" = "));
						//Get the value (after the =)
						string value;
						value = currentLine.substr(currentLine.find(" = ") + 3, currentLine.size());

						//Check if parameter is equal to any of the parameters in SimulationParameters
						//If it is then set the parameter to the value
						if (parameter == "groupStrength")
						{
							para.groupStrength = stof(value);
						}
						else if (parameter == "groupDistance")
						{
							para.groupDistance = stof(value);
						}
						else if (parameter == "groupHeading")
						{
							para.groupHeading = stof(value);
						}
						else if (parameter == "boidMaxSpeed")
						{
							para.boidMaxSpeed = stof(value);
						}
						else if (parameter == "obstacleSize")
						{
							para.obstacleSize = stof(value);
						}
						else if (parameter == "mapSize")
						{
							para.mapSize = stof(value);
						}
						else if (parameter == "starvationTime")
						{
							para.starvationTime = stof(value);
						}
						else if (parameter == "spawnDelay")
						{
							para.spawnDelay = stof(value);
						}
						else if (parameter == "canBreed")
						{
							para.canBreed = (value == "true" || value == "1");
						}
						else if (parameter == "showDebugMenu")
						{
							para.showDebugMenu = (value == "true" || value == "1");
						}
						else if (parameter == "showDebugForces")
						{
							para.showDebugForces = (value == "true" || value == "1");
						}
						else if (parameter == "showDebugSight")
						{
							para.showDebugSight = (value == "true" || value == "1");
						}
						else if (parameter == "showDebugWaypoints")
						{
							para.showDebugWaypoints = (value == "true" || value == "1");
						}
						else if (parameter == "cursorObstacle")
						{
							para.cursorObstacle = (value == "true" || value == "1");
						}
						else if (parameter == "respawnOnDeath")
						{
							para.respawnOnDeath = (value == "true" || value == "1");
						}
						else if (parameter == "respawnOnFinish")
						{
							para.respawnOnFinish = (value == "true" || value == "1");
						}
					}
				}
			}
			else if (currentLine == "<waypoint>")
			{
				Waypoint* w = new Waypoint;
				Vector3 pos = Vector3(0.0f, 0.0f, 0.0f);
				while (currentLine != "</waypoint>" && !file.eof())
				{
					//Move to next line
					getline(file, currentLine);
					if (currentLine.find("<x>") != string::npos && currentLine.find("</x>") != string::npos)
					{
						int start = currentLine.find("<x>") + 3;
						int end = currentLine.find("</x>");
						pos.x = stof(currentLine.substr(start, end - start));
					}
					else if (currentLine.find("<y>") != string::npos && currentLine.find("</y>") != string::npos)
					{
						int start = currentLine.find("<y>") + 3;
						int end = currentLine.find("</y>");
						pos.y = stof(currentLine.substr(start, end - start));
					}
					else if (currentLine.find("<z>") != string::npos && currentLine.find("</z>") != string::npos)
					{
						int start = currentLine.find("<z>") + 3;
						int end = currentLine.find("</z>");
						pos.z = stof(currentLine.substr(start, end - start));
					}
					else if (currentLine.find("<size>") != string::npos && currentLine.find("</size>") != string::npos)
					{
						int start = currentLine.find("<size>") + 6;
						int end = currentLine.find("</size>");
						w->setAreaOfInfluence(stof(currentLine.substr(start, end - start)));
					}
					else if (currentLine.find("<type>") != string::npos && currentLine.find("</type>") != string::npos)
					{
						int start = currentLine.find("<type>") + 6;
						int end = currentLine.find("</type>");
						string type = (currentLine.substr(start, end - start));
						if (type == "start")
						{
							w->setMyType(waypointType::start);
						}
						else if (type == "outpost")
						{
							w->setMyType(waypointType::outpost);
						}
						else if (type == "finish")
						{
							w->setMyType(waypointType::finish);
						}
					}
					else if (currentLine.find("<typeToAffect>") != string::npos && currentLine.find("</typeToAffect>") != string::npos)
					{
						int start = currentLine.find("<typeToAffect>") + 14;
						int end = currentLine.find("</typeToAffect>");
						w->setTypeToAffect(stoi((currentLine.substr(start, end - start))));
					}
				}
				w->SetPos(pos);
				boidMan->addWaypoint(w);
			}
			else if (currentLine == "<obstacle>")
			{
				Vector3 pos = Vector3(0.0f, 0.0f, 0.0f);
				while (currentLine != "</waypoint>" && !file.eof())
				{
					//Move to next line
					getline(file, currentLine);
					if (currentLine.find("<x>") != string::npos && currentLine.find("</x>") != string::npos)
					{
						int start = currentLine.find("<x>") + 3;
						int end = currentLine.find("</x>");
						pos.x = stof(currentLine.substr(start, end - start));
					}
					else if (currentLine.find("<y>") != string::npos && currentLine.find("</y>") != string::npos)
					{
						int start = currentLine.find("<y>") + 3;
						int end = currentLine.find("</y>");
						pos.y = stof(currentLine.substr(start, end - start));
					}
					else if (currentLine.find("<z>") != string::npos && currentLine.find("</z>") != string::npos)
					{
						int start = currentLine.find("<z>") + 3;
						int end = currentLine.find("</z>");
						pos.z = stof(currentLine.substr(start, end - start));
					}
				}
				Boid* b = boidMan->spawnBoid(0);
				b->SetPos(pos);
			}
			else if (currentLine == "<type>")
			{
				Type* t = new Type();

				while (currentLine != "</type>" && !file.eof())
				{
					//Move to next line
					getline(file, currentLine);
					if (currentLine.find("<id>") != string::npos && currentLine.find("</id>") != string::npos)
					{
						int start = currentLine.find("<id>") + 4;
						int end = currentLine.find("</id>");
						t->id = stoi(currentLine.substr(start, end - start));
					}
					else if (currentLine.find("<scale>") != string::npos && currentLine.find("</scale>") != string::npos)
					{
						int start = currentLine.find("<scale>") + 7;
						int end = currentLine.find("</scale>");
						t->scale = stoi(currentLine.substr(start, end - start));
					}
					else if (currentLine.find("<sight>") != string::npos && currentLine.find("</sight>") != string::npos)
					{
						int start = currentLine.find("<sight>") + 7;
						int end = currentLine.find("</sight>");
						t->sight = stoi(currentLine.substr(start, end - start));
					}
					else if (currentLine.find("<breedDelay>") != string::npos && currentLine.find("</breedDelay>") != string::npos)
					{
						int start = currentLine.find("<breedDelay>") + 12;
						int end = currentLine.find("</breedDelay>");
						t->breedDelay = stof(currentLine.substr(start, end - start));
					}
					else if (currentLine.find("<speed>") != string::npos && currentLine.find("</speed>") != string::npos)
					{
						int start = currentLine.find("<speed>") + 7;
						int end = currentLine.find("</speed>");
						t->speed = stof(currentLine.substr(start, end - start));
					}
					else if (currentLine.find("<health>") != string::npos && currentLine.find("</health>") != string::npos)
					{
						int start = currentLine.find("<health>") + 8;
						int end = currentLine.find("</health>");
						t->health = stof(currentLine.substr(start, end - start));
					}
					else if (currentLine.find("<prey>") != string::npos && currentLine.find("</prey>") != string::npos)
					{
						int start = currentLine.find("<prey>") + 6;
						int end = currentLine.find("</prey>");
						t->prey.push_back(stoi(currentLine.substr(start, end - start)));
					}

				}
				boidMan->addType(t);
			}
		}
		//Close the file
		file.close();
	}
	else{
		ofstream outputFile;
		outputFile.open(fileName);
		outputFile << "groupStrength = " << para.groupStrength << endl;
		outputFile << "groupDistance = " << para.groupDistance << endl;
		outputFile << "groupHeading = " << para.groupHeading << endl;
		outputFile << "boidMaxSpeed = " << para.boidMaxSpeed << endl;
		outputFile << "mapSize = " << para.mapSize << endl;
		outputFile << "starvationTime = " << para.starvationTime << endl;
		outputFile << "spawnDelay = " << para.spawnDelay << endl;
		outputFile << "canBreed = " << para.canBreed << endl;
		outputFile << "showDebugMenu = " << para.showDebugMenu << endl;
		outputFile << "showDebugForces = " << para.showDebugForces << endl;
		outputFile << "showDebugSight = " << para.showDebugSight << endl;
		outputFile << "showDebugWaypoints = " << para.showDebugWaypoints << endl;
		outputFile << "cursorObstacle = " << para.cursorObstacle << endl;
		outputFile << "respawnOnFinish = " << para.respawnOnFinish << endl;
		outputFile << "respawnOnDeath = " << para.respawnOnDeath << endl;
		outputFile << "obstacleSize = " << para.obstacleSize << endl;
		outputFile.close();
	}

}


Game::Game(ID3D11Device* _pd3dDevice, HINSTANCE _hInstance) :m_playTime(0), m_myEF(nullptr)
{
	//Initialize random seed
	srand((unsigned int)time(NULL));

	//Initialize keyboard and direct input
	m_pKeyboard = nullptr;
	m_pDirectInput = nullptr;

	//Set up DirectXTK Effects system
	m_myEF  = new MyEffectFactory(_pd3dDevice);
#if DEBUG
	m_myEF->SetPath(L"Debug\\");
#else
	m_myEF->SetPath(L"Release\\");
#endif
	
	//Create other render resources here
	m_States=new DirectX::CommonStates(_pd3dDevice);

	//Direct Input Stuff
	HRESULT hr = DirectInput8Create(_hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDirectInput, NULL);
	hr = m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, NULL);
	hr = m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hr = m_pKeyboard->SetCooperativeLevel(g_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	//Initialize the mouse
	hr = m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pMouse, NULL);
	hr = m_pMouse->SetCooperativeLevel(g_hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	hr = m_pMouse->SetDataFormat(&c_dfDIMouse);

	//Initialize game data
	m_GD = new GameData();
	m_GD->keyboard = m_keyboardState;
	m_GD->prevKeyboard = m_prevKeyboardState;
	m_GD->mouse = &m_mouse_state;
	m_GD->prevMouse = &m_prev_mouse_state;
	m_GD->GS = GS_PLAY_PLAY;
	m_GD->p3d = _pd3dDevice;
	m_GD->EF = m_myEF;
	
	//Initialize boid manager
	boidMan = new boidManager();
	m_GameObjects.push_back(boidMan);

	//Load in simulation paramaters
	loadParameters();


	//Initialize main camera
	m_mainCam = new Camera(0.25f * XM_PI, 640.0f / 480.0f, 1.0f, 10000.0f, Vector3::Zero, Vector3::UnitY);
	m_mainCam->SetPos(Vector3(0.0f, 1000.0f, 100.0f));
	m_GameObjects.push_back(m_mainCam);
		
	//Initialize predator camera
	m_predCamera = new PredCamera(0.25f * XM_PI, 640.0f / 480.0f, 1.0f, 10000.0f, Vector3::Up, Vector3(0.0f, 100.0f, -150.0f));
	m_predCamera->SetPos(Vector3(0.0f, 1000.0f, 100.0f));
	m_GameObjects.push_back(m_predCamera);
	
	//Initialize light
	m_Light = new Light(Vector3(0.0f, 100.0f, 160.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), Color(1.0f, 1.0f, 1.0f, 1.0f));
	m_GameObjects.push_back(m_Light);

	//Initialize 3d device
	ID3D11DeviceContext* pd3dImmediateContext;
	_pd3dDevice->GetImmediateContext(&pd3dImmediateContext);
	
	//Create DirectXTK spritebatch stuff
	m_DD2D = new DrawData2D();
	m_DD2D->m_Sprites.reset(new SpriteBatch(pd3dImmediateContext));
	m_DD2D->m_Font.reset(new SpriteFont(_pd3dDevice, L"italic.spritefont"));
		
	//Create Draw Data
	m_DD = new DrawData();
	m_DD->cam = m_mainCam;
	m_DD->pd3dImmediateContext = pd3dImmediateContext;
	m_DD->states = m_States;
	m_DD->light = m_Light;

	//Initilise the defaults for the VBGOs
	VBGO::Init(_pd3dDevice);
}

Game::~Game()
{
	//Cleanup VBGO
	VBGO::CleanUp();

	//Tidy away Direct Input Stuff
	if (m_pMouse)
	{
		m_pMouse->Unacquire();
		m_pMouse->Release();
	}
	if (m_pKeyboard)
	{
		m_pKeyboard->Unacquire();
		m_pKeyboard->Release();
	}
	if (m_pDirectInput) m_pDirectInput->Release();

	//Get rid of the game objects
	for (list<GameObject *>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		delete (*it);
	}
	m_GameObjects.clear();

	//And the 2D game objects
	for (list<GameObject2D *>::iterator it = m_GameObject2Ds.begin(); it != m_GameObject2Ds.end(); it++)
	{
		delete (*it);
	}
	m_GameObject2Ds.clear();

	//Delete other stuff
	delete m_States;
	delete m_myEF;
	delete m_GD;
	delete m_DD;
}

bool Game::update()
{
	//Read input
	ReadKeyboard();
	ReadMouse();

	if (m_mouse_state.rgbButtons[0] & 0x80 && !(m_prev_mouse_state.rgbButtons[0] & 0x80))
	{
		POINT mousePos;
		GetCursorPos(&mousePos);
		if (mousePos.x <= 380)
		{
			if (SimulationParameters::showDebugMenu)
			{
				if (mousePos.y < 400 && mousePos.y > 378)
				{
					//Clicked respawn on death
					SimulationParameters::showDebugForces = !SimulationParameters::showDebugForces;
				}
				else if (mousePos.y < 422 && mousePos.y > 400)
				{
					//Clicked respawn on death
					SimulationParameters::showDebugSight = !SimulationParameters::showDebugSight;
				}
				else if (mousePos.y < 444 && mousePos.y > 422)
				{
					//Clicked respawn on death
					SimulationParameters::showDebugWaypoints = !SimulationParameters::showDebugWaypoints;
				}
				else if (mousePos.y < 466 && mousePos.y > 444)
				{
					//Clicked respawn on death
					SimulationParameters::respawnOnDeath = !SimulationParameters::respawnOnDeath;
				}
				else if (mousePos.y < 488 && mousePos.y > 466)
				{
					//Clicked respawn on death
					SimulationParameters::respawnOnFinish = !SimulationParameters::respawnOnFinish;
				}
				else if (mousePos.y < 510 && mousePos.y > 488)
				{
					//Clicked respawn on death
					SimulationParameters::spawnDelay += 200.0f;
				}
			}
			else
			{
				if (mousePos.y <= 75 && mousePos.y >= 48)
				{
					//Clicked respawn on death
					SimulationParameters::showDebugMenu = !SimulationParameters::showDebugMenu;
				}
			}
		}
	}
	//Right click
	else
	{
		if (m_mouse_state.rgbButtons[1] & 0x80 && !(m_prev_mouse_state.rgbButtons[1] & 0x80))
		{
			POINT mousePos;
			GetCursorPos(&mousePos);
			if (mousePos.x <= 380)
			{
				if (SimulationParameters::showDebugMenu)
				{
					if (mousePos.y <= 510 && mousePos.y >= 488)
					{
						if (SimulationParameters::spawnDelay > 0)
						{
							SimulationParameters::spawnDelay -= 200.0f;
						}
					}
				}
			}
		}
	}

	//Exit the simulation
	if (m_keyboardState[DIK_ESCAPE] & 0x80 || m_mouse_state.rgbButtons[2] & 0x80)
	{
		return false;
	}

	//Toggle spawn/kill
	if (((m_keyboardState[DIK_DELETE] & 0x80) && !(m_prevKeyboardState[DIK_DELETE] & 0x80)) || ((m_keyboardState[DIK_BACKSPACE] & 0x80) && !(m_prevKeyboardState[DIK_BACKSPACE] & 0x80)))
	{
		spawn = !spawn;
	}

	//Toggle cursor obstacle
	if ((m_keyboardState[DIK_TAB] & 0x80) && !(m_prevKeyboardState[DIK_TAB] & 0x80))
	{
		if (m_DD->cam == m_mainCam)
		{
			SimulationParameters::cursorObstacle = !SimulationParameters::cursorObstacle;
			ShowCursor(!SimulationParameters::cursorObstacle);
		}
	}

	//Toggle breeding
	if ((m_keyboardState[DIK_B] & 0x80) && !(m_prevKeyboardState[DIK_B] & 0x80))
	{
		SimulationParameters::canBreed = !SimulationParameters::canBreed;
	}

	//Toggle pause/play
	if ((m_keyboardState[DIK_SPACE] & 0x80) && !(m_prevKeyboardState[DIK_SPACE] & 0x80))
	{
		if (m_GD->GS == GS_PLAY_PAUSE)
		{
			m_GD->GS = m_GD->prePauseGS;
		}
		else
		{
			m_GD->prePauseGS = m_GD->GS;
			m_GD->GS = GS_PLAY_PAUSE;
		}
	}

	//Toggle camera
	if ((m_keyboardState[DIK_RETURN] & 0x80) && !(m_prevKeyboardState[DIK_RETURN] & 0x80))
	{
		if (m_DD->cam != m_predCamera)
		{
			m_DD->cam = m_predCamera;
			m_predCamera->changeTarget(boidMan->getHighestBOID());
		}
		else
		{
			m_DD->cam = m_mainCam;
		}
	}

	//Zoom in
	if ((m_keyboardState[DIK_UPARROW] & 0x80))
	{
		if (m_DD->cam == m_mainCam){
			if (m_mainCam->GetPos().y > 0.0f){
				m_mainCam->SetPos(m_mainCam->GetPos() - Vector3(0.0f, 100.0f, 0.0f));
			}
		}
	}

	//Zoom out
	if ((m_keyboardState[DIK_DOWNARROW] & 0x80))
	{
		if (m_DD->cam == m_mainCam){
			m_mainCam->SetPos(m_mainCam->GetPos() + Vector3(0.0f, 100.0f, 0.0f));
		}
	}

	//Increase spawn per press
	if (m_keyboardState[DIK_ADD] & 0x80 || m_keyboardState[DIK_NUMPADPLUS] & 0x80)
	{
		spawnPerPress += 1;
	}

	//Decrease spawn per press
	if (m_keyboardState[DIK_SUBTRACT] & 0x80 || m_keyboardState[DIK_MINUS] & 0x80)
	{
		if (spawnPerPress > 1)
		{
			spawnPerPress -= 1;
		}
	}

	//Initialize type as high number so it can be used to check if 1-5 was pressed
	int type = 9999;
	//Change type based on key pressed
	if ((m_keyboardState[DIK_0] & 0x80) && !(m_prevKeyboardState[DIK_0] & 0x80))
	{
		type = 0;
	}
	if ((m_keyboardState[DIK_1] & 0x80) && !(m_prevKeyboardState[DIK_1] & 0x80))
	{
		type = 1;
	}
	if ((m_keyboardState[DIK_2] & 0x80) && !(m_prevKeyboardState[DIK_2] & 0x80))
	{
		type = 2;
	}
	if ((m_keyboardState[DIK_3] & 0x80) && !(m_prevKeyboardState[DIK_3] & 0x80))
	{
		type = 3;
	}
	if ((m_keyboardState[DIK_4] & 0x80) && !(m_prevKeyboardState[DIK_4] & 0x80))
	{
		type = 4;
	}
	if ((m_keyboardState[DIK_5] & 0x80) && !(m_prevKeyboardState[DIK_5] & 0x80))
	{
		type = 5;
	}
	//If a key was pressed
	if (type != 9999)
	{
		//If killing
		if (!spawn)
		{
			//Kill x boids of type
			for (int i = 0; i < spawnPerPress; i++)
			{
				boidMan->deleteBoid(type);
			}
		}
		else
		{
			//Spawn x boids of type
			for (int i = 0; i < spawnPerPress; i++)
			{

				boidMan->spawnBoid(type);
			}
		}
	}

	//Toggle debug information
	if ((m_keyboardState[DIK_F1] & 0x80) && !(m_prevKeyboardState[DIK_F1] & 0x80))
	{
		SimulationParameters::showDebugMenu = !SimulationParameters::showDebugMenu;
	}
	//Reload parameters
	if ((m_keyboardState[DIK_F5] & 0x80) && !(m_prevKeyboardState[DIK_F5] & 0x80))
	{
		loadParameters();
		boidMan->respawnAllBoids(true);
	}
	/*Open parameters file
	if ((m_keyboardState[DIK_F6] & 0x80) && !(m_prevKeyboardState[DIK_F6] & 0x80))
	{
		string cmd = "notepad.exe " + fileName;
		system(cmd.c_str());
		loadParameters();
	}
	//Open map file
	if ((m_keyboardState[DIK_F8] & 0x80) && !(m_prevKeyboardState[DIK_F8] & 0x80))
	{
		boidMan->deleteAllWaypoints();
		string cmd = "notepad.exe " + boidMan->fileName;
		system(cmd.c_str());
		boidMan->loadMap();
	}
	*/
	//Delete all boids
	if ((m_keyboardState[DIK_F9] & 0x80) && !(m_prevKeyboardState[DIK_F9] & 0x80))
	{
		boidMan->deleteAll();
	}

	//Calculate frame time-step dt for passing down to game objects
	DWORD currentTime = GetTickCount();
	m_GD->dt = min((float)(currentTime - m_playTime) / 1000.0f, 0.1f);
	m_playTime = currentTime;


	//Update all objects
	for (list<GameObject *>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		(*it)->Tick(m_GD);
	}
	for (list<GameObject2D *>::iterator it = m_GameObject2Ds.begin(); it != m_GameObject2Ds.end(); it++)
	{
		(*it)->tick(m_GD);
	}
	return true;
}

void Game::render(ID3D11DeviceContext* _pd3dImmediateContext)
{
	m_DD->pd3dImmediateContext = _pd3dImmediateContext;
	

	VBGO::UpdateConstantBuffer(m_DD);

	//draw all 3D objects
	for (list<GameObject *>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		(*it)->Draw(m_DD);
	}

	// Draw sprite batch stuff
	m_DD2D->m_Sprites->Begin();
	for (list<GameObject2D *>::iterator it = m_GameObject2Ds.begin(); it != m_GameObject2Ds.end(); it++)
	{
		(*it)->draw(m_DD2D);
	}
	//Draw debug text
	DirectX::XMVECTOR textColor = Colors::Green;
	if (SimulationParameters::showDebugMenu){
		stringstream sstm;
		float yPos = 10.0f;
		sstm << "~ Debug Menu ~";
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::SlateGray, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 44.0f;

		sstm << "~ Controls ~";
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::LightSlateGray, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 22.0f;

		sstm << "[F1] - Toggle Menu";
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), textColor, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 22.0f;

		sstm << "[F5] - Reload Settings";
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), textColor, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 22.0f;

		sstm << "[F9] - Kill Everything";
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), textColor, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 22.0f;

		sstm << "[Tab] - Toggle Obstacle Mouse";
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), textColor, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 22.0f;

		string spawnkill = "Spawn";
		if (!spawn)
		{
			spawnkill = "Kill";
		}
		sstm << "[1-5] - " << spawnkill << " " << spawnPerPress << " boid(s)";
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), textColor, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 22.0f;

		sstm << "[Delete] - Toggle Spawn/Kill";
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), textColor, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 22.0f;

		sstm << "[+/-] Adjust Spawn Amount";
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), textColor, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 22.0f;

		sstm << "[Enter] - Switch Camera";
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), textColor, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 22.0f;

		sstm << "[Space] Play/Pause";
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), textColor, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 44.0f;

		sstm << "~ Simulation Settings ~";
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::LightSlateGray, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 22.0f;

		string tempS = "False";
		XMVECTOR tempColor = Colors::Red;
		if (SimulationParameters::showDebugForces) { tempS = "True"; tempColor = Colors::Green; }
		sstm << "Display Force Arrows: " << tempS;
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), tempColor, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 22.0f;

		tempS = "False";
		tempColor = Colors::Red;
		if (SimulationParameters::showDebugSight) { tempS = "True"; tempColor = Colors::Green; }
		sstm << "Display Sight Range: " << tempS;
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), tempColor, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 22.0f;

		tempS = "False";
		tempColor = Colors::Red;
		if (SimulationParameters::showDebugWaypoints) { tempS = "True"; tempColor = Colors::Green; }
		sstm << "Display Waypoints: " << tempS;
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), tempColor, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 22.0f;

		tempS = "False";
		tempColor = Colors::Red;
		if (SimulationParameters::respawnOnDeath) { tempS = "True"; tempColor = Colors::Green; }
		sstm << "Respawn On Death: " << tempS;
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), tempColor, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 22.0f;

		tempS = "False";
		tempColor = Colors::Red;
		if (SimulationParameters::respawnOnFinish) { tempS = "True"; tempColor = Colors::Green; }
		sstm << "Respawn On Finish: " << tempS;
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), tempColor, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 22.0f;

		tempS = "False";
		tempColor = Colors::Red;
		if (SimulationParameters::canBreed) { tempS = "True"; tempColor = Colors::Green; }
		sstm << "Breeding: " << tempS;
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), tempColor, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 22.0f;

		sstm << "Spawn Delay: " << SimulationParameters::spawnDelay;
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), textColor, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 44.0f;

		sstm << "~ Boid Settings ~";
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::LightSlateGray, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 22.0f;

		sstm << "Max Speed (Tier 1): " << SimulationParameters::boidMaxSpeed;
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), textColor, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 22.0f;

		sstm << "Flock Size: " << SimulationParameters::groupDistance;
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), textColor, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 22.0f;

		sstm << "Flocking Strength: " << SimulationParameters::groupStrength;
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), textColor, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 22.0f;

		sstm << "Velocity Matching Strength: " << SimulationParameters::groupHeading;
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), textColor, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 44.0f;

		if (SimulationParameters::boidCount.size() > 0)
		{
			sstm << "~ Tally ~";
			m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::LightSlateGray, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
			sstm.str(std::string());
			yPos += 22.0f;
		}

		for (map<int, int>::iterator it = SimulationParameters::boidCount.begin(); it != SimulationParameters::boidCount.end(); it++)
		{
			sstm << "# of type [" << (*it).first << "]: " << (*it).second;
			m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), textColor, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
			sstm.str(std::string());
			yPos += 22.0f;
		}
		if (SimulationParameters::boidCount.size() > 0)
		{
			yPos += 22.0f;
		}

		POINT mouse;
		GetCursorPos(&mouse);
		sstm << "Mouse Position: x:" << mouse.x << " y:" << mouse.y;
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), textColor, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 44.0f;

		if (m_DD->cam == m_predCamera && m_predCamera->GetTarget() != nullptr && m_predCamera->GetTarget()->isAlive())
		{
			sstm << "~ Selected Unit ~";
			m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::LightSlateGray, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
			sstm.str(std::string());
			yPos += 22.0f;

			sstm << "Current Health: " << m_predCamera->GetTarget()->getHealth();
			m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), textColor, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
			sstm.str(std::string());
			yPos += 22.0f;

			sstm << "Current Weight: " << m_predCamera->GetTarget()->getWeight();
			m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), textColor, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
			sstm.str(std::string());
			yPos += 22.0f;

			sstm << "Current Speed: " << m_predCamera->GetTarget()->getSpeed();
			m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), textColor, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
			sstm.str(std::string());
			yPos += 22.0f;

			sstm << "Maximum Speed: " << m_predCamera->GetTarget()->getMaxSpeed();
			m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), textColor, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
			sstm.str(std::string());
			yPos += 22.0f;

			sstm << "Current Position (x,y,z): " << m_predCamera->GetTarget()->GetPos().x << ", " << m_predCamera->GetTarget()->GetPos().y << ", " << m_predCamera->GetTarget()->GetPos().z;
			m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), textColor, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
			sstm.str(std::string());
			yPos += 22.0f;

			sstm << "Current Direction (x,y,z): " << m_predCamera->GetTarget()->getDirection().x << ", " << m_predCamera->GetTarget()->getDirection().y << ", " << m_predCamera->GetTarget()->getDirection().z;
			m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), textColor, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
			sstm.str(std::string());
			yPos += 22.0f;
		}
	}
	else
	{
		string s = "[F1] - Toggle Debug Menu";
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(s.c_str()), Vector2(10, 10), textColor, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
	}
	//End drawing
	m_DD2D->m_Sprites->End();
	_pd3dImmediateContext->OMSetDepthStencilState(m_States->DepthDefault(), 0);

}

bool Game::ReadKeyboard()
{
	//Copy over old keyboard state
	memcpy(m_prevKeyboardState, m_keyboardState, sizeof(m_keyboardState));

	//Clear out previous state
	ZeroMemory(&m_keyboardState, sizeof(m_keyboardState));

	//Read the keyboard device.
	HRESULT hr = m_pKeyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if (FAILED(hr))
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			m_pKeyboard->Acquire();
		}
		else
		{
			return false;
		}
	}
	return true;
}

bool Game::ReadMouse()
{
	//Clear out previous state
	CopyMemory(&m_prev_mouse_state, &m_mouse_state, sizeof(m_mouse_state));
	ZeroMemory(&m_mouse_state, sizeof(m_mouse_state));

	//Read the Mouse device.
	HRESULT hr = m_pMouse->GetDeviceState(sizeof(m_mouse_state), (LPVOID)&m_mouse_state);
	if (FAILED(hr))
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if ((hr == DIERR_INPUTLOST) || (hr == DIERR_NOTACQUIRED))
		{
			m_pMouse->Acquire();
		}
		else
		{
			return false;
		}
	}
	//Set the cursor to the middle of the screen
	if (SimulationParameters::cursorObstacle)
	{
		RECT rc;
		const HWND hDesktop = GetDesktopWindow();
		GetWindowRect(hDesktop, &rc);
		SetCursorPos(rc.right / 2, rc.bottom / 2);
	}
	return true;

}
