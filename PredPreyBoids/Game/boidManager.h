#ifndef BOID_MANAGER
#define BOID_MANAGER

#include "gameobject.h"
#include "gamedata.h"
#include <vector>

using namespace std;
using namespace DirectX;

class Boid;

class boidManager : public GameObject
{
public:
	boidManager();
	virtual ~boidManager();

	virtual void Tick(GameData* _GD);
	virtual void Draw(DrawData* _DD);
	Boid* spawnBoid(BoidType type);
	void killBoid(BoidType type);
	void killBoid(Boid* target);
	void aquireTarget(Boid* pred);
	Boid* closestPrey(Boid* _pred);
private:
	vector<Boid*> myBoids;
	Boid* cursor;
};

#endif