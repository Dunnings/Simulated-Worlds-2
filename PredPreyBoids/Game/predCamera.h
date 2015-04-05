#ifndef _PRED_CAMERA_H_
#define _PRED_CAMERA_H_

#include "camera.h"
#include "boid.h"
#include "gamedata.h"

class PredCamera : public Camera
{
public:
	PredCamera(float _fieldOfView, float _aspectRatio, float _nearPlaneDistance, float _farPlaneDistance, Vector3 _up, Vector3 _dpos);
	virtual ~PredCamera();
	virtual void Tick(GameData* GD);
	//Return the boid that this camera is connected to
	Boid* GetTarget(){ return m_targetObject; };
	//Set the boid that this camera is connected to
	void changeTarget(Boid* _target);
protected:
	//Target boid
	Boid* m_targetObject;
	//Offset vector
	Vector3	m_dpos;
	//Smooothed out yaw
	float m_aimYaw;
};

#endif