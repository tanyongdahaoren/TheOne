#include "ShaderModule.h"
#include "Shader.h"

#include "Camera.h"
#include "Tree.h"
#include "Director.h"
#include "BaseLight.h"

ShaderInfo::ShaderInfo(string vert, string frag, string module, ...)
{
	_vert = vert;
	_frag = frag;

	va_list args;
	va_start(args, module);

	if (module != "")
	{
		_modules.push_back(module);
		string i = va_arg(args, string);
		while (i != "")
		{
			_modules.push_back(i);
			i = va_arg(args, string);
		}
	}

	va_end(args);
}






std::string BaseLightShaderModule::VertexStr()
{
	GLchar buff[256];
	SNPRINTF(buff, sizeof(buff) - 1,
		"\nuniform int u_open_normal_map;\n"
		);
	return buff;
}

std::string BaseLightShaderModule::FragStr()
{
	GLchar buff[256];
	SNPRINTF(buff, sizeof(buff) - 1,
		"uniform int u_open_normal_map; \n"
		"const int kMaxPointLightNum =  %d; \n"
		"const int kMaxSpotLightNum = %d; \n",
		kMaxPointLightNum,
		kMaxSpotLightNum);
	return buff;
}

void BaseLightShaderModule::InitUniformsLocation(GLuint programID)
{
	//direction light
	_dirLightUniformLocation.color = GLGetUniformLocation(programID, "u_direction_light.base.color");

	_dirLightUniformLocation.ambientIntensity = GLGetUniformLocation(programID, "u_direction_light.base.ambientIntensity");

	_dirLightUniformLocation.diffuseIntensity = GLGetUniformLocation(programID, "u_direction_light.base.diffuseIntensity");

	_dirLightUniformLocation.direction = GLGetUniformLocation(programID, "u_direction_light.direction");


	//point lights
	_pointLightsNumLocation = GLGetUniformLocation(programID, "u_point_light_num");

	for (unsigned int i = 0; i < kMaxPointLightNum; i++)
	{
		char Name[128];
		memset(Name, 0, sizeof(Name));

		SNPRINTF(Name, sizeof(Name), "u_point_lights[%d].base.color", i);
		_pointLightsLocation[i].color = GLGetUniformLocation(programID, Name);

		SNPRINTF(Name, sizeof(Name), "u_point_lights[%d].base.ambientIntensity", i);
		_pointLightsLocation[i].ambientIntensity = GLGetUniformLocation(programID, Name);

		SNPRINTF(Name, sizeof(Name), "u_point_lights[%d].base.diffuseIntensity", i);
		_pointLightsLocation[i].diffuseIntensity = GLGetUniformLocation(programID, Name);

		SNPRINTF(Name, sizeof(Name), "u_point_lights[%d].world_pos", i);
		_pointLightsLocation[i].worldPos = GLGetUniformLocation(programID, Name);

		SNPRINTF(Name, sizeof(Name), "u_point_lights[%d].constant", i);
		_pointLightsLocation[i].constant = GLGetUniformLocation(programID, Name);

		SNPRINTF(Name, sizeof(Name), "u_point_lights[%d].linear", i);
		_pointLightsLocation[i].linear = GLGetUniformLocation(programID, Name);

		SNPRINTF(Name, sizeof(Name), "u_point_lights[%d].exp", i);
		_pointLightsLocation[i].exp = GLGetUniformLocation(programID, Name);
	}


	//spot lights
	_spotLightsNumLocation = GLGetUniformLocation(programID, "u_spot_light_num");

	for (unsigned int i = 0; i < kMaxSpotLightNum; i++)
	{
		char Name[128];
		memset(Name, 0, sizeof(Name));

		SNPRINTF(Name, sizeof(Name), "u_spot_light[%d].base.base.color", i);
		_spotLightsLocation[i].color = GLGetUniformLocation(programID, Name);

		SNPRINTF(Name, sizeof(Name), "u_spot_light[%d].base.base.ambientIntensity", i);
		_spotLightsLocation[i].ambientIntensity = GLGetUniformLocation(programID, Name);

		SNPRINTF(Name, sizeof(Name), "u_spot_light[%d].base.base.diffuseIntensity", i);
		_spotLightsLocation[i].diffuseIntensity = GLGetUniformLocation(programID, Name);

		SNPRINTF(Name, sizeof(Name), "u_spot_light[%d].base.world_pos", i);
		_spotLightsLocation[i].worldPos = GLGetUniformLocation(programID, Name);

		SNPRINTF(Name, sizeof(Name), "u_spot_light[%d].base.constant", i);
		_spotLightsLocation[i].constant = GLGetUniformLocation(programID, Name);

		SNPRINTF(Name, sizeof(Name), "u_spot_light[%d].base.linear", i);
		_spotLightsLocation[i].linear = GLGetUniformLocation(programID, Name);

		SNPRINTF(Name, sizeof(Name), "u_spot_light[%d].base.exp", i);
		_spotLightsLocation[i].exp = GLGetUniformLocation(programID, Name);

		SNPRINTF(Name, sizeof(Name), "u_spot_light[%d].direction", i);
		_spotLightsLocation[i].direction = GLGetUniformLocation(programID, Name);

		SNPRINTF(Name, sizeof(Name), "u_spot_light[%d].cutoff", i);
		_spotLightsLocation[i].cutoff = GLGetUniformLocation(programID, Name);
	}

	//others
	_eyeWorldPosLocation = GLGetUniformLocation(programID, "u_world_eyepos");

	_specularIntensityLocation = GLGetUniformLocation(programID, "u_specular_intensity");

	_specularPowerLocation = GLGetUniformLocation(programID, "u_specular_power");

	_openNormalMapLocation = GLGetUniformLocation(programID, "u_open_shadow");
}

