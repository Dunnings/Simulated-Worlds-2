#ifndef _WAYPOINT_H
#define _WAYPOINT_H
#include "VBGO.h"
#include "boid.h"

enum waypointType{
	start,
	outpost,
	finish
};

class Waypoint : public VBGO
{
public:
	Waypoint();
	~Waypoint();
	void Tick(GameData* GD);
	void Draw(DrawData* DD);
	//Initialize the vertex array for this object
	virtual void initialize();
	//Return the vector from a boid to me
	Vector3 GetToward(Boid* b) { return m_pos - b->GetPos(); };
	//Return the normalized vector from a boid to me
	Vector3 GetNormalizedToward(Boid* b) { Vector3 toReturn = (m_pos - b->GetPos()); toReturn.Normalize(); return toReturn; };
	//Return my area of influence
	float GetAreaOfInfluence() { return areaOfInfluence; };
	//Return the type ID that I affect
	int GetTypeToAffect() { return typeToAffect; };
	//Return my type of waypoint
	int GetMyType() { return myType; };

	//Set the type ID that I affect
	void SetTypeToAffect(int _newType) { typeToAffect = _newType; };
	//Set my type of waypoint
	void SetMyType(waypointType _newType) { myType = _newType; };
	//Return my area of influence
	void SetAreaOfInfluence(float aoi){ areaOfInfluence = aoi; };
private:
	//Which type ID does this waypoint affect
	int typeToAffect;
	//Which type of waypoint am I
	waypointType myType;
	//How far is my area of influence from my center
	float areaOfInfluence = 200.0f;
	//Every vertex of this waypoint
	myVertex* m_vertices;
	//Line vertices for drawing debug info
	vector<myVertex> lineVertices;
};

#endif