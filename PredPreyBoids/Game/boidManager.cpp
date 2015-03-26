#include "boidManager.h"
#include "boid.h"
#include "userInterfaceManager.h"
#include <sstream>

ID3D11Device* GameData::p3d;
MyEffectFactory* GameData::EF;

boidManager::boidManager()
{
	cursor = spawnBoid(BOID_OBSTACLE);
	cursor->SetSight(50.0f);
	cursor->SetPos(Vector3(0.0f, 0.0f, 0.0f));
	for (int i = 0; i < 200; i++)
	{
		spawnBoid(BOID_SPHERE);
	}
	for (int i = 0; i < 3; i++)
	{
		spawnBoid(BOID_RED_SPHERE);
	}
}

boidManager::~boidManager()
{

}

Boid* boidManager::spawnBoid(BoidType type)
{
	Boid* newBoid = new Boid();
	newBoid->SetMaxSpeed(SimulationParameters::boidMaxSpeed * type);
	float r1 = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0f - -1.0f)));
	float r2 = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0f - -1.0f)));
	float r3 = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0f - -1.0f)));
	float r4 = 1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (SimulationParameters::boidMaxSpeed - 1.0f)));
	newBoid->SetDirection(Vector3(r1, 0.0f, r2));
	newBoid->SetSpeed(r4);
	newBoid->SetPos(Vector3(r3 * SimulationParameters::mapSize * 0.5f, 0.0f, r1* SimulationParameters::mapSize * 0.5f));
	newBoid->SetType(type);
	newBoid->initialize();
	if (type == 1)
	{
		newBoid->SetScale(0.6f);
	}
	myBoids.push_back(newBoid);
	return newBoid;
}

