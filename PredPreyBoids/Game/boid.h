#ifndef _BOID_H_
#define _BOID_H_
#include "VBCube.h"
#include "gamedata.h"
#include "VBCube.h"
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

using std::vector;

class Boid : public VBGO
{
public:
	Boid();
	~Boid();

	void initialize();

	//CMOGO inherited tick
	virtual void Tick(GameData* GD);
	//CMOGO inherited draw
	virtual void Draw(DrawData* DD);
	//Set the BOID's current speed
	void SetSpeed(float speed) { m_speed = speed; };
	//Set the BOID's maximum speed
	void SetMaxSpeed(float speed) { max_speed = speed; };
	//Set how far the BOID can see
	void SetSight(float sight) { m_sight = sight; };
	//Set the direction the BOID is travelling
	void SetDirection(Vector3 dir) { m_direction = dir; };
	//Set the type of BOID
	void SetType(BoidType type) { m_type = type; };
	//Set how full the BOID is
	void SetWeight(int w){ m_weight = w; };
	
	//Get the BOID's current speed
	float getSpeed() { return m_speed; };
	//Get the BOID's maximum speed
	float getMaxSpeed() { return max_speed; };
	//Get the BOID's sight
	float getSight() { return m_sight; };
	//Get the BOID's current direction
	Vector3 getDirection() { return m_direction; };
	//Get the BOID's type
	BoidType getType() { return m_type; };
	//Get the BOID's alive state
	bool isAlive() { return m_alive; };
	//Get the BOID's current weight
	int getWeight(){ return m_weight; };
	//Get the BOID's smooth yaw
	float getSmoothYaw(){ return smooth_yaw; };
	//Get the BOID's scale
	Vector3 GetScale() { return m_scale; };
	//Nearest group position
	Vector3 m_grouping;

	//Damage the BOID and kill it if necessary
	void Damage(float _dmg) { m_health -= _dmg; if (m_health <= 0.0f) { m_alive = false; m_health = 0.0f; } };
	//Increment weight and increase physical size
	void Eat();
	//Decrement weight and decrease physical size
	void Starve();
protected:
	//The BOID's type
	BoidType m_type;
	//Current weight
	int m_weight = 0;
	//Current state
	BoidState m_state;
	//Current direction
	Vector3 m_direction;
	//Is the BOID alive?
	bool m_alive = true;
	//Current health
	float m_health = 100.0f; 
	//Current speed
	float m_speed = 0.0f;
	//Maximum speed
	float max_speed = 0.0f;
	//Sight distance
	float m_sight = 100.0f;
	//Value used to smppth out yaw
	float smooth_yaw = 0.0f;
	//Time BOID last ate
	ULONGLONG lastKillTickCount;
	//Every vertex of this BOID
	myVertex* m_vertices;
	//Force lines
	myVertex lineVertices[2];
	//Small function to modify the VB to produce a sphere
	virtual void SphereTransform(Color c)
	{
		for (int i = 0; i< m_numPrims * 3; i++)
		{
			Vector3 vertPos = m_vertices[i].Pos;
			Vector3 spherePos = m_vertices[i].Pos;

			spherePos.Normalize();

			Vector3 newPos = 10.0f *spherePos;
			m_vertices[i].Color = c;
			m_vertices[i].Pos = newPos;
		}
		for (int i = 0; i<m_numPrims * 3; i++)
		{
			Vector3 vertPos = m_vertices[i].Pos;

			Matrix transform = Matrix::CreateTranslation(0.0f, 6.0f - sqrt(vertPos.x*vertPos.x + vertPos.z*vertPos.z), 0.0f);
			Matrix rotate = Matrix::CreateRotationX(M_PI / 2);
			Vector3 newPos = Vector3::Transform(vertPos, transform);

			m_vertices[i].Pos = newPos;
		}
	}
};

#endif