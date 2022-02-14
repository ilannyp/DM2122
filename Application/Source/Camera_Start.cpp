#include "Camera_Start.h"
#include "Application.h"
#include "Mtx44.h"

Camera_Start::Camera_Start()
{
}

Camera_Start::~Camera_Start()
{
}

void Camera_Start::Init(const Vector3& pos, const Vector3& target, const Vector3& up)
{
	this->position = defaultPosition = pos;
	this->target = defaultTarget = target;
	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);

	right.y = 0;
	right.Normalize();
	this->up = defaultUp = right.Cross(view).Normalized();
}

void Camera_Start::Update(double dt)
{
	
	static const float CAMERA_SPEED = 80.f;
	static const float ZOOM_SPEED = 20.f;

	Vector3 temp;
	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);


	
	
}

void Camera_Start::Reset()
{
	position = defaultPosition;
	target = defaultTarget;
	up = defaultUp;
}