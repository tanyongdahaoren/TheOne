#pragma once

#include "ShaderManager.h"
#include "Ref.h"
#include "ShaderProgram.h"

class Shader_Sprite3D : public ShaderProgram
{
public:
	Shader_Sprite3D();

	~Shader_Sprite3D();

protected:
	virtual void initUniform();
protected:
	GLuint _uSamplerID;
	GLuint _uMVPMatrixID;
};
