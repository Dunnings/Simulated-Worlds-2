#include "boid.h"
#include <Windows.h>


Boid::Boid(string _fileName) : CMOGO(_fileName, GameData::p3d, GameData::EF)
{
	m_pos.y = 10.0f;
	m_scale = 2.0f * Vector3::One;
}

Boid::~Boid()
{

}

void Boid::Tick(GameData* GD)
{
	m_direction.y = 0.0f;
	//If the boid has a target set direction to aim at that target and go toward it
	if (m_target != nullptr)
	{
		Vector3 dir = m_target->GetPos() - m_pos;
		dir.Normalize();
		m_direction = dir;
		m_speed = SimulationParameters::boidMaxSpeed;
		m_speed *= 1.5f;
		m_speed += 0.05f * (m_target->GetPos() - m_pos).Length();
		if ((m_target->GetPos() - m_pos).Length() < 5.0f)
		{
			m_target->Damage(100.0f);
			if (!m_target->isAlive())
			{
				m_target = nullptr;
				m_level += 1;
				float scale = 1.0f + (m_level * 0.05);
				SetScale(scale);
			}
		}
		m_direction.Normalize();
		m_pos += m_direction * m_speed * GD->dt;
	}
	else
	{
		if (m_pos.x > SimulationParameters::mapSize / 2 || m_pos.x < -(SimulationParameters::mapSize / 2) || m_pos.z > SimulationParameters::mapSize / 2 || m_pos.z < -(SimulationParameters::mapSize / 2))
		{
			Vector3 centerModifier = (Vector3(0.0f, 0.0f, 0.0f) - m_pos);
			centerModifier.Normalize();
			m_direction += centerModifier;
		}
		//Modify m_pos
		m_direction.Normalize();
		m_pos += m_direction * m_speed * GD->dt;
	}
	//Point in direction of travel
	m_yaw = atan2(m_direction.x, m_direction.z);
	GameObject::Tick(GD);
}

void Boid::Draw(DrawData* DD)
{
	CMOGO::Draw(DD);
}

