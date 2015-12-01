#pragma once

#include "Camera.h"
#include "Types.h"
#include "DrawNode.h"
#include "RenderToTexture.h"
#define DEBUG_LIGHT 1

class BaseLight : public Camera
{
	friend class Tree;
public:
	BaseLight();
	~BaseLight();

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

	void OpenShadow(bool b);
	bool IsOpenShadow(){ return _openShadow; }

	Texture2D* GetTexture();
	const mat4& GetVP(){ return _VP; }
protected:
	virtual void CaculateVP(){}

	void BindRenderShadow();
	void UnBindRenderShadow();
protected:
	Color3F _color;
	float _ambientIntensity;
	float _diffuseIntensity;
	static float _specularIntensity;
	static float _specularPower;
	
	//for shadow
	bool _openShadow;
	RenderToTexture* _shadowRTT;
	mat4 _VP;
};

class DirectionLight : public BaseLight
{
	friend class Tree;
public:
	DirectionLight();

	void SetDirection(vec3 direction);
	vec3 GetDirection(){ return _direction; }

protected:
	virtual void CaculateVP();
protected:
	vec3 _direction;
};

class PointLight : public BaseLight
{
public:
	PointLight();

	void SetConstant(float val){ _constant = val; }
	float GetConstant(){ return _constant; }

	void SetLinear(float val){ _linear = val; }
	float GetLinear(){ return _linear; }

	void SetExp(float val){ _exp = val; }
	float GetExp(){ return _exp; }
protected:
	float _constant;
	float _linear;
	float _exp;
#if DEBUG_LIGHT
	DrawPoints* _dp;
#endif
};

class SpotLight : public PointLight
{
public:
	SpotLight();

	void SetDirection(vec3 direction);
	vec3 GetDirection(){ return _direction; }

	void SetCutoff(float val);
	float GetCutoff(){ return _cutoff; }
protected:
	vec3 _direction;
	float _cutoff;
#if DEBUG_LIGHT
	DrawLines* _dl;
#endif
};