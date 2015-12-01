#pragma once

#include "Shader.h"
#include<vector>
using namespace std;

const int kMaxPointLightNum = 2;
const int kMaxSpotLightNum = 2;
const int kMaxBoneNum = 100;

//由于这里的uniform比较多  所以没有实用string为每个uniform变量命名
class Mesh;
class ShaderBaseLight : public Shader
{
public:
	static string GetVertShader();
	static string GetFragShader();

	static string GetNormalVertShader();
	static string GetNormalFragShader();

	void SetBonesTransform(Mesh* mesh);
protected:
	virtual void InitUniformsLocation();
	virtual void CustomEffect(Mesh* mesh, mat4 toWorldTransform);
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

	GLuint _eyeWorldPosLocation;
	GLuint _specularIntensityLocation;
	GLuint _specularPowerLocation;

	//for shadow
	GLuint _openShadowLocation;
	GLuint _lightMVPLocation;
	GLuint _shadowmapSamplerLocation;

	//for skelon
	GLuint _openSkelonLocation;
	GLuint _boneLocation[kMaxBoneNum];
};
