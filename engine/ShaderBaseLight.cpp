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
}

