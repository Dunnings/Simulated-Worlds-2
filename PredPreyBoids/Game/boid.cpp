#include "boid.h"
#include <Windows.h>


Boid::Boid(string _fileName) : CMOGO(_fileName, GameData::p3d, GameData::EF)
{
	m_pos.y = 10.0f;
	m_scale = 2.0f * Vector3::One;
	max_speed = SimulationParameters::boidMaxSpeed;
}

Boid::~Boid()
{

}

void Boid::Eat(){
	m_scale.x += (m_weight * 0.05);
	m_scale.y += (m_weight * 0.05);
	m_scale.z += (m_weight * 0.05);
	m_weight++;
}
void Boid::Starve(){
	m_scale.x -= (m_weight * 0.05);
	m_scale.y -= (m_weight * 0.05);
	m_scale.z -= (m_weight * 0.05);
	m_weight--;
}

void Boid::Tick(GameData* GD)
{
	if (m_type != BOID_OBSTACLE){
		if (m_weight > 0){
			if (GetTickCount64() - lastKillTickCount > SimulationParameters::starvationTime){
				Starve();
				lastKillTickCount = GetTickCount64();
			}
		}

		m_direction.y = 0.0f;
		//If the boid has a target set direction to aim at that target and go toward it ignoring modifier
		if (m_target != nullptr)
		{
			if (m_target->isAlive())
			{
				Vector3 dir = m_target->GetPos() - m_pos;
				dir.Normalize();
				m_direction = dir;
				m_speed = max_speed - (m_weight * 3.0f);
				if ((m_target->GetPos() - m_pos).Length() < 5.0f)
				{
					m_target->Damage(100.0f);
					if (!m_target->isAlive())
					{

						m_target = nullptr;
						Eat();
						lastKillTickCount = GetTickCount64();
					}
				}
			}

		}
		else
		{
			float distanceFromCentre = ((Vector3(0.0f, 0.0f, 0.0f) - m_pos)).Length();
			if (distanceFromCentre > (SimulationParameters::mapSize / 2))
			{
			Vector3 centerModifier = (Vector3(0.0f, 0.0f, 0.0f) - m_pos);
			centerModifier.Normalize();
			m_direction += centerModifier * (distanceFromCentre / 500.0f);
			}
		}
		//Point in direction of travel
		m_direction.Normalize();
		m_pos += m_direction * m_speed * GD->dt;
		smooth_yaw = (smooth_yaw * 0.9f) + (atan2(m_direction.x, m_direction.z) * 0.1f);
		m_yaw = smooth_yaw;
	}
	GameObject::Tick(GD);
}

void Boid::Draw(DrawData* DD)
{
	CMOGO::Draw(DD);
}

