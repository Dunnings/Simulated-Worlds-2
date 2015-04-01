#include "predCamera.h"
#include <d3d.h>
#include <DirectXMath.h>

PredCamera::PredCamera(float _fieldOfView, float _aspectRatio, float _nearPlaneDistance, float _farPlaneDistance, Vector3 _up, Vector3 _dpos)
	:Camera(_fieldOfView, _aspectRatio, _nearPlaneDistance, _farPlaneDistance, Vector3(0.0f, 0.0f, 0.0f), _up)
{
	//Set target to null
	m_targetObject = nullptr;
	m_dpos = _dpos;
}

PredCamera::~PredCamera()
{

}

void PredCamera::changeTarget(Boid* _target)
{
	//Change the current target
	m_targetObject = _target;
}

void PredCamera::Tick(GameData* _GD)
{
	//If the camera has a target
	if (m_targetObject != nullptr)
	{
		//Set m_target to the target's position
		m_target = m_targetObject->GetPos();
		//Rotate the camera around it's target based on the target's yaw
		float objectYaw = atan2(m_targetObject->getDirection().x, m_targetObject->getDirection().z);
		Matrix rotMat = Matrix::CreateRotationY(objectYaw);
		//Move the camera to the correct position using rotation matrix
		m_pos = m_target + Vector3::Transform(m_dpos, rotMat);
	}
	//Tick the camera
	Camera::Tick(_GD);
}