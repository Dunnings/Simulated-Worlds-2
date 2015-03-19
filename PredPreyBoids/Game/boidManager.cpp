#include "boidManager.h"
#include "boid.h"

ID3D11Device* GameData::p3d;
MyEffectFactory* GameData::EF;

boidManager::boidManager()
{
	for (int i = 0; i < 100; i++)
	{
		spawnBoid(BOID_ROACH);
	}
	for (int i = 0; i < 20; i++)
	{
		spawnBoid(BOID_CRAB);
	}
}

boidManager::~boidManager()
{

}


void boidManager::aquireTarget(Boid* predator)
{
	Boid* target = nullptr;
	float distance = predator->getSight();
	for (vector<Boid*>::iterator it = myBoids.begin(); it != myBoids.end(); it++)
	{
		Boid* currentBoid = (*it);
		if (predator->getType() > currentBoid->getType() && currentBoid->getType() != BOID_OBSTACLE)
		{
			if ((currentBoid->GetPos() - predator->GetPos()).Length() < distance)
			{
				target = currentBoid;
				distance = (currentBoid->GetPos() - predator->GetPos()).Length();
			}
		}
	}
	predator->setTarget(target);
}

void boidManager::spawnBoid(BoidType type)
{
	Boid* newBoid;
	if (type == BOID_CRAB)
	{
		newBoid = new Boid("crab.cmo");
		newBoid->setSight(200.0f);
	}
	else
	{
		newBoid = new Boid("roach.cmo");
	}
	newBoid->setMaxSpeed(SimulationParameters::boidMaxSpeed * type);
	float r1 = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0f - -1.0f)));
	float r2 = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0f - -1.0f)));
	float r3 = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0f - -1.0f)));
	float r4 = 1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (SimulationParameters::boidMaxSpeed - 1.0f)));
	aquireTarget(newBoid);
	newBoid->setDirection(Vector3(r1, 0.0f, r2));
	newBoid->setSpeed(r4);
	newBoid->SetPos(Vector3(r3 * SimulationParameters::mapSize * 0.5f, 0.0f, r1* SimulationParameters::mapSize * 0.5f));
	newBoid->setType(type);
	myBoids.push_back(newBoid);
}

void boidManager::Tick(GameData* GD)
{
	for (vector<Boid*>::iterator it = myBoids.begin(); it != myBoids.end();)
	{
		Boid* currentBoid = (*it);
		Vector3 avDir;
		Vector3 avPos;
		bool overrideModifier = false;
		int count = 0;
		Vector3 modifier = Vector3(0.0f, 0.0f, 0.0f);

		if (currentBoid->getType() > 1)
		{
			if (currentBoid->getTarget() == nullptr)
			{
				aquireTarget(currentBoid);
			}
		}

		for (vector<Boid*>::iterator it2 = it; it2 != myBoids.end(); ++it2)
		{
			Boid* newBoid = (*it2);
			float dist = (newBoid->GetPos() - currentBoid->GetPos()).Length();
			//currentBoid.type = prey | newBoid.type = predator
			if (currentBoid->getType() < newBoid->getType())
			{
				if (dist < currentBoid->getSight())
				{
					Vector3 fearModifier = (currentBoid->GetPos() - newBoid->GetPos());
					if (!overrideModifier)
					{
						modifier = fearModifier;
						currentBoid->setSpeed(SimulationParameters::boidMaxSpeed);
						overrideModifier;
					}
				}
			}
			//currentBoid.type == newBoid.type 
			else if (currentBoid->getType() == newBoid->getType())
			{
				if (dist < (currentBoid->GetScale().x + (currentBoid->GetScale().y + currentBoid->GetScale().z) / 3) + 20.0f)
				{
					Vector3 contactModifier = (currentBoid->GetPos() - newBoid->GetPos());
					contactModifier *= (dist / 20.0f);
					if (!overrideModifier)
					{
						modifier = contactModifier;
						overrideModifier;
					}
				}
			}
			if (dist < SimulationParameters::groupDistance && currentBoid->getType() == newBoid->getType())
			{
				avPos += newBoid->GetPos();
				avDir += newBoid->getDirection();
				count++;
			}
		}
		if (count > 0)
		{
			avDir /= static_cast<float>(count);
			avPos /= static_cast<float>(count);
			Vector3 toAverage = currentBoid->GetPos() - avPos;
			if (!overrideModifier)
			{
				modifier += (toAverage * SimulationParameters::groupStrength);
				modifier += (avDir * SimulationParameters::groupHeading);
			}
			//Tick if Boid is alive
			if (currentBoid->isAlive())
			{
				modifier.Normalize();
				currentBoid->setDirection(currentBoid->getDirection() + modifier);
				currentBoid->Tick(GD);
				++it;
			}
			else
			{
				it = myBoids.erase(it);
			}
		}
	}
	GameObject::Tick(GD);
}

void boidManager::Draw(DrawData* DD)
{
	for (vector<Boid*>::iterator it = myBoids.begin(); it != myBoids.end(); ++it)
	{
		Boid* currentBoid = (*it);
		currentBoid->Draw(DD);
	}
}