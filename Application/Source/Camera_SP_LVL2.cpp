#include "Camera_SP_LVL2.h"
#include "Application.h"
#include "Mtx44.h"

Camera_SP_LVL2::Camera_SP_LVL2()
{
	intnpc = 0;
}

Camera_SP_LVL2::~Camera_SP_LVL2()
{
}

void Camera_SP_LVL2::setNPCinteract(int intnpc)
{
	this->intnpc = intnpc;
}

int Camera_SP_LVL2::getNPCinteract()
{
	return intnpc;
}

void Camera_SP_LVL2::Init(const Vector3& pos, const Vector3& target, const Vector3& up)
{
	this->position = defaultPosition = pos;
	this->target = defaultTarget = target;
	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);

	right.y = 0;
	right.Normalize();
	this->up = defaultUp = right.Cross(view).Normalized();
}

void Camera_SP_LVL2::Update(double dt)
{
	
	

	Vector3 temp;
	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);
	

	
	if(Application::IsKeyPressed(VK_RIGHT))
	{
		float yaw = -CAMERA_SPEED * static_cast<float>(dt);
		Mtx44 rotation;
		rotation.SetToRotation(yaw, 0, 1, 0);
		up = rotation * up;
		view = rotation * view;
		target = position + view;
	}
	if(Application::IsKeyPressed(VK_LEFT))
	{
		float yaw = CAMERA_SPEED * static_cast<float>(dt);
		Mtx44 rotation;
		rotation.SetToRotation(yaw, 0, 1, 0);
		up = rotation * up;
		view = rotation * view;
		target = position + view;
	}
	if(Application::IsKeyPressed(VK_DOWN))
	{
		float pitch = -CAMERA_SPEED * static_cast<float>(dt);
		right.y = 0;
		right.Normalize();
		up = right.Cross(view).Normalized();
		Mtx44 rotation;
		rotation.SetToRotation(pitch, right.x, right.y, right.z);
		view = rotation * view;
		target = position + view;
	}
	if(Application::IsKeyPressed(VK_UP))
	{
		float pitch = CAMERA_SPEED * static_cast<float>(dt);
		right.y = 0;
		right.Normalize();
		up = right.Cross(view).Normalized();
		Mtx44 rotation;
		rotation.SetToRotation(pitch, right.x, right.y, right.z);
		view = rotation * view;
		target = position + view;
	}
	if(Application::IsKeyPressed('W'))
	{
		temp = position + view * ZOOM_SPEED * static_cast<float>(dt);
		if (!(temp.z > 135 /*front*/) && !(temp.x > 34 /*utmost left*/) && !(temp.x < -34/*utmost right*/) && !(temp.z < -243 /*back*/) && !(getNPCinteract() == 1))
		{
			position += view * ZOOM_SPEED * static_cast<float>(dt);
			position.y = 3;
			target = position + view;
		}
		temp = position;
	}	
	if(Application::IsKeyPressed('S'))
	{
		temp = position - view * ZOOM_SPEED * static_cast<float>(dt);
		if (!(temp.z > 135 /*front*/) && !(temp.x > 34 /*utmost left*/) && !(temp.x < -34/*utmost right*/) && !(temp.z < -243 /*back*/) && !(getNPCinteract() == 1))
		{
			position -= view * ZOOM_SPEED * static_cast<float>(dt);
			position.y = 3;
			target = position + view;
		}
		temp = position;
	}
	if (Application::IsKeyPressed('A'))
	{
		temp = position - right * ZOOM_SPEED * static_cast<float>(dt);
		if (!(temp.z > 135 /*front*/) && !(temp.x > 34 /*utmost left*/) && !(temp.x < -34/*utmost right*/) && !(temp.z < -243 /*back*/) && !(getNPCinteract() == 1))
		{
			position -= right * ZOOM_SPEED * static_cast<float>(dt);
			position.y = 3;
			target = position + view;
		}
		temp = position;
	}
	if (Application::IsKeyPressed('D'))
	{
		temp = position + right * ZOOM_SPEED * static_cast<float>(dt);
		if (!(temp.z > 135 /*front*/) && !(temp.x > 34 /*utmost left*/) && !(temp.x < -34/*utmost right*/) && !(temp.z < -243 /*back*/) && !(getNPCinteract() == 1))
		{
			position += right * ZOOM_SPEED * static_cast<float>(dt);
			position.y = 3;
			target = position + view;
		}
		temp = position;
	}

	if(Application::IsKeyPressed('R'))
	{
		Reset();
	}
}

void Camera_SP_LVL2::Reset()
{
	position = defaultPosition;
	target = defaultTarget;
	up = defaultUp;
}