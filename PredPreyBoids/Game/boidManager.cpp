#include "boidManager.h"
#include "boid.h"
#include <sstream>
#include <iterator>
#include <map>
#include <istream>
#include <fstream>

ID3D11Device* GameData::p3d;
MyEffectFactory* GameData::EF;

boidManager::boidManager()
{

	loadMap();
	lastSpawnTime = GetTickCount64();

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


void boidManager::loadMap(){
	ifstream mapFile;
	//Open the file
	mapFile.open(mapFileName);
	//If the file successfully opened
	if (mapFile.is_open())
	{
		//Loop through each line in the file
		while (!mapFile.eof())
		{
			//Get the current line of the file
			string currentLine;
			getline(mapFile, currentLine);

			//We have reached a waypoint declaration
			if (currentLine == "<waypoint>"){
				Waypoint* w = new Waypoint;
				Vector3 pos = Vector3(0.0f, 0.0f, 0.0f);
				while (currentLine != "</waypoint>" && !mapFile.eof()){
					//Move to next line
					getline(mapFile, currentLine);
					if (currentLine.find("<x>") != string::npos && currentLine.find("</x>") != string::npos){
						int start = currentLine.find("<x>") + 3;
						int end = currentLine.find("</x>");
						pos.x = stof(currentLine.substr(start, end - start));
					}
					else if (currentLine.find("<y>") != string::npos && currentLine.find("</y>") != string::npos){
						int start = currentLine.find("<y>") + 3;
						int end = currentLine.find("</y>");
						pos.y = stof(currentLine.substr(start, end - start));
					}
					else if (currentLine.find("<z>") != string::npos && currentLine.find("</z>") != string::npos){
						int start = currentLine.find("<z>") + 3;
						int end = currentLine.find("</z>");
						pos.z = stof(currentLine.substr(start, end - start));
					}
					else if (currentLine.find("<size>") != string::npos && currentLine.find("</size>") != string::npos){
						int start = currentLine.find("<size>") + 6;
						int end = currentLine.find("</size>");
						w->setAreaOfInfluence(stof(currentLine.substr(start, end - start)));
					}
					else if (currentLine.find("<type>") != string::npos && currentLine.find("</type>") != string::npos){
						int start = currentLine.find("<type>") + 6;
						int end = currentLine.find("</type>");
						string type = (currentLine.substr(start, end - start));
						if (type == "start"){
							w->setMyType(waypointType::start);
						}
						else if (type == "outpost"){
							w->setMyType(waypointType::outpost);
						}
						else if (type == "finish"){
							w->setMyType(waypointType::finish);
						}
					}
					else if (currentLine.find("<typeToAffect>") != string::npos && currentLine.find("</typeToAffect>") != string::npos){
						int start = currentLine.find("<typeToAffect>") + 14;
						int end = currentLine.find("</typeToAffect>");
						w->setTypeToAffect(stoi((currentLine.substr(start, end - start))));
					}
				}
				w->SetPos(pos);
				addWaypoint(w);
			}
			else if (currentLine == "<obstacle>")
			{
				Vector3 pos = Vector3(0.0f, 0.0f, 0.0f);
				while (currentLine != "</waypoint>" && !mapFile.eof()){
					//Move to next line
					getline(mapFile, currentLine);
					if (currentLine.find("<x>") != string::npos && currentLine.find("</x>") != string::npos){
						int start = currentLine.find("<x>") + 3;
						int end = currentLine.find("</x>");
						pos.x = stof(currentLine.substr(start, end - start));
					}
					else if (currentLine.find("<y>") != string::npos && currentLine.find("</y>") != string::npos){
						int start = currentLine.find("<y>") + 3;
						int end = currentLine.find("</y>");
						pos.y = stof(currentLine.substr(start, end - start));
					}
					else if (currentLine.find("<z>") != string::npos && currentLine.find("</z>") != string::npos){
						int start = currentLine.find("<z>") + 3;
						int end = currentLine.find("</z>");
						pos.z = stof(currentLine.substr(start, end - start));
					}
				}
				Boid* b = spawnBoid(0);
				b->SetPos(pos);
			}
		}
	}
}


void boidManager::addWaypoint(waypointType _type, int _affects, Vector3 _pos, float _aoi){
	//Initialize new waypoint
	Waypoint* newW = new Waypoint;
	//Set waypoint attributes to function arguments
	newW->SetPos(_pos);
	newW->setMyType(_type);
	newW->setTypeToAffect(_affects);
	newW->setAreaOfInfluence(_aoi);
	//Add waypoint
	addWaypoint(newW);
}

void boidManager::addWaypoint(Waypoint* w){
	//Initialize waypoint
	w->initialize();
	//Add waypoint to waypoint vector
	m_waypoints.push_back(w);
}

void boidManager::deleteAllWaypoints(){
	//Loop through all waypoints
	for (vector<Waypoint*>::iterator it = m_waypoints.begin(); it != m_waypoints.end(); it++){
		//Delete waypoint
		delete((*it));
		//Set waypoint pointer to null
		(*it) = nullptr;
	}
	//Enpty m_waypoints
	m_waypoints.clear();
}

Boid* boidManager::respawnBoid(Boid* b, bool keepPos){
	Boid* b2 = spawnBoid(b->getType());
	if (keepPos){
		b2->SetPos(b->GetPos());
	}
	deleteBoid(b);
	return b2;
}

void boidManager::respawnAllBoids(bool keepPos){
	//Loop through all boids in myBoids
	for (vector<Boid*>::iterator it = myBoids.begin(); it != myBoids.end(); it++)
	{
		//Spawn new boid
		Boid* b2 = spawnBoid((*it)->getType());
		if (keepPos){
			b2->SetPos((*it)->GetPos());
		}
		(*it)->Damage(1000.0f);
	}
}

Boid* boidManager::spawnBoid(int type)
{
	//Initilize waypoint variables
	Vector3 startPos = Vector3(0.0f, 0.0f, 0.0f);
	Waypoint* outpost = nullptr;
	Waypoint* finish = nullptr;

	//If there is a start position set, spawn there
	bool startSet = false;
	vector<Waypoint*> startWaypoints;
	for (vector<Waypoint*>::iterator it = m_waypoints.begin(); it != m_waypoints.end(); it++)
	{
		if ((*it)->getMyType() == waypointType::start)
		{
			if ((*it)->getTypeToAffect() == type)
			{
				startWaypoints.push_back((*it));
			}
		}
	}
	int startRandom = 0;
	if (startWaypoints.size() > 1)
	{
		startRandom = rand() % (startWaypoints.size());
	}
	int i = 0;
	for (vector<Waypoint*>::iterator it = startWaypoints.begin(); it != startWaypoints.end(); it++)
	{
		if (i == startRandom)
		{
			float r1 = ((*it)->GetPos().x - (*it)->getAreaOfInfluence()) + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (((*it)->GetPos().x + (*it)->getAreaOfInfluence()) - ((*it)->GetPos().x - (*it)->getAreaOfInfluence()))));
			float r2 = ((*it)->GetPos().z - (*it)->getAreaOfInfluence()) + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (((*it)->GetPos().z + (*it)->getAreaOfInfluence()) - ((*it)->GetPos().z - (*it)->getAreaOfInfluence()))));
			startPos = Vector3(r1, 0.0f, r2);
			break;
		}
		i++;
	}

	vector<Waypoint*> outpostWaypoints;
	for (vector<Waypoint*>::iterator it = m_waypoints.begin(); it != m_waypoints.end(); it++)
	{
		if ((*it)->getMyType() == waypointType::outpost)
		{
			if ((*it)->getTypeToAffect() == type)
			{
				outpostWaypoints.push_back((*it));
			}
		}
	}
	int outpostRandom = 0;
	if (outpostWaypoints.size() > 1)
	{
		outpostRandom = rand() % (outpostWaypoints.size());
	}
	int i2 = 0;
	for (vector<Waypoint*>::iterator it = outpostWaypoints.begin(); it != outpostWaypoints.end(); it++)
	{
		if (i2 == outpostRandom)
		{
			outpost = (*it);
			break;
		}
		i2++;
	}

	vector<Waypoint*> finishWaypoints;
	for (vector<Waypoint*>::iterator it = m_waypoints.begin(); it != m_waypoints.end(); it++)
	{
		if ((*it)->getMyType() == waypointType::finish)
		{
			if ((*it)->getTypeToAffect() == type)
			{
				finishWaypoints.push_back((*it));
			}
		}
	}
	int finishRandom = 0;
	if (finishWaypoints.size() > 1)
	{
		finishRandom = rand() % (finishWaypoints.size());
	}
	int i3 = 0;
	for (vector<Waypoint*>::iterator it = finishWaypoints.begin(); it != finishWaypoints.end(); it++)
	{
		if (i3 == finishRandom)
		{
			finish = (*it);
			break;
		}
		i3++;
	}

	//Dynamically create a new boid
	Boid* newBoid = new Boid(finish, outpost);
	//Set the max speed to the pre-determined value
	newBoid->SetMaxSpeed(SimulationParameters::boidMaxSpeed * type);

	if(startPos == Vector3(0.0f, 0.0f, 0.0f)){
		//Create four random floats between -1.0 and 1.0 
		float r1 = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0f - -1.0f)));
		float r2 = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0f - -1.0f)));
		float r3 = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0f - -1.0f)));
		float r4 = -1.0f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (1.0f - -1.0f)));
		//Set the boid position to a random location within pre-determined map bounds
		newBoid->SetPos(Vector3(r1 * SimulationParameters::mapSize * 0.5f, 0.0f, r2* SimulationParameters::mapSize * 0.5f));
		//Set direction randomly
		newBoid->SetDirection(Vector3(r3, 0.0f, r4));
	}
	else
	{
		newBoid->SetPos(startPos);
	}
	//Set speed to half max speed
	newBoid->SetSpeed(((SimulationParameters::boidMaxSpeed*type) / 2));
	//Set the boid type
	newBoid->SetType(type);
	//Initialise boid vertex buffer
	newBoid->initialize();
	//Scale down prey
	if (type == 1)
	{
		newBoid->SetScale(0.6f);
	}
	//Add boid to manager's vector
	toSpawn.push_back(newBoid);
	//Return the created boid
	return newBoid;
}

