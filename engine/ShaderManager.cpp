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
		"uniform sampler2D u_texture_color_sampler;\n"
		"uniform sampler2D u_texture_normal_map_sampler;\n",
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

		GLchar InfoLog[1024];
		glGetShaderInfoLog(*shader, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader : '%s'\n", InfoLog);
		printf("ERROR: Failed to compile shader:\n%s", InfoLog);

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
		{ ATTRIBUTE_NAME_BLEND_INDEX, eShaderVertAttribute_blend_index },
		{ ATTRIBUTE_NAME_BLEND_WEIGHT, eShaderVertAttribute_blend_weight },
		{ ATTRIBUTE_NAME_TANGENT, eShaderVertAttribute_tangent },
	};

	const int size = ARRAY_SIZE(attribute_locations);

	for (int i = 0; i < size; i++)
	{
		glBindAttribLocation(program, attribute_locations[i].location, attribute_locations[i].attributeName);
	}
}

Shader* ShaderManager::LoadShader(string shaderName)
{
	Shader* p = GetShader(shaderName);
	if (p)
	{
		return p;
	}

	Vector<ShaderModule*> modules;

	const ShaderInfo& shaderInfo = ShaderInfoMapTable.at(shaderName);
	string vertStr;
	string fragStr;
	for (auto& moduleName : shaderInfo._modules)
	{
		ShaderModule* module = ShaderModuleMapTable.at(moduleName)();
		vertStr += module->VertexStr();
		fragStr += module->FragStr();
		modules.push_back(module);
	}
	vertStr += shaderInfo._vert;
	fragStr += shaderInfo._frag;
	//printf("%s vertStr:%s\n", shaderName.c_str(), vertStr.c_str());
	//printf("%s fragStr:%s\n", shaderName.c_str(), vertStr.c_str());

	GLuint vertShader = 0, fragShader = 0;

	if (!compileShader(&vertShader, GL_VERTEX_SHADER, vertStr.c_str()))
	{
		printf("ERROR: Failed to compile vertex shader\n");
	}

	if (!compileShader(&fragShader, GL_FRAGMENT_SHADER, fragStr.c_str()))
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
		p = new Shader;
		p->Init(program, modules);
		_shaders.insert(make_pair(shaderName, p));

		printf("Load shader success:%s\n", shaderName.c_str());
	}
	return p;
}

void ShaderManager::LoadDefaultShaders()
{
	LoadShader(shader_position_color);

	LoadShader(shader_position_texture);

	LoadShader(shader_depth_rtt);

	LoadShader(shader_base_light);
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