void BaseLightShaderModule::Use(Mesh* mesh, mat4 toWorldTransform)
{
	float specularPower = BaseLight::GetSpecularPower();
	float specularIntensity = BaseLight::GetSpecularIntensity();

	glUniform1f(_specularPowerLocation, specularPower);
	glUniform1f(_specularIntensityLocation, specularIntensity);

	Camera* camera = Director::GetInstance()->GetCurrentTree()->GetCurrentCamera();
	vec3 eyePos = camera->GetPositionInWorld();
	glUniform3f(_eyeWorldPosLocation, eyePos.x, eyePos.y, eyePos.z);

	DirectionLight* dirLight = Director::GetInstance()->GetCurrentTree()->_directionLight;
	if (dirLight)
	{
		glUniform3f(_dirLightUniformLocation.color, dirLight->GetColor().r, dirLight->GetColor().g, dirLight->GetColor().b);

		glUniform1f(_dirLightUniformLocation.ambientIntensity, dirLight->GetAmbientIntensity());

		glUniform1f(_dirLightUniformLocation.diffuseIntensity, dirLight->GetDiffuseIntensity());

		vec3 direction = dirLight->GetDirection();
		direction = glm::normalize(direction);
		glUniform3f(_dirLightUniformLocation.direction, direction.x, direction.y, direction.z);
	}

	vector<PointLight*>& pointLights = Director::GetInstance()->GetCurrentTree()->_pointLights;
	glUniform1i(_pointLightsNumLocation, pointLights.size());
	for (int i = 0; i < pointLights.size(); i++)
	{
		PointLight* light = pointLights[i];
		tPointLightLocation& loc = _pointLightsLocation[i];

		glUniform3f(loc.color, light->GetColor().r, light->GetColor().g, light->GetColor().b);

		glUniform1f(loc.ambientIntensity, light->GetAmbientIntensity());

		glUniform1f(loc.diffuseIntensity, light->GetDiffuseIntensity());

		vec3 lightWorldPos = light->GetPositionInWorld();
		glUniform3f(loc.worldPos, lightWorldPos.x, lightWorldPos.y, lightWorldPos.z);

		glUniform1f(loc.constant, light->GetConstant());

		glUniform1f(loc.linear, light->GetLinear());

		glUniform1f(loc.exp, light->GetExp());
	}


	vector<SpotLight*>& spotLights = Director::GetInstance()->GetCurrentTree()->_spotLights;
	glUniform1i(_spotLightsNumLocation, spotLights.size());
	for (int i = 0; i < spotLights.size(); i++)
	{
		SpotLight* light = spotLights[i];
		tSpotLightLocation& loc = _spotLightsLocation[i];

		glUniform3f(loc.color, light->GetColor().r, light->GetColor().g, light->GetColor().b);

		glUniform1f(loc.ambientIntensity, light->GetAmbientIntensity());

		glUniform1f(loc.diffuseIntensity, light->GetDiffuseIntensity());

		vec3 lightWorldPos = light->GetPositionInWorld();
		glUniform3f(loc.worldPos, lightWorldPos.x, lightWorldPos.y, lightWorldPos.z);

		glUniform1f(loc.constant, light->GetConstant());

		glUniform1f(loc.linear, light->GetLinear());

		glUniform1f(loc.exp, light->GetExp());

		vec3 direction = light->GetDirection();
		direction = glm::normalize(direction);
		glUniform3f(loc.direction, direction.x, direction.y, direction.z);

		glUniform1f(loc.cutoff, light->GetCutoff());
	}

	bool normalMap = false;
	if (mesh->HaveAttribute(eShaderVertAttribute_tangent))
		normalMap = true;
	glUniform1i(_openNormalMapLocation, normalMap ? 1 : 0);
}





