#include "Shader.h"
#include "Defines.h"
#include "ShaderValue.h"


GLint GLGetUniformLocation(GLuint programID, const char* pUniformName)
{
	GLuint Location = glGetUniformLocation(programID, pUniformName);

	if (Location == INVALID_UNIFORM_LOCATION) {
		fprintf(stderr, "Warning! Unable to get the location of uniform '%s'\n", pUniformName);
	}

	return Location;
}

Shader::Shader()
	: _programID(INVALID_OGL_VALUE)
{

}

Shader::~Shader()
{
}

void Shader::InitWithProgramId(GLuint programID)
{
	_programID = programID;

	Active();

	InitUniformsLocation();
}

void Shader::Active()
{
	glUseProgram(_programID);
}

void Shader::InitUniformsLocation()
{
	_uniformsLocation[UNIFORM_M] = GLGetUniformLocation(_programID, UNIFORM_M.c_str());
	
	_uniformsLocation[UNIFORM_V] = GLGetUniformLocation(_programID, UNIFORM_V.c_str());

	_uniformsLocation[UNIFORM_MVP] = GLGetUniformLocation(_programID, UNIFORM_MVP.c_str());

	_uniformsLocation[UNIFORM_TEXTURE_COLOR_SAMPLER] = GLGetUniformLocation(_programID, UNIFORM_TEXTURE_COLOR_SAMPLER.c_str());

	_uniformsLocation[UNIFORM_TEXTURE_NORMAL_MAP_SAMPLER] = GLGetUniformLocation(_programID, UNIFORM_TEXTURE_NORMAL_MAP_SAMPLER.c_str());
}

void Shader::SetUniformLocationWith1i(string uniform, GLint i1)
{
	glUniform1i(_uniformsLocation[uniform], i1);
}

void Shader::SetUniformLocationWithMatrix4(string uniform, const mat4& matrix)
{
	glUniformMatrix4fv(_uniformsLocation[uniform], 1, GL_FALSE, &matrix[0][0]);
}
