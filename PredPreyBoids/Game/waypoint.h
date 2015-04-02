#ifndef _WAYPOINT_H
#define _WAYPOINT_H
#include "VBGO.h"
#include "boid.h"

enum waypointType{
	start,
	outpost,
	finish
};

class Waypoint : public Boid
{
public:
	Waypoint();
	~Waypoint();
	Vector3 returnToward(Boid* b){ return m_pos - b->GetPos(); };
	Vector3 returnNormalizedToward(Boid* b){ Vector3 toReturn = (m_pos - b->GetPos()); toReturn.Normalize(); return toReturn; };
	float getAreaOfInfluence(){ return areaOfInfluence; }
	int getTypeToAffect(){ return typeToAffect; }
	void setTypeToAffect(int _newType){ typeToAffect = _newType; };
	int getMyType(){ return myType; }
	void setMyType(waypointType _newType){ myType = _newType; };
	void setAreaOfInfluence(float aoi){ areaOfInfluence = aoi; }
	void Tick(GameData* GD);
	void Draw(DrawData* DD);
	virtual void initialize();
private:
	int typeToAffect;
	waypointType myType;
	float areaOfInfluence = 200.0f;
};

#endif