#include "ShaderBaseLight.h"
#include "Director.h"
#include "Tree.h"

void ShaderBaseLight::InitUniformsLocation()
{
	Shader::InitUniformsLocation();

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
	vec3 eyePos = camera->GetEyePos();
	glUniform3f(_eyeWorldPos, eyePos.x, eyePos.y, eyePos.z);

	DirectionLight* dirLight = Director::GetInstance()->GetCurrentTree()->_directionLight;
	if (dirLight)
	{
		glUniform3f(_dirLightUniformLocation.color,
			dirLight->GetColor().r, dirLight->GetColor().g, dirLight->GetColor().b);

		glUniform1f(_dirLightUniformLocation.ambientIntensity,
			dirLight->GetAmbientIntensity());

		glUniform1f(_dirLightUniformLocation.diffuseIntensity,
			dirLight->GetDiffuseIntensity());

		vec3 direction = dirLight->GetDirection();
		direction = glm::normalize(direction);
		glUniform3f(_dirLightUniformLocation.direction,
			direction.x, direction.y, direction.z);
	}

	vector<PointLight*>& pointLights = Director::GetInstance()->GetCurrentTree()->_pointLights;
	for (auto it:pointLights)
	{

	}
}