void boidManager::deleteBoid(Boid* b){
	//If the given boid is the cursor boid
	if (cursor == b){
		//Delete the cursor boid
		delete b;
		b = nullptr;
	}
	else{
		//Otherwise loop through all boids in myBoids
		for (vector<Boid*>::iterator it = myBoids.begin(); it != myBoids.end();)
		{
			//If the current boid is the given boid
			if ((*it) == b)
			{
				//Delete this boid and stop searching
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
	//Loop through all boids in myBoids
	for (vector<Boid*>::iterator it = myBoids.begin(); it != myBoids.end();)
	{
		//If the current boid is the same type as the given type
		if ((*it)->getType() == type)
		{
			//Delete this boid and stop searching
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
	//Loop through all boids in myBoids
	for (vector<Boid*>::iterator it = myBoids.begin(); it != myBoids.end(); it++)
	{
		//Kill the boid
		(*it)->Damage(1000.0f);
		(*it) = nullptr;
	}
	//Loop through all boids in toSpawn
	for (vector<Boid*>::iterator it = toSpawn.begin(); it != toSpawn.end(); it++)
	{
		//Kill the boid
		(*it)->Damage(1000.0f);
		(*it) = nullptr;
	}
	//Empty myBoids
	myBoids.clear();
	//Empty toSpawn
	toSpawn.clear();
}

Boid* boidManager::getHighestBOID()
{
	//Start with type 0
	int type = 0;
	//Initialize a boid to return
	Boid* toReturn = nullptr;
	//Loop through all boids in myBoids
	for (vector<Boid*>::iterator it = myBoids.begin(); it != myBoids.end(); it++)
	{
		//If this boid's type is higher than type
		if ((*it)->getType() > type)
		{
			//Set the boid to return to this one
			toReturn = (*it);
			//Set the type to this boid's type
			type = toReturn->getType();
		}
	}
	//Return the highest boid
	return toReturn;
}

void boidManager::Tick(GameData* GD)
{
	if (toSpawn.size() > 0){
		if (GetTickCount64() - lastSpawnTime > SimulationParameters::spawnDelay){
			myBoids.push_back(toSpawn.front());
			toSpawn.erase(toSpawn.begin());
			lastSpawnTime = GetTickCount64();
		}
	}

	//If the simulation is running
	if (GD->GS == GS_PLAY_PLAY)
	{
		//Initialise a map to hold the boid counts
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

		//Loop through each boid in myBoids
		for (vector<Boid*>::iterator firstLoop = myBoids.begin(); firstLoop != myBoids.end();)
		{
			//For ease of visibility, create a boid pointer to hold the current boid
			Boid* currentBoid = (*firstLoop);
			//Vector that holds the average direction of all boids near the current boid
			Vector3 avDir;
			//Vector that holds the average position of all boids near the current boid
			Vector3 avPos;
			//Integer that holds how many prey the currentBoid is attracted to
			int preyCount = 0;
			//Integer that holds how many boids are nearby for grouping
			int count = 0;
			//Boolean that states if the boid is impeded from grouping
			bool isImpeded = false;
			//Vector that holds the direction toward prey
			Vector3 targetHeading = Vector3(0.0f, 0.0f, 0.0f);
			//Vector that holds the direction to travel this tick
			Vector3 modifier = Vector3(0.0f, 0.0f, 0.0f);

			//Loop through each boid in myBoids
			for (vector<Boid*>::iterator it2 = myBoids.begin(); it2 != myBoids.end(); ++it2)
			{
				//For ease of visibility, create a boid pointer to hold the new current boid
				Boid* newBoid = (*it2);
				//Distance between two boids
				float dist = (newBoid->GetPos() - currentBoid->GetPos()).Length();
				//If the boid is not currentBoid & the boid is in sight range of currentBoid
				if (currentBoid != newBoid && dist < currentBoid->getSight())
				{
					//If the new boid is an obstacle
					if (newBoid->getType() == 0)
					{
						//If the currentBoid is within the obstacles sphere of influence
						if (dist < SimulationParameters::obstacleSize)
						{
							//Create a vector pointing from the obstacle to the current boid
							Vector3 contactModifier = (currentBoid->GetPos() - newBoid->GetPos());
							//Normalize the vector
							contactModifier.Normalize();
							//Set the currentBoid's direction to this vector
							currentBoid->SetDirection(contactModifier);
							//This boid has been impeded by an obstacle
							isImpeded = true;
							//Leave for loop
							break;
						}
					}
					//If the current boid is a lower type than the new boid
					else if (currentBoid->getType() < newBoid->getType())
					{
						//Create a vector pointing from the new boid to the current boid
						Vector3 fearModifier = (currentBoid->GetPos() - newBoid->GetPos());
						//Normalize the vector
						fearModifier.Normalize();
						//Add this fear vector to the modifier
						modifier += fearModifier;
						//Set the current boid's speed to it's max speed
						currentBoid->SetSpeed(currentBoid->getMaxSpeed());
						//This boid has been impeded by fear
						isImpeded = true;
					}
					//If the current boid is the same type as the new boid
					else if (currentBoid->getType() == newBoid->getType())
					{
						//If the two boid's are colliding
						if (dist < 20.0f)
						{
							//Create a vector pointing from the newBoid to the current boid
							Vector3 contactModifier = (currentBoid->GetPos() - newBoid->GetPos());
							//Normalize the vector
							contactModifier.Normalize();
							//Set the currentBoid's direction to this vector
							currentBoid->SetDirection(contactModifier);
							//This boid has been impeded by an obstacle
							isImpeded = true;
							//Leave for loop
							break;
						}
						//If the boid's are within grouping range
						else if (dist < SimulationParameters::groupDistance)
						{
							//Add the new boid's position to the average position vector
							avPos += newBoid->GetPos();
							//Add the new boid's direction to the average direction vector
							avDir += newBoid->getDirection();
							//Increment the group count
							count++;
						}
					}
					//If the current boid is a higher type than the new boid
					else if (currentBoid->getType() > newBoid->getType())
					{
						//If the current boid is in eating range of the new boid
						if ((newBoid->GetPos() - currentBoid->GetPos()).Length() < 10.0f)
						{
							//Damage the new boid
							newBoid->Damage(100.0f);
							//If the new boid is dead
							if (!newBoid->isAlive())
							{
								if (SimulationParameters::respawnOnDeath){
									spawnBoid(newBoid->getType());
								}
								//Current boid has just eaten
								currentBoid->Eat();
							}
						}
						else
						{
							//Check if the current boid has an outpost assigned to it, if it has only hunt the new boid if it is within the outpost bounds
							bool isHuntable = false;
							if (currentBoid->GetOutpost() != nullptr){
								if ((newBoid->GetPos() - currentBoid->GetOutpost()->GetPos()).Length() < currentBoid->GetOutpost()->getAreaOfInfluence())
								{
									isHuntable = true;
								}
							}
							else
							{
								isHuntable = true;
							}
							if (isHuntable)
							{
								//Add the vector from the current boid to the new boid multiplied by 1/d*d to targetHeading where d is the distance between the boids
								targetHeading += (1.0f / (newBoid->GetPos() - currentBoid->GetPos()).LengthSquared()) * ((newBoid->GetPos() - currentBoid->GetPos()));
								preyCount++;
							}
						}
						//Set the current boid's speed to it's max speed
						currentBoid->SetSpeed(currentBoid->getMaxSpeed());
					}
				}
			}
			//If the current boid has no prey to go after and it can group
			if (targetHeading != Vector3::Zero && !isImpeded)
			{
				//Set the boid's speed to it's max speed
				currentBoid->SetSpeed(currentBoid->getMaxSpeed());
				//Average the targetHeading vector
				targetHeading /= (float)preyCount;
				//Normalize the vector
				targetHeading.Normalize();
				//Set the current boid's direction to targetHeading
				currentBoid->SetDirection(targetHeading);
				//This boid has been impeded by hunting
				isImpeded = true;
			}

			//If there boids nearby and it is not impeded
			if (count > 0 && !isImpeded)
			{
				//Average avDir vector
				avDir /= static_cast<float>(count);
				//Average avPos vector
				avPos /= static_cast<float>(count);
				//Create a vector from the current boid to the average position
				Vector3 toAverage = currentBoid->GetPos() - (avPos + avDir * currentBoid->getSpeed());
				//Normalize the vector
				toAverage.Normalize();
				//Set the group heading of this boid to toAverage
				currentBoid->SetGroupHeading(toAverage);
				//Set the group direction of this boid to avDir
				currentBoid->SetGroupDirection(avDir);
				//Normalize the modifier
				modifier.Normalize();
				//Add average position and average direction vectors to modifier after scaling them to simualtion paramaters
				modifier += (toAverage * SimulationParameters::groupStrength);
				modifier += (avDir * SimulationParameters::groupHeading);
			}

			//If the current boid is alive
			if (currentBoid->isAlive())
			{
				//Normalize the modifier
				modifier.Normalize();
				
				bool nothingToDo = true;
				//If there is a finish point for this boid then add it to the modifier
				if (currentBoid->GetFinish() != nullptr){
					nothingToDo = false;
					if (currentBoid->GetFinish()->returnToward(currentBoid).Length() < currentBoid->GetFinish()->getAreaOfInfluence())
					{
						if (SimulationParameters::respawnOnFinish){
							spawnBoid(currentBoid->getType());
						}
						currentBoid->Damage(100.0f);
					}
					else{
						modifier += 0.5f * currentBoid->GetFinish()->returnNormalizedToward(currentBoid);
					}
				}
				//If there is no finish, check if there is an outpost the current boid should go to
				if (nothingToDo){
				if (currentBoid->GetOutpost())
					if (currentBoid->GetOutpost()->returnToward(currentBoid).Length() > currentBoid->GetOutpost()->getAreaOfInfluence())
					{
						//Set the current boid's direction to the center modifier
						currentBoid->SetDirection(currentBoid->GetOutpost()->returnNormalizedToward(currentBoid));
						modifier = Vector3::Zero; 
					}
					nothingToDo = false;
				}
				//If there is still nothing to do, make sure I'm near the center of the screen
				if (nothingToDo){
					//Create a vector from the current boid to 0,0,0
					Vector3 centerModifier = Vector3(0.0f, 0.0f, 0.0f) - currentBoid->GetPos();
					//If the boid is outside the map
					if (centerModifier.Length() > (SimulationParameters::mapSize / 2))
					{
						//Normalize the center vector
						centerModifier.Normalize();
						//Set the modifier to the center modifier
						modifier = centerModifier;
					}
				}

				//Add modifier to the boid's current direction
				currentBoid->SetDirection(currentBoid->getDirection() + modifier);
				//Tick the current boid
				currentBoid->Tick(GD);
				//Increment count
				++firstLoop;
				//Increment boidCount count for the current boid's type
				boidCount[currentBoid->getType()]++;
			}
			else
			{
				//If the boid is dead erase it from myBoids
				firstLoop = myBoids.erase(firstLoop);
			}
		}
		//Update simulation parameters with boidCount
		SimulationParameters::boidCount = boidCount;
	}

	//Loop through each checkpoint
	for (vector<Waypoint*>::iterator it = m_waypoints.begin(); it != m_waypoints.end(); it++){
		//Draw this checkpoint
		(*it)->Tick(GD);
	}
}

void boidManager::Draw(DrawData* DD)
{
	//Loop through each boid in myBoids
	for (vector<Boid*>::iterator it = myBoids.begin(); it != myBoids.end(); ++it)
	{
		//Draw this boid
		(*it)->Draw(DD);
	}
	//Loop through each checkpoint
	for (vector<Waypoint*>::iterator it = m_waypoints.begin(); it != m_waypoints.end(); it++){
		//Draw this checkpoint
		(*it)->Draw(DD);
	}
}