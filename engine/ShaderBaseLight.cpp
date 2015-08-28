#include "ShaderBaseLight.h"
#include "Director.h"
#include "Tree.h"
#include "Camera.h"

void ShaderBaseLight::InitUniformsLocation()
{
	Shader::InitUniformsLocation();

	//direction light
	_dirLightUniformLocation.color = GetUniformLocation("u_direction_light.base.color");

	_dirLightUniformLocation.ambientIntensity = GetUniformLocation("u_direction_light.base.ambientIntensity");

	_dirLightUniformLocation.diffuseIntensity = GetUniformLocation("u_direction_light.base.diffuseIntensity");

	_dirLightUniformLocation.direction = GetUniformLocation("u_direction_light.direction");

	
	//point lights
	_pointLightsNumLocation = GetUniformLocation("u_point_light_num");

	for (unsigned int i = 0; i < kMaxPointLightNum; i++)
	{
		char Name[128];
		memset(Name, 0, sizeof(Name));

		sprintf(Name, "u_point_lights[%d].base.color", i);
		_pointLightsLocation[i].color = GetUniformLocation(Name);

		sprintf(Name, "u_point_lights[%d].base.ambientIntensity", i);
		_pointLightsLocation[i].ambientIntensity = GetUniformLocation(Name);

		sprintf(Name, "u_point_lights[%d].base.diffuseIntensity", i);
		_pointLightsLocation[i].diffuseIntensity = GetUniformLocation(Name);

		sprintf(Name, "u_point_lights[%d].world_pos", i);
		_pointLightsLocation[i].worldPos = GetUniformLocation(Name);

		sprintf(Name, "u_point_lights[%d].constant", i);
		_pointLightsLocation[i].constant = GetUniformLocation(Name);

		sprintf(Name, "u_point_lights[%d].linear", i);
		_pointLightsLocation[i].linear = GetUniformLocation(Name);

		sprintf(Name, "u_point_lights[%d].exp", i);
		_pointLightsLocation[i].exp = GetUniformLocation(Name);
	}


	//spot lights
	_spotLightsNumLocation = GetUniformLocation("u_spot_light_num");

	for (unsigned int i = 0; i < kMaxSpotLightNum; i++)
	{
		char Name[128];
		memset(Name, 0, sizeof(Name));

		sprintf(Name, "u_spot_light[%d].base.base.color", i);
		_spotLightsLocation[i].color = GetUniformLocation(Name);

		sprintf(Name, "u_spot_light[%d].base.base.ambientIntensity", i);
		_spotLightsLocation[i].ambientIntensity = GetUniformLocation(Name);

		sprintf(Name, "u_spot_light[%d].base.base.diffuseIntensity", i);
		_spotLightsLocation[i].diffuseIntensity = GetUniformLocation(Name);

		sprintf(Name, "u_spot_light[%d].base.world_pos", i);
		_spotLightsLocation[i].worldPos = GetUniformLocation(Name);

		sprintf(Name, "u_spot_light[%d].base.constant", i);
		_spotLightsLocation[i].constant = GetUniformLocation(Name);

		sprintf(Name, "u_spot_light[%d].base.linear", i);
		_spotLightsLocation[i].linear = GetUniformLocation(Name);

		sprintf(Name, "u_spot_light[%d].base.exp", i);
		_spotLightsLocation[i].exp = GetUniformLocation(Name);

		sprintf(Name, "u_spot_light[%d].direction", i);
		_spotLightsLocation[i].direction = GetUniformLocation(Name);

		sprintf(Name, "u_spot_light[%d].cutoff", i);
		_spotLightsLocation[i].cutoff = GetUniformLocation(Name);
	}


	//others
	_eyeWorldPos = GetUniformLocation("u_world_eyepos");

	_specularIntensity = GetUniformLocation("u_specular_intensity");

	_specularPower = GetUniformLocation("u_specular_power");
}

void ShaderBaseLight::CustomEffect()
{
	float specularPower = BaseLight::GetSpecularPower();
	float specularIntensity = BaseLight::GetSpecularIntensity();

	glUniform1f(_specularPower, specularPower);
	glUniform1f(_specularIntensity, specularIntensity);

	Camera* camera = Director::GetInstance()->GetCurrentTree()->GetCurrentCamera();
	vec3 eyePos = camera->GetPositionInWorld();
	glUniform3f(_eyeWorldPos, eyePos.x, eyePos.y, eyePos.z);

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
}

