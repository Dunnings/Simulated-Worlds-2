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
		//Starvation
		if (m_weight > 0){
			if (GetTickCount64() - lastKillTickCount > SimulationParameters::starvationTime){
				Starve();
				lastKillTickCount = GetTickCount64();
			}
		}
		m_direction.y = 0.0f;
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

