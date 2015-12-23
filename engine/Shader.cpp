#include "Shader.h"
#include "Defines.h"
#include "ShaderValue.h"
#include "ShaderName.h"
#include "Mesh.h"
#include "Camera.h"

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

void Shader::Init(GLuint programID, Vector<ShaderModule*> modules)
{
	_programID = programID;
	_mudules = modules;

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

	for (auto module : _mudules)
	{
		module->InitUniformsLocation(_programID);
	}
}

void Shader::SetUniformLocationWith1i(string uniform, GLint i1)
{
	if (_uniformsLocation[uniform] != INVALID_OGL_VALUE)
	{
		glUniform1i(_uniformsLocation[uniform], i1);
	}
}

void Shader::SetUniformLocationWithMatrix4(string uniform, const mat4& matrix)
{
	if (_uniformsLocation[uniform] != INVALID_OGL_VALUE)
	{
		glUniformMatrix4fv(_uniformsLocation[uniform], 1, GL_FALSE, &matrix[0][0]);
	}
}

void Shader::Use(unsigned int textureFlag, Mesh* mesh, mat4 toWorldTransform, Camera* camera)
{
	if (textureFlag & COLOR_TEXTURE_INDEX)
	{
		SetUniformLocationWith1i(UNIFORM_TEXTURE_COLOR_SAMPLER, COLOR_TEXTURE_INDEX);
	}
	if (textureFlag & NORMAL_TEXTURE_INDEX)
	{
		SetUniformLocationWith1i(UNIFORM_TEXTURE_NORMAL_MAP_SAMPLER, NORMAL_TEXTURE_INDEX);
	}

	mat4 projTransform = camera->GetProjectTransform();
	mat4 viewTransform = camera->GetViewTransform();

	glm::mat4 MVP = projTransform * viewTransform * toWorldTransform;
	SetUniformLocationWithMatrix4(UNIFORM_M,   toWorldTransform);
	SetUniformLocationWithMatrix4(UNIFORM_V,   viewTransform);
	SetUniformLocationWithMatrix4(UNIFORM_MVP, MVP);

	for (auto module : _mudules)
	{
		module->Use(mesh, toWorldTransform, camera);
	}
}
