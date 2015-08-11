#pragma once

#include "Ref.h"
#include "MathH.h"

class Camera : public Ref
{
public:
	void Perspective(float fieldOfView, float aspectRatio, float nearPlane, float farPlane);

	void Orthographic(float width, float height, float nearPlane, float farPlane);

	void LookAt(const vec3& eyePosition, const vec3& targetPosition);

	const mat4& GetProjectTransform();

	const mat4& GetViewTransform();

	vec3 GetEyePos();
public:
	vec3 _eyePos;
	mat4 _projectTransform;
	mat4 _viewTransform;
};