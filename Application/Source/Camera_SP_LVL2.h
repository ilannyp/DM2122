#ifndef CAMERA_SP_LVL2_H
#define CAMERA_SP_LVL2_H

#include "Camera.h"

class Camera_SP_LVL2 : public Camera
{
public:
	//Vector3 position;
	//Vector3 target;
	//Vector3 up;
	
	int intnpc;
	Vector3 defaultPosition;
	Vector3 defaultTarget;
	Vector3 defaultUp;

	Camera_SP_LVL2();
	~Camera_SP_LVL2();
	void setNPCinteract(int intnpc);
	int getNPCinteract();

	virtual void Init(const Vector3& pos, const Vector3& target, const Vector3& up);
	virtual void Update(double dt);
	virtual void Reset();
	
};

#endif