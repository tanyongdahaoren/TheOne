#include "Shader.h"
#include "Defines.h"
#include "ShaderValue.h"
#include "ShaderName.h"
#include "Mesh.h"

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
	glUniform1i(_uniformsLocation[uniform], i1);
}

void Shader::SetUniformLocationWithMatrix4(string uniform, const mat4& matrix)
{
	glUniformMatrix4fv(_uniformsLocation[uniform], 1, GL_FALSE, &matrix[0][0]);
}

void Shader::Use(unsigned int textureFlag, Mesh* mesh, mat4 toWorldTransform, mat4 viewTransform, mat4 projTransform)
{
	if (textureFlag & COLOR_TEXTURE_INDEX)
	{
		SetUniformLocationWith1i(UNIFORM_TEXTURE_COLOR_SAMPLER, COLOR_TEXTURE_INDEX);
	}
	if (textureFlag & NORMAL_TEXTURE_INDEX)
	{
		SetUniformLocationWith1i(UNIFORM_TEXTURE_NORMAL_MAP_SAMPLER, NORMAL_TEXTURE_INDEX);
	}
	
	glm::mat4 MVP = projTransform * viewTransform * toWorldTransform;
	SetUniformLocationWithMatrix4(UNIFORM_M,   toWorldTransform);
	SetUniformLocationWithMatrix4(UNIFORM_V,   viewTransform);
	SetUniformLocationWithMatrix4(UNIFORM_MVP, MVP);

	for (auto module : _mudules)
	{
		module->Use(mesh, toWorldTransform, viewTransform, projTransform);
	}
}







std::string ShaderShadowMap::GetVertShader()
{
	GLchar buff[256];
	SNPRINTF(buff, sizeof(buff) - 1,
		"\n const int kMaxBoneNum =  %d; \n",
		kMaxBoneNum);
	string str = string(buff) + string(DepthRTT_vert);
	return str;
}

std::string ShaderShadowMap::GetFragShader()
{
	return DepthRTT_frag;
}

void ShaderShadowMap::InitUniformsLocation()
{
	Shader::InitUniformsLocation();

	//skelon
	_openSkelonLocation = GLGetUniformLocation(_programID, "u_open_skelon");
	for (int i = 0; i < kMaxBoneNum; i++)
	{
		char Name[128];
		memset(Name, 0, sizeof(Name));
		SNPRINTF(Name, sizeof(Name), "u_bone[%d]", i);
		_boneLocation[i] = GLGetUniformLocation(_programID, Name);
	}
}

void ShaderShadowMap::Use(Mesh* mesh, mat4 toWorldTransform)
{
	//for skelon
	glUniform1i(_openSkelonLocation, mesh->HaveBone() ? 1 : 0);
	for (int i = 0; i < mesh->_boneNum; i++)
	{
		const mat4& transform = mesh->_bonesInfo[i].FinalTransformation;
		glUniformMatrix4fv(_boneLocation[i], 1, GL_FALSE, (const GLfloat*)&transform[0][0]);
	}
}
