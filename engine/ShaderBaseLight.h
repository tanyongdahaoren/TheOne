#pragma once

#include "Shader.h"
#include<vector>
using namespace std;

const int kMaxPointLightNum = 2;
const int kMaxSpotLightNum = 2;
const int kMaxBoneNum = 100;

//���������uniform�Ƚ϶�  ����û��ʵ��stringΪÿ��uniform��������

class ShaderBaseLight : public Shader
{
public:
	static string GetVertShader();
	static string GetFragShader();

	static string GetNormalVertShader();
	static string GetNormalFragShader();
protected:
	virtual void InitUniformsLocation();
	virtual void CustomEffect();
protected:
	//base
	struct tBaseLightLocation
	{
		GLuint color;
		GLuint ambientIntensity;
		GLuint diffuseIntensity;
	};

	//direction light
	struct tDirectionLightLocation : public tBaseLightLocation
	{
		GLuint direction;
	} _dirLightUniformLocation;

	//point light
	GLuint _pointLightsNumLocation;
	struct tPointLightLocation : public tBaseLightLocation
	{
		GLuint worldPos;
		GLuint constant;
		GLuint linear;
		GLuint exp;
	} _pointLightsLocation[kMaxPointLightNum];

	//spot light
	GLuint _spotLightsNumLocation;
	struct tSpotLightLocation : public tPointLightLocation
	{
		GLuint direction;
		GLuint cutoff;
	} _spotLightsLocation[kMaxSpotLightNum];

	GLuint _eyeWorldPos;
	GLuint _specularIntensity;
	GLuint _specularPower;
};

class ShaderBaseLightSkelon :public ShaderBaseLight
{
public:
	static string GetVertShader();
	static string GetFragShader();
	void SetBonesTransform(vector<mat4> transforms);
protected:
	virtual void InitUniformsLocation();
	virtual void CustomEffect();
protected:
	GLuint _boneLocation[kMaxBoneNum];
};