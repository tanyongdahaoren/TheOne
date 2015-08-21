#include <stdio.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <stdlib.h>
#include <string.h>

using namespace std;
#include "Defines.h"
#include "ShaderValue.h"

#include "ShaderManager.h"
#include "ShaderName.h"
#include "ShaderBaseLight.h"

static ShaderManager* s_instance = nullptr;
ShaderManager* ShaderManager::GetInstance()
{
	if (! s_instance)
	{
		s_instance = new ShaderManager();
	}

	return s_instance;
}

void ShaderManager::DestroyInstance()
{
	SAFE_DELETE(s_instance);
}

ShaderManager::ShaderManager()
{
	LoadDefaultShaders();
}

ShaderManager::~ShaderManager()
{

}

bool compileShader(GLuint * shader, GLenum type, const GLchar* source)
{
	GLint status;

	if (!source)
	{
		return false;
	}

	const GLchar *sources[] = {
		"\n#version 330 core\n"
		"uniform mat4 u_MVP;\n"
		"uniform mat4 u_M;\n"
		"uniform mat4 u_V;\n"
		"uniform sampler2D u_sampler;\n",
		source,
	};

	*shader = glCreateShader(type);
	glShaderSource(*shader, sizeof(sources) / sizeof(*sources), sources, nullptr);
	glCompileShader(*shader);

	glGetShaderiv(*shader, GL_COMPILE_STATUS, &status);

	if (!status)
	{
		GLsizei length;
		glGetShaderiv(*shader, GL_SHADER_SOURCE_LENGTH, &length);
		GLchar* src = (GLchar *)malloc(sizeof(GLchar) * length);

		glGetShaderSource(*shader, length, nullptr, src);
		printf("ERROR: Failed to compile shader:\n%s", src);

		free(src);

		return false;;
	}
	return (status == GL_TRUE);
}

void bindPredefinedVertexAttribs(GLuint program)
{
	static const struct {
		const char *attributeName;
		int location;
	} attribute_locations[] =
	{
		{ ATTRIBUTE_NAME_POSITION, eShaderVertAttribute_pos },
		{ ATTRIBUTE_NAME_COLOR, eShaderVertAttribute_color },
		{ ATTRIBUTE_NAME_TEX_COORD, eShaderVertAttribute_texcood },
		{ ATTRIBUTE_NAME_NORMAL, eShaderVertAttribute_normal },
		{ ATTRIBUTE_NAME_BLEND_WEIGHT, eShaderVertAttribute_blend_weight },
		{ ATTRIBUTE_NAME_BLEND_INDEX, eShaderVertAttribute_blend_index },
	};

	const int size = ARRAY_SIZE(attribute_locations);

	for (int i = 0; i < size; i++)
	{
		glBindAttribLocation(program, attribute_locations[i].location, attribute_locations[i].attributeName);
	}
}

Shader* ShaderManager::LoadShaders(const GLchar* vShaderByteArray, const GLchar* fShaderByteArray, string shaderName, std::function<Shader*()> newProgramFunc)
{
	printf("Load shader begin:%s\n", shaderName.c_str());

	Shader* p = GetShader(shaderName);
	if (p)
	{
		return p;
	}
	
	GLuint vertShader = 0, fragShader = 0;

	if (!compileShader(&vertShader, GL_VERTEX_SHADER, vShaderByteArray))
	{
		printf("ERROR: Failed to compile vertex shader\n");
	}

	if (!compileShader(&fragShader, GL_FRAGMENT_SHADER, fShaderByteArray))
	{
		printf("ERROR: Failed to compile frag shader\n");
	}

	if (vertShader && fragShader)
	{
		GLuint program = glCreateProgram();

		glAttachShader(program, vertShader);
		glAttachShader(program, fragShader);

		bindPredefinedVertexAttribs(program);

		glLinkProgram(program);

		GLint status;
		glGetProgramiv(program, GL_LINK_STATUS, &status);
		if (!status)
		{
			printf("ERROR: Failed to link program");
		}

		glDeleteShader(vertShader);
		glDeleteShader(fragShader);

		//insert to _shaders
		p = newProgramFunc();
		p->InitWithProgramId(program);
		_shaders.insert(make_pair(shaderName, p));

		printf("Load shader success:%s\n", shaderName.c_str());
	}
	return p;
}

void ShaderManager::LoadDefaultShaders()
{
	LoadShaders(PositionColor_vert, PositionColor_frag, shader_position_color, [](){return new Shader();});

	LoadShaders(PositionTexture2D_vert, PositionTexture2D_frag, shader_position_texure_2D, [](){return new Shader();});

	LoadShaders(PositionTexture3D_vert, PositionTexture3D_frag, shader_position_texure_3D, [](){return new Shader();});

	GLchar buff[256];
	_snprintf(buff, sizeof(buff) - 1,
		"\nconst int kMaxPointLightNum =  %d; \n"
		"\nconst int kMaxSpotLightNum = %d; \n",
		kMaxPointLightNum,
		kMaxSpotLightNum);
	string str = string(buff) + string(BaseLight_frag);
	LoadShaders(BaseLight_vert, str.c_str(), shader_base_light_3D, [](){return new ShaderBaseLight(); });
}

Shader* ShaderManager::GetShader(string shaderName)
{
	auto it = _shaders.find(shaderName);
	if (it != _shaders.end())
	{
		return it->second;
	}
	return NULL;
}
