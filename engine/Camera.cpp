#include "Camera.h"

void Camera::Perspective(float fieldOfView, float aspectRatio, float nearPlane, float farPlane)
{
	_projectTransform = glm::perspective(fieldOfView, aspectRatio, nearPlane, farPlane);
}

void Camera::Orthographic(float width, float height, float nearPlane, float farPlane)
{
	_projectTransform = glm::ortho(-width/2, width/2, -height/2, height/2, nearPlane, farPlane);
}

const mat4& Camera::GetProjectTransform()
{
	return _projectTransform;
}

const mat4& Camera::GetViewTransform()
{
	return _viewTransform;
}

glm::vec3 Camera::GetEyePos()
{
	return _eyePos;
}

void Camera::SetEyePos(vec3 pos)
{
	_eyePos = pos;
}

void Camera::SetTarget(vec3 target)
{
	_direction = normalize(target - _eyePos);

	vec3 hDir(_direction.x, 0.0, _direction.z);
	hDir = normalize(hDir);

	_verticalAngle = glm::degrees(asin(_direction.y));

	if (hDir.z >= 0.0f)
	{
		if (hDir.x >= 0.0f)
		{
			_horizontalAngle = 360.0f - glm::degrees(asin(hDir.z));
		}
		else
		{
			_horizontalAngle = 180.0f + glm::degrees(asin(hDir.z));
		}
	}
	else
	{
		if (hDir.x >= 0.0f)
		{
			_horizontalAngle = glm::degrees(asin(-hDir.z));
		}
		else
		{
			_horizontalAngle = 90.0f + glm::degrees(asin(-hDir.z));
		}
	}
}

void Camera::UpdateViewTransform()
{
	float vangle = radians(_verticalAngle);
	float hangle = radians(_horizontalAngle);

	glm::vec3 direction(
		cos(vangle) * cos(hangle),
		sin(vangle),
		cos(vangle) * sin(hangle)
		);

	// Right vector
	glm::vec3 right = glm::vec3(
		cos(hangle - 3.14f / 2.0f),
		0,
		sin(hangle - 3.14f / 2.0f)
		);

	// Up vector

	glm::vec3 up = glm::cross(direction, right);
	_viewTransform = glm::lookAt(_eyePos, _eyePos + direction, up);
}

void Camera::SetAngle(vec2 delta)
{
	_horizontalAngle += delta.x;
	_verticalAngle += delta.y;

	UpdateViewTransform();
}

