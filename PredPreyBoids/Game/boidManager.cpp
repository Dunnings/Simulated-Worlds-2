#include "boidManager.h"
#include "boid.h"

ID3D11Device* GameData::p3d;
MyEffectFactory* GameData::EF;

boidManager::boidManager()
{
	for (int i = 0; i < 200; i++)
	{
		spawnBoid(BOID_PREY);
	}
	spawnBoid(BOID_PREDATOR);
}

boidManager::~boidManager()
{

}

void boidManager::spawnBoid(BoidType type)
{
	Boid* newBoid = new Boid();
	newBoid->SetScale(1.0f);
	if (type == BOID_PREDATOR)
	{
		newBoid = new Boid();
		newBoid->SetScale(2.0f);
		newBoid->aquireTarget(myBoids);
	}
	else
	{
		float r1 = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0f - -1.0f)));
		float r2 = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0f - -1.0f)));
		float r3 = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0f - -1.0f)));
		float r4 = 1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (SimulationParameters::boidMaxSpeed - 1.0f)));
		newBoid->setDirection(Vector3(r1, r2, r3));
		newBoid->setSpeed(r4);
	}
	newBoid->setType(type);
	myBoids.push_back(newBoid);
}

void boidManager::Tick(GameData* GD)
{
	for (vector<Boid*>::iterator it = myBoids.begin(); it != myBoids.end(); )
	{
		Boid* currentBoid = (*it);
		Vector3 avDir;
		Vector3 avPos;
		int count = 0;
		Vector3 modifier = Vector3(0.0f, 0.0f, 0.0f);
		if (currentBoid->getType() == BOID_PREDATOR)
		{
			if (currentBoid->getTarget() == nullptr)
			{
				currentBoid->aquireTarget(myBoids);
			}
		}
		else
		{
			for (vector<Boid*>::iterator it2 = it; it2 != myBoids.end(); ++it2)
			{
				Boid* newBoid = (*it2);
				//Running away
				if (currentBoid->getType() < newBoid->getType())
				{
					if ((newBoid->GetPos() - currentBoid->GetPos()).Length() < SimulationParameters::boidSight)
					{
						Vector3 fearModifier = (currentBoid->GetPos() - newBoid->GetPos());
						fearModifier.Normalize();
						modifier += fearModifier;
					}
				}
				//Keeping away from similar
				else if (currentBoid->getType() == newBoid->getType())
				{
					float dist = (newBoid->GetPos() - currentBoid->GetPos()).Length();
					if (dist < 20.0f)
					{
						Vector3 contactModifier = (currentBoid->GetPos() - newBoid->GetPos());
						contactModifier *= (dist / 20.0f);
						contactModifier.Normalize();
						modifier += contactModifier;
					}
					else
					{
						if (dist < SimulationParameters::groupDistance)
						{
							avDir += newBoid->getDirection();
							avPos += newBoid->GetPos();
							count++;
						}
					}
				}
			}
		}
		if (count > 0 && modifier == Vector3(0.0f, 0.0f, 0.0f))
		{
			avDir /= count;
			avPos /= count;
			Vector3 toAverage = avPos - m_pos;
			toAverage.Normalize();
			avDir.Normalize();
			Vector3 groupModifier = (avDir * 0.2f) + (toAverage * 0.7f);
			modifier = groupModifier;
		}
		Vector3 centerModifier = (Vector3(0.0f, 0.0f, 0.0f) - currentBoid->GetPos()) * 0.3f;
		modifier += centerModifier;
		if (currentBoid->isAlive())
		{
			currentBoid->Tick(GD, modifier);
			++it;
		}
		else
		{
			it = myBoids.erase(it);
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