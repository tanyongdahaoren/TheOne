#pragma once

#include "Ref.h"
#include "MathH.h"

class Camera : public Ref
{
public:
	Camera();

	void Perspective(float fieldOfView, float aspectRatio, float nearPlane, float farPlane);

	void Orthographic(float width, float height, float nearPlane, float farPlane);

	const mat4& GetProjectTransform();

	const mat4& GetViewTransform();

	void SetEyePos(vec3 pos);

	vec3 GetEyePos();

	void SetTarget(vec3 target);

	void ChangeCameraAngle(vec2 delta);

	void SetMouseSpeed(float speed);

	vec3 GetDirection();

	vec3 GetRight();

protected:
	void UpdateViewTransform();

public:
	vec3 _eyePos;
	vec3 _direction;
	mat4 _projectTransform;
	mat4 _viewTransform;

	float _horizontalAngle;
	float _verticalAngle;
	float _mouseSpeed;

	bool _viewDirty;
};