#include "boidManager.h"
#include "boid.h"
#include <sstream>
#include <iterator>
#include <map>

ID3D11Device* GameData::p3d;
MyEffectFactory* GameData::EF;

boidManager::boidManager()
{
	//If given a boidcount then generate boids based on this
	if (SimulationParameters::boidCount.size() > 0){
		for (map<int, int>::iterator it = SimulationParameters::boidCount.begin(); it != SimulationParameters::boidCount.end(); it++){
			for (int i = 0; i < (*it).second; i++){
				spawnBoid((*it).second);
			}
		}
	}
	else{
		//Otherwise spawn default amount
		for (int i = 0; i < 100; i++)
		{
			spawnBoid(1);
		}
		for (int i = 0; i < 3; i++)
		{
			spawnBoid(2);
		}
	}
}

boidManager::~boidManager()
{

}

Boid* boidManager::spawnBoid(int type)
{
	//Dynamically create a new BOID
	Boid* newBoid = new Boid();
	//Set the max speed to the pre-determined value
	newBoid->SetMaxSpeed(SimulationParameters::boidMaxSpeed * type);
	//Create four random floats between -1.0 and 1.0 
	float r1 = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0f - -1.0f)));
	float r2 = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0f - -1.0f)));
	float r3 = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0f - -1.0f)));
	float r4 = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0f - -1.0f)));
	//Set the BOID position to a random location within pre-determined map bounds
	newBoid->SetPos(Vector3(r1 * SimulationParameters::mapSize * 0.5f, 0.0f, r2* SimulationParameters::mapSize * 0.5f));
	//Set direction randomly
	newBoid->SetDirection(Vector3(r3, 0.0f, r4));
	//Create a random float between 1.0 and the pre-determined max speed
	float rS = 1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / ((SimulationParameters::boidMaxSpeed*type) - 1.0f)));
	newBoid->SetSpeed(rS);
	//Set the BOID type
	newBoid->SetType(type);
	//Initialise BOID vertex buffer
	newBoid->initialize();
	//Scale down prey
	if (type == 1)
	{
		newBoid->SetScale(0.6f);
	}
	//Add BOID to manager's vector
	myBoids.push_back(newBoid);
	//Return the created BOID
	return newBoid;
}

void boidManager::deleteBoid(Boid* b){
	if (cursor == b){
		delete b;
		b = nullptr;
	}
	else{
		for (vector<Boid*>::iterator it = myBoids.begin(); it != myBoids.end();)
		{
			if ((*it) == b)
			{
				delete b;
				b = nullptr;
				it = myBoids.erase(it);
				break;
			}
			else
			{
				it++;
			}
		}
	}
}

void boidManager::deleteBoid(int type){
	for (vector<Boid*>::iterator it = myBoids.begin(); it != myBoids.end();)
	{
		if ((*it)->getType() == type)
		{
			(*it) = nullptr;
			delete((*it));
			it = myBoids.erase(it);
			break;
		}
		else
		{
			it++;
		}
	}
}

void boidManager::deleteAll()
{
	for (vector<Boid*>::iterator it = myBoids.begin(); it != myBoids.end(); it++)
	{
		(*it)->Damage(1000.0f);
	}
	myBoids.clear();
}

Boid* boidManager::getHighestBOID()
{
	int type = 0;
	Boid* toReturn = nullptr;
	for (vector<Boid*>::iterator it = myBoids.begin(); it != myBoids.end(); it++)
	{
		if ((*it)->getType() > type)
		{
			toReturn = (*it);
			type = toReturn->getType();
		}
	}
	return toReturn;
}

void boidManager::Tick(GameData* GD)
{
	map<int, int> boidCount;
	if (SimulationParameters::cursorObstacle)
	{
		if (cursor == nullptr)
		{
			//Spawn one obstacle for the cursor
			cursor = spawnBoid(0);
			cursor->SetSight(50.0f);
			cursor->SetPos(Vector3(0.0f, 0.0f, 0.0f));
		}
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
		cursor->Tick(GD);
	}
	else{
		if (cursor != nullptr){
			if (cursor->isAlive()){
				cursor->Damage(100.0f);
				cursor = nullptr;
			}
		}
	}

	for (vector<Boid*>::iterator it = myBoids.begin(); it != myBoids.end();)
	{
		Boid* currentBoid = (*it);
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
			if ((*it) != (*it2) && ((*it)->GetPos() - (*it2)->GetPos()).Length() < max((*it)->getSight(), (*it2)->getSight())){
				Boid* newBoid = (*it2);
				float dist = (newBoid->GetPos() - currentBoid->GetPos()).Length();
				// Bumping into OBSTACLE
				if (newBoid->getType() == 0){
					if (dist < 20.0f)
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
						currentBoid->SetSpeed(currentBoid->getMaxSpeed() * currentBoid->getType());
						overrideModifier = true;
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
				else if (currentBoid->getType() > newBoid->getType() && newBoid->getType() != 0){
					if ((newBoid->GetPos() - currentBoid->GetPos()).Length() < 10.0f)
					{
						newBoid->Damage(100.0f);
						currentBoid->Eat();
					}
					else if ((newBoid->GetPos() - currentBoid->GetPos()).Length() < currentBoid->getSight()){
						targetHeading += (1.0f / (newBoid->GetPos() - currentBoid->GetPos()).LengthSquared()) * ((newBoid->GetPos() - currentBoid->GetPos()));
						preyCount++;
					}
					currentBoid->SetSpeed(currentBoid->getMaxSpeed() * currentBoid->getType());
				}
			}
		}
		if (targetHeading != Vector3(0.0f, 0.0f, 0.0f) && !overrideModifier){
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
			//Only move toward center of group if they are in the outer 40% of the group
			if (toAverage.Length() > SimulationParameters::groupDistance * 0.6){
				modifier += (toAverage * SimulationParameters::groupStrength);
			}
			//Only move in average direction if in inner 50% of group
			//if (toAverage.Length() < SimulationParameters::groupDistance * 0.5){
				modifier += (avDir * SimulationParameters::groupHeading);
			//}
			
			currentBoid->m_grouping = toAverage - m_pos;

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

			boidCount[currentBoid->getType()]++;

		}
		else
		{
			it = myBoids.erase(it);
		}
	}
	SimulationParameters::boidCount = boidCount;
	GameObject::Tick(GD);
}

void boidManager::Draw(DrawData* DD)
{
	for (vector<Boid*>::iterator it = myBoids.begin(); it != myBoids.end(); ++it)
	{
		(*it)->Draw(DD);
	}
}