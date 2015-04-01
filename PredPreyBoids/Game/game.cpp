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
bool SimulationParameters::cursorObstacle;
float SimulationParameters::starvationTime;
bool SimulationParameters::showDebug;

void Game::loadParameters()
{
	SimulationParameters para;

	//Default values
	para.groupStrength = 0.3f;
	para.groupDistance = 200.0f;
	para.groupHeading = 0.7f;
	para.boidMaxSpeed = 20.0f;
	para.mapSize = 500.0f;
	para.starvationTime = 5000.0f;
	para.showDebug = true;
	para.cursorObstacle = false;
	para.obstacleSize = 40.0f;
	ifstream parameterFile;
	parameterFile.open(fileName);
	if (parameterFile.is_open())
	{
		loadedFile = true;
		while (!parameterFile.eof())
		{
			string currentLine;
			getline(parameterFile, currentLine);
			if (currentLine.find(" = "))
			{
				string parameter;
				string value;
				parameter = currentLine.substr(0, currentLine.find(" = "));
				value = currentLine.substr(currentLine.find(" = ") + 3, currentLine.size());

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
				else if (parameter == "showDebug")
				{
					para.showDebug = (value == "true");
				}
				else if (parameter == "cursorObstacle")
				{
					para.cursorObstacle = (value == "true");
				}
			}
		}
		parameterFile.close();
	}
}


