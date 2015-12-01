#include "ShaderBaseLight.h"
#include "Director.h"
#include "Tree.h"
#include "Camera.h"
#include "ShaderName.h"
#include "Defines.h"
#include "Mesh.h"
void ShaderBaseLight::InitUniformsLocation()
{
	Shader::InitUniformsLocation();

	//direction light
	_dirLightUniformLocation.color = GLGetUniformLocation(_programID, "u_direction_light.base.color");

	_dirLightUniformLocation.ambientIntensity = GLGetUniformLocation(_programID, "u_direction_light.base.ambientIntensity");

	_dirLightUniformLocation.diffuseIntensity = GLGetUniformLocation(_programID, "u_direction_light.base.diffuseIntensity");

	_dirLightUniformLocation.direction = GLGetUniformLocation(_programID, "u_direction_light.direction");

	
	//point lights
	_pointLightsNumLocation = GLGetUniformLocation(_programID, "u_point_light_num");

	for (unsigned int i = 0; i < kMaxPointLightNum; i++)
	{
		char Name[128];
		memset(Name, 0, sizeof(Name));

		SNPRINTF(Name, sizeof(Name), "u_point_lights[%d].base.color", i);
		_pointLightsLocation[i].color = GLGetUniformLocation(_programID, Name);

		SNPRINTF(Name, sizeof(Name), "u_point_lights[%d].base.ambientIntensity", i);
		_pointLightsLocation[i].ambientIntensity = GLGetUniformLocation(_programID, Name);

		SNPRINTF(Name, sizeof(Name), "u_point_lights[%d].base.diffuseIntensity", i);
		_pointLightsLocation[i].diffuseIntensity = GLGetUniformLocation(_programID, Name);

		SNPRINTF(Name, sizeof(Name), "u_point_lights[%d].world_pos", i);
		_pointLightsLocation[i].worldPos = GLGetUniformLocation(_programID, Name);

		SNPRINTF(Name, sizeof(Name), "u_point_lights[%d].constant", i);
		_pointLightsLocation[i].constant = GLGetUniformLocation(_programID, Name);

		SNPRINTF(Name, sizeof(Name), "u_point_lights[%d].linear", i);
		_pointLightsLocation[i].linear = GLGetUniformLocation(_programID, Name);

		SNPRINTF(Name, sizeof(Name), "u_point_lights[%d].exp", i);
		_pointLightsLocation[i].exp = GLGetUniformLocation(_programID, Name);
	}


	//spot lights
	_spotLightsNumLocation = GLGetUniformLocation(_programID, "u_spot_light_num");

	for (unsigned int i = 0; i < kMaxSpotLightNum; i++)
	{
		char Name[128];
		memset(Name, 0, sizeof(Name));

		SNPRINTF(Name, sizeof(Name), "u_spot_light[%d].base.base.color", i);
		_spotLightsLocation[i].color = GLGetUniformLocation(_programID, Name);

		SNPRINTF(Name, sizeof(Name), "u_spot_light[%d].base.base.ambientIntensity", i);
		_spotLightsLocation[i].ambientIntensity = GLGetUniformLocation(_programID, Name);

		SNPRINTF(Name, sizeof(Name), "u_spot_light[%d].base.base.diffuseIntensity", i);
		_spotLightsLocation[i].diffuseIntensity = GLGetUniformLocation(_programID, Name);

		SNPRINTF(Name, sizeof(Name), "u_spot_light[%d].base.world_pos", i);
		_spotLightsLocation[i].worldPos = GLGetUniformLocation(_programID, Name);

		SNPRINTF(Name, sizeof(Name), "u_spot_light[%d].base.constant", i);
		_spotLightsLocation[i].constant = GLGetUniformLocation(_programID, Name);

		SNPRINTF(Name, sizeof(Name), "u_spot_light[%d].base.linear", i);
		_spotLightsLocation[i].linear = GLGetUniformLocation(_programID, Name);

		SNPRINTF(Name, sizeof(Name), "u_spot_light[%d].base.exp", i);
		_spotLightsLocation[i].exp = GLGetUniformLocation(_programID, Name);

		SNPRINTF(Name, sizeof(Name), "u_spot_light[%d].direction", i);
		_spotLightsLocation[i].direction = GLGetUniformLocation(_programID, Name);

		SNPRINTF(Name, sizeof(Name), "u_spot_light[%d].cutoff", i);
		_spotLightsLocation[i].cutoff = GLGetUniformLocation(_programID, Name);
	}

	//others
	_eyeWorldPosLocation = GLGetUniformLocation(_programID, "u_world_eyepos");

	_specularIntensityLocation = GLGetUniformLocation(_programID, "u_specular_intensity");

	_specularPowerLocation = GLGetUniformLocation(_programID, "u_specular_power");

	//shadowmap
	_openShadowLocation = GLGetUniformLocation(_programID, "u_open_shadow");

	_lightMVPLocation = GLGetUniformLocation(_programID, "u_light_MVP");

	_shadowmapSamplerLocation = GLGetUniformLocation(_programID, "u_sampler_shadowmap");

	//skelon
	_openSkelonLocation = GLGetUniformLocation(_programID, "u_open_skelon");
	for (int i = 0; i < kMaxBoneNum; i++)
	{
		char Name[128];
		memset(Name, 0, sizeof(Name));
		SNPRINTF(Name, sizeof(Name), "u_bone[%d]", i);
		_boneLocation[i] = GLGetUniformLocation(_programID, Name);
	}
}

void ShaderBaseLight::CustomEffect(Mesh* mesh, mat4 toWorldTransform)
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

		//shadow
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

			glUniform1i(_shadowmapSamplerLocation, 20);
			dirLight->GetTexture()->Bind(GL_TEXTURE20);
		}
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

	//for skelon
	glUniform1i(_openSkelonLocation, mesh->_skelon ? 1 : 0);
	for (int i = 0; i < mesh->_boneNum; i++)
	{
		const mat4& transform = mesh->_bonesInfo[i].FinalTransformation;
		glUniformMatrix4fv(_boneLocation[i], 1, GL_FALSE, (const GLfloat*)&transform[0][0]);
	}
}

std::string ShaderBaseLight::GetVertShader()
{	
	GLchar buff[256];
	SNPRINTF(buff, sizeof(buff) - 1,
		"\n const int kMaxBoneNum =  %d; \n",
		kMaxBoneNum);
	string str = string(buff) + string(BaseLight_vert);
	return str;
}

std::string ShaderBaseLight::GetFragShader()
{
	GLchar buff[256];
	SNPRINTF(buff, sizeof(buff) - 1,
		"\nconst int kMaxPointLightNum =  %d; \n"
		"const int kMaxSpotLightNum = %d; \n",
		kMaxPointLightNum,
		kMaxSpotLightNum);
	string str = string(buff) + string(BaseLight_frag);
	return str;
}

std::string ShaderBaseLight::GetNormalFragShader()
{
	GLchar buff[256];
	SNPRINTF(buff, sizeof(buff) - 1,
		"\nconst int kMaxPointLightNum =  %d; \n"
		"\nconst int kMaxSpotLightNum = %d; \n",
		kMaxPointLightNum,
		kMaxSpotLightNum);
	string str = string(buff) + string(BaseLight_NormalMap_frag);
	return str;
}

std::string ShaderBaseLight::GetNormalVertShader()
{
	return string(BaseLight_NormalMap_vert);
}