std::string ShadowMapShaderModule::VertexStr()
{
	char buff[256];
	SNPRINTF(buff, sizeof(buff) - 1,
		"\nuniform int u_open_shadow; \n"//是否开启阴影 0为没开 1为开
		"uniform mat4 u_light_MVP; \n"//将顶点变换到光源为摄像机的位置
		"out vec4 o_pos_light_camera; \n"//光源为摄像机下的位置
		);
	return buff;
}

std::string ShadowMapShaderModule::FragStr()
{
	char buff[256];
	SNPRINTF(buff, sizeof(buff) - 1,
		"\nuniform int u_open_shadow; \n"//是否开启阴影 0为没开 1为开
		"uniform sampler2D u_sampler_shadowmap; \n"//将顶点变换到光源为摄像机的位置
		"in vec4 o_pos_light_camera; \n"//光源为摄像机下的位置
		);
	return buff;
}

void ShadowMapShaderModule::InitUniformsLocation(GLuint programID)
{
	_openShadowLocation = GLGetUniformLocation(programID, "u_open_shadow");

	_lightMVPLocation = GLGetUniformLocation(programID, "u_light_MVP");

	_shadowmapSamplerLocation = GLGetUniformLocation(programID, "u_sampler_shadowmap");
}

void ShadowMapShaderModule::Use(Mesh* mesh, mat4 toWorldTransform)
{
	DirectionLight* dirLight = Director::GetInstance()->GetCurrentTree()->_directionLight;

	bool isOpenShadow = dirLight->IsOpenShadow();
	glUniform1i(_openShadowLocation, isOpenShadow ? 1 : 0);
	if (isOpenShadow)
	{
		glm::mat4 biasMatrix(
			0.5, 0.0, 0.0, 0.0,
			0.0, 0.5, 0.0, 0.0,
			0.0, 0.0, 0.5, 0.0,
			0.5, 0.5, 0.5, 1.0
			);
		mat4 vp = biasMatrix * dirLight->GetVP() * toWorldTransform;
		glUniformMatrix4fv(_lightMVPLocation, 1, GL_FALSE, &vp[0][0]);

		glUniform1i(_shadowmapSamplerLocation, SHADOW_MAP_TEXTURE_DIRECTION_LIGHT_INDEX);
		dirLight->GetTexture()->Bind(SHADOW_MAP_TEXTURE_DIRECTION_LIGHT);
	}
}





std::string SkeletonShaderModule::VertexStr()
{
	char buff[256];
	SNPRINTF(buff, sizeof(buff) - 1,
		"\nconst int kMaxBoneNum =  %d; \n"
		"uniform int u_open_skelon; \n"
		"uniform mat4 u_bone[kMaxBoneNum]; \n"
		"in vec4 a_blend_index; \n"
		"in vec4 a_blend_weight; \n",
		kMaxBoneNum);
	return buff;
}

std::string SkeletonShaderModule::FragStr()
{
	return "";
}

void SkeletonShaderModule::InitUniformsLocation(GLuint programID)
{
	//skelon
	_openSkelonLocation = GLGetUniformLocation(programID, "u_open_skelon");
	for (int i = 0; i < kMaxBoneNum; i++)
	{
		char Name[128];
		memset(Name, 0, sizeof(Name));
		SNPRINTF(Name, sizeof(Name), "u_bone[%d]", i);
		_boneLocation[i] = GLGetUniformLocation(programID, Name);
	}
}

void SkeletonShaderModule::Use(Mesh* mesh, mat4 toWorldTransform)
{
	//for skelon
	glUniform1i(_openSkelonLocation, mesh->_skelon ? 1 : 0);
	for (int i = 0; i < mesh->_boneNum; i++)
	{
		const mat4& transform = mesh->_bonesInfo[i].FinalTransformation;
		glUniformMatrix4fv(_boneLocation[i], 1, GL_FALSE, (const GLfloat*)&transform[0][0]);
	}
}






