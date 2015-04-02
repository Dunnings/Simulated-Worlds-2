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
	Boid* spawnBoid(int type);
	//Delete given boid
	void deleteBoid(Boid* b);
	//Delete first boid of given type
	void deleteBoid(int type);
	//Return the top-most level boid
	Boid* getHighestBOID();
	//Delete all boids
	void deleteAll();
private:
	//Vector containing all boids waiting to be spawned
	vector<Boid*> toSpawn;
	//Vector containing all boids
	vector<Boid*> myBoids;
	//Boid used for cursor
	Boid* cursor = nullptr;
	//Waypoints
	vector<Waypoint*> m_waypoints;
	//Last spawn time
	ULONGLONG lastSpawnTime;
};

#endif