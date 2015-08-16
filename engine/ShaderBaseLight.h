#pragma once

#include "Shader.h"

const int kMaxPointLightNum = 2;
const int kMaxSpotLightNum = 2;

//由于这里的uniform比较多  所以没有实用string为每个uniform变量命名

class ShaderBaseLight : public Shader
{
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