void boidManager::Tick(GameData* GD)
{
	int totalBoids = 0;
	int totalObstacles = 0;
	int totalPrey = 0;
	int totalPred = 0;
	if (SimulationParameters::cursorObstacle)
	{
		// Cursor Position
		if (GD->mouse->lX > 0)
		{
			cursor->SetPos(cursor->GetPos() + Vector3(GD->mouse->lX / 2.0f, 0.0f, 0.0f));
		}
		else if (GD->mouse->lX < 0)
		{
			cursor->SetPos(cursor->GetPos() + Vector3(GD->mouse->lX / 2.0f, 0.0f, 0.0f));
		}

		if (GD->mouse->lY > 0)
		{
			cursor->SetPos(cursor->GetPos() + Vector3(0.0f, 0.0f, GD->mouse->lY / 2.0f));
		}
		else if (GD->mouse->lY < 0)
		{
			cursor->SetPos(cursor->GetPos() + Vector3(0.0f, 0.0f, GD->mouse->lY / 2.0f));
		}
	}

	for (vector<Boid*>::iterator it = myBoids.begin(); it != myBoids.end();)
	{
		Boid* currentBoid = (*it);

		totalBoids++;
		if (currentBoid->getType() == BOID_OBSTACLE){
			totalObstacles++;
		}
		else if (currentBoid->getType() == BOID_ROACH){
			totalPrey++;
		}
		else if (currentBoid->getType() == BOID_CRAB){
			totalPred++;
		}
		Vector3 avDir;
		float avSpeed = 0.0f;
		Vector3 avPos;
		int preyCount = 0;
		bool overrideModifier = false;
		int count = 0;
		Vector3 targetHeading = Vector3(0.0f, 0.0f, 0.0f);
		Vector3 modifier = Vector3(0.0f, 0.0f, 0.0f);

		for (vector<Boid*>::iterator it2 = myBoids.begin(); it2 != myBoids.end(); ++it2)
		{
			if ((*it) != (*it2)){
				Boid* newBoid = (*it2);
				float dist = (newBoid->GetPos() - currentBoid->GetPos()).Length();
				// Bumping into OBSTACLE
				if (newBoid->getType() == BOID_OBSTACLE){
					if (dist < newBoid->getSight())
					{
						Vector3 contactModifier = (currentBoid->GetPos() - newBoid->GetPos());
						contactModifier.Normalize();
						currentBoid->SetDirection(contactModifier);
						overrideModifier = true;
						break;
					}
				}
				//currentBoid.type = prey | newBoid.type = predator
				else if (currentBoid->getType() < newBoid->getType())
				{
					if (dist < currentBoid->getSight())
					{
						Vector3 fearModifier = (currentBoid->GetPos() - newBoid->GetPos());
						currentBoid->SetDirection(fearModifier);
						overrideModifier = true;
						currentBoid->SetSpeed(currentBoid->getMaxSpeed());
					}
				}
				//currentBoid.type == newBoid.type 
				else if (currentBoid->getType() == newBoid->getType())
				{
					if (dist < 20.0f)
					{
						Vector3 contactModifier = (currentBoid->GetPos() - newBoid->GetPos());
						contactModifier.Normalize();
						modifier = contactModifier;
						overrideModifier = true;
						break;
					}
					else if (dist < SimulationParameters::groupDistance)
					{
						avPos += newBoid->GetPos();
						avDir += newBoid->getDirection();
						avSpeed += newBoid->getSpeed();
						count++;
					}
				}
				//Predator Hunting Prey
				else if (currentBoid->getType() > newBoid->getType() && newBoid->getType() != BOID_OBSTACLE){
					if ((newBoid->GetPos() - currentBoid->GetPos()).Length() < 10.0f)
					{
						newBoid->Damage(100.0f);
						currentBoid->Eat();
					}
					else if ((newBoid->GetPos() - currentBoid->GetPos()).Length() < currentBoid->getSight()){
						targetHeading += (1.0f / (newBoid->GetPos() - currentBoid->GetPos()).LengthSquared()) * ((newBoid->GetPos() - currentBoid->GetPos()));
						preyCount++;
					}
				}
			}
		}
		if (targetHeading != Vector3(0.0f, 0.0f, 0.0f)){
			currentBoid->SetSpeed(currentBoid->getMaxSpeed());
			targetHeading /= preyCount;
			targetHeading.Normalize();
			currentBoid->SetDirection(targetHeading);
			overrideModifier = true;
		}
		if (count > 0 && !overrideModifier)
		{
			avDir /= static_cast<float>(count);
			avPos /= static_cast<float>(count);
			avSpeed /= static_cast<float>(count);
			Vector3 toAverage = currentBoid->GetPos() - avPos;
			modifier.Normalize();
			if (toAverage.Length() > SimulationParameters::groupDistance * 0.6){
				modifier += (toAverage * SimulationParameters::groupStrength);
			}
			if (toAverage.Length() > SimulationParameters::groupDistance * 0.3){
				modifier += (avDir * SimulationParameters::groupHeading);
			}
			currentBoid->SetSpeed(avSpeed);

		}

		//Tick if Boid is alive
		if (currentBoid->isAlive())
		{
			modifier.Normalize();
			Vector3 centerModifier = Vector3(0.0f, 0.0f, 0.0f) - currentBoid->GetPos();
			if (centerModifier.Length() > (SimulationParameters::mapSize / 2))
			{
				float l = centerModifier.Length();
				centerModifier.Normalize();
				//Gradient out over 100 units
				centerModifier *= (((l - (SimulationParameters::mapSize / 2)) / 100.0f));
				modifier += centerModifier;
			}

			currentBoid->SetDirection(currentBoid->getDirection() + modifier);
			currentBoid->Tick(GD);
			++it;
		}
		else
		{
			it = myBoids.erase(it);
		}
	}
	GD->UIManager->addToDraw("Total Boids: " + to_string(totalBoids), "total_boids");
	GD->UIManager->addToDraw("Total Obstacles: " + to_string(totalObstacles), "total_obstacles");
	GD->UIManager->addToDraw("Total Prey: " + to_string(totalPrey), "total_prey");
	GD->UIManager->addToDraw("Total Predators: " + to_string(totalPred), "total_pred");

	GameObject::Tick(GD);
}

void boidManager::Draw(DrawData* DD)
{
	for (vector<Boid*>::iterator it = myBoids.begin(); it != myBoids.end(); ++it)
	{
		(*it)->Draw(DD);
	}
}