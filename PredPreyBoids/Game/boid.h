#ifndef _BOID_H_
#define _BOID_H_
#include "VBCube.h"
#include "gamedata.h"
#include "CMOGO.h"
#include <vector>

using std::vector;

class Boid : public CMOGO
{
public:
	Boid(string _fileName);
	~Boid();

	virtual void Tick(GameData* GD);
	virtual void Draw(DrawData* DD);
	void SetSpeed(float speed) { m_speed = speed; };
	void SetMaxSpeed(float speed) { max_speed = speed; };
	void SetSight(float sight) { m_sight = sight; };
	void SetDirection(Vector3 dir) { m_direction = dir; };
	void SetType(BoidType type) { m_type = type; };
	void SetTarget(Boid* t) { m_target = t; };
	void SetKills(int level){ m_weight = level; };
	void SetLastKillTickCount(ULONGLONG time){ lastKillTickCount = time; };
	
	
	
	float getSpeed() { return m_speed; };
	float getMaxSpeed() { return max_speed; };
	float getSight() { return m_sight; };
	Vector3 getDirection() { return m_direction; };
	BoidType getType() { return m_type; };;
	Boid* getTarget() { return m_target; };
	bool isAlive() { return m_alive; };
	int getWeight(){ return m_weight; };
	Vector3 GetScale() { return m_scale; };


	void incrementKills(){ ++m_weight; };
	void Damage(float _dmg) { m_health -= _dmg; if (m_health <= 0.0f) { m_alive = false; m_health = 0.0f; } };
	void Eat();
	void Starve();
protected:
	BoidType m_type;
	int m_weight = 0;
	BoidState m_state;
	Boid* m_target = nullptr;
	Vector3 m_direction;
	bool m_alive = true;
	float m_health = 100.0f; 
	float m_speed = 0.0f;
	float max_speed = 0.0f;
	float m_sight = 100.0f;
	float smooth_yaw = 0.0f;
	ULONGLONG lastKillTickCount;

};

#endif