#ifndef _BOID_H_
#define _BOID_H_
#include "VBCube.h"
#include "gamedata.h"
#include <vector>

using std::vector;

class Boid : public VBCube
{
public:
	Boid();
	~Boid();

	virtual void Tick(GameData* GD, Vector3 modifier);
	virtual void Draw(DrawData* DD);
	void setSpeed(float speed) { m_speed = speed; };
	float getSpeed() { return m_speed; };
	void setDirection(Vector3 dir) { m_direction = dir; };
	Vector3 getDirection() { return m_direction; };
	BoidType getType() { return m_type; };
	void setType(BoidType type) { m_type = type; };
	Boid* getTarget() { return m_target; };
	void aquireTarget(vector<Boid*> boidVector);
	void Damage(float _dmg) { m_health -= _dmg; if (m_health <= 0.0f) { m_alive = false; m_health = 0.0f; } };
	bool isAlive() { return m_alive; };
protected:
	BoidType m_type;
	BoidState m_state;
	Boid* m_target = nullptr;
	Vector3 m_direction;
	bool m_alive = true;
	float m_health = 100.0f; 
	float m_speed = 0.0f;

};

#endif