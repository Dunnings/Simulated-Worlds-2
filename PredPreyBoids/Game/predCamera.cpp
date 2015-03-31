#include "predCamera.h"
#include <d3d.h>
#include <DirectXMath.h>

PredCamera::PredCamera(float _fieldOfView, float _aspectRatio, float _nearPlaneDistance, float _farPlaneDistance, Vector3 _up, Vector3 _dpos)
	:Camera(_fieldOfView, _aspectRatio, _nearPlaneDistance, _farPlaneDistance, Vector3(0.0f, 0.0f, 0.0f), _up)
{
	m_targetObject = nullptr;
	m_dpos = _dpos;
}

PredCamera::~PredCamera()
{

}

void PredCamera::changeTarget(Boid* _target)
{
	m_targetObject = _target;
}

void PredCamera::Tick(GameData* _GD)
{
	if (m_targetObject != nullptr)
	{
		m_target = m_targetObject->GetPos();
		float objectYaw = atan2(m_targetObject->getDirection().x, m_targetObject->getDirection().z);

		Matrix rotMat = Matrix::CreateRotationY(objectYaw);
		m_pos = m_target + Vector3::Transform(m_dpos, rotMat);

	}
		Camera::Tick(_GD);
}