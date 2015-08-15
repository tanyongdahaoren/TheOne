#pragma once

#include "Shader.h"

const int kMaxPointLightNum = 2;

//���������uniform�Ƚ϶�  ����û��ʵ��stringΪÿ��uniform��������

class ShaderBaseLight : public Shader
{
protected:
	virtual void CustomEffect();
	virtual void InitUniformsLocation();
protected:
	struct {
		GLuint color;
		GLuint ambientIntensity;
		GLuint diffuseIntensity;
		//------special
		GLuint direction;
	}_dirLightUniformLocation;//�����Uniform

	GLuint _pointLightsNumLocation;
	struct {
		GLuint color;
		GLuint ambientIntensity;
		GLuint diffuseIntensity;
		//------special
		GLuint worldPos;
		GLuint constant;
		GLuint linear;
		GLuint exp;
	}_pointLightsLocation[kMaxPointLightNum];

	GLuint _eyeWorldPos;
	GLuint _specularIntensity;
	GLuint _specularPower;
};