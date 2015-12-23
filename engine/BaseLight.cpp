#include "BaseLight.h"
#include "MathH.h"
#include "Defines.h"

//-------------------
//BaseLight
//-------------------
float BaseLight::_specularIntensity = 0;
float BaseLight::_specularPower = 0;

BaseLight::BaseLight()
	:_shadowRTT(NULL)
{
	_color = Color3F::WHITE;
	_ambientIntensity = 0.0f;
	_diffuseIntensity = 0.0f;
	_openShadow = false;
}

BaseLight::~BaseLight()
{
	SAFE_DELETE(_shadowRTT);
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

void BaseLight::OpenShadow(bool b)
{
	if (_openShadow == b)
	{
		return;
	}
	_openShadow = b;

	if (_openShadow)
	{
		_shadowRTT = new RenderToTexture;
		_shadowRTT->InitBuffer(sWinW, sWinH, PixelFormat::depth);
	}
	else
	{
		SAFE_DELETE(_shadowRTT);
	}
}

void BaseLight::BindRenderShadow()
{
	_shadowRTT->Bind();
}

void BaseLight::UnBindRenderShadow()
{
	_shadowRTT->UnBind();
}

Texture2D* BaseLight::GetTexture()
{
	return _shadowRTT->_texture;
}

//-------------------
//DirectionLight
//-------------------
DirectionLight::DirectionLight()
{

}

void DirectionLight::SetDirection(vec3 direction)
{
	SetPosition(-direction);
	LookAt(vec3(0,0,0));
	_direction = direction;
}

void DirectionLight::CaculateVP()
{
	Orthographic(20, 20, -10, 30);
	UpdateViewTransform();
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

//-------------------
//SpotLight
//-------------------
SpotLight::SpotLight()
{
#if DEBUG_LIGHT
	_dl = new DrawLines;
	this->AddChild(_dl);
	_dl->SetLineWidth(5);
#endif
}

void SpotLight::SetDirection(vec3 direction)
{
	_direction = normalize(direction);
	_dl->Clear();
	_dl->DrawLine(vec3(0, 0, 0), _direction, Color3B::YELLOW, Color3B::GREEN);
}

void SpotLight::SetCutoff(float val)
{
	_cutoff = val;
}
