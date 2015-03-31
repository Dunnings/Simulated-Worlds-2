#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "gameobject.h"
struct DrawData;

class Light : public GameObject
{
public:
	Light(Vector3 _pos, Color _colour, Color _ambientColour);
	~Light();
	virtual void Draw(DrawData* _DD){ _DD; };
	virtual void Tick(GameData* _GD);
	//Returns the light's colour
	Color GetColour() { return m_colour; }
	//Returns the light's ambient colour
	Color GetAmbCol(){ return m_ambientColour; }
	//Sets the light's colour
	void SetColor(Color _colour) { m_colour = _colour; }
	//Sets the light's ambient colour
	void SetAmbCol(Color _colour){ m_ambientColour = _colour; }
protected:
	//Light colour
	Color m_colour;
	//Light ambient colour
	Color m_ambientColour;
};

#endif