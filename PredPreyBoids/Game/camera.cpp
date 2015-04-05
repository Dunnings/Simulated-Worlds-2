#include "camera.h"

Camera::Camera(float _fieldOfView, float _aspectRatio, float _nearPlaneDistance, float _farPlaneDistance, Vector3 _target, Vector3 _up)
{
	//Set variables
	m_fieldOfView = _fieldOfView;
	m_aspectRatio = _aspectRatio;
	m_nearPlaneDistance = _nearPlaneDistance;
	m_farPlaneDistance = _farPlaneDistance;
	m_target = _target;
	m_up = _up;
}

Camera::~Camera()
{
}

void Camera::Tick(GameData* _GD)
{
	//Create projection matrix
	m_projMat = Matrix::CreatePerspectiveFieldOfView(m_fieldOfView, m_aspectRatio, m_nearPlaneDistance, m_farPlaneDistance);
	//Create view matrix
	m_viewMat = Matrix::CreateLookAt(m_pos, m_target, m_up);
	//Tick the camera
	GameObject::Tick(_GD);
}

void Camera::Draw(DrawData* _DD)
{
	//Don't need to draw the camera
	//To make the compiler happy
	_DD;
}