Game::Game(ID3D11Device* _pd3dDevice, HINSTANCE _hInstance) :m_playTime(0), m_myEF(nullptr)
{
	/* initialize random seed: */
	srand(time(NULL));

	m_pKeyboard = nullptr;
	m_pDirectInput = nullptr;

	//Set up DirectXTK Effects system
	m_myEF  = new MyEffectFactory(_pd3dDevice);
#if DEBUG
	m_myEF->SetPath(L"Debug\\");
#else
	m_myEF->SetPath(L"Release\\");
#endif
	
	// Create other render resources here
	m_States=new DirectX::CommonStates(_pd3dDevice);

	//Direct Input Stuff
	HRESULT hr = DirectInput8Create(_hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_pDirectInput, NULL);
	hr = m_pDirectInput->CreateDevice(GUID_SysKeyboard, &m_pKeyboard, NULL);
	hr = m_pKeyboard->SetDataFormat(&c_dfDIKeyboard);
	hr = m_pKeyboard->SetCooperativeLevel(g_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

	// initialize the mouse
	hr = m_pDirectInput->CreateDevice(GUID_SysMouse, &m_pMouse, NULL);
	hr = m_pMouse->SetCooperativeLevel(g_hWnd, DISCL_BACKGROUND | DISCL_NONEXCLUSIVE);
	hr = m_pMouse->SetDataFormat(&c_dfDIMouse);

	m_GD = new GameData();
	m_GD->keyboard = m_keyboardState;
	m_GD->prevKeyboard = m_prevKeyboardState;
	m_GD->mouse = &m_mouse_state;
	m_GD->prevMouse = &m_prev_mouse_state;
	m_GD->GS = GS_PLAY_PLAY;
	m_GD->p3d = _pd3dDevice;
	m_GD->EF = m_myEF;
	
	loadParameters();

	boidMan = new boidManager();
	m_GameObjects.push_back(boidMan);
	
	m_mainCam = new Camera(0.25f * XM_PI, 640.0f / 480.0f, 1.0f, 10000.0f, Vector3::Zero, Vector3::UnitY);
	m_mainCam->SetPos(Vector3(0.0f, 1000.0f, 100.0f));
	m_GameObjects.push_back(m_mainCam);
		
	m_predCamera = new PredCamera(0.25f * XM_PI, 640.0f / 480.0f, 1.0f, 10000.0f, Vector3::Up, Vector3(0.0f, 100.0f, -150.0f));
	m_predCamera->SetPos(Vector3(0.0f, 1000.0f, 100.0f));
	m_GameObjects.push_back(m_predCamera);
	
	m_Light = new Light(Vector3(0.0f, 100.0f, 160.0f), Color(1.0f, 1.0f, 1.0f, 1.0f), Color(0.4f, 0.1f, 0.1f, 1.0f));
	m_GameObjects.push_back(m_Light);

	
	ID3D11DeviceContext* pd3dImmediateContext;
	_pd3dDevice->GetImmediateContext(&pd3dImmediateContext);
	
	// Create DirectXTK spritebatch stuff
	m_DD2D = new DrawData2D();
	m_DD2D->m_Sprites.reset(new SpriteBatch(pd3dImmediateContext));
	m_DD2D->m_Font.reset(new SpriteFont(_pd3dDevice, L"italic.spritefont"));
		
	//create Draw Data
	m_DD = new DrawData();
	m_DD->cam = m_mainCam;
	m_DD->pd3dImmediateContext = pd3dImmediateContext;
	m_DD->states = m_States;
	m_DD->light = m_Light;


	//initilise the defaults for the VBGOs
	VBGO::Init(_pd3dDevice);

}

Game::~Game()
{
	VBGO::CleanUp();

	//tidy away Direct Input Stuff
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

	//get rid of the game objects here
	for (list<GameObject *>::iterator it = m_GameObjects.begin(); it != m_GameObjects.end(); it++)
	{
		delete (*it);
	}

	m_GameObjects.clear();
	//and the 2D ones
	for (list<GameObject2D *>::iterator it = m_GameObject2Ds.begin(); it != m_GameObject2Ds.end(); it++)
	{
		delete (*it);
	}

	m_GameObject2Ds.clear();

	delete m_States;
	delete m_myEF;
	delete m_GD;
	delete m_DD;

}

bool Game::update()
{
	ReadKeyboard();
	ReadMouse();

	if (m_keyboardState[DIK_ESCAPE] & 0x80)
	{
		return false;
	}

	if (m_mouse_state.rgbButtons[2] & 0x80)
	{
		return false;
	}

	if ((m_keyboardState[DIK_F2] & 0x80) && !(m_prevKeyboardState[DIK_F2] & 0x80))
	{
		if (m_DD->cam == m_mainCam)
		{
			SimulationParameters::cursorObstacle = !SimulationParameters::cursorObstacle;
			ShowCursor(!SimulationParameters::cursorObstacle);
		}
	}

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

	if ((m_keyboardState[DIK_MINUS] & 0x80) && !(m_prevKeyboardState[DIK_MINUS] & 0x80))
	{
		if (spawnPerPress >= 1)
		{
			spawnPerPress -= 1;
		}
	}

	if ((m_keyboardState[DIK_ADD] & 0x80) && !(m_prevKeyboardState[DIK_ADD] & 0x80))
	{
		spawnPerPress += 1;
	}

	int type = 9999;
	bool kill = false;
	bool isShiftHeld = false;
	if (m_keyboardState[DIK_LSHIFT] & 0x80)
	{
		isShiftHeld = true;
	}
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
	if (type != 9999)
	{
		if (isShiftHeld)
		{
			for (int i = 0; i < spawnPerPress; i++)
			{

				boidMan->deleteBoid(type);
			}
		}
		else
		{
			for (int i = 0; i < spawnPerPress; i++)
			{

				boidMan->spawnBoid(type);
			}
		}
	}

	if ((m_keyboardState[DIK_F1] & 0x80) && !(m_prevKeyboardState[DIK_F1] & 0x80))
	{
		SimulationParameters::showDebug = !SimulationParameters::showDebug;
	}
	if ((m_keyboardState[DIK_F5] & 0x80) && !(m_prevKeyboardState[DIK_F5] & 0x80))
	{
		loadParameters();
	}
	if ((m_keyboardState[DIK_F6] & 0x80) && !(m_prevKeyboardState[DIK_F6] & 0x80))
	{
		ifstream parameterFile;
		parameterFile.open(fileName);
		string cmd = "notepad.exe " + fileName;
		system(cmd.c_str());
		loadParameters();
	}
	if ((m_keyboardState[DIK_F9] & 0x80) && !(m_prevKeyboardState[DIK_F9] & 0x80))
	{
		boidMan->deleteAll();
	}


	//calculate frame time-step dt for passing down to game objects
	DWORD currentTime = GetTickCount();
	m_GD->dt = min((float)(currentTime - m_playTime) / 1000.0f, 0.1f);
	m_playTime = currentTime;


	//update all objects
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
	if (SimulationParameters::showDebug){
		stringstream sstm;
		float yPos = 10.0f;
		sstm << "~ Debug Menu ~";
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::Green, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 40.0f;

		sstm << "~ Booleans ~";
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::Green, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 20.0f;

		string tempS = "False";
		if (loadedFile){ tempS = "True"; }
		sstm << "Loaded from file: " << tempS;
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::Green, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 20.0f;

		tempS = "False";
		if (SimulationParameters::cursorObstacle){ tempS = "True"; }
		sstm << "Mouse is obstacle: " << tempS;
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::Green, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 40.0f;

		sstm << "~ Controls ~";
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::Green, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 20.0f;

		sstm << "[F1] - Toggle debug menu";
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::Green, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 20.0f;

		sstm << "[F2] - Toggle obstacle cursor";
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::Green, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 20.0f;

		sstm << "[F5] - Update parameters from file";
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::Green, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 20.0f;

		sstm << "[F6] - Open parameters file";
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::Green, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 20.0f;

		sstm << "[F9] - Delete all BOIDs";
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::Green, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 20.0f;

		sstm << "[1-5] - Spawn " << spawnPerPress << " BOID(s)";
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::Green, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 20.0f;

		sstm << "[Shift + 1-5] - Delete " << spawnPerPress << " BOID(s)";
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::Green, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 20.0f;

		sstm << "[+/-] Modify spawn per press";
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::Green, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 20.0f;

		sstm << "[Space] Pause/unpause simulation";
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::Green, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 20.0f;

		sstm << "[Enter] - Switch camera";
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::Green, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 40.0f;

		sstm << "~ Simulation Parameters ~";
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::Green, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 20.0f;

		sstm << "Spawn per key press: " << spawnPerPress;
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::Green, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 20.0f;

		sstm << "Time to starve: " << SimulationParameters::starvationTime;
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::Green, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 20.0f;

		sstm << "Max speed: " << SimulationParameters::boidMaxSpeed;
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::Green, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 20.0f;

		sstm << "Group size: " << SimulationParameters::groupDistance;
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::Green, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 20.0f;

		sstm << "Group strength: " << SimulationParameters::groupStrength;
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::Green, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 20.0f;

		sstm << "Group direction matching strength: " << SimulationParameters::groupHeading;
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::Green, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
		sstm.str(std::string());
		yPos += 40.0f;

		if (SimulationParameters::boidCount.size() > 0)
		{
			sstm << "~ BOID tally ~";
			m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::Green, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
			sstm.str(std::string());
			yPos += 20.0f;
		}

		for (map<int, int>::iterator it = SimulationParameters::boidCount.begin(); it != SimulationParameters::boidCount.end(); it++)
		{
			sstm << "# of type [" << (*it).first << "]: " << (*it).second;
			m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::Green, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
			sstm.str(std::string());
			yPos += 20.0f;
		}
		if (SimulationParameters::boidCount.size() > 0)
		{
			yPos += 20.0f;
		}

		if (m_DD->cam == m_predCamera && m_predCamera->GetTarget() != nullptr && m_predCamera->GetTarget()->isAlive())
		{
			sstm << "~ Current BOID ~";
			m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::Green, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
			sstm.str(std::string());
			yPos += 20.0f;

			sstm << "Current health: " << m_predCamera->GetTarget()->getHealth();
			m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::Green, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
			sstm.str(std::string());
			yPos += 20.0f;

			sstm << "Current weight: " << m_predCamera->GetTarget()->getWeight();
			m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::Green, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
			sstm.str(std::string());
			yPos += 20.0f;

			sstm << "Current speed: " << m_predCamera->GetTarget()->getSpeed();
			m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::Green, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
			sstm.str(std::string());
			yPos += 20.0f;

			sstm << "Maximum speed: " << m_predCamera->GetTarget()->getMaxSpeed();
			m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::Green, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
			sstm.str(std::string());
			yPos += 20.0f;

			sstm << "Current position (x,y,z): " << m_predCamera->GetTarget()->GetPos().x << ", " << m_predCamera->GetTarget()->GetPos().y << ", " << m_predCamera->GetTarget()->GetPos().z;
			m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::Green, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
			sstm.str(std::string());
			yPos += 20.0f;

			sstm << "Current direction (x,y,z): " << m_predCamera->GetTarget()->getDirection().x << ", " << m_predCamera->GetTarget()->getDirection().y << ", " << m_predCamera->GetTarget()->getDirection().z;
			m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(sstm.str().c_str()), Vector2(10, yPos), Colors::Green, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
			sstm.str(std::string());
			yPos += 20.0f;
		}
	}
	else
	{
		string s = "[F1] - Toggle debug menu";
		m_DD2D->m_Font->DrawString(m_DD2D->m_Sprites.get(), Helper::charToWChar(s.c_str()), Vector2(10, 10), Colors::Green, 0.0f, g_XMZero, Vector2(0.5, 0.5), SpriteEffects::SpriteEffects_None, 0.0f);
	}
	m_DD2D->m_Sprites->End();
	_pd3dImmediateContext->OMSetDepthStencilState(m_States->DepthDefault(), 0);

}

bool Game::ReadKeyboard()
{
	//copy over old keyboard state
	memcpy(m_prevKeyboardState, m_keyboardState, sizeof(m_keyboardState));

	//clear out previous state
	ZeroMemory(&m_keyboardState, sizeof(m_keyboardState));

	// Read the keyboard device.
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
	//clear out previous state
	CopyMemory(&m_prev_mouse_state, &m_mouse_state, sizeof(m_mouse_state));
	ZeroMemory(&m_mouse_state, sizeof(m_mouse_state));

	// Read the Mouse device.
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
	if (SimulationParameters::cursorObstacle)
	{
		RECT rc;
		const HWND hDesktop = GetDesktopWindow();
		GetWindowRect(hDesktop, &rc);
		SetCursorPos(rc.right / 2, rc.bottom / 2);
	}
	return true;

}
