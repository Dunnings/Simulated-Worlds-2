#ifndef _TPSCAMERA_H_
#define _TPSCAMERA_H_
#include "camera.h"
#include "boid.h"
#include "gamedata.h"
//a basic Third person camera which follows a give GameObject at a displacement of _dpos

class PredCamera : public Camera
{
public:
	PredCamera(float _fieldOfView, float _aspectRatio, float _nearPlaneDistance, float _farPlaneDistance, Boid* _target, Vector3 _up, Vector3 _dpos);
	virtual ~PredCamera();
	Boid* GetTarget(){ return m_targetObject; };
	void SetTarget(Boid*	_m_targetObject){ m_targetObject = _m_targetObject; };
	virtual void Tick(GameData* GD);

protected:
	Boid*	m_targetObject;
	Vector3	m_dpos;
	float m_aimYaw;
};

#endif