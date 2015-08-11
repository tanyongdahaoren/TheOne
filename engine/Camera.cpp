#include "Camera.h"

void Camera::Perspective(float fieldOfView, float aspectRatio, float nearPlane, float farPlane)
{
	_projectTransform = glm::perspective(fieldOfView, aspectRatio, nearPlane, farPlane);
}

void Camera::Orthographic(float width, float height, float nearPlane, float farPlane)
{
	_projectTransform = glm::ortho(-width/2, width/2, -height/2, height/2, nearPlane, farPlane);
}

void Camera::LookAt(const vec3& eyePosition, const vec3& targetPosition)
{
	_eyePos = eyePosition;
	_viewTransform = glm::lookAt(eyePosition, targetPosition, vec3(0.0f, 1.0f, 0.0f));
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

