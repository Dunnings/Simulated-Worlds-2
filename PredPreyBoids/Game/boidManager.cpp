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
		for (int i = 0; i < 2; i++)
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
	//Set speed to half max speed
	newBoid->SetSpeed(((SimulationParameters::boidMaxSpeed*type) / 2));
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
	//If the given BOID is the cursor BOID
	if (cursor == b){
		//Delete the cursor BOID
		delete b;
		b = nullptr;
	}
	else{
		//Otherwise loop through all BOIDs in myBoids
		for (vector<Boid*>::iterator it = myBoids.begin(); it != myBoids.end();)
		{
			//If the current BOID is the given BOID
			if ((*it) == b)
			{
				//Delete this BOID and stop searching
				delete b;
				b = nullptr;
				it = myBoids.erase(it);
				break;
			}
			//Otherwise keep looking
			else
			{
				it++;
			}
		}
	}
}

void boidManager::deleteBoid(int type){
	//Loop through all BOIDs in myBoids
	for (vector<Boid*>::iterator it = myBoids.begin(); it != myBoids.end();)
	{
		//If the current BOID is the same type as the given type
		if ((*it)->getType() == type)
		{
			//Delete this BOID and stop searching
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
	//Loop through all BOIDs in myBoids
	for (vector<Boid*>::iterator it = myBoids.begin(); it != myBoids.end(); it++)
	{
		//Kill the BOID
		(*it)->Damage(1000.0f);
		(*it) = nullptr;
	}
	//Empty myBoids
	myBoids.clear();
}

Boid* boidManager::getHighestBOID()
{
	//Start with type 0
	int type = 0;
	//Initialize a BOID to return
	Boid* toReturn = nullptr;
	//Loop through all BOIDs in myBoids
	for (vector<Boid*>::iterator it = myBoids.begin(); it != myBoids.end(); it++)
	{
		//If this BOID's type is higher than type
		if ((*it)->getType() > type)
		{
			//Set the BOID to return to this one
			toReturn = (*it);
			//Set the type to this BOID's type
			type = toReturn->getType();
		}
	}
	//Return the highest BOID
	return toReturn;
}

void boidManager::Tick(GameData* GD)
{
	//If the simulation is running
	if (GD->GS == GS_PLAY_PLAY)
	{
		//Initialise a map to hold the BOID counts
		map<int, int> boidCount;
		//If the user's cursor is an obstacle
		if (SimulationParameters::cursorObstacle)
		{
			//If the cursor doesn't exist
			if (cursor == nullptr)
			{
				//Spawn one obstacle for the cursor
				cursor = spawnBoid(0);
				cursor->SetSight(50.0f);
				cursor->SetPos(Vector3(0.0f, 0.0f, 0.0f));
			}
			//Move the cursor obstacle when the cursor is moved
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
		else
		{
			//If the cursor obstacle is still there, remove it
			if (cursor != nullptr)
			{
				if (cursor->isAlive())
				{
					cursor->Damage(1000.0f);
					cursor = nullptr;
				}
			}
		}

		//Loop through each BOID in myBoids
		for (vector<Boid*>::iterator it = myBoids.begin(); it != myBoids.end();)
		{
			//For ease of visibility, create a BOID pointer to hold the current BOID
			Boid* currentBoid = (*it);
			//Vector that holds the average direction of all BOIDs near the current BOID
			Vector3 avDir;
			//Vector that holds the average position of all BOIDs near the current BOID
			Vector3 avPos;
			//Integer that holds how many prey the currentBoid is attracted to
			int preyCount = 0;
			//Integer that holds how many BOIDs are nearby for grouping
			int count = 0;
			//Boolean that states if the BOID is impeded from grouping
			bool isImpeded = false;
			//Vector that holds the direction toward prey
			Vector3 targetHeading = Vector3(0.0f, 0.0f, 0.0f);
			//Vector that holds the direction to travel this tick
			Vector3 modifier = Vector3(0.0f, 0.0f, 0.0f);

			//Loop through each BOID in myBoids
			for (vector<Boid*>::iterator it2 = myBoids.begin(); it2 != myBoids.end(); ++it2)
			{
				//For ease of visibility, create a BOID pointer to hold the new current BOID
				Boid* newBoid = (*it2);
				//Distance between two BOIDs
				float dist = (newBoid->GetPos() - currentBoid->GetPos()).Length();
				//If the BOID is not currentBoid & the BOID is in sight range of currentBoid
				if (currentBoid != newBoid && dist < currentBoid->getSight())
				{
					//If the new BOID is an obstacle
					if (newBoid->getType() == 0)
					{
						//If the currentBoid is within the obstacles sphere of influence
						if (dist < SimulationParameters::obstacleSize)
						{
							//Create a vector pointing from the obstacle to the current BOID
							Vector3 contactModifier = (currentBoid->GetPos() - newBoid->GetPos());
							//Normalize the vector
							contactModifier.Normalize();
							//Set the currentBoid's direction to this vector
							currentBoid->SetDirection(contactModifier);
							//This BOID has been impeded by an obstacle
							isImpeded = true;
							//Leave for loop
							break;
						}
					}
					//If the current BOID is a lower type than the new BOID
					else if (currentBoid->getType() < newBoid->getType())
					{
						//Create a vector pointing from the new BOID to the current BOID
						Vector3 fearModifier = (currentBoid->GetPos() - newBoid->GetPos());
						//Normalize the vector
						fearModifier.Normalize();
						//Add this fear vector to the modifier
						modifier += fearModifier;
						//Set the current BOID's speed to it's max speed
						currentBoid->SetSpeed(currentBoid->getMaxSpeed());
						//This BOID has been impeded by fear
						isImpeded = true;
					}
					//If the current BOID is the same type as the new BOID
					else if (currentBoid->getType() == newBoid->getType())
					{
						//If the two BOID's are colliding
						if (dist < 20.0f)
						{
							//Create a vector pointing from the newBoid to the current BOID
							Vector3 contactModifier = (currentBoid->GetPos() - newBoid->GetPos());
							//Normalize the vector
							contactModifier.Normalize();
							//Set the currentBoid's direction to this vector
							currentBoid->SetDirection(contactModifier);
							//This BOID has been impeded by an obstacle
							isImpeded = true;
							//Leave for loop
							break;
						}
						//If the BOID's are within grouping range
						else if (dist < SimulationParameters::groupDistance)
						{
							//Add the new BOID's position to the average position vector
							avPos += newBoid->GetPos();
							//Add the new BOID's direction to the average direction vector
							avDir += newBoid->getDirection();
							//Increment the group count
							count++;
						}
					}
					//If the current BOID is a higher type than the new BOID
					else if (currentBoid->getType() > newBoid->getType())
					{
						//If the current BOID is in eating range of the new BOID
						if ((newBoid->GetPos() - currentBoid->GetPos()).Length() < 10.0f)
						{
							//Damage the new BOID
							newBoid->Damage(100.0f);
							//If the new BOID is dead
							if (!newBoid->isAlive())
							{
								//Current BOID has just eaten
								currentBoid->Eat();
							}
						}
						else
						{
							//Add the vector from the current BOID to the new BOID multiplied by 1/d*d to targetHeading where d is the distance between the BOIDs
							targetHeading += (1.0f / (newBoid->GetPos() - currentBoid->GetPos()).LengthSquared()) * ((newBoid->GetPos() - currentBoid->GetPos()));
							preyCount++;
						}
						//Set the current BOID's speed to it's max speed
						currentBoid->SetSpeed(currentBoid->getMaxSpeed());
					}
				}
			}
			//If the current BOID has no prey to go after and it can group
			if (targetHeading != Vector3::Zero && !isImpeded)
			{
				//Set the BOID's speed to it's max speed
				currentBoid->SetSpeed(currentBoid->getMaxSpeed());
				//Average the targetHeading vector
				targetHeading /= preyCount;
				//Normalize the vector
				targetHeading.Normalize();
				//Set the current BOID's direction to targetHeading
				currentBoid->SetDirection(targetHeading);
				//This BOID has been impeded by hunting
				isImpeded = true;
			}

			//If there BOIDs nearby and it is not impeded
			if (count > 0 && !isImpeded)
			{
				//Average avDir vector
				avDir /= static_cast<float>(count);
				//Average avPos vector
				avPos /= static_cast<float>(count);
				//Create a vector from the current BOID to the average position
				Vector3 toAverage = currentBoid->GetPos() - (avPos + avDir * currentBoid->getSpeed());
				//Normalize the vector
				toAverage.Normalize();
				//Set the group heading of this BOID to toAverage
				currentBoid->SetGroupHeading(toAverage);
				//Set the group direction of this BOID to avDir
				currentBoid->SetGroupDirection(avDir);
				//Normalize the modifier
				modifier.Normalize();
				//Add average position and average direction vectors to modifier after scaling them to simualtion paramaters
				modifier += (toAverage * SimulationParameters::groupStrength);
				modifier += (avDir * SimulationParameters::groupHeading);
			}

			//If the current BOID is alive
			if (currentBoid->isAlive())
			{
				//Normalize the modifier
				modifier.Normalize();
				//Create a vector from the current BOID to 0,0,0
				Vector3 centerModifier = Vector3(0.0f, 0.0f, 0.0f) - currentBoid->GetPos();
				//If the BOID is outside the map
				if (centerModifier.Length() > (SimulationParameters::mapSize / 2))
				{
					//Normalize the center vector
					centerModifier.Normalize();
					//Set the modifier to the center modifier
					modifier = centerModifier;
				}
				//Add modifier to the BOID's current direction
				currentBoid->SetDirection(currentBoid->getDirection() + modifier);
				//Tick the current BOID
				currentBoid->Tick(GD);
				//Increment count
				++it;
				//Increment boidCount count for the current BOID's type
				boidCount[currentBoid->getType()]++;
			}
			else
			{
				//If the BOID is dead erase it from myBoids
				it = myBoids.erase(it);
			}
		}
		//Update simulation parameters with boidCount
		SimulationParameters::boidCount = boidCount;
	}
}

void boidManager::Draw(DrawData* DD)
{
	//Loop through each BOID in myBoids
	for (vector<Boid*>::iterator it = myBoids.begin(); it != myBoids.end(); ++it)
	{
		//Draw this BOID
		(*it)->Draw(DD);
	}
}