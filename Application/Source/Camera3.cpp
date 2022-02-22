#include "Camera3.h"
#include "Application.h"
#include "Mtx44.h"

Camera3::Camera3()
{
}

Camera3::~Camera3()
{
}

void Camera3::Init(const Vector3& pos, const Vector3& target, const Vector3& up)
{
	this->position = defaultPosition = pos;
	this->target = defaultTarget = target;
	Vector3 view = (target - position).Normalized();
	Vector3 right = view.Cross(up);

	right.y = 0;
	right.Normalize();
	this->up = defaultUp = right.Cross(view).Normalized();
}

void Camera3::Update(double dt)
{
	
	static const float CAMERA_SPEED = 80.f;
	static const float ZOOM_SPEED = 20.f;

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
		if (Application::yourself.get_first_scammed() == false)
		{
			temp = position + view * ZOOM_SPEED * static_cast<float>(dt);
			if (!(temp.z < -60 /*back*/) && !(temp.x > 18 /*utmost left*/) && !(temp.x < -103/*utmost right*/) &&
				!(temp.z > 103/*utmost front*/) && !(temp.x < -7 && temp.x >= -34 && temp.z < 72 && temp.z > -25/*wall on the right from origin*/) &&
				!(temp.z < 74 && temp.z > -25 && temp.x >= -32 && temp.x < -7) && !(temp.z <= 17 && temp.z > -25 && temp.x <= -33)
				&& !(temp.x <= -13 && temp.x >= -17 && temp.z <= 103 && temp.z >= 50) && !(temp.x <= -95 && temp.x >= -103 && temp.z <= 30 && temp.z >= 17))
			{
				position += view * ZOOM_SPEED * static_cast<float>(dt);
				position.y = 3;
				target = position + view;
			}
			temp = position;
		}
		else
		{
			temp = position + view * ZOOM_SPEED * static_cast<float>(dt);
			if (!(temp.z < -60 /*back*/) && !(temp.x > 18 /*utmost left*/) && !(temp.x < -103/*utmost right*/) &&
				!(temp.z > 103/*utmost front*/) && !(temp.x < -7 && temp.x >= -34 && temp.z < 72 && temp.z > -25/*wall on the right from origin*/) &&
				!(temp.z < 74 && temp.z > -25 && temp.x >= -32 && temp.x < -7) && !(temp.z <= 17 && temp.z > -25 && temp.x <= -33)
				&& !(temp.x <= -13 && temp.x >= -17 && temp.z <= 103 && temp.z >= 84) && !(temp.x <= -95 && temp.x >= -103 && temp.z <= 30 && temp.z >= 17))
			{
				position += view * ZOOM_SPEED * static_cast<float>(dt);
				position.y = 3;
				target = position + view;
			}
			temp = position;
		}
	}	
	if(Application::IsKeyPressed('S'))
	{
		if (Application::yourself.get_first_scammed() == false)
		{
			temp = position - view * ZOOM_SPEED * static_cast<float>(dt);
			if (!(temp.z < -60 /*back*/) && !(temp.x > 18 /*utmost left*/) && !(temp.x < -103/*utmost right*/) &&
				!(temp.z > 103/*utmost front*/) && !(temp.x < -7 && temp.x >= -34 && temp.z < 72 && temp.z > -25/*wall on the right from origin*/) &&
				!(temp.z < 74 && temp.z > -25 && temp.x >= -32 && temp.x < -7) && !(temp.z <= 17 && temp.z > -25 && temp.x <= -33) &&
				!(temp.x <= -13 && temp.x >= -17 && temp.z <= 103 && temp.z >= 50) && !(temp.x <= -95 && temp.x >= -103 && temp.z <= 30 && temp.z >= 17))
			{
				position -= view * ZOOM_SPEED * static_cast<float>(dt);
				position.y = 3;
				target = position + view;
			}
			temp = position;
		}
		else
		{
			temp = position - view * ZOOM_SPEED * static_cast<float>(dt);
			if (!(temp.z < -60 /*back*/) && !(temp.x > 18 /*utmost left*/) && !(temp.x < -103/*utmost right*/) &&
				!(temp.z > 103/*utmost front*/) && !(temp.x < -7 && temp.x >= -34 && temp.z < 72 && temp.z > -25/*wall on the right from origin*/) &&
				!(temp.z < 74 && temp.z > -25 && temp.x >= -32 && temp.x < -7) && !(temp.z <= 17 && temp.z > -25 && temp.x <= -33) &&
				!(temp.x <= -13 && temp.x >= -17 && temp.z <= 103 && temp.z >= 84) && !(temp.x <= -95 && temp.x >= -103 && temp.z <= 30 && temp.z >= 17))
			{
				position -= view * ZOOM_SPEED * static_cast<float>(dt);
				position.y = 3;
				target = position + view;
			}
			temp = position;
		}
	}
	if (Application::IsKeyPressed('A'))
	{
		if (Application::yourself.get_first_scammed() == false)
		{
			temp = position - right * ZOOM_SPEED * static_cast<float>(dt);
			if (!(temp.z < -60 /*back*/) && !(temp.x > 18 /*utmost left*/) && !(temp.x < -103/*utmost right*/) &&
				!(temp.z > 103/*utmost front*/) && !(temp.x < -7 && temp.x >= -34 && temp.z < 72 && temp.z > -25/*wall on the right from origin*/) &&
				!(temp.z < 74 && temp.z > -25 && temp.x >= -32 && temp.x < -7) && !(temp.z <= 17 && temp.z > -25 && temp.x <= -33) &&
				!(temp.x <= -13 && temp.x >= -17 && temp.z <= 103 && temp.z >= 50) && !(temp.x <= -95 && temp.x >= -103 && temp.z <= 30 && temp.z >= 17))
			{
				position -= right * ZOOM_SPEED * static_cast<float>(dt);
				position.y = 3;
				target = position + view;
			}
			temp = position;
		}
		else
		{
			temp = position - right * ZOOM_SPEED * static_cast<float>(dt);
			if (!(temp.z < -60 /*back*/) && !(temp.x > 18 /*utmost left*/) && !(temp.x < -103/*utmost right*/) &&
				!(temp.z > 103/*utmost front*/) && !(temp.x < -7 && temp.x >= -34 && temp.z < 72 && temp.z > -25/*wall on the right from origin*/) &&
				!(temp.z < 74 && temp.z > -25 && temp.x >= -32 && temp.x < -7) && !(temp.z <= 17 && temp.z > -25 && temp.x <= -33) &&
				!(temp.x <= -13 && temp.x >= -17 && temp.z <= 103 && temp.z >= 84) && !(temp.x <= -95 && temp.x >= -103 && temp.z <= 30 && temp.z >= 17))
			{
				position -= right * ZOOM_SPEED * static_cast<float>(dt);
				position.y = 3;
				target = position + view;
			}
			temp = position;
		}
	}
	if (Application::IsKeyPressed('D'))
	{
		if (Application::yourself.get_first_scammed() == false)
		{
			temp = position + right * ZOOM_SPEED * static_cast<float>(dt);
			if (!(temp.z < -60 /*back*/) && !(temp.x > 18 /*utmost left*/) && !(temp.x < -103/*utmost right*/) &&
				!(temp.z > 103/*utmost front*/) && !(temp.x < -7 && temp.x >= -34 && temp.z < 72 && temp.z > -25/*wall on the right from origin*/) &&
				!(temp.z < 74 && temp.z > -25 && temp.x >= -32 && temp.x < -7) && !(temp.z <= 17 && temp.z > -25 && temp.x <= -33) &&
				!(temp.x <= -13 && temp.x >= -17 && temp.z <= 103 && temp.z >= 50) && !(temp.x <= -95 && temp.x >= -103 && temp.z <= 30 && temp.z >= 17))
			{
				position += right * ZOOM_SPEED * static_cast<float>(dt);
				position.y = 3;
				target = position + view;
			}
			temp = position;
		}
		else
		{
			temp = position + right * ZOOM_SPEED * static_cast<float>(dt);
			if (!(temp.z < -60 /*back*/) && !(temp.x > 18 /*utmost left*/) && !(temp.x < -103/*utmost right*/) &&
				!(temp.z > 103/*utmost front*/) && !(temp.x < -7 && temp.x >= -34 && temp.z < 72 && temp.z > -25/*wall on the right from origin*/) &&
				!(temp.z < 74 && temp.z > -25 && temp.x >= -32 && temp.x < -7) && !(temp.z <= 17 && temp.z > -25 && temp.x <= -33) &&
				!(temp.x <= -13 && temp.x >= -17 && temp.z <= 103 && temp.z >= 84) && !(temp.x <= -95 && temp.x >= -103 && temp.z <= 30 && temp.z >= 17))
			{
				position += right * ZOOM_SPEED * static_cast<float>(dt);
				position.y = 3;
				target = position + view;
			}
			temp = position;
		}
	}

	if(Application::IsKeyPressed('R'))
	{
		Reset();
	}
}

void Camera3::Reset()
{
	position = defaultPosition;
	target = defaultTarget;
	up = defaultUp;
}