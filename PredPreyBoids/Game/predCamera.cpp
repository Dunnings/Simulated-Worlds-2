#include "predCamera.h"
//a basic Third person camera which follows a give GameObject at a displacement of _dpos

PredCamera::PredCamera(float _fieldOfView, float _aspectRatio, float _nearPlaneDistance, float _farPlaneDistance, Boid* _target, Vector3 _up, Vector3 _dpos)
	:Camera(_fieldOfView, _aspectRatio, _nearPlaneDistance, _farPlaneDistance, _target->GetPos(), _up)
{
	m_targetObject = _target;
	m_dpos = _dpos;
}

PredCamera::~PredCamera()
{

}

void PredCamera::Tick(GameData* _GD)
{

	//Set up position of camera and target position of camera based on new position and orientation of target object
	m_target = m_targetObject->GetPos();
	float newYaw = atan2(m_targetObject->getDirection().x, m_targetObject->getDirection().z);

	Matrix rotMat = Matrix::CreateRotationY((newYaw * 0.6f) + (lastYaw * 0.4f));
	m_pos = m_target + Vector3::Transform(m_dpos, rotMat);


	//and then set up proj and view matrices

	lastYaw = newYaw;

	Camera::Tick(_GD);
}