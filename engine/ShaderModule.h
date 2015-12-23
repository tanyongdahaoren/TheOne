#pragma once

#include "Mesh.h"
#include "MathH.h"
#include "Defines.h"
#include <stdarg.h>
#include <stdio.h>
#include "ShaderName.h"
#include "ShaderValue.h"
using namespace std;

//all module names
static const string module_base_light = "module_base_light";
static const string module_skeleton = "module_skeleton";
static const string module_shadow_map = "module_shadow_map";
static const string module_sky_box = "module_sky_box";

struct ShaderInfo
{
	ShaderInfo(string vert, string frag, string module, ...);
	vector<string> _modules;
	string _vert;
	string _frag;
};

class Camera;
class ShaderModule : public Ref
{
public:
	virtual string VertexStr() = 0;

	virtual string FragStr() = 0;

	virtual void InitUniformsLocation(GLuint programID){}

	virtual void Use(Mesh* mesh, mat4 toWorldTransform, Camera* camera){}
};


const int kMaxPointLightNum = 2;
const int kMaxSpotLightNum = 2;
class BaseLightShaderModule : public ShaderModule
{
public:
	virtual string VertexStr();

	virtual string FragStr();

	virtual void InitUniformsLocation(GLuint programID);

	virtual void Use(Mesh* mesh, mat4 toWorldTransform, Camera* camera);
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

	GLuint _openNormalMapLocation;
};

class ShadowMapShaderModule : public ShaderModule
{
public:
	virtual string VertexStr();

	virtual string FragStr();

	virtual void InitUniformsLocation(GLuint programID);

	virtual void Use(Mesh* mesh, mat4 toWorldTransform, Camera* camera);
protected:
	GLuint _openShadowLocation;
	GLuint _lightMVPLocation;
	GLuint _shadowmapSamplerLocation;
};

const int kMaxBoneNum = 100;
class SkeletonShaderModule : public ShaderModule
{
public:
	virtual string VertexStr();

	virtual string FragStr();
	
	virtual void InitUniformsLocation(GLuint programID);

	virtual void Use(Mesh* mesh, mat4 toWorldTransform, Camera* camera);
protected:
	GLuint _openSkelonLocation;
	GLuint _boneLocation[kMaxBoneNum];
};

class SkyBoxShaderModule : public ShaderModule
{
public:
	virtual string VertexStr();

	virtual string FragStr();

	virtual void InitUniformsLocation(GLuint programID);

	virtual void Use(Mesh* mesh, mat4 toWorldTransform, Camera* camera);
protected:
	GLuint _cameraRotationLocation;
	GLuint _cubeSamplerLocation;
};


typedef std::map<string, ShaderInfo> ShaderInfoMap; //key:ShaderName value:ShaderInfo
typedef ShaderInfoMap::value_type ShaderInfoMapValue;
static const ShaderInfoMapValue ShaderInfoMapTableValue[] =
{
	ShaderInfoMapValue(shader_position_color,   ShaderInfo(PositionColor_vert, PositionColor_frag, string(""))),
	ShaderInfoMapValue(shader_position_texture, ShaderInfo(PositionTexture_vert, PositionTexture_frag, module_skeleton, string(""))),
	ShaderInfoMapValue(shader_depth_rtt,        ShaderInfo(DepthRTT_vert, DepthRTT_frag, module_skeleton, string(""))),
	ShaderInfoMapValue(shader_base_light,       ShaderInfo(BaseLight_vert, BaseLight_frag, module_base_light, module_shadow_map, module_skeleton, string(""))),
	ShaderInfoMapValue(shader_sky_box,          ShaderInfo(SkyBox_vert, SkyBox_frag, module_sky_box, string("")))
};
const ShaderInfoMap ShaderInfoMapTable(ShaderInfoMapTableValue,
	ShaderInfoMapTableValue + ARRAY_SIZE(ShaderInfoMapTableValue));

typedef std::map<string, std::function<ShaderModule*()> > ShaderModuleMap; //key:ModuleName value:new module
typedef ShaderModuleMap::value_type ShaderModuleMapValue;
static const ShaderModuleMapValue ShaderModuleMapTableValue[] =
{
	ShaderModuleMapValue(module_base_light, [](){return new BaseLightShaderModule(); }),
	ShaderModuleMapValue(module_shadow_map, [](){return new ShadowMapShaderModule(); }),
	ShaderModuleMapValue(module_skeleton,   [](){return new SkeletonShaderModule(); }),
	ShaderModuleMapValue(module_sky_box,    [](){return new SkyBoxShaderModule(); }),
};
const ShaderModuleMap ShaderModuleMapTable(ShaderModuleMapTableValue,
	ShaderModuleMapTableValue + ARRAY_SIZE(ShaderModuleMapTableValue));