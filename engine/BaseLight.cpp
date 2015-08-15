#include "BaseLight.h"
//-------------------
//BaseLight
//-------------------
float BaseLight::_specularIntensity = 0;
float BaseLight::_specularPower = 0;

BaseLight::BaseLight()
{
	_color = Color3F::WHITE;
	_ambientIntensity = 0.0f;
	_diffuseIntensity = 0.0f;
}

void BaseLight::SetColor(Color3F color)
{
	_color = color;
}

void BaseLight::SetAmbientIntensity(float ambientIntensity)
{
	_ambientIntensity = ambientIntensity;
}

void BaseLight::SetDiffuseIntensity(float diffuseIntensity)
{
	_diffuseIntensity = diffuseIntensity;
}

void BaseLight::SetSpecularPower(float power)
{
	_specularPower = power;
}

void BaseLight::SetSpecularIntensity(float intensity)
{
	_specularIntensity = intensity;
}

float BaseLight::GetSpecularPower()
{
	return _specularPower;
}

float BaseLight::GetSpecularIntensity()
{
	return _specularIntensity;
}

//-------------------
//DirectionLight
//-------------------
DirectionLight::DirectionLight()
{

}

void DirectionLight::SetDirection(vec3 direction)
{
	_direction = direction;
}

//-------------------
//PointLight
//-------------------
PointLight::PointLight()
{
	_constant = 1.0f;
	_linear = 0.0f;
	_exp = 0.0f;

#if DEBUG_LIGHT
	_dp = new DrawPoints;
	this->AddChild(_dp);
	_dp->SetPointSize(10);
	_dp->DrawPoint(vec3(0, 0, 0), Color3B::YELLOW);
#endif
}
