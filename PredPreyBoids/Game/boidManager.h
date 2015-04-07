#ifndef BOID_MANAGER
#define BOID_MANAGER

#include "gameobject.h"
#include "gamedata.h"
#include "waypoint.h"
#include <vector>

using namespace std;
using namespace DirectX;

class Boid;

class boidManager : public GameObject
{
public:
	boidManager();
	~boidManager();
	virtual void Tick(GameData* _GD);
	virtual void Draw(DrawData* _DD);
	//Create and return a new boid of given type
	Boid* SpawnBoid(int type);
	//Breed boids
	void BreedBoids(Boid* a, Boid* b);
	//Respawn current boid
	Boid* ReSpawnBoid(Boid* _b, bool keepPosition);
	//Respawn all boids
	void RespawnAllBoids(bool keepPosition);
	//Delete given boid
	void DeleteBoid(Boid* b);
	//Delete first boid of given type
	void DeleteBoid(int type);
	//Return the top-most level boid
	Boid* GetHighestBoid();
	//Delete all boids
	void DeleteAll();
	//Create a waypoint
	void AddWaypoint(waypointType _type, int _affects, Vector3 _pos, float _aoi);
	void AddWaypoint(Waypoint* w);
	//Delete all waypoints
	void DeleteAllWaypoints();
	//Load types from file
	void AddType(Type* t);
private:
	//Vector containing all boids waiting to be spawned
	vector<Boid*> toSpawn;
	//Vector containing all boids
	vector<Boid*> myBoids;
	//Boid used for cursor
	Boid* cursor = nullptr;
	//Waypoints
	vector<Waypoint*> m_waypoints;
	//Types
	vector<Type*> m_types;
	//Last spawn time
	ULONGLONG lastSpawnTime;
};

#endif