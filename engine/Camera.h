#pragma once

#include "Node.h"
#include "MathH.h"

class Camera : public Node
{
public:
	Camera();

	//--------------
	//projection transform
	//--------------
	void Perspective(float fieldOfView, float aspectRatio, float nearPlane, float farPlane);

	void Orthographic(float width, float height, float nearPlane, float farPlane);

	const mat4& GetProjectTransform();

	//--------------
	//view transform
	//--------------
	void LookAt(const vec3& lookAtPos, const vec3& up = vec3(0,1,0));

	const mat4& GetViewTransform();

protected:
	void UpdateViewTransform();

public:
	mat4 _projectTransform;
	mat4 _viewTransform;
};