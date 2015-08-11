#pragma once

#include "Shader.h"

//由于这里的uniform比较多  所以没有实用string为每个uniform变量命名

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
	}_dirLightUniformLocation;//方向光Uniform

	GLuint _eyeWorldPos;
	GLuint _specularIntensity;
	GLuint _specularPower;
};