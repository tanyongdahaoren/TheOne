#include "Shader_Sprite3D.h"
#include "Defines.h"

Shader_Sprite3D::Shader_Sprite3D()
{
}

Shader_Sprite3D::~Shader_Sprite3D()
{
}

void Shader_Sprite3D::initUniform()
{
	_uSamplerID = glGetUniformLocation(_programID, "u_sampler");
	_uMVPMatrixID = glGetUniformLocation(_programID, "u_MVP");
}
