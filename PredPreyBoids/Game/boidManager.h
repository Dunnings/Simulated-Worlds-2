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
	Boid* spawnBoid(int type);
	void deleteBoid(Boid* b);
	void deleteBoid(int type);
	Boid* getHighestBOID();
	void deleteAll();
private:
	vector<Boid*> myBoids;
	Boid* cursor = nullptr;
};

#endif