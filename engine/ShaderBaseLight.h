#pragma once

#include "Shader.h"

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
		GLuint direction;
	}_dirLightUniformLocation;//�����Uniform

	GLuint _eyeWorldPos;
	GLuint _specularIntensity;
	GLuint _specularPower;
};