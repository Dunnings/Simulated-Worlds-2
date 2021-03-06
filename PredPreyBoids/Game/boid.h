#ifndef _BOID_H_
#define _BOID_H_

#define _USE_MATH_DEFINES

#include "gamedata.h"
#include "VBGO.h"
#include "vertex.h"
#include <vector>
#include <math.h>

using std::vector;
class Waypoint;

class Boid : public VBGO
{
public:
	Boid(Waypoint* _finish, Waypoint* _outpost);
	~Boid();
	virtual void Tick(GameData* GD);
	virtual void Draw(DrawData* DD);
	//Create the vertex buffer and initialize the object
	virtual void initialize();
	//Damage the boid and kill it if necessary
	void Damage(float _dmg) { m_health -= _dmg; if (m_health <= 0.0f) { m_alive = false; m_health = 0.0f; } };
	//Increment weight and increase physical size
	void Eat();
	//Decrement weight and decrease physical size
	void Starve();


	//Set the boid's current speed
	void SetSpeed(float speed) { m_speed = speed; };
	//Set the boid's maximum speed
	void SetMaxSpeed(float speed) { max_speed = speed; };
	//Set how far the boid can see
	void SetSight(float sight) { m_sight = sight; };
	//Set the direction the boid is travelling
	void SetDirection(Vector3 dir) { m_direction = dir; };
	//Set the grouping direction of the boid
	void SetGroupDirection(Vector3 dir) { m_groupDirection = dir; };
	//Set the grouping heading of the boid
	void SetGroupHeading(Vector3 dir) { m_groupHeading = dir; };
	//Set the type of boid
	void SetType(Type* type) { m_type = type; };
	//Set time between breeding
	void SetBreedDelay(float _d) { breedDelay = _d; };
	//Set how full the boid is
	void SetWeight(int w) { m_weight = w; };
	//Boid has bred
	void Breed();


	//Get finish waypoint
	Waypoint* GetFinish() { return finish; };
	//Get outpost waypoint
	Waypoint* GetOutpost() { return outpost; };
	//Get the boid's current speed
	float GetSpeed() { return m_speed; };
	//Get the boid's maximum speed
	float GetMaxSpeed() { return max_speed; };
	//Get the boid's sight
	float GetSight() { return m_sight; };
	//Get the boid's current direction
	Vector3 GetDirection() { return m_direction; };
	//Get the boid's type
	Type* GetType() { return m_type; };
	//Get the boid's alive state
	bool GetAliveState() { return m_alive; };
	//Can this boid breed?
	bool GetBreedingStatus() { if (GetTickCount64() - lastBreedTickCount > breedDelay) { return true; } return false; };
	//Get the boid's current weight
	int GetWeight() { return m_weight; };
	//Get the boid's current health
	float GetHealth() { return m_health; };
	//Get last time boid bred
	ULONGLONG GetLastBreedTime() { return lastBreedTickCount; };
	//Get the boid's scale
	Vector3 GetScale() { return m_scale; };
	//Get the boid's scale
	float GetFloatScale() { return (m_scale.x + m_scale.y + m_scale.z) / 3; };
protected:
	//The boid's hierarchial level
	Type* m_type;
	//The boid's target finish
	Waypoint* finish;
	//The boid's target outpost
	Waypoint* outpost;
	//Current weight
	int m_weight = 0;
	//Current direction
	Vector3 m_direction;
	//Group direction
	Vector3 m_groupDirection;
	//Group heading
	Vector3 m_groupHeading;
	//Is the boid alive?
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
	//Time boid last ate
	ULONGLONG lastUpdateTickCount;
	//Time boid last bred
	ULONGLONG lastBreedTickCount;
	//How often can this boid breed?
	float breedDelay;
	//Every vertex of this boid
	myVertex* m_vertices;
	//Force lines array
	vector<myVertex> lineVertices;
	//Small function to modify the VB to produce a coloured cone
	virtual void ConeTransform(Color c)
	{
		for (unsigned int i = 0; i< m_numPrims * 3; i++)
		{
			Vector3 vertPos = m_vertices[i].Pos;
			Vector3 spherePos = m_vertices[i].Pos;

			spherePos.Normalize();

			Vector3 newPos = 10.0f *spherePos;
			m_vertices[i].Color = c;
			m_vertices[i].Pos = newPos;
		}
		for (unsigned int i = 0; i<m_numPrims * 3; i++)
		{
			Vector3 vertPos = m_vertices[i].Pos;

			Matrix transform = Matrix::CreateTranslation(0.0f, 6.0f - sqrt(vertPos.x*vertPos.x + vertPos.z*vertPos.z), 0.0f);
			Matrix rotate = Matrix::CreateRotationX(M_PI / 2);
			Vector3 newPos = Vector3::Transform(vertPos, transform);

			m_vertices[i].Pos = newPos;
		}
	}
	//Small function to modify the VB to produce a coloured sphere
	virtual void SphereTransform(Color c)
	{
		for (unsigned int i = 0; i< m_numPrims * 3; i++)
		{
			Vector3 vertPos = m_vertices[i].Pos;
			Vector3 spherePos = m_vertices[i].Pos;

			spherePos.Normalize();

			Vector3 newPos = 10.0f *spherePos;
			m_vertices[i].Color = c;
			m_vertices[i].Pos = newPos;
		}
	}
};

#endif