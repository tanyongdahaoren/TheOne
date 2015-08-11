#pragma once

#include "Node.h"
#include "Types.h"

class BaseLight : public Node
{
public:
	BaseLight();

	static void SetSpecularPower(float power);
	static float GetSpecularPower();

	static void SetSpecularIntensity(float intensity);
	static float GetSpecularIntensity();

	void SetColor(Color3F color);
	Color3F GetColor(){ return _color; }

	void SetAmbientIntensity(float ambientIntensity);
	float GetAmbientIntensity(){ return _ambientIntensity; }

	void SetDiffuseIntensity(float diffuseIntensity);
	float GetDiffuseIntensity(){ return _diffuseIntensity; }
protected:
	Color3F _color;
	float _ambientIntensity;
	float _diffuseIntensity;
	static float _specularIntensity;
	static float _specularPower;
};

class DirectionLight : public BaseLight
{
public:
	DirectionLight();

	void SetDirection(vec3 direction);
	vec3 GetDirection(){ return _direction; }
protected:
	vec3 _direction;
};