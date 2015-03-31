#ifndef _CAMERA_H_
#define _CAMERA_H_
#include "gameobject.h"

struct DrawData;

class Camera : public GameObject
{
public:
	Camera(float _fieldOfView, float _aspectRatio, float _nearPlaneDistance, float _farPlaneDistance, Vector3 _target, Vector3 _up);
	~Camera();
	virtual void Tick(GameData* _GD);
	virtual void Draw(DrawData* _DS);

	//Retuns the camera's projection matrix
	Matrix GetProj(){ return m_projMat; }
	//Returns the camera's view matrix
	Matrix GetView(){ return m_viewMat; }
	//Returns the camera's target
	Vector3 GetTarget(){ return m_target; }
	//Returns the camera's up vector
	Vector3 GetUp(){ return m_up; }
protected:
	//Camera's projection matrix
	Matrix m_projMat;
	//Camera's view matrix
	Matrix m_viewMat;
	//Where the camera is looking
	Vector3 m_target;
	//Which way is up
	Vector3 m_up;
	//Field of view
	float m_fieldOfView;
	//Aspect ratio
	float m_aspectRatio;
	//Closest viewable distance
	float m_nearPlaneDistance;
	//Furthest viewable distance
	float m_farPlaneDistance;

};


#endif