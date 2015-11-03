#include "Camera.h"

Camera::Camera()
	:_viewDirty(true)
{

}

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
	UpdateViewTransform();
	_viewDirty = false;
	
	return _viewTransform;
}

void Camera::UpdateViewTransform()
{
	mat4 viewInv = GetToWorldTransform();
	_viewTransform = glm::inverse(viewInv);
}

void Camera::LookAt(const vec3& lookAtPos, const vec3& up /*= vec3(0,1,0)*/)
{
	vec3 upv = normalize(up);
	
	vec3 zaxis = normalize(this->GetPosition() - lookAtPos);

	vec3 xaxis = normalize(cross(upv, zaxis));

	vec3 yaxis = normalize(cross(zaxis, xaxis));

	mat4 rotation;
	rotation[0] = vec4(xaxis.x, xaxis.y, xaxis.z, 0);
	rotation[1] = vec4(yaxis.x, yaxis.y, yaxis.z, 0);
	rotation[2] = vec4(zaxis.x, zaxis.y, zaxis.z, 0);

	quat q = quat(rotation);
	SetRotationQuat(q);
}

