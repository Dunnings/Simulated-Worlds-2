#include "boid.h"
#include <Windows.h>


Boid::Boid(string _fileName, ID3D11Device* _pd3dDevice, MyEffectFactory* _EF) :CMOGO(_fileName, _pd3dDevice, _EF)
{
	m_pos.y = 10.0f;
	m_scale = 2.0f * Vector3::One;
}

Boid::~Boid()
{

}

void Boid::Tick(GameData* GD, Vector3 modifier)
{
	//Initialise variable to hold new position
	Vector3 newPos = m_pos;
	//Set direction and position y value to 0
	newPos.y = 0.0f;
	m_direction.y = 0.0f;
	//If the boid has a target set direction to aim at that target and go toward it
	if (m_target != nullptr)
	{
		Vector3 dir = m_target->GetPos() - m_pos;
		dir.Normalize();
		m_direction = dir;
		m_speed = m_target->getSpeed() * 1.3f;
		if ((m_target->GetPos() - m_pos).Length() < 5.0f)
		{
			m_target->Damage(100.0f);
			if (!m_target->isAlive())
			{
				m_target = nullptr;
			}
		}
	}
	newPos += (m_direction + modifier) * m_speed * GD->dt;
	newPos += modifier;
	bool canMove = true;
	//Too far right
	if (newPos.x > SimulationParameters::mapSize / 2 || newPos.x < -(SimulationParameters::mapSize / 2))
	{
		m_direction.x = -m_direction.x;
		canMove = false;
	}
	if (newPos.z > SimulationParameters::mapSize / 2 || newPos.z < -(SimulationParameters::mapSize / 2))
	{
		m_direction.z = -m_direction.z;
		canMove = false;
	}
	if (canMove)
	{
		Vector3 yawDirection = (m_direction + modifier);
		yawDirection.Normalize();
		m_yaw = atan2(yawDirection.x, yawDirection.z);
		m_pos = newPos;
	}
	GameObject::Tick(GD);
}

void Boid::Draw(DrawData* DD)
{
	CMOGO::Draw(DD);
}

void Boid::aquireTarget(vector<Boid*> boidVector)
{
	Boid* target = nullptr;
	float distance;
	for (vector<Boid*>::iterator it = boidVector.begin(); it != boidVector.end(); ++it)
	{
		Boid* currentBoid = (*it);
		if (m_type > currentBoid->getType() && currentBoid->getType() != BoidType::BOID_OBSTACLE)
		{
			if (target == nullptr)
			{
				distance = (currentBoid->GetPos() - m_pos).Length();
				target = currentBoid;
			}
			else
			{
				if ((currentBoid->GetPos() - m_pos).Length() < distance)
				{
					target = currentBoid;
				}
			}
		}
	}
	m_target = target;
}