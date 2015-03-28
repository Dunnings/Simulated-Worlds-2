#include "predCamera.h"

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
	m_target = m_targetObject->GetPos();
	float objectYaw = atan2(m_targetObject->getDirection().x, m_targetObject->getDirection().z);

	Matrix rotMat = Matrix::CreateRotationY(objectYaw);
	m_pos = m_target + Vector3::Transform(m_dpos, rotMat);

	Camera::Tick(_GD);
}