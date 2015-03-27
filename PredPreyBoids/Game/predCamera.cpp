#include "predCamera.h"
//a basic Third person camera which follows a give GameObject at a displacement of _dpos

PredCamera::PredCamera(float _fieldOfView, float _aspectRatio, float _nearPlaneDistance, float _farPlaneDistance, Boid* _target, Vector3 _up, Vector3 _dpos)
	:Camera(_fieldOfView, _aspectRatio, _nearPlaneDistance, _farPlaneDistance, _target->GetPos(), _up)
{
	m_targetObject = _target;
	m_dpos = _dpos;
	m_aimYaw = 0.0f;
}

PredCamera::~PredCamera()
{

}

void PredCamera::Tick(GameData* _GD)
{
	m_target = m_targetObject->GetPos();
	//m_aimYaw = atan2(m_targetObject->getDirection().x, m_targetObject->getDirection().z);

	float objectYaw = atan2(m_targetObject->getDirection().x, m_targetObject->getDirection().z);

	if (m_aimYaw + XM_2PI + 0.3f < objectYaw + XM_2PI || m_aimYaw + XM_2PI - 0.3f > objectYaw + XM_2PI){
		// DO NOTHING
	}
	else if (m_aimYaw + XM_2PI > objectYaw + XM_2PI)
	{
		m_aimYaw -= 0.4f;
	}
	else if (m_aimYaw + XM_2PI < objectYaw + XM_2PI)
	{
		m_aimYaw += 0.4f;
	}

	Matrix rotMat = Matrix::CreateRotationY(m_aimYaw);
	m_pos = m_target + Vector3::Transform(m_dpos, rotMat);

	while (m_aimYaw > XM_2PI){
		m_aimYaw -= XM_2PI;
	}

	while (m_aimYaw < -XM_2PI){
		m_aimYaw += XM_2PI;
	}

	Camera::Tick(_GD);
}