#ifndef CAMERA_SP_LVL3_H
#define CAMERA_SP_LVL3_H

#include "Camera.h"

class Camera_SP_LVL3 : public Camera
{
public:
	//Vector3 position;
	//Vector3 target;
	//Vector3 up;

	Vector3 defaultPosition;
	Vector3 defaultTarget;
	Vector3 defaultUp;

	Camera_SP_LVL3();
	~Camera_SP_LVL3();
	virtual void Init(const Vector3& pos, const Vector3& target, const Vector3& up);
	virtual void Update(double dt);
	virtual void Reset();
};

#endif