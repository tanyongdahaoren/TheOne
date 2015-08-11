#include "Shader.h"
#include "Defines.h"
#include "ShaderValue.h"

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
	_uniformsLocation[UNIFORM_NAME_M] = GetUniformLocation(UNIFORM_NAME_M.c_str());
	
	_uniformsLocation[UNIFORM_NAME_V] = GetUniformLocation(UNIFORM_NAME_V.c_str());

	_uniformsLocation[UNIFORM_NAME_MVP] = GetUniformLocation(UNIFORM_NAME_MVP.c_str());

	_uniformsLocation[UNIFORM_NAME_SAMPLER] = GetUniformLocation(UNIFORM_NAME_SAMPLER.c_str());
}

void Shader::SetUniformLocationWith1i(string uniform, GLint i1)
{
	glUniform1i(_uniformsLocation[uniform], i1);
}

void Shader::SetUniformLocationWithMatrix4fv(string uniform, const GLfloat* matrixArray)
{
	glUniformMatrix4fv(_uniformsLocation[uniform], 1, GL_FALSE, matrixArray);
}

GLint Shader::GetUniformLocation(const char* pUniformName)
{
	GLuint Location = glGetUniformLocation(_programID, pUniformName);

	if (Location == INVALID_UNIFORM_LOCATION) {
		fprintf(stderr, "Warning! Unable to get the location of uniform '%s'\n", pUniformName);
	}

	return Location;
